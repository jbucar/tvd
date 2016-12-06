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

var Backend = require('../src/backend');

describe('Backend', function() {
	var reg = null;
	var adapter = null;
	var sys = null;

	beforeEach(function() {
		reg = Mocks.init('silly');
		reg.put( 'pkgmgr', new Mocks.PackageManager([]) );

		sys = new Mocks.System();
		reg.put('system',sys);

		adapter = new Mocks.ServiceAdapter( reg, 'ar.edu.unlp.info.lifia.tvd.system' );
		reg.put( 'platform', new Mocks.Factory() );

		reg.put( 'appmgr', new Mocks.ApplicationManager() );
	});

	afterEach(function() {
		Mocks.fin();
		adapter = null;
		sys = null;
		reg = null;
	});

	describe('onStart/onStop', function() {
		it('should start/stop/start', function(done) {
			var backend = new Backend(adapter);

			backend.onStart(function(err,api) {
				assert.equal( err );
				assert.isObject( api );

				backend.onStop(function() {
					backend.onStart(function(err,api) {
						assert.equal( err );
						assert.isObject( api );
						backend.onStop(done);
					});
				});
			});
		});
	});

	describe('api', function() {
		var api = null;
		var oldRequest = null;
		var backend = null;

		beforeEach(function(done) {
			oldRequest = tvdutil.doRequest;

			backend = new Backend(adapter);
			backend.onStart(function(err,impl) {
				assert.equal( err );
				assert.isObject( impl );
				api = impl;
				done();
			});
		});

		afterEach(function( done ) {
			backend.onStop(function() {
				tvdutil.doRequest = oldRequest;
				done();
			});
		});

		it( 'check API', function() {
			assert.isFunction( api.on );
			assert.isFunction( api.removeListener );
			assert.isFunction( api.getFirmwareUpdateConfig );
			assert.isFunction( api.getPackageUpdateConfig );
			assert.isFunction( api.getCrashServer );
		});

		it( 'getFirmwareUpdateconfig', function() {
			assert.isFunction( api.getFirmwareUpdateConfig );
			var cfg = api.getFirmwareUpdateConfig();
			assert.equal( cfg.url, "http://www.pepe.com/firmware/update" );
			assert.equal( cfg.retryTimeout, 300000 );
			assert.equal( cfg.checkTimeout, 3600000 );
		});

		it( 'getPackageUpdateconfig', function() {
			assert.isFunction( api.getPackageUpdateConfig );
			var cfg = api.getPackageUpdateConfig();
			assert.equal( cfg.url, "http://www.pepe.com/package/update" );
			assert.equal( cfg.retryTimeout, 300000 );
			assert.equal( cfg.checkTimeout, 7200000 );
		});

		it( 'getPackageUpdateconfig', function() {
			assert.isFunction( api.getCrashServer );
			assert.equal( api.getCrashServer(), "http://www.pepe.com/tacerror" );
		});
	});

	function backendUrl(p) {
		var configServer = 'http://pepe.com';

		if (!p) {
			p = '/config';
		}
		return configServer + p;
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

	describe('report error', function() {
		var api = null;
		var oldRequest = null;
		var backend = null;

		function createBackend(done) {
			backend = new Backend(adapter);
			backend.onStart(function(err,impl) {
				assert.equal( err );
				assert.isObject( impl );
				api = impl;
				done();
			});
		}

		beforeEach(function() {
			oldRequest = tvdutil.doRequest;
		});

		afterEach(function( done ) {
			backend.onStop(function() {
				tvdutil.doRequest = oldRequest;
				done();
			});
		});

		it( 'should report error', function(done) {
			createBackend(function() {
				var calls = 0;
				var crashInfo = { 'crashInfo': 'ok' };

				tvdutil.doRequest = function( params, callback ) {
					calls++;
					assert.isTrue( params.url === 'http://www.pepe.com/tacerror' );
					assert.equal( params.data, crashInfo );

					assert.isFunction( callback );
					postResponse(callback,{},200);
					done();
				};

				sys.sendReport( crashInfo);
			});
		});

		it( 'should handle invalid status code when send error', function(done) {
			createBackend(function() {
				var calls = 0;
				var crashInfo = { 'crashInfo': 'ok' };

				tvdutil.doRequest = function( params, callback ) {
					calls++;
					assert.isTrue( params.url === 'http://www.pepe.com/tacerror' );
					assert.equal( params.data, crashInfo );

					assert.isFunction( callback );
					postResponse(callback);
					done();
				};

				sys.sendReport( crashInfo );
			});
		});

		it( 'should handle error when send error', function(done) {
			createBackend(function() {
				var calls = 0;
				var crashInfo = { 'crashInfo': 'ok' };

				tvdutil.doRequest = function( params, callback ) {
					calls++;
					assert.isTrue( params.url === 'http://www.pepe.com/tacerror' );
					assert.equal( params.data, crashInfo );

					assert.isFunction( callback );
					postResponse(callback,{},200,new Error('pepe'));
					done();
				};

				sys.sendReport( crashInfo );
			});
		});

		it( 'should not report error if crash url not defined', function(done) {
			adapter = new Mocks.ServiceAdapter( reg );
			adapter.load = function( file, defaultsValues ) {
				defaultsValues.config_server.url = backendUrl();
				defaultsValues.urlCrashServer = undefined;
				return defaultsValues;
			};
			createBackend(function() {
				sys.sendReport( {} );
				done();
			});
		});
	});

	describe('store', function() {
		var backend = null;
		var api = null;

		function createBackend(done) {
			backend = new Backend(adapter);
			backend.onStart(function(err,impl) {
				assert.equal( err );
				assert.isObject( impl );
				api = impl;
				done();
			});
		}

		afterEach(function( done ) {
			backend.onStop(function() {
				done();
			});
		});

		it( 'check release store', function(done) {
			var plat = reg.get('platform');
			var oldConfig = plat.getConfig;
			plat.getConfig = function(key) {
				if (key === 'build') {
					return {
						type: 'release'
					};
				}
				return oldConfig(key);
			};

			createBackend(function() {
				var storeID = 'ar.edu.unq.utics.tac.tienda';
				var appMgr = reg.get('appmgr');
				var app = appMgr.get( storeID );
				assert( app );
				assert.equal( app.main,  'http://tiendatac.minplan.gob.ar/tienda' );
				done();
			});
		});

		it( 'check debug store', function(done) {
			var plat = reg.get('platform');
			var oldConfig = plat.getConfig;
			plat.getConfig = function(key) {
				if (key === 'build') {
					return {
						type: 'debug'
					};
				}
				return oldConfig(key);
			};

			createBackend(function() {
				var storeID = 'ar.edu.unq.utics.tac.tienda';
				var appMgr = reg.get('appmgr');
				var app = appMgr.get( storeID );
				assert( app );
				assert.equal( app.main,  'http://tiendatac_dev.minplan.gob.ar/tienda' );
				done();
			});
		});
	});

	describe('backend', function() {
		var backend = null;
		var oldRequest = null;

		beforeEach(function() {
			oldRequest = tvdutil.doRequest;
			adapter = new Mocks.ServiceAdapter( reg );
			adapter.load = function( file, defaultsValues ) {
				defaultsValues.config_server.url = backendUrl();
				defaultsValues.config_server.retryTimeout = 5;
				return defaultsValues;
			};
			backend = new Backend(adapter);
		});

		afterEach(function( done ) {
			backend.onStop(function() {
				tvdutil.doRequest = oldRequest;
				done();
			});
		});

		function defaultConfig( serialID ) {
			return {
				serial: serialID,
				log: {
					enabled: false,
					maxHistorySize: 1000,
					maxWidth: null, // Disable maxWidth
					level: 'info',
					port:3045
				},
				config_server: {
					'url': backendUrl(),
					'retryTimeout': 5,	//	Changed from original to force request config
					'checkTimeout': 60*60*1000	//	hours
				},
				firmware_update_server: {
					'url': backendUrl('/update_firmware'),
					'retryTimeout': 5,	//	Changed from original to force request config
					'checkTimeout': 60*60*1000	//	hours
				},
				package_update_server: {
					'url': backendUrl('/update_package'),
					'retryTimeout': 5,	//	Changed from original to force request config
					'checkTimeout': 60*60*1000	//	hours
				},
				urlCrashServer: 'http://172.16.0.206:3044/crash',
				backendtem_packages: []
			};
		}

		it('should request config to backend with defaults values', function(done) {
			var calls=0;
			tvdutil.doRequest = function( params, callback ) {
				calls++;
				assert.isTrue( params.url === backendUrl() );
				assert.isTrue( params.data.id === "1" );
				assert.isTrue( params.data.platform === "Test" );
				assert.isTrue( params.data.version === "1.0.XXXX" );
				assert.isFunction( callback );
				var cfg = _.cloneDeep(defaultConfig(1));
				postResponse(callback,cfg);
			};

			backend.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				api.on('ConfigChanged', function(cfg) {
					assert.isObject( cfg );
					assert.deepEqual( cfg, defaultConfig(1) );
					done();
				});
			});
		});

		it('should request config to backend and handle error', function(done) {
			var calls=0;
			tvdutil.doRequest = function( params, callback ) {
				calls++;
				postResponse(callback,{},201,new Error('pepe'));
				done();
			};

			backend.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
			});
		});

		it('should request config to backend and handle not found', function(done) {
			var calls=0;
			tvdutil.doRequest = function( params, callback ) {
				calls++;
				postResponse(callback,{},404);
				done();
			};

			backend.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
			});
		});

		it('should request config and change backend url', function(done) {
			var calls=0;
			tvdutil.doRequest = function( params, callback ) {
				calls++;
				if (calls === 1) {	//	Return config2
					var cfg = _.cloneDeep(defaultConfig(1));
					cfg.config_server.url = backendUrl('/config2');
					cfg.config_server.checkTimeout = 10;
					postResponse(callback,cfg);
				}
				else if (calls === 2) {
					assert.isTrue( params.url === backendUrl('/config2') );
					postResponse(callback);
					done();
				}
			};

			backend.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
			});
		});

		it('should request config and validate serial', function(done) {
			var calls=0;
			tvdutil.doRequest = function( params, callback ) {
				calls++;
				if (calls === 1) {	//	Return config2
					var cfg = _.cloneDeep(defaultConfig(1));
					cfg.config_server.url = backendUrl('/config2');
					cfg.config_server.checkTimeout = 10;
					postResponse(callback,cfg);
				}
				else if (calls === 2) {
					var cfg2 = _.cloneDeep(defaultConfig(1));
					cfg2.config_server.url = backendUrl('/config3');
					postResponse(callback,cfg2);
				}
				else if (calls === 3) {
					assert.isTrue( params.url === backendUrl('/config2') );
					done();
				}
			};

			backend.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
			});
		});
	});
});

