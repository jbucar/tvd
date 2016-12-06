"use strict";

var assert = require('assert');
var path = require('path');
var EventEmitter = require('events').EventEmitter;
var tvdutil = require('tvdutil');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var DBus = require('dbus');

function ServiceImpl(adapter) {
	var _dbus = null;
	var _devices = null;
	var _sys = null;

	var self = this;
	var events = new EventEmitter();
	tvdutil.forwardEventEmitter(self, events);

	function changeMountState( devID, isMounted ) {
		assert(devID);
		assert(isMounted !== undefined);

		var oldState = _devices[devID];
		if (oldState !== isMounted) {
			log.info( 'diskmgr', 'Change mount state: devID=%s, isMounted=%d', devID, isMounted );
			_devices[devID] = isMounted;

			events.emit( 'mountChanged', {
				devID: devID,
				isMounted: isMounted
			});
		}
	}

	function getMountState( devID ) {
		//	Get device info
		return _dbus.get(
			'org.freedesktop.UDisks',
			devID,
			'org.freedesktop.UDisks.Device',
			'DeviceIsMounted'
		);
	}

	function checkMountState( devID) {
		return getMountState( devID )
			.then(function(isMounted) {
				changeMountState( devID, isMounted );
			},function(err) {
				log.error( 'diskmgr', 'Error trying to get DeviceIsMounted property: err=%s', err.message );
			});
	}

	function scanMountState() {
		var enumerateDevicesAsync = bPromise.promisify(self.enumerateDevices);
		return enumerateDevicesAsync()
			.map(function (devID) {
				return getMountState(devID)
					.then(function (isMounted) {
						log.verbose( 'diskmgr', 'ScanMount: devID=%s, isMounted=%s', devID, isMounted );
						_devices[devID] = isMounted;
					});
			});
	}

	function onDeviceChanged( devID ) {
		log.verbose( 'diskmgr', 'Device changed: devID=%s', devID );
		assert(_dbus);
		events.emit( 'changed', devID );

		checkMountState( devID );
	}

	function onDeviceAdded( devID ) {
		log.verbose( 'diskmgr', 'Device addded: devID=%s', devID );

		//	Add device
		_devices[devID] = false;
		events.emit( 'added', devID );

		//	Check mount state
		checkMountState( devID );
	}

	function onDeviceRemoved( devID ) {
		//	Remove mounts
		changeMountState( devID, false );

		//	Remove device
		log.verbose( 'diskmgr', 'Device removed: devID=%s', devID );
		delete _devices[devID];
		events.emit( 'removed', devID );
	}

	function getDiskIface(cb) {
		assert(_dbus);
		_dbus.getInterface(
			'org.freedesktop.UDisks',
			'/org/freedesktop/UDisks',
			'org.freedesktop.UDisks',
			cb
		);
	}

	function parseSystemConfig() {
		return fs.readFileAsync('/etc/fstab')
			.then(function(data) {
				return data.toString().split("\n")
					.filter(function(line) {
						return /^[^#]/.test(line);
					}).map(function(line) {
						var obj = line.trim().split(/\s+/g);
						return {
							device: obj[0],
							mount: path.resolve(obj[1]),
							type: obj[2]
						};
					});
			})
			.then(function(fstab) {
				//	Add getInstallRootPath and getDataRootPath
				fstab.push( { device: 'unknown', mount: adapter.getInstallRootPath(), type: 'unknown' } );
				fstab.push( { device: 'unknown', mount: adapter.getDataRootPath(), type: 'unknown' } );

				log.silly( 'diskmgr', 'System configuration: fstab=%j', fstab );
				_sys = fstab;
			});
	}

	function setupSystemDevice(oInfo) {
		for (var i=0; i<oInfo.mountPaths.length; i++) {
			let fMountPaths = path.resolve(oInfo.mountPaths[i]);
			for (var j=0; j<_sys.length; j++) {
				if (fMountPaths === _sys[j].mount) {
					oInfo.isSystemDevice = true;
					return;
				}
			}
		}
	}

	//	API
	self.enumerateDevices = function(cb) {
		assert(_dbus);
		assert(cb);
		_dbus.invoke(
			'org.freedesktop.UDisks',
			'/org/freedesktop/UDisks',
			'org.freedesktop.UDisks',
			'EnumerateDevices' )
			.then( (aDevices) => cb( undefined, aDevices ), cb );
	};

	self.get = function( devID, cb ) {
		assert(_dbus);
		assert(devID);
		assert(cb);
		_dbus.getAll(
			'org.freedesktop.UDisks',
			devID,
			'org.freedesktop.UDisks.Device'
		).then(function(devInfo) {
			var oInfo = {
				id: devID,
				isReadOnly: devInfo.DeviceIsReadOnly,
				isMounted: devInfo.DeviceIsMounted,
				mountPaths: devInfo.DeviceIsMounted ? devInfo.DeviceMountPaths : [],
				isSystemDevice: false,
				drive: {
					vendor: devInfo.DriveVendor,
					model: devInfo.DriveModel,
					revision: devInfo.DriveRevision,
					serial: devInfo.DriveSerial
				},
				label: devInfo.IdLabel,
				size: devInfo.DeviceSize
			};

			//	Setup isSystemDevice
			if (oInfo.isMounted) {
				setupSystemDevice(oInfo);
			}

			cb( undefined, oInfo );
		},cb);
	};

	self.init = function(cb) {
		assert(cb);
		_devices = [];

		//	Setup dbus
		_dbus = new DBus();
		var initAsync = bPromise.promisify(_dbus.init,_dbus);
		var getDiskIfaceAsync = bPromise.promisify(getDiskIface,undefined);

		initAsync()
			.then(parseSystemConfig)
			.then(scanMountState)
			.then( () => getDiskIfaceAsync() )
			.then(function(diskIface) {
				diskIface.addListener( 'DeviceChanged', onDeviceChanged );
				diskIface.addListener( 'DeviceAdded', onDeviceAdded );
				diskIface.addListener( 'DeviceRemoved', onDeviceRemoved );
			})
			.then(function() {
				//	Ok! ... return api
				cb( undefined, new ServiceApi(self) );
			},function(err) {
				cb( tvdutil.warnErr( 'diskmgr', 'Error trying to setup dbus: err=%s', err.message ) );
			});
	};

	self.fin = function(cb) {
		assert(_dbus);

		//	Remove notifications
		getDiskIface(function( err, diskIface ) {
			if (!err) {
				diskIface.removeListener( 'DeviceChanged', onDeviceChanged );
				diskIface.removeListener( 'DeviceAdded', onDeviceAdded );
				diskIface.removeListener( 'DeviceRemoved', onDeviceRemoved );
			}

			_dbus.fin(function() {
				cb();
			});
		});
	};

	return self;
}

function ServiceApi(impl) {
	var self = {};

	self.enumerateDevices = impl.enumerateDevices.bind(impl);
	self.enumerateDevices.isAsync = true;

	self.get = impl.get.bind(impl);
	self.get.isAsync = true;

	self.on = impl.on.bind(impl);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;
	self.removeListener = impl.removeListener.bind(impl);

	self.getWebAPI = function() {
		return {
			'name': 'diskmgr',
			'public': [
				'enumerateDevices',
				'get',
				'on'
			]
		};
	};

	return self;
}

//	Service entry point
function Service(adapter) {
	var self = {};
	var _impl = null;

	self.onStart = function(cb) {
		log.info( 'diskmgr', 'Start' );
		_impl = new ServiceImpl(adapter);
		_impl.init(cb);
	};

	self.onStop = function(cb) {
		log.info( 'diskmgr', 'Stop' );
		assert(_impl);
		_impl.fin(function() {
			_impl = null;
			cb();
		});
	};

	return Object.freeze(self);
}

module.exports = Service;

