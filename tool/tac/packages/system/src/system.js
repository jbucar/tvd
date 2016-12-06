"use strict";

var assert = require('assert');
var EventEmitter = require('events').EventEmitter;
var _ = require('lodash');
var util = require('util');
var bPromise = require('bluebird');
var path = require('path');
var fs = bPromise.promisifyAll( require('fs-extra') );
var tvdutil = require('tvdutil');

//	Config filename
var sys_config = 'system.json';

function SystemImpl( adapter ) {
	var _cfg = null;
	var _sendReport = null;
	var _reports = [];

	var self = {};
	self.events = new EventEmitter();

	function registry() {
 		return adapter.registry();
	}

	function backtraceFile() {
		return adapter.getDataPath('tac.trace');
	}

	function crashPath() {
		return adapter.getDataPath( 'crashes' );
	}

	function defaultConfig() {
		return {
			inDevelopmentMode: false,
			geolocationEnabled: true,
			remotePort: 2000,
			log: {
				enabled: false,
				maxHistorySize: 1000,
				maxWidth: null, // Disable maxWidth
				level: 'info'
			}
		};
	}

	function convertConfig( oldCfg ) {
		if (_.isUndefined(oldCfg.geolocationEnabled)) {
			oldCfg.geolocationEnabled = true;
		}
		return oldCfg;
	}

	function saveReport( data ) {
		function zeroFill(n) {
			return ('0'+n).slice(-2);
		}

		if (!_cfg.inDevelopmentMode) {
			return bPromise.resolve();
		}

		let crashDir = crashPath();
		return fs.ensureDirAsync( crashDir )
			.then(function() {
				let d = new Date(Date.now());
				let fsName = zeroFill(d.getDate()) + '.' + zeroFill(d.getMonth() + 1) + '.' + d.getFullYear();
				fsName += '_' + zeroFill(d.getHours()) + '.' + zeroFill(d.getMinutes()) + '.' + zeroFill(d.getSeconds());
				return fs.writeFileAsync( path.join(crashDir,fsName+'.crash'), data )
					.catch( () => {} );
			});
	}

	function sendBacktraceAsync() {
		return fs.readFileAsync( backtraceFile() )
			.then(function(data) {
				var bt=data.toString();
				var sndErr = new Error('Core dump: tac.trace');
				sndErr.stack = bt;
				self.sendError( sndErr );
				return backtraceFile();
			})
			.then(fs.unlinkAsync)
			.catch(function() {
				return undefined;
			});
	}

	self.saveConfig = function() {
		adapter.save( sys_config, _cfg );
	};

	//	Logging
	self.getLogConfig = function() {
		return _.cloneDeep(_cfg.log);
	};

	self.getRemotePort = function() {
		return _cfg.remotePort;
	};

	self.setLogConfig = function(cfg) {
		if ((typeof cfg.enabled === 'boolean') &&
			(cfg.maxHistorySize >= 0 && typeof cfg.maxHistorySize === 'number') &&
			(cfg.maxWidth === null || (typeof cfg.maxWidth === 'number' && cfg.maxWidth >= 0)) &&
			(['error','warn', 'http', 'info', 'verbose', 'silly'].indexOf(cfg.level) >= 0))
		{
			try {
				//	Save configuration
				_cfg.log = _.assign(_cfg.log, cfg);
				self.saveConfig();

				//	Send log config changed!
				self.events.emit( 'LogConfigChanged', _cfg.log );

				return true;
			} catch(err) {
				log.error( 'System', 'Cannot set log configuration: err=%s', err.message );
			}
		}

		return false;
	};

	//	Send error to url crash server
	self.sendError = function( err ) {
		if (!err || !err.message || !err.stack) {
			throw new Error( tvdutil.warnErr('System', 'sendError: err is not a Error') );
		}

		log.info( 'System', 'Send error: err=%s', err.message );

		//	Format logs
		var formatLogMessage = function(msg) {
			return util.format('[%s] %s: %s', msg.level, msg.prefix, msg.message );
		};
		var logs = log.record
			.filter(function(msg) {
				return msg.level !== "silly";
			})
			.map(function(msg) {
				return formatLogMessage(msg);
			});

		var reg = registry();
		var data = {
			message: err.message,
			stack: err.stack,
			systemInfo: self.getInfo(),
			log: logs.slice(-50)
		};

		{	//	Get all packages installed with this version
			var pkgmgr = reg.get('pkgmgr');
			if (pkgmgr) {
				data.packages = pkgmgr.getAllInfo().reduce(function(pkgs, info) {
					pkgs[info.id] = info.version;
					return pkgs;
				}, {});
			} else {
				data.packages = {};
			}
		}

		{	//	Get all services running
			var srvmgr = reg.get('srvmgr');
			if (srvmgr) {
				//	Dump state!
				data.services = srvmgr.getAllRunning().reduce(function(srvs,id) {
					srvs[id] = srvmgr.dump(id);
					return srvs;
				}, {});
			} else {
				data.services = {};
			}
		}

		//	Send report or queue
		if (_sendReport) {
			_sendReport( data );
		}
		else {
			_reports.push( data );
		}

		return saveReport( data );
	};

	self.setReportCallback = function(cb) {
		_sendReport = cb;
		if (cb) {
			return sendBacktraceAsync()
				.then(function() {
					_reports.forEach(function(crash) {
						_sendReport(crash);
					});
					_reports = [];
				});
		}
		return bPromise.resolve();
	};

	self.getInfo = function() {
		var info = {};
		info.id = tac.getSerial();
		info.platform = tac.getPlatform();
		info.system = tac.getSystemVersion();
		info.build = registry().get('platform').getConfig('build');
		return info;
	};

	//	Shutdown methods
	self.powerOff = function() {
		log.info('System', 'Power off');
		tac.powerOff();
	};

	self.reboot = function() {
		log.info('System', 'Reboot');
		tac.reboot();
	};

	self.factoryReset = function() {
		log.info('System', 'Factory reset');
		tac.factoryReset();
	};

	self.updateFirmware = function( url, now ) {
		log.info('System', 'Update firmware: url=%s, now=%d', url, now );
		tac.updateFirmware( url, now );
	};

	//	First boot/license
	self.isFirstBoot = function() {
		return tac.isFirstBoot();
	};

	self.licenseAccepted = function() {
		tac.licenseAccepted();
	};

	//	Development mode
	self.enableDevelopmentMode = function(state) {
		if (_cfg.inDevelopmentMode != state) {
			log.info( 'System', 'Development mode changed: state=%d', state );
			tac.enableDevelopmentMode( state );
			_cfg.inDevelopmentMode = state;
			self.saveConfig();
			self.events.emit( 'DevelopmentModeEnabled', state );
		}
	};

	self.isDevelopmentMode = function() {
		return _cfg.inDevelopmentMode;
	};

	//	Geolocation
	self.geolocationEnabled = function() {
		return _cfg.geolocationEnabled;
	};

	self.enableGeolocation = function(enable) {
		if (_cfg.geolocationEnabled != enable) {
			log.info( 'System', 'Enable Geolocation: enable=%d', enable );
			_cfg.geolocationEnabled = enable;
			self.saveConfig();
		}
	};

	//	Initialization
	self.start = function( cb ) {
		//	Load config
		_cfg = adapter.load( sys_config, defaultConfig(), convertConfig );

		//	Setup dev mode global
		tac.enableDevelopmentMode( _cfg.inDevelopmentMode );

		//	Cleanup reports API
		_sendReport = null;
		_reports = [];

		//	Setup API
		cb( undefined, new SystemApi(self) );
	};

	self.stop = function(cb) {
		//	Remove all listeners
		self.events.removeAllListeners();

		cb();
	};

	return self;
}

function SystemApi( sys ) {
	var self = this;

	//	Events
	tvdutil.forwardEventEmitter(self,sys.events);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	//	Get basic information
	self.getInfo = function() {
		return sys.getInfo();
	};
	self.getInfo._description = 'Get system version';

	//	Get remote url port
	self.getRemotePort = sys.getRemotePort.bind(sys);
	self.getRemotePort._description = 'Get API port';

	//	Power off
	self.powerOff = function() {
		sys.powerOff();
	};
	self.powerOff._description = 'Shutdown system';

	//	Reboot
	self.reboot = function() {
		sys.reboot();
	};
	self.reboot._description = 'Reboot system';

	//	Factory reset
	self.factoryReset = function() {
		sys.factoryReset();
	};
	self.factoryReset._description = 'Recovery system';

	//	Update firmware
	self.updateFirmware = function( url, now ) {
		sys.updateFirmware( url, (now !== undefined) ? now : true );
	};
	self.updateFirmware._description = 'Update firmware';

	//	Send error to error server if enabled
	self.sendError = function(err) {
		return sys.sendError( err );
	};

	self.setReportCallback = function(cb) {
		return sys.setReportCallback(cb);
	};

	self.getLogConfig = function() {
		return sys.getLogConfig();
	};

	self.setLogConfig = function(cfg) {
		assert(cfg);
		return sys.setLogConfig(cfg);
	};

	self.isDevelopmentMode = function() {
		return sys.isDevelopmentMode();
	};

	self.enableDevelopmentMode = function() {
		sys.enableDevelopmentMode(true);
	};

	self.disableDevelopmentMode = function() {
		sys.enableDevelopmentMode(false);
	};

	self.isFirstBoot = function() {
		return sys.isFirstBoot();
	};

	self.licenseAccepted = function() {
		sys.licenseAccepted();
	};

	self.isGeolocationEnabled = function() {
		return sys.geolocationEnabled();
	};

	self.enableGeolocation = function(enable) {
		sys.enableGeolocation(enable);
	};

	self.getWebAPI = function() {
		return {
			'name': 'system',
			'public': [
				'getInfo',
				'getRemotePort',
				'getLogConfig',
				'sendError',
				'isFirstBoot',
				'isGeolocationEnabled',
				'on'
			],
			'private': [
				'powerOff',
				'reboot',
				'factoryReset',
				'licenseAccepted',
				'updateFirmware',
				'setLogConfig',
				'isDevelopmentMode',
				'disableDevelopmentMode',
				'enableGeolocation'
			]
		};
	};

	return Object.freeze(self);
}

//	Service entry point
function System(adapter) {
	var _impl = new SystemImpl(adapter);
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

module.exports = System;
