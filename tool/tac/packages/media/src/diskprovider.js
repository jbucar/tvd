"use strict";

var assert = require('assert');
var path = require('path');
var ProviderImpl = require('./providerimpl');
var ImageDelegate = require('./disk/image');
var VideoDelegate = require('./disk/video');
var bPromise = require('bluebird');

function media(n) {
	return 'ar.edu.unlp.info.lifia.tvd.media.' + n;
}

function ServiceImpl(adapter) {
	var self = {};
	var _diskmgr = null;

	function reg() {
		return adapter.registry();
	}

	function getName(diskInfo,mountPath) {
		if (diskInfo.label) {
			return diskInfo.label;
		}
		if (diskInfo.drive.vendor) {
			return diskInfo.drive.vendor;
		}
		if (diskInfo.drive.model) {
			return diskInfo.drive.model;
		}
		if (diskInfo.drive.revision) {
			return diskInfo.drive.revision;
		}
		if (diskInfo.drive.serial) {
			return diskInfo.drive.serial;
		}

		return path.basename(mountPath);
	}

	function removeLocalProvider( srvName, diskID ) {
		var srv = reg().get(media(srvName));
		assert(srv);
		if (srv.hasProvider(diskID)) {
			srv.rmProvider( diskID );
		}
	}

	function addLocalProvider( srvName, dlg ) {
		var srv = reg().get(media(srvName));
		assert(srv);
		if (!srv.hasProvider(dlg.id)) {
			srv.addProvider( new ProviderImpl( dlg ) );
		}
	}

	function updateProvider( diskID ) {
		_diskmgr.get( diskID, function(err,diskInfo) {
			var remove = false;

			if (err) {
				log.silly( 'DiskProvider', 'Ignoring device, error getting information: err=%s', err.message );
				remove = true;
			}
			else if (diskInfo.isSystemDevice || diskInfo.mountPaths.length === 0) {
				assert(diskID === diskInfo.id);
				remove = true;
			}
			else {
				assert(diskID === diskInfo.id);
				remove = false;
			}

			if (remove) {
				rmProvider( diskID );
			}
			else {
				var mountPath = diskInfo.mountPaths[0];
				log.info('DiskProvider', 'Add providers for disk: diskID=%s, mnt=%s, diskInfo=%j', diskID, mountPath, diskInfo );

				//	Add temporary providers
				addLocalProvider( 'image', new ImageDelegate( diskInfo.id, getName(diskInfo,mountPath), mountPath ) );
				addLocalProvider( 'video', new VideoDelegate( diskInfo.id, getName(diskInfo,mountPath), mountPath ) );
			}
		});
	}

	function addProvider( diskID ) {
		log.verbose( 'DiskProvider', 'Disk added, update provider: diskID=%s', diskID );
		updateProvider( diskID );
	}

	function chgProvider( diskID ) {
		log.verbose( 'DiskProvider', 'Disk changed, update provider: diskID=%s', diskID );
		updateProvider( diskID );
	}

	function rmProvider( diskID ) {
		log.verbose( 'DiskProvider', 'Disk removed, remove provider: diskID=%s', diskID );

		//	Remove temporary providers
		removeLocalProvider( 'image', diskID );
		removeLocalProvider( 'video', diskID );
	}

	//	API functions
	self.start = function(cb) {
		_diskmgr = reg().get('diskmgr');
		assert(_diskmgr);

		let enumDevicesAsync = bPromise.promisify(_diskmgr.enumerateDevices);

		_diskmgr.on( 'added', addProvider );
		_diskmgr.on( 'changed', chgProvider );
		_diskmgr.on( 'removed', rmProvider );
		enumDevicesAsync()
			.then(function(aDevices) {
				aDevices.forEach(function(devID) {
					addProvider( devID );
				});

				cb( undefined, {} );
			}, cb );
	};

	self.stop = function(cb) {
		_diskmgr.removeListener( 'added', addProvider );
		_diskmgr.removeListener( 'changed', chgProvider );
		_diskmgr.removeListener( 'removed', rmProvider );
		_diskmgr = null;
		cb();
	};

	return self;
}

//	Service entry point
function Service(adapter) {
	var self = {};
	var _impl = null;

	self.onStart = function(cb) {
		log.info( 'DiskProvider', 'Start' );

		//	Create service implementation
		_impl = new ServiceImpl( adapter );

		//	Start
		_impl.start( cb );
	};

	self.onStop = function(cb) {
		log.info( 'DiskProvider', 'Stop' );
		_impl.stop(function() {
			_impl = null;
			cb();
		});
	};

	return Object.freeze(self);
}

module.exports = Service;

