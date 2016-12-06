'use strict';

var fs = require('fs');
var EventEmitter = require('events').EventEmitter;
var _ = require("lodash");
var tvdutil = require('tvdutil');

var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;
var mockfs = require('mock-fs');
var Mocks = require('mocks');

var UpdateManager = require('../src/updatemanager');


function backendUrl(p) {
	var configServer = 'http://pepe.com';
	return configServer + p;
}

function serverConfig( p, retry, check ) {
	return {
		'url': backendUrl(p),
		'retryTimeout': retry,
		'checkTimeout': check
	};
}

function defaultConfig( serialID ) {
	return {
		serial: serialID,
		config_server: serverConfig('/config', 5*60*1000, 60*60*1000),
		firmware_update_server: serverConfig( '/update_firmware', 5*60*1000, 60*60*1000 ),
		package_update_server: serverConfig( '/update_package', 5*60*1000, 60*60*1000 ),
		urlCrashServer: backendUrl('/crash'),
		system_packages: []
	};
}

function defaultFirmwareUpdate() {
	return {
        "url": "http://no_existe.com",
        "version": "1.0.0",
		"name": "Name",
        "description": "Es una actualizacion pepe",
		"md5": "algun_md5",
		"size": "algun_size"
    };
}

function defaultPackageUpdate() {
	return {
        "url": "http://no_existe.com",
		"version": "1.0.1",
		"md5": "pepe_md5",
		"size": 10
    };
}

function postResponse(cb,data,st,err) {
	if (!data) {
		st = 404;
	}
	if (!st) {
		st = 200;
	}
	cb( err, {
		"statusCode": st,
		"data": data
	});
}

function BackendMock() {
	var self = {};

	self.signals = {};
	self.cfg = defaultConfig(1);

	self.getFirmwareUpdateConfig = function() {
		return self.cfg.firmware_update_server;
	};
	self.getPackageUpdateConfig = function() {
		return self.cfg.package_update_server;
	};
	self.getCrashServer = function() {
		return self.cfg.crash_server;
	};

	self.on = function( sig, cb ) {
		self.signals[sig] = cb;
	};

	self.removeListener = function( sig ) {
		delete self.signals[sig];
	};

	self.emit = function( sig, data ) {
		self.signals[sig]( data );
	};

	return self;
}

function SystemMock() {
	var self = {};
	self.events = new EventEmitter();

	tvdutil.forwardEventEmitter(self,self.events);

	self.getInfo = function() {
		return {
			id: '1',
			platform: {
				name : 'UNKNOWN'
			},
			system: {
				version: '1.0.0'
			}
		}
	}

	self.sendError = function(err) {
		//console.log( 'sendError: err=%s', err );
	}

	return self;
}

describe('UpdateManager', function() {
	describe('constructor', function() {
		it('should construct a UpdateManager service', function() {
			assert.isObject(new UpdateManager());
		});
	});

	var reg = null;
	var adapter = null;
	var up = null;
	var sys = null;
	var backend = null;

	beforeEach(function() {
		reg = Mocks.init('silly');
		reg.put( 'pkgmgr', new Mocks.PackageManager([]) );

		sys = new SystemMock();
		reg.put('system',sys);

		backend = new BackendMock();
		reg.put('backend',backend);

		adapter = new Mocks.ServiceAdapter( reg, 'ar.edu.unlp.info.lifia.tvd.system' );

		up = new UpdateManager(adapter);
	});

	afterEach(function() {
		Mocks.fin();
		adapter = null;
		up = null;
		sys = null;
		reg = null;
	});

	describe('onStart/onStop', function() {
		it('should start/stop/start', function(done) {
			up.onStart(function(err,api) {
				assert.equal( err );
				assert.isObject( api );
				assert.isFunction( api.on );
				assert.isFunction( api.removeListener );
				assert.isFunction( api.checkFirmwareUpdate );
				assert.isFunction( api.downloadFirmwareUpdate );
				assert.isFunction( api.updateFirmware );

				up.onStop(function() {
					up.onStart(function(err,api) {
						assert.equal( err );
						assert.isObject( api );
						up.onStop(done);
					});
				});
			});
		});
	});

	describe('methods', function() {
		var oldRequest = null;
		var oldFetch = null;

		beforeEach(function() {
			oldRequest = tvdutil.doRequest;
			oldFetch = tvdutil.fetchURL;
		});

		afterEach(function(done) {
			up.onStop(done);
			tvdutil.doRequest = oldRequest;
			tvdutil.fetchURL = oldFetch;
		});

		it('should return web api', function(done) {
			up.onStart(function(err,api) {
				assert.equal( err );
				assert.isObject( api );
				var webApi = api.getWebAPI();
				assert.isObject(webApi);
				assert.strictEqual( webApi.name, 'updatemgr' );
				assert.deepEqual( webApi['public'], ['on'] );
				assert.deepEqual( webApi['private'], ['getStatus','checkFirmwareUpdate','downloadFirmwareUpdate','updateFirmware'] );
				done();
			});
		});

		describe('firwmware', function() {
			it('should check for firmware on start', function(done) {
				backend.getFirmwareUpdateConfig = function() {
					var cfg = _.cloneDeep(defaultConfig(0).firmware_update_server);
					cfg.retryTimeout = 5;
					return cfg;
				};

				tvdutil.doRequest = function( params, callback ) {
					assert.isTrue( params.url === backendUrl('/update_firmware') );
					assert.isTrue( params.data['id'] === "1" );
					assert.isTrue( params.data['platform'] === "UNKNOWN" );
					assert.isTrue( params.data['version'] === "1.0.0" );
					assert.isFunction( callback );
					postResponse(callback,defaultFirmwareUpdate());
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					assert.equal( api.getStatus(), 'idle' );

					var count=0;
					api.on('Firmware',function(evt) {
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available' );
							assert.deepEqual( evt.info, defaultFirmwareUpdate() );
							done();
						}
						count++;
					});
				});
			});

			it('should check for firmware update when idle', function(done) {
				tvdutil.doRequest = function( params, callback ) {
					postResponse(callback,defaultFirmwareUpdate());
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var count=0;
					api.on('Firmware',function(evt) {
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available');
							assert.deepEqual( evt.info, defaultFirmwareUpdate() );
							done();
						}

						count++;
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should check for errors in firmware update', function(done) {
				tvdutil.doRequest = function( params, callback ) {
					postResponse(callback,undefined,404,new Error('Alguno'));
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var count=0;
					api.on('Firmware',function(evt) {
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'idle');
							done();
						}

						count++;
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should manual check for firmware update only when idle', function(done) {
				var count=-1;
				tvdutil.doRequest = function( params, callback ) {
					count++;
					if (count > 0) {
						postResponse(callback,defaultFirmwareUpdate());
					}
					else {
						postResponse(callback,{});
					}
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var st=-1;
					api.on('Firmware',function(evt) {
						st++;
						if (st === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (st === 1) {
							assert.isTrue( evt.status === 'idle' );
							api.checkFirmwareUpdate();
						}
						else if (st === 3) {
							assert.isTrue( evt.status === 'available');
							assert.deepEqual( evt.info, defaultFirmwareUpdate() );
							done();
						}
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should not check for firmware when update is available', function(done) {
				tvdutil.doRequest = function( params, callback ) {
					postResponse(callback,defaultFirmwareUpdate());
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var count=0;
					api.on('Firmware',function(evt) {
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available');
							api.checkFirmwareUpdate();
							done();
						}
						count++;
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should download a firmware update', function(done) {
				tvdutil.fetchURL = function(options,cb) {
					if (options.progress !== undefined) {
						// Simulate progress
						options.progress(50);
						options.progress(100);
					}
					cb();
				};

				tvdutil.doRequest = function( params, callback ) {
					postResponse(callback,defaultFirmwareUpdate());
				}

				var fs = {};
				fs['/system/data/config/ar.edu.unlp.info.lifia.tvd.system/'] = {};
				mockfs(fs);

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var count=-1;
					api.on('Firmware',function(evt) {
						count++;
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available' );
							api.downloadFirmwareUpdate();
						}
						else if (count < 5) {
							assert.strictEqual( evt.status, 'downloading' );
						}
						else if (count === 5) {
							assert.isTrue( evt.status === 'downloaded' );
							assert.isTrue( evt.url.indexOf('/system/data/config/ar.edu.unlp.info.lifia.tvd.system/image_' ) !== -1 );
							mockfs.restore();
							done();
						}
						else if (evt.status === 'error') {
							assert.isTrue(false);
						}
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should check if mktemp fail in a firmware update', function(done) {
				tvdutil.fetchURL = function(options,cb) { cb(); };

				tvdutil.doRequest = function( params, callback ) {
					postResponse(callback,defaultFirmwareUpdate());
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var count=-1;
					api.on('Firmware',function(evt) {
						count++;
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available' );
							api.downloadFirmwareUpdate();
						}
						else if (count === 2) {
							assert.isTrue( evt.status === 'downloading' );
						}
						else if (count === 3) {
							assert.isTrue( evt.status === 'error' );
							assert.isTrue( evt.error.indexOf('ENOENT: no such file or directory, open') >= 0 );
							done();
						}
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should check if status is in error', function(done) {
				tvdutil.fetchURL = function(options,cb) { cb(); };

				tvdutil.doRequest = function( params, callback ) {
					postResponse(callback,defaultFirmwareUpdate());
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var count=-1;
					api.on('Firmware',function(evt) {
						count++;
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available' );
							api.downloadFirmwareUpdate();
						}
						else if (count === 2) {
							assert.isTrue( evt.status === 'downloading' );
						}
						else if (count === 3) {
							assert.isTrue( evt.status === 'error' );
							assert.isTrue( evt.error.indexOf('ENOENT: no such file or directory, open') >= 0 );
							api.checkFirmwareUpdate();
						}
						else if (count === 4) {
							assert.isTrue( evt.status === 'idle' );
						}
						else if (count === 5) {
							assert.isTrue( evt.status === 'checking' );
							done();
						}
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should check if fetchURL fail in a firmware update. Should remove file', function(done) {
				var fsMock = {};
				fsMock['/system/data/config/ar.edu.unlp.info.lifia.tvd.system/'] = {};
				mockfs(fsMock);

				tvdutil.fetchURL = function(options,cb) {
					fs.writeFileSync( options.fileOutput, 'pepe' );
					cb(new Error('alguno'));
				};

				tvdutil.doRequest = function( params, callback ) {
					postResponse(callback,defaultFirmwareUpdate());
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var count=-1;
					api.on('Firmware',function(evt) {
						count++;
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available' );
							api.downloadFirmwareUpdate();
						}
						else if (count === 2) {
							assert.isTrue( evt.status === 'downloading' );
						}
						else if (count === 3) {
							assert.isTrue( evt.status === 'error' );
							assert.isTrue( evt.error.indexOf('alguno') >= 0 );
							var filesInTemp = fs.readdirSync('/system/data/config/ar.edu.unlp.info.lifia.tvd.system');
							assert.isTrue( filesInTemp.length === 0 );
							mockfs.restore();
							done();
						}
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should check, download and apply a mandatory firmware update', function(done) {
				var fs = {};
				fs['/system/data/config/ar.edu.unlp.info.lifia.tvd.system/'] = {};
				mockfs(fs);

				tvdutil.fetchURL = function(options,cb) { cb(); };

				tvdutil.doRequest = function( params, callback ) {
					var firm = _.cloneDeep(defaultFirmwareUpdate());
					firm.mandatory = true;
					postResponse(callback,firm);
				}

				sys.updateFirmware = function( url, now ) {
					assert.isFalse( now );
					assert.isTrue( url.indexOf('/system/data' ) !== -1 );
					mockfs.restore();
					done();
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var count=-1;
					api.on('Firmware',function(evt) {
						count++;
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available' );
						}
						else if (count === 2) {
							assert.isTrue( evt.status === 'downloading' );
						}
						else if (count === 3) {
							assert.isTrue( evt.status === 'downloaded' );
							assert.isTrue( evt.url.indexOf('/system/data/config/ar.edu.unlp.info.lifia.tvd.system/image_' ) !== -1 );
						}
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should ignore download firmware in invalid state', function(done) {
				var fs = {};
				fs['/system/data/config/ar.edu.unlp.info.lifia.tvd.system/'] = {};
				mockfs(fs);

				tvdutil.fetchURL = function(options,cb) { cb(); };

				tvdutil.doRequest = function( params, callback ) {
					postResponse(callback,defaultFirmwareUpdate());
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var count=-1;
					api.on('Firmware',function(evt) {
						count++;
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
							api.downloadFirmwareUpdate();
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available' );
							api.downloadFirmwareUpdate();
						}
						else if (count === 2) {
							assert.isTrue( evt.status === 'downloading' );
							api.downloadFirmwareUpdate();
						}
						else if (count === 3) {
							api.downloadFirmwareUpdate();
							assert.isTrue( evt.status === 'downloaded' );
							mockfs.restore();
							done();
						}
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should ignore checkFirmwareUpdate if no idle', function(done) {
				var fs = {};
				fs['/system/data/config/ar.edu.unlp.info.lifia.tvd.system/'] = {};
				mockfs(fs);

				tvdutil.fetchURL = function(options,cb) { cb(); };

				tvdutil.doRequest = function( params, callback ) {
					postResponse(callback,defaultFirmwareUpdate());
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var count=-1;
					api.on('Firmware',function(evt) {
						count++;
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
							api.checkFirmwareUpdate();
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available' );
							api.checkFirmwareUpdate();
							api.downloadFirmwareUpdate();
						}
						else if (count === 2) {
							assert.isTrue( evt.status === 'downloading' );
							api.checkFirmwareUpdate();
						}
						else if (count === 3) {
							assert.isTrue( evt.status === 'downloaded' );
							api.checkFirmwareUpdate();
							mockfs.restore();
							done();
						}
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should not update firmware if there are no update available', function(done) {
				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					api.on('Firmware',function(evt) {
						assert.isTrue(false);
					});

					api.updateFirmware();
					done();
				});
			});

			it('should call system.powerOff if updateFirmware is called while updating', function(done) {
				var fs = {};
				fs['/system/data/config/ar.edu.unlp.info.lifia.tvd.system/'] = {};
				mockfs(fs);

				tvdutil.fetchURL = function(options,cb) { cb(); };

				tvdutil.doRequest = function( params, callback ) {
					var firm = _.cloneDeep(defaultFirmwareUpdate());
					firm.mandatory = true;
					postResponse(callback,firm);
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					sys.powerOff = function() {
						mockfs.restore();
						done();
					};

					sys.updateFirmware = function( url, now ) {
						assert.isFalse( now );
						assert.isTrue( url.indexOf('/system/data/' ) !== -1 );
						api.updateFirmware();
					}

					var count=-1;
					api.on('Firmware',function(evt) {
						count++;
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available' );
						}
						else if (count === 2) {
							assert.isTrue( evt.status === 'downloading' );
						}
						else if (count === 3) {
							assert.isTrue( evt.status === 'downloaded' );
							assert.isTrue( evt.url.indexOf('/system/data/config/ar.edu.unlp.info.lifia.tvd.system/image_' ) !== -1 );
						}
					});

					api.checkFirmwareUpdate();
				});
			});

			it('should call system.powerOff if onBackendConfigChanged is called while updating', function(done) {
				var fs = {};
				fs['/system/data/config/ar.edu.unlp.info.lifia.tvd.system/'] = {};
				mockfs(fs);

				tvdutil.fetchURL = function(options,cb) { cb(); };

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					tvdutil.doRequest = function( params, callback ) {
						var firm = _.cloneDeep(defaultFirmwareUpdate());
						firm.mandatory = true;
						backend.emit('ConfigChanged', cfg);
						postResponse(callback,firm);
					}

					sys.updateFirmware = function( url, now ) {
						assert.isFalse( now );
						assert.isTrue( url.indexOf('/system/data' ) !== -1 );
						mockfs.restore();
						done();
					}

					var count=-1;
					api.on('Firmware',function(evt) {
						count++;
						if (count === 0) {
							assert.isTrue( evt.status === 'checking' );
						}
						else if (count === 1) {
							assert.isTrue( evt.status === 'available' );
						}
						else if (count === 2) {
							assert.isTrue( evt.status === 'downloading' );
						}
						else if (count === 3) {
							assert.isTrue( evt.status === 'downloaded' );
							assert.isTrue( evt.url.indexOf('/system/data/config/ar.edu.unlp.info.lifia.tvd.system/image_' ) !== -1 );
						}
					});

					var cfg = _.cloneDeep(defaultConfig(0));
					cfg.firmware_update_server.retryTimeout = 100;
					backend.emit('ConfigChanged', cfg);
				});
			});

			it('should update firmware checker when backend configuration changed', function(done) {
				tvdutil.doRequest = function( params, callback ) {
					assert.isTrue( params.url === backendUrl('/update2') );
					postResponse(callback,defaultFirmwareUpdate());
					done();
				}

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					var cfg = _.cloneDeep(defaultConfig(0));
					cfg.firmware_update_server.url = backendUrl('/update2');
					backend.emit('ConfigChanged',cfg);

					api.checkFirmwareUpdate();
				});
			});
		});

		describe('packages', function() {
			it('should ensure system packages when backend configuration changed', function(done) {
				tvdutil.doRequest = function( params, callback ) {
					assert.isTrue( params.url === backendUrl('/update_package') );
					if (params.data.pkgID === 'pepe_name') {
						postResponse(callback,defaultPackageUpdate());
					}
					else {
						postResponse(callback,{});
					}
				}

				var pkgMgr = reg.get('pkgmgr');
				pkgMgr.update = function(opts,cb) {
					assert.isTrue( opts.id === 'pepe_name' );
					cb();
				};

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					api.on('Package',function(evt) {
						assert.isTrue( evt.id === 'pepe_name' );
						done();
					});

					var cfg = _.cloneDeep(defaultConfig(0));
					cfg.system_packages = ['pepe_name'];
					cfg.package_update_server.retryTimeout = 5;
					backend.emit('ConfigChanged',cfg);
				});
			});

			it('should update packages checker when backend configuration changed', function(done) {
				tvdutil.doRequest = function( params, callback ) {
					assert.isTrue( params.url === backendUrl('/update2') );
					postResponse(callback,{});
					done();
				}

				var pkgMgr = reg.get('pkgmgr');
				var app = Mocks.createApp( 'test_app' );
				app.manifest.update = true;
				pkgMgr.setPkgs( [app.manifest] );

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );


					var cfg = _.cloneDeep(defaultConfig(0));
					cfg.package_update_server.url = backendUrl('/update2');
					cfg.package_update_server.retryTimeout = 5;
					backend.emit('ConfigChanged',cfg);
				});
			});

			it('should check for packages update on start', function(done) {
				backend.getPackageUpdateConfig = function() {
					var cfg = _.cloneDeep(defaultConfig(0).package_update_server);
					cfg.retryTimeout = 5;
					return cfg;
				};

				tvdutil.doRequest = function( params, callback ) {
					assert.isTrue( params.url === backendUrl('/update_package') );
					assert.isTrue( params.data['id'] === "1" );
					assert.isTrue( params.data['platform'] === "UNKNOWN" );
					assert.isTrue( params.data['version'] === "1.0.0" );
					assert.isTrue( params.data['pkgID'] === "ar.edu.unlp.info.lifia.tvd.test_app" );
					assert.isTrue( params.data['pkg-version'] === "1.0.0" );
					assert.isFunction( callback );

					postResponse(callback,defaultPackageUpdate());
				}

				var pkgMgr = reg.get('pkgmgr');
				var app = Mocks.createApp( 'test_app' );
				app.manifest.update = true;
				pkgMgr.setPkgs( [app.manifest] );

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					api.on('Package',function(evt) {
						done();
					});
				});
			});

			it('should check for packages update after package install', function(done) {
				backend.getPackageUpdateConfig = function() {
					var cfg = _.cloneDeep(defaultConfig(0).package_update_server);
					cfg.retryTimeout = 5;
					return cfg;
				};

				tvdutil.doRequest = function( params, callback ) {
					assert.isTrue( params.url === backendUrl('/update_package') );
					assert.isTrue( params.data['id'] === "1" );
					assert.isTrue( params.data['platform'] === "UNKNOWN" );
					assert.isTrue( params.data['version'] === "1.0.0" );
					assert.isTrue( params.data['pkgID'] === "ar.edu.unlp.info.lifia.tvd.test_app" );
					assert.isTrue( params.data['pkg-version'] === "1.0.0" );
					assert.isFunction( callback );

					postResponse(callback,defaultPackageUpdate());
				};

				var pkgMgr = reg.get('pkgmgr');
				var app = Mocks.createApp( 'test_app' );
				pkgMgr.setPkgs( [app.manifest] );

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					app.manifest.update = true;
					api.on('Package',function(/*evt*/) {
						done();
					});
					pkgMgr.install({id: "ar.edu.unlp.info.lifia.tvd.test_app"}, function() {});
				});
			});

			it('should emit error if package update fail', function(done) {
				backend.getPackageUpdateConfig = function() {
					var cfg = _.cloneDeep(defaultConfig(0).package_update_server);
					cfg.retryTimeout = 5;
					return cfg;
				};

				tvdutil.doRequest = function( params, callback ) {
					assert.isTrue( params.url === backendUrl('/update_package') );
					assert.isTrue( params.data['id'] === "1" );
					assert.isTrue( params.data['platform'] === "UNKNOWN" );
					assert.isTrue( params.data['version'] === "1.0.0" );
					assert.isTrue( params.data['pkgID'] === "ar.edu.unlp.info.lifia.tvd.test_app" );
					assert.isTrue( params.data['pkg-version'] === "1.0.0" );
					assert.isFunction( callback );

					postResponse(callback,defaultPackageUpdate());
				};

				var pkgMgr = reg.get('pkgmgr');
				var app = Mocks.createApp( 'test_app' );
				pkgMgr.setPkgs( [app.manifest] );

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					app.manifest.update = true;
					api.on('Package',function(evt) {
						if (evt.error) {
							assert.strictEqual(evt.error.message, 'Package update fail');
						}
					});
					sys.sendError = function(err) {
						assert.strictEqual(err.message, 'Package update fail');
						done();
					};

					pkgMgr.install({id: "ar.edu.unlp.info.lifia.tvd.test_app"}, function() {});
					pkgMgr.on('update', function() {
						throw new Error('Package update fail');
					});
				});
			});

			it('should not check for packages update after package install if not has update attribute', function(done) {
				backend.getPackageUpdateConfig = function() {
					var cfg = _.cloneDeep(defaultConfig(0).package_update_server);
					cfg.retryTimeout = 5;
					return cfg;
				};

				tvdutil.doRequest = function( /*params, callback*/ ) {
					done(new Error('Should not chec for package update'));
				};

				var pkgMgr = reg.get('pkgmgr');
				var app = Mocks.createApp( 'test_app' );
				pkgMgr.setPkgs( [app.manifest] );

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					pkgMgr.install({id: "ar.edu.unlp.info.lifia.tvd.test_app"}, function() {});
					done();
				});
			});

			it('should check if a request is in porgres after package uninstall', function(done) {
				sys.getBackendConfig = function() {
					var cfg = _.cloneDeep(defaultConfig(0));
					cfg.package_update_server.retryTimeout = 5;
					return cfg;
				};

				tvdutil.doRequest = function( /*params, callback*/ ) {
					done(new Error('Should not chec for package update'));
				};

				var pkgMgr = reg.get('pkgmgr');
				var app = Mocks.createApp( 'test_app' );
				pkgMgr.setPkgs( [app.manifest] );

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					pkgMgr.uninstall("ar.edu.unlp.info.lifia.tvd.test_app", function(id) {
						assert.strictEqual( id, "ar.edu.unlp.info.lifia.tvd.test_app");
						done()
					});
				});
			});

			it('should cancel in porgres request after package uninstall', function(done) {
				backend.getPackageUpdateConfig = function() {
					var cfg = _.cloneDeep(defaultConfig(0).package_update_server);
					cfg.retryTimeout = 5;
					return cfg;
				};

				tvdutil.doRequest = function( params, callback ) {
					assert.isTrue( params.url === backendUrl('/update_package') );
					assert.isTrue( params.data['id'] === "1" );
					assert.isTrue( params.data['platform'] === "UNKNOWN" );
					assert.isTrue( params.data['version'] === "1.0.0" );
					assert.isTrue( params.data['pkgID'] === "ar.edu.unlp.info.lifia.tvd.test_app" );
					assert.isTrue( params.data['pkg-version'] === "1.0.0" );
					assert.isFunction( callback );

					process.nextTick(function() {
						postResponse(callback,defaultPackageUpdate());
					});
				};

				var pkgMgr = reg.get('pkgmgr');
				var app = Mocks.createApp( 'test_app' );
				app.manifest.update = true;
				pkgMgr.setPkgs( [app.manifest] );

				up.onStart(function(err,api) {
					assert.equal( err );
					assert.isObject( api );

					pkgMgr.uninstall("ar.edu.unlp.info.lifia.tvd.test_app", function(id) {
						assert.strictEqual( id, "ar.edu.unlp.info.lifia.tvd.test_app");
						done()
					});
				});
			});
		});
	});
});

