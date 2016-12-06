'use strict';

var _ = require('lodash');
var tvdutil = require('tvdutil');
var DBus = require('dbus');
var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var Mocks = require('mocks');
var EventEmitter = require('events').EventEmitter;

var _sbdInfo = {
	DeviceIsReadOnly: false,
	DeviceIsMounted: true,
	DeviceMountPaths: ['/mnt'],
	DriveVendor: 'pepeVendor',
	DriveModel: 'pepeModel',
	DriveRevision: 'pepeRevision',
	DriveSerial: 'pepeSerial',
	Idlabel: 'label',
	DeviceSize: 1000
};

function DiskIface() {
	var self = {};
	var _events = {};
	self.addListener = function(evt,cb) {
		_events[evt] = cb;
	};
	self.removeListener = function(evt,cb) {
		delete _events[evt];
	};
	self.emit = function(evt,data) {
		_events[evt](data);
	};
	return self;
}

var _setupMock;

function mockDBus() {
	var self = {}
	self.init = function(cb) {
		cb( undefined );
	};
	self.fin = function(cb) {
		cb();
	};
	self.getInterface = function( srv, path, iface, cb ) {
		cb( undefined, new DiskIface() );
	};

	self.invoke = function( srv, path, iface, func, paramFormat ) {
		if (func === 'EnumerateDevices') {
			return bPromise.resolve([]);
		}
		return bPromise.resolve();
	};

	self.get = function( srv, path, iface, prop ) {
		return self.invoke( srv, path, 'org.freedesktop.DBus.Properties', 'Get', 'ss', iface, prop );
	};

	self.getAll = function( srv, path, iface ) {
		return self.invoke( srv, path, 'org.freedesktop.DBus.Properties', 'GetAll', 's', iface );
	};

	if (_setupMock) {
		_setupMock(self);
	}

	return self;
}

describe('DiskMgr', function() {
	var reg = null;
	var adapter = null;
	var diskMgr = null;

	beforeEach(function() {
		reg = Mocks.init('silly');
		adapter = new Mocks.ServiceAdapter(reg);
		_setupMock = null;
		diskMgr = Mocks.mockRequire( require, '../src/main', {dbus: mockDBus})(adapter);
	});

	afterEach(function() {
		diskMgr = null;
		Mocks.fin();
		adapter = null;
	});

	describe('constructor', function() {
		it('should construct basic DiskManager', function() {
			assert(diskMgr);
		});
	});

	describe('onStart/onStop', function() {
		it('should check api from diskMgr', function(done) {
			diskMgr.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				assert.isFunction(api.enumerateDevices);
				assert.isFunction(api.get);
				assert.isFunction( api.getWebAPI );
				var webApi = api.getWebAPI();
				assert.equal( webApi.name, 'diskmgr' );
				assert.deepEqual( webApi.public, [
					'enumerateDevices',
					'get',
					'on'
				]);
				diskMgr.onStop(done);
			});
		});

		it('should start/stop/start', function(done) {
			diskMgr.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				diskMgr.onStop(function() {
					diskMgr.onStart(function(err1,api1) {
						assert.equal(err1);
						assert.isObject(api1);
						assert( api1 !== api );
						diskMgr.onStop(done);
					});
				});
			});
		});

		it('should load fstab file', function(done) {
			let oldRead = fs.readFileAsync;
			fs.readFileAsync = bPromise.method(function(fileName) {
				return '/dev/root	/		ext4	ro,noauto	0	1\n/dev/pepe /home/ ext4 rw 0 1\n'
			});

			_setupMock = function(obj) {
				obj.getAll = function() {
					var _info = _.cloneDeep(_sbdInfo);
					_info.DeviceMountPaths = ['/home'];
					return bPromise.resolve( _info );
				};
			};

			diskMgr.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);

				api.get( 'pepeID', function(err,devInfo) {
					assert.equal( err, undefined );
					assert.equal( devInfo.id, 'pepeID' );
					assert.deepEqual( devInfo.mountPaths, ['/home'] );
					assert.equal( devInfo.isSystemDevice, true );

					diskMgr.onStop(function() {
						fs.readFileAsync = oldRead;
						done();
					});
				});
			});
		});

		it('should handle error on init', function(done) {
			_setupMock = function(obj) {
				obj.init = function(cb) {
					cb( new Error('pepe') );
				};
			};

			diskMgr.onStart(function(err,api) {
				assert.equal(err,'Error: Error trying to setup dbus: err=pepe');
				done();
			});
		});

		it('should handle error on getInterface', function(done) {
			_setupMock = function(obj) {
				obj.getInterface = function( srv, path, iface, cb ) {
					cb( new Error('pepe') );
				};
			};

			diskMgr.onStart(function(err,api) {
				assert.equal(err,'Error: Error trying to setup dbus: err=pepe');
				done();
			});
		});

		it('should handle error on stop getInterface', function(done) {
			var count=0;

			_setupMock = function(obj) {
				obj.getInterface = function( srv, path, iface, cb ) {
					count++;
					if (count === 1) {
						cb( undefined, new DiskIface() );
					}
					else {
						cb( new Error('pepe') );
					}
				};
			};

			diskMgr.onStart(function(err,api) {
				assert.equal(err);
				diskMgr.onStop(function() {
					done();
				});
			});
		});
	});

	describe('api', function() {
		var api = null;

		function init(cb) {
			diskMgr.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert.isObject(api);
				cb();
			});
		}

		afterEach(function(done) {
			diskMgr.onStop(function() {
				api = null;
				done();
			});
		});

		describe('enumerateDevices', function() {
			it( 'should check devices returned', function(done) {
				_setupMock = function(obj) {
					obj.invoke = function( srv, path, iface, func, paramFormat ) {
						return bPromise.resolve( ['devID1', 'devID2'] );
					};
				};

				init(function() {
					api.enumerateDevices(function(err,aDevices) {
						assert( err == undefined );
						assert.equal( aDevices.length, 2 );
						assert.equal( aDevices[0], 'devID1' );
						assert.equal( aDevices[1], 'devID2' );
						done();
					});
				});
			});

			it( 'should work with empty devices', function(done) {
				_setupMock = function(obj) {
					obj.invoke = function( srv, path, iface, func, paramFormat ) {
						return bPromise.resolve( [] );
					};
				};

				init(function() {
					api.enumerateDevices(function(err,aDevices) {
						assert( err == undefined );
						assert.equal( aDevices.length, 0 );
						done();
					});
				});
			});

			it( 'should work with error', function(done) {
				let count=-1;
				_setupMock = function(obj) {
					obj.invoke = function( srv, path, iface, func, paramFormat ) {
						count++;
						if (count === 0) {
							return bPromise.resolve([]);
						}
						return bPromise.reject( new Error('pepe') );
					};
				};

				init(function() {
					api.enumerateDevices(function(err,aDevices) {
						assert( err !== undefined );
						assert.equal( err.message, 'pepe' );
						done();
					});
				});
			});
		});

		describe('get', function() {
			it( 'should work with error', function(done) {
				_setupMock = function(obj) {
					obj.getAll = function() {
						return bPromise.reject( new Error('pepe') );
					};
				};

				init(function() {
					api.get( 'pepeID', function(err,devInfo) {
						assert( err !== undefined );
						assert.equal( err.message, 'pepe' );
						done();
					});
				});
			});

			it( 'should validate device information', function(done) {
				_setupMock = function(obj) {
					obj.getAll = function() {
						return bPromise.resolve( _sbdInfo );
					};
				};

				init(function() {
					api.get( 'pepeID', function(err,devInfo) {
						assert.equal( err, undefined );
						assert.equal( devInfo.id, 'pepeID' );
						assert.equal( devInfo.isReadOnly, false );
						assert.equal( devInfo.isMounted, true );
						assert.equal( devInfo.isSystemDevice, false );
						assert.deepEqual( devInfo.mountPaths, ['/mnt'] );
						done();
					});
				});
			});

			it( 'should fix isSystemDevice if mounted on rootfs', function(done) {
				_setupMock = function(obj) {
					obj.getAll = function() {
						var _info = _.cloneDeep(_sbdInfo);
						_info.DeviceMountPaths = ['/'];
						return bPromise.resolve( _info );
					};
				};

				init(function() {
					api.get( 'pepeID', function(err,devInfo) {
						assert.equal( err, undefined );
						assert.equal( devInfo.id, 'pepeID' );
						assert.equal( devInfo.isReadOnly, false );
						assert.equal( devInfo.isMounted, true );
						assert.equal( devInfo.isSystemDevice, true );
						assert.deepEqual( devInfo.mountPaths, ['/'] );
						done();
					});
				});
			});

			it( 'should check isMounted', function(done) {
				_setupMock = function(obj) {
					obj.getAll = function() {
						var _info = _.cloneDeep(_sbdInfo);
						_info.DeviceIsMounted = false;
						return bPromise.resolve( _info );
					};
				};

				init(function() {
					api.get( 'pepeID', function(err,devInfo) {
						assert.equal( err, undefined );
						assert.equal( devInfo.id, 'pepeID' );
						assert.equal( devInfo.isReadOnly, false );
						assert.equal( devInfo.isMounted, false );
						assert.equal( devInfo.isSystemDevice, false );
						assert.equal( devInfo.mountPaths.length, 0 );
						done();
					});
				});
			});
		});

		describe('signals', function() {
			var dbus = null;
			var diskEvents = null;

			beforeEach(function(done) {
				dbus = null;
				diskEvents = null;

				_setupMock = function(obj) {
					dbus = obj;
					obj.invoke = function( srv, path, iface, func, paramFormat ) {
						if (func === 'EnumerateDevices') {
							return bPromise.resolve(['pepeID']);
						}
						return bPromise.resolve();
					};

					obj.get = function() {
						return bPromise.resolve(true);
					};

					obj.getInterface = function( srv, path, iface, cb ) {
						diskEvents = new DiskIface();
						cb( undefined, diskEvents );
					};
				};

				init(done);
			});

			it( 'should setup changed signal', function(done) {
				api.on( 'changed', function(devID) {
					assert.equal( devID, 'pepeID' );
					done();
				});

				diskEvents.emit( 'DeviceChanged', 'pepeID' );
			});

			it( 'should setup added signal', function(done) {
				api.on( 'added', function(devID) {
					assert.equal( devID, 'pepeID1' );
					done();
				});

				diskEvents.emit( 'DeviceAdded', 'pepeID1' );
			});

			it( 'should setup removed signal', function(done) {
				api.on( 'removed', function(devID) {
					assert.equal( devID, 'pepeID' );
					done();
				});

				diskEvents.emit( 'DeviceRemoved', 'pepeID' );
			});

			describe( 'mount changed signal', function() {
				it( 'should setup mount signal', function(done) {
					var isMounted = true;
					var sendError = undefined;

					dbus.get = function() {
						if (sendError) {
							return bPromise.reject(sendError);
						}
						return bPromise.resolve(isMounted);
					};

					var count = -1
					api.on( 'mountChanged', function(info) {
						count++;
						//console.log( 'mount changed: count=%d, info=%j', count, info );
						if (count === 0) {
							assert.deepEqual( info, { devID: 'pepeID2', isMounted: true } );
							isMounted = false;
							diskEvents.emit( 'DeviceChanged', 'pepeID2' );
						}
						else if (count === 1) {
							assert.deepEqual( info, { devID: 'pepeID2', isMounted: false } );

							//	Send again!
							diskEvents.emit( 'DeviceChanged', 'pepeID2' );

							sendError = new Error('pepe');
							diskEvents.emit( 'DeviceChanged', 'pepeID2' );
							done();
						}
						else {
							assert(false);
						}
					});

					diskEvents.emit( 'DeviceChanged', 'pepeID' );
					diskEvents.emit( 'DeviceChanged', 'pepeID2' );
				});

				it( 'should check mount changed when a device was added/removed', function(done) {
					var isMounted = true;
					var sendError = undefined;
					var count = -1

					dbus.get = function() {
						return bPromise.resolve(isMounted);
					};

					function checkState( step, state ) {
						if (step === 0) {
							assert.equal( state.devID, 'pepeID' );
							assert.equal( state.isMounted, true );

							//	First event
							isMounted=false;
							diskEvents.emit( 'DeviceRemoved', 'pepeID' );
						}
						else if (step === 1) {
							assert.equal( state.devID, 'pepeID' );
							assert.equal( state.isMounted, false );

							//	Add without mount
							isMounted=false;
							diskEvents.emit( 'DeviceAdded', 'pepeID' );

							//	Change mount
							isMounted=true;
							diskEvents.emit( 'DeviceChanged', 'pepeID' );
						}
						else if (step === 2) {
							assert.equal( state.devID, 'pepeID' );
							assert.equal( state.isMounted, true );

							//	Add without mount
							isMounted=false;
							diskEvents.emit( 'DeviceChanged', 'pepeID' );
						}
						else if (step === 3) {
							assert.equal( state.devID, 'pepeID' );
							assert.equal( state.isMounted, false );

							done();
						}
					}

					api.on( 'mountChanged', function(info) {
						count++;
						setTimeout( checkState.bind(undefined,count,info), 2 );
					});

					//	First event
					isMounted=true;
					diskEvents.emit( 'DeviceAdded', 'pepeID' );
				});
			});
		});
	});
});
