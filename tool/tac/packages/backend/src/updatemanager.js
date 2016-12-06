"use strict";

var assert = require('assert');
var fs = require('fs');
var EventEmitter = require('events').EventEmitter;
var _ = require('lodash');
var bPromise = require('bluebird');
var mktemp = bPromise.promisifyAll( require('mktemp') );
var tvdutil = require('tvdutil');
var RequestInterval = require('./requestinterval');

function UpdateManagerImpl( adapter ) {
	var _firmware = {};
	var _packages = {};
	var _updateCondition = null;

	function reg() {
		return adapter.registry();
	}

	//	status can be:
	//		error
	//		idle
	//		checking
	//		available
	//		downloading
	//		downloaded
	//		updating
	function firmwareStatus( st, key, data ) {
		assert.notEqual(_firmware.status, st);

		//	Update state
		var old = _firmware.status;
		_firmware.status = st;

		//	Update info
		if (key && data) {
			_firmware[key] = data;
		}

		//	Notify
		var notifyEvent = { status: _firmware.status };
		 if (st === 'available') {
			notifyEvent.info = _firmware.info;
		}
		else if (st === 'downloading') {
			notifyEvent.progress = 0;
		}
		else if (st === 'downloaded') {
			notifyEvent.url = _firmware.firmwareFile;
		}
		else if (st === 'error') {
			notifyEvent.error = _firmware.error.message;
		}
		self.events.emit( 'Firmware', notifyEvent );

		if (st === 'downloading') {
			//	Wait for fetch response
			_updateCondition.inc();
		}

		if (old === 'downloading') {
			//	Remove working state
			_updateCondition.dec();
		}
	}

	var self = {};

	//	Events
	self.events = new EventEmitter();

	self.onPkgInstalled = function( pkgID ) {
		log.verbose( "UpdateManager", "On package installed: pkgID=%s", pkgID );
		//	Add package checker if need update
		var pkg = reg().get('pkgmgr').get( pkgID );
		if (pkg.update) {
			var cfg = reg().get('backend').getPackageUpdateConfig();
			self.startPackageUpdate( cfg, pkgID, pkg.version );
		}
	};

	self.onPkgUninstalled = function( pkgID ) {
		log.verbose( "UpdateManager", "On package uninstalled: pkgID=%s", pkgID );
		//	Remove package checker if already present
		var pkgChecker = _packages[pkgID];
		if (pkgChecker) {
			pkgChecker.stop();
			delete _packages[pkgID];
		}
	};

	self.onBackendConfigChanged = function(cfg) {
		log.verbose( "UpdateManager", "On backend config changed: serial=%s", cfg.serial );

		//	Update firmware requests
		_firmware.checker.config( RequestInterval.makeOptionsFromConfig(cfg.firmware_update_server) );

		//	Update package update config
		_.keys(_packages).forEach(function(id) {
			_packages[id].config( RequestInterval.makeOptionsFromConfig(cfg.package_update_server) );
		});

		//	Ensure system packages
		for (var p=0; p<cfg.system_packages.length; p++) {
			self.startPackageUpdate( cfg.package_update_server, cfg.system_packages[p], '0.0.0' );
		}
	};

	//	Firmware
	self.getFirmwareRequestInfo = function() {
		assert.equal(_firmware.status,'idle');
		firmwareStatus( 'checking' );

		var info = reg().get('system').getInfo();
		return {
			id: info.id,
			platform: info.platform.name,
			version: info.system.version
		};
	};

	self.startFirmwareUpdate = function( cfg ) {
		_firmware = {};
		_firmware.status = 'idle';

		_firmware.checker = new RequestInterval.create();
		_firmware.checker.start(
			RequestInterval.makeOptionsFromConfig(cfg),
			self.getFirmwareRequestInfo,
			self.onFirmwareResponse
		);
	};

	self.checkFirmwareUpdate = function() {
		if (_firmware.status === 'error') {
			firmwareStatus( 'idle' );
		}

		if (_firmware.status === 'idle') {
			_firmware.checker.forceRequest();
		}
		else {
			log.warn('UpdateManager', 'Cannot check for firmware update; invalid state: state=%s', _firmware.status );
		}
	};

	self.onFirmwareResponse = function( err, data ) {
		assert.equal(_firmware.status,'checking');

		//	Check mandatory fields
		if (!err && data.version && data.url && data.name && data.description && data.md5 && data.size) {
			log.verbose('UpdateManager', 'On firmware update received: data=%j', data );

			//	Notify
			firmwareStatus( 'available', 'info', data );

			//	Check if mandatory
			if (data.mandatory) {
				//	Download firmware
				log.info('UpdateManager', 'Mandatory firmware update received, download update' );
				self.downloadFirmwareUpdate();
			}

			return false;
		}
		else {
			//	Set state to idle
			firmwareStatus( 'idle' );
		}

		return true;
	};

	self.downloadFirmwareUpdate = function() {
		if (_firmware.status != 'available') {
			log.verbose('UpdateManager', 'Cannot download firmware, invalid status: status=%s', _firmware.status );
			return;
		}

		log.verbose('UpdateManager', 'Download firmware update' );

		//	Notify
		firmwareStatus( 'downloading' );

		//	Download
		mktemp.createFileAsync(adapter.getDataPath('image_XXXXXXXX.tpk'))
			.then(function(tmpFile) {
				var options = {
					"url": _firmware.info.url,
					"md5": _firmware.info.md5,
					"size": _firmware.info.size,
					"fileOutput": tmpFile,
					'progress': function(p) {
						//	Notify
						self.events.emit( 'Firmware', { status: _firmware.status, progress: p } );
					}
				};
				tvdutil.fetchURL( options, function(err) {
					if (!err) {
						log.info('UpdateManager', 'Firmware downloaded: url=%s', tmpFile );

						//	Notify
						firmwareStatus( 'downloaded', 'firmwareFile', tmpFile );

						//	If mandatory, apply!
						if (_firmware.info.mandatory) {
							self.updateFirmware(false);
						}
					}
					else {
						log.error('UpdateManager', 'Firmware download failed: fetch URL error: err=%s', err.message );
						fs.unlink( tmpFile, function() {
							firmwareStatus( 'error', 'error', err );
						});
					}
				});
			}, function(err) {
				log.error('UpdateManager', 'Firmware download failed: mktemp error: err=%s', err.message );
				firmwareStatus( 'error', 'error', err );
			});
	};

	self.updateFirmware = function(now) {
		if (_firmware.status === 'downloaded') {
			firmwareStatus( 'updating' );
			reg().get('system').updateFirmware( _firmware.firmwareFile, now );
		}
		else if (_firmware.status === 'updating') {
			reg().get('system').powerOff();
		}
		else {
			log.warn('UpdateManager', 'Cannot update firmware; firmware not available: status=%s', _firmware.status );
		}
	};

	//	Packages update
	self.onPackageUpdateResponse = function( pkgID, err, data ) {
		//	Check mandatory fields
		if (!err && data.url && data.version && data.md5 && data.size) {
			log.verbose('UpdateManager', 'On package update received: data=%j', data );

			var handleResponse = function(err) {
				if (err) {
					log.error('UpdateManager', 'Cannot update package. err=%s', err.message );
					reg().get('system').sendError( err );
					self.events.emit('Package', { id: pkgID, err: err.message } );
				}
				else {
					log.verbose('UpdateManager', 'Update package successfully: pkg=%s', pkgID );
					self.events.emit('Package', { id: pkgID } );
				}

				_updateCondition.dec();
			};

			var pkgMgr = reg().get('pkgmgr');
			var opts = {
				"url": data.url,
				"md5": data.md5,
				"size": data.size
			};

			//	Wait for update response
			_updateCondition.inc();

			//	Update/install
			if (pkgMgr.hasPackage(pkgID)) {
				opts.id = pkgID;
				pkgMgr.update( opts, handleResponse );
			}
			else {
				pkgMgr.install( opts, handleResponse );
			}

			//	No check again. On package uninstalled, the package checker is removed
			//	and installed on package install signal
			return false;
		}

		return true;
	};

	self.getPackageRequestInfo = function(pkgID,version) {
		var info = reg().get('system').getInfo();
		return {
			"id": info.id,
			"platform": info.platform.name,
			"version": info.system.version,
			"pkgID": pkgID,
			"pkg-version": version
		};
	};

	self.startPackageUpdate = function(cfg, pkgID, version) {
		log.info( 'UpdateManager', 'Start update for package: pkgID=%s', pkgID );
		var tmp = new RequestInterval.create();
		_packages[pkgID] = tmp;
		tmp.start(
			RequestInterval.makeOptionsFromConfig(cfg),
			function() { return self.getPackageRequestInfo(pkgID,version); },
			function(err,data) { return self.onPackageUpdateResponse(pkgID,err,data); }
		);
	};

	self.startPackagesUpdate = function(pkgMgr,cfg) {
		_packages = {};

		//	Filter only packages with update flag in on
		var pkgs = pkgMgr.getAllInfo().filter(function(pkg) {
			return pkg.update;
		});
		pkgs.forEach(function(pkg) {
			self.startPackageUpdate(cfg,pkg.id,pkg.version);
		});
	};

	self.stopPackagesUpdate = function() {
		_.keys(_packages).forEach(function(id) {
			_packages[id].stop();
		});
		_packages = {};
	};

	self.status = function() {
		return _firmware.status;
	};

	//	Initialization
	self.start = function(cb) {
		//	Setup update condition
		_updateCondition = new tvdutil.BasicWaitCondition();

		//	Listen for package manager install/unininstall events
		var pkgMgr = reg().get('pkgmgr');
		pkgMgr.on('install', self.onPkgInstalled );
		pkgMgr.on('uninstall', self.onPkgUninstalled );

		//	Register on backend configuration changes
		var backend = reg().get('backend');
		backend.on( 'ConfigChanged', self.onBackendConfigChanged );

		//	Setup && start firmware update
		self.startFirmwareUpdate( backend.getFirmwareUpdateConfig() );

		//	Start all packages updater
		self.startPackagesUpdate( pkgMgr, backend.getPackageUpdateConfig() );

		cb( undefined, new UpdateManagerApi(self) );
	};

	self.stop = function(cb) {
		//	Wait for any update in progress
		_updateCondition.wait(function() {
			//	Remove listener on system configuration changes
			reg().get('system').removeListener( 'ConfigChanged', self.onBackendConfigChanged );

			{	//	Remove listener for package manager install/unininstall events
				var pkgMgr = reg().get('pkgmgr');
				pkgMgr.removeListener('install', self.onPkgInstalled );
				pkgMgr.removeListener('uninstall', self.onPkgUninstalled );
			}

			//	Stop firmware update checker
			_firmware.checker.stop();
			_firmware.checker = null;

			//	Stop all packages update checker
			self.stopPackagesUpdate();

			cb();
		});
	};

	return self;
}

function UpdateManagerApi( impl ) {
	var self = {};

	//	Events
	tvdutil.forwardEventEmitter(self,impl.events);
	self.on.isSignal = true;
    self.on.unregisterMethod = self.removeListener;

	//	Force a new check for firmware update
	self.checkFirmwareUpdate = function() {
		impl.checkFirmwareUpdate();
	};

	//	Download firmware
	self.downloadFirmwareUpdate = function() {
		impl.downloadFirmwareUpdate();
	};

	//	Apply firmware update
	self.updateFirmware = function() {
		impl.updateFirmware(true);
	};

	self.getStatus = function() {
		return impl.status();
	};

	self.getWebAPI = function() {
		return {
			'name': 'updatemgr',
			'public': ['on'],
			'private': ['getStatus', 'checkFirmwareUpdate','downloadFirmwareUpdate','updateFirmware']
		};
	};

	return self;
}

//	Service entry point
function UpdateManager(adapter) {
	var _impl = new UpdateManagerImpl(adapter);
	var self = {};

	self.onStart = function(cb) {
		assert(cb);
		_impl.start(cb);
	};

	self.onStop = function(cb) {
		assert(cb);
		_impl.stop(cb);
	};

	return Object.freeze(self);
}

module.exports = UpdateManager;

