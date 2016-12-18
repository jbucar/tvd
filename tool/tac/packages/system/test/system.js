'use strict';

var tvdutil = require('tvdutil');
var _ = require('lodash');
var fs = require('fs');
var mockfs = require('mock-fs');
var http = require('http');
var nock = require('nock');
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;
var path = require('path');
var stream = require('stream');
var Mocks = require('mocks');
const util = require('util');

var System = require('../src/system');

var url = 'http://localhost:3045';

var testID = 'ar.edu.unlp.info.lifia.tvd.test';
var testComponent = {
	"id": testID,
	"pkgID": testID,
	"name": "Test app",
	"type": "app",
	"version": "1.0.0",
	"main": "test.html",
};

var testPackage = {
	"id": testID,
	"version": "1.0.0",
	"vendor": "LIFIA",
	"vendor-url": "http://lifia.info.unlp.edu.ar/",
	"api-level": "1.0",
	"platform": "all",
	"system": true,
	"update": false,
	"components": [ testComponent ]
};

function testLogDown(done) {
	nock.enableNetConnect();
	http.get(url, function() {}).on('error', function(e) {
		assert.equal("connect ECONNREFUSED 127.0.0.1:3045", e.message);
		done();
	});
	nock.disableNetConnect();
}

describe('System', function() {
	describe('constructor', function() {
		it('should construct a System service', function() {
			assert.isObject(new System());
		});
	});

	var reg = null;
	var adapter = null;
	var sys = null;

	beforeEach(function() {
		reg = Mocks.init('silly');
		reg.put( 'pkgmgr', new Mocks.PackageManager(_.cloneDeep([testPackage])) );
		reg.put( 'srvmgr', new Mocks.ServiceManager() );
		reg.put( 'platform', new Mocks.Factory() );
	});

	afterEach(function() {
		reg = null;
		Mocks.fin();
	});

	describe('onLoad', function() {
		it('should construct system service', function() {
			var adapter = new Mocks.ServiceAdapter( reg );
			assert.isObject( new System(adapter) );
		});
	});

	describe('onStart', function() {
		beforeEach(function() {
			adapter = new Mocks.ServiceAdapter( reg );
			sys = new System(adapter);
		});

		afterEach(function( done ) {
			sys.onStop( done );
		});

		it('should start/stop/start', function(done) {
			sys.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				sys.onStop( function() {
					sys.onStart(function(err,api) {
						assert.equal(err);
						assert.isObject(api);
						done();
					});
				});
			});
		});

		it('should call to convert ol config', function(done) {
			adapter.load = function( file, defaultsValues, convert ) {
				assert( convert );
				var cfg = _.cloneDeep(defaultsValues);
				var newCfg = convert(cfg);
				assert.deepEqual( cfg, newCfg );
				return cfg;
			};

			sys.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				sys.onStop( function() {
					done();
				});
			});
		});

		it('convert should add geolocationEnabled field to old config', function(done) {
			adapter.load = function( file, defaultsValues, convert ) {
				assert( convert );
				assert.isTrue( defaultsValues.geolocationEnabled );
				var cfg = _.omit( defaultsValues, 'geolocationEnabled' );
				assert.isUndefined( cfg.geolocationEnabled );
				var newCfg = convert( cfg );
				assert.deepEqual( defaultsValues, newCfg );
				return newCfg;
			};

			sys.onStart(function(err,api) {
				assert.isUndefined(err);
				assert.isObject(api);
				sys.onStop(done);
			});
		});

		it('should return web api', function(done) {
			sys.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);

				assert.deepEqual(api.getWebAPI(), {
					'name': 'system',
					'public': ['getInfo', 'getRemotePort', 'getLogConfig', 'sendError', 'isFirstBoot', 'isGeolocationEnabled', 'on'],
					'private': [
						'powerOff', 'reboot', 'factoryReset', 'licenseAccepted', 'updateFirmware',
						'setLogConfig', 'isDevelopmentMode', 'disableDevelopmentMode',
						'enableGeolocation'
					]
				});

				done();
			});
		});

		function introspection( api, info ) {
			console.log( 'intro: info=%j', info );
			let meta = {
				name: info.name,
				description: info.description,
				methods:[],
				properties: []
			};
			for (let propName in api) {
				if (propName === '_meta') {
					meta.name = api['_meta'].name;
					meta.desc = api['_meta'].desc;
				}
				else if (typeof api[propName] === 'function') {
					meta.methods.push({
						name: propName,
						description: api[propName]._description
					});
				}
			}
			return meta;
		}
	});

	describe('onStop', function() {
		beforeEach(function() {
			adapter = new Mocks.ServiceAdapter( reg );
			sys = new System(adapter);
		});

		it('should stop the log server', function(done) {
			sys.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				sys.onStop( function() {
					testLogDown(done);
				});
			});
		});

		it('should do nothing when is called twice', function(done) {
			sys.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				sys.onStop( function() {
					sys.onStop( done );
				});
			});
		});
	});

	describe('API', function() {
		function createAPI(cb,load) {
			adapter = new Mocks.ServiceAdapter( reg, 'ar.edu.unlp.info.lifia.tvd.system' );
			if (load) {
				adapter.load = load;
			}
			adapter.saveCalled = false;
			adapter.save = function() {
				adapter.saveCalled = true;
			};
			sys = new System(adapter);
			sys.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				cb(api);
			});
		}

		afterEach(function(done) {
			sys.onStop( done );
		});

		describe('developmentMode', function() {
			var api = null;

			it('should not be in development mode by default', function(done) {
				createAPI(function(api) {
					assert.isFalse(tac.inDevelopmentMode());
					assert.isFalse(api.isDevelopmentMode());
					done();
				});
			});

			it('should load enabled from stored config', function(done) {
				var customLoad = function( file, defaultsValues ) {
					defaultsValues.inDevelopmentMode = true;
					return defaultsValues;
				}
				createAPI(function(api) {
					assert.isTrue(api.isDevelopmentMode());
					assert.isTrue(tac.inDevelopmentMode());
					done();
				}, customLoad );
			});

			it('should enable/disable development mode', function(done) {
				createAPI(function(api) {
					assert.isFalse(tac.inDevelopmentMode());
					assert.isFalse(api.isDevelopmentMode());
					api.enableDevelopmentMode();
					assert.isTrue(tac.inDevelopmentMode());
					assert.isTrue(api.isDevelopmentMode());
					api.disableDevelopmentMode();
					assert.isFalse(tac.inDevelopmentMode());
					assert.isFalse(api.isDevelopmentMode());
					done();
				});
			});

			it('should signal enable/disable development mode', function(done) {
				createAPI(function(api) {
					var called = 0;
					api.on('DevelopmentModeEnabled', function(isEnabled) {
						assert.isTrue(isEnabled);
						called++;
					});
					assert.isFalse(api.isDevelopmentMode());
					api.enableDevelopmentMode();
					assert.equal(called, 1);
					api.enableDevelopmentMode();
					assert.equal(called, 1);
					done();
				});
			});
		});

		describe('geolocation', function() {
			var api = null;

			beforeEach(function(done) {
				createAPI(function(impl) {
					api = impl;
					done();
				});
			});

			it('should be enabled by default', function() {
				assert.isTrue( api.isGeolocationEnabled() );
			});

			it('should enable/disable geolocation', function() {
				assert.isTrue( api.isGeolocationEnabled() );
				api.enableGeolocation( false );
				assert.isFalse( api.isGeolocationEnabled() );
				api.enableGeolocation( true );
				assert.isTrue( api.isGeolocationEnabled() );
			});

			it('should not do anything if already enabled', function() {
				assert.isTrue( api.isGeolocationEnabled() );
				api.enableGeolocation( true );
				assert.isTrue( api.isGeolocationEnabled() );
			});
		});

		describe('getRemotePort', function() {
			it('should get default values', function(done) {
				createAPI(function(api) {
					assert.equal( api.getRemotePort(), 2000 );
					done();
				});
			});
		});

		describe('getInfo', function() {
			it('should get default values', function(done) {
				createAPI(function(api) {
					var info = api.getInfo();
					assert.isObject( info );
					assert.equal( info.system.api, 1 );
					assert.equal( info.system.version, '1.0.0' );
					assert.equal( info.system.commit, 'na' );
					assert.equal( info.platform.name, 'UNKNOWN' );
					assert.equal( info.platform.version, '0.0.0' );
					assert.equal( info.platform.commit, '0' );
					assert.equal( info.id, '1' );
					assert.deepEqual( info.build, {
						name:'Desarrollo',
						type: 'debug',
						config: 'dev',
						file: 'dev.conf'
					});
					done();
				});
			});

			it('should get platform from file /etc/platform.version', function(done) {
				tac.platform.name = 'AML_ISDBT_1';
				tac.platform.version = '1.0.296';
				tac.platform.commit = 'g989fdf2';
				createAPI(function(api) {
					var info = api.getInfo();
					assert.isObject( info );
					assert.equal( info.platform.name, 'AML_ISDBT_1' );
					assert.equal( info.platform.version, '1.0.296' );
					assert.equal( info.platform.commit, 'g989fdf2' );
					done();
				});
			});

			it('should get id from enviroment TAC_SERIAL', function(done) {
				tac.serial = 'pepe';
				createAPI(function(api) {
					var info = api.getInfo();
					assert.isObject( info );
					assert.equal( info.id, 'pepe' );
					done();
				});
			});
		});

		describe('log', function() {
			var api = null;

			beforeEach(function(done) {
				createAPI(function(impl) {
					api = impl;
					done();
				});
			});

			it('should return a copy the config', function() {
				var cfg = api.getLogConfig();
				cfg.enabled = true;
				assert.isTrue( cfg !== api.getLogConfig() );
			});

			it('should save the new config', function() {
				var cfg = api.getLogConfig();
				cfg.level = 'silly';
				cfg.maxHistorySize = 150;
				cfg.maxWidth = 80;

				var count=0;
				api.on( 'LogConfigChanged', function(cfg) {
					count++;
					assert.equal( cfg.level, 'silly' );
				});

				assert.isTrue( api.setLogConfig( cfg ) );
				assert.isTrue(adapter.saveCalled);
				assert.equal( count, 1 );
			});

			it('should not save invalid config', function() {
				var cfg = api.getLogConfig();
				cfg.level = 'pepe';
				assert.isFalse( api.setLogConfig( cfg ) );
				assert.isFalse(adapter.saveCalled);
			});

			it('should return false if adapter fails to save config', function() {
				var saveCalled = false;
				adapter.save = function() {
					saveCalled = true;
					throw new Error();
				};
				var cfg = api.getLogConfig();
				assert.isFalse( api.setLogConfig(cfg) );
				assert.isTrue( saveCalled );
			});
		});

		describe('update', function() {
			var api = null;
			var oldRequest = null;

			beforeEach(function(done) {
				oldRequest = tvdutil.doRequest;
				createAPI(function(impl) {
					api = impl;
					done();
				});
			});

			afterEach(function() {
				tvdutil.doRequest = oldRequest;
			});

			it('should update now using default', function(done) {
				tac.updateFirmware = function(url,now) {
					assert.equal(url,'http://alguna');
					assert.equal(now,true);
					done();
				};
				api.updateFirmware('http://alguna');
			});

			it('should update now with explicit param', function(done) {
				tac.updateFirmware = function(url,now) {
					assert.equal(url,'http://alguna');
					assert.equal(now,true);
					done();
				};
				api.updateFirmware('http://alguna', true);
			});
		});

		describe('generic', function() {
			var api = null;

			beforeEach(function(done) {
				createAPI(function(impl) {
					api = impl;
					done();
				});
			});

			it( 'should accept license', function() {
				assert.equal( api.isFirstBoot(), true );
				api.licenseAccepted();
				assert.equal( api.isFirstBoot(), false );
			});

			it('should powerOff',function(done) {
				tac.powerOff = function() {
					done();
				};
				api.powerOff();
			});

			it('should reboot',function(done) {
				tac.reboot = function() {
					done();
				};
				api.reboot();
			});

			it('should factory reset',function(done) {
				tac.factoryReset = function() {
					done();
				};
				api.factoryReset();
			});
		});

		describe('sendError', function() {
			var api = null;

			it('should send the core dump when start', function(done) {
				var fs = {};
				fs['/system/data/config/ar.edu.unlp.info.lifia.tvd.system/tac.trace'] = 'trace_data';
				mockfs(fs);
				createAPI(function(impl) {
					api = impl;
					var reports = [];
					var onReport = function(crash) {
						reports.push(crash);
					}
					api.setReportCallback( onReport ).then(function() {
						assert.equal( reports.length, 1 );
						var report = reports[0];

						assert.isObject(report);
						assert.equal(report.stack,'trace_data');

						mockfs.restore();
						done();
					});
				});
			});

			it('should send enqueued errors when start', function(done) {
				createAPI(function(impl) {
					api = impl;
					var reports = [];
					var onReport = function(crash) {
						reports.push(crash);
					}
					api.sendError( new Error('Some error') );
					api.setReportCallback( onReport ).then(function() {
						assert.equal( reports.length, 1 );
						api.setReportCallback().then(done);
					});
				});
			});

			it('should write errors if inDevelopmentMode', function(done) {
				var customLoad = function( file, defaultsValues ) {
					defaultsValues.inDevelopmentMode = true;
					return defaultsValues;
				}
				let pkgdir = '/system/data/config/ar.edu.unlp.info.lifia.tvd.system/'
				var mockFS = {};
				mockFS[pkgdir] = {};
				mockfs(mockFS);
				createAPI(function(api) {
					api.sendError( new Error('Some error') ).then(function() {
						let dirs = fs.readdirSync( path.join(pkgdir,'crashes') );
						assert.equal( dirs.length, 1 );
						mockfs.restore();
						done();
					});
				}, customLoad );
			});

			describe( 'runtime', function() {
				var reports = [];
				var onReport = function(crash) {
					reports.push(crash);
				}

				beforeEach(function(done) {
					createAPI(function(impl) {
						api = impl;
						reports = [];
						api.setReportCallback( onReport ).then(done);
					});
				});

				it('should fail when no error is provided', function() {
					let fnc = function() {
						api.sendError();
					};
					assert.throw(fnc,Error,'sendError: err is not a Error');
				});

				it('should fail when string is provided', function() {
					let fnc = function() {
						api.sendError('some error as string')
					};
					assert.throw(fnc,Error,'sendError: err is not a Error');
				});

				it('should ok when is a Error', function(done) {
					api.sendError(new Error('algo')).then(function() {

						assert.equal( reports.length, 1 );
						var report = reports[0];

						assert.equal( report.message, 'algo' );
						assert( report.stack.length > 0 );

						assert.isObject( report.systemInfo );
						assert( report.systemInfo.id );
						assert( report.systemInfo.platform );
						assert( report.systemInfo.system );
						assert( report.systemInfo.build );

						assert.isObject( report.packages );
						assert( Object.keys(report.packages).length > 0 );

						assert.isObject( report.services );
						assert( Object.keys(report.services).length === 0 );

						done();
					});
				});

				it('should ok when is a RecoveryError', function(done) {
					api.sendError(new RecoveryError('algo')).then(function() {
						assert.equal( reports.length, 1 );
						var report = reports[0];

						assert.equal( report.message, 'algo' );

						done();
					});
				});

				it('should send the stacktrace', function() {
					api.sendError(new Error("Some error"));

					assert.equal( reports.length, 1 );
					var report = reports[0];

					assert.isObject(report);
					assert( report.stack.length > 0 );
					assert.match(report.stack, /^Error: Some error*/);
				});

				it('should send last log records', function() {
					log.info( 'test', 'Test1' );
					log.silly( 'test', 'Filtered' );
					log.error( 'test', 'Test2' );
					log.warn( 'test', 'Test3' );
					api.sendError(new Error("Some error"));

					assert.equal( reports.length, 1 );
					var crashReport = reports[0];

					assert.isObject(crashReport);
					var count = crashReport.log.length;
					assert.isTrue(count > 4);
					assert.equal( crashReport.log[count-2], '[warn] test: Test3' );
					assert.equal( crashReport.log[count-3], '[error] test: Test2' );
					assert.equal( crashReport.log[count-4], '[info] test: Test1' );
				});

				it('should send error if pkgmgr not exists', function() {
					reg.swap('pkgmgr', undefined);

					api.sendError(new Error("Some error"));

					assert.equal( reports.length, 1 );
					var crashReport = reports[0];

					assert.isObject(crashReport);
					assert.isObject( crashReport.packages );
					assert.equal( Object.keys(crashReport.packages).length, 0 );
					assert.match(crashReport.stack, /^Error: Some error*/);
				});

				it('should send error if srvmgr not exists', function() {
					reg.swap('srvmgr', undefined);

					api.sendError(new Error("Some error"));
					assert.equal( reports.length, 1 );
					var crashReport = reports[0];

					assert.isObject(crashReport);
					assert.isObject( crashReport.services );
					assert.equal( Object.keys(crashReport.services).length, 0 );
					assert.equal(crashReport.message, 'Some error');
				});

				it('should send error if pkgmgr and srvmgr not exists', function() {
					reg.swap('pkgmgr', undefined);
					reg.swap('srvmgr', undefined);

					api.sendError(new Error("Some error"));
					assert.equal( reports.length, 1 );
					var crashReport = reports[0];

					assert.isObject(crashReport);
					assert.isObject( crashReport.packages );
					assert.equal( Object.keys(crashReport.packages).length, 0 );
					assert.isObject( crashReport.services );
					assert.equal( Object.keys(crashReport.services).length, 0 );
					assert.match(crashReport.stack, /^Error: Some error*/);
				});

				it('should send a list with the packages installed', function() {
					var pkgsList = {
						'ar.edu.unlp.info.lifia.tvd.test': '1.0.0'
					};

					api.sendError(new Error("Some error"));
					assert.equal( reports.length, 1 );
					var crashReport = reports[0];

					assert.isObject(crashReport);
					assert.isObject(crashReport.packages);
					assert.deepEqual(crashReport.packages, pkgsList);
				});

				it('should send the info of all the running services', function() {
					var dump = {
						"dump": "dump",
						"otro": "otro"
					};
					var id = "ar.edu.unlp.info.lifia.tvd.browser";
					var result = {};
					result[id] = dump;

					var srvMgr = reg.get('srvmgr');
					srvMgr.run( id );
					srvMgr.dump = function(/*id*/) {
						return dump;
					};

					api.sendError(new Error("Some error"));
					assert.equal( reports.length, 1 );
					var crashReport = reports[0];

					assert.isObject(crashReport.services);
					assert.deepEqual(crashReport.services, result);
				});

				it('should send the system identification', function() {
					var systemInfo = {
						'platform': {
							name: 'UNKNOWN',
							version: '0.0.0',
							commit: '0'
						},
						'system': {
							api: 1,
							version: '1.0.0',
							commit: 'na'
						},
						'id': '1',
						'build': {
							name:'Desarrollo',
							type: 'debug',
							config: 'dev',
							file: 'dev.conf'
						}
					};

					api.sendError(new Error("Some error"));
					assert.equal( reports.length, 1 );
					var crashReport = reports[0];

					assert.isObject(crashReport.systemInfo);
					assert.deepEqual(crashReport.systemInfo, systemInfo);
				});
			});
		});
	});
});

