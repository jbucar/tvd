'use strict';

var path = require('path');
var fs = require('fs');
var _ = require("lodash");
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;
var bPromise = require("bluebird");
var mockfs = require('mock-fs');
var Mocks = require('mocks');
var Module = require('module');
var Service = require('../src/service');

var testID = 'ar.edu.unlp.info.lifia.tvd.test';
var test_service_install_path = '/system/original/packages/' + testID;
var test_service_data_path = '/system/data/config/' + testID;

var testID2 = 'ar.edu.unlp.info.lifia.tvd.test2';

var testComponent = {
	"id": testID,
	"pkgID": testID,
	"name": "Test Service",
	"type": "service",
	"version": "1.0.0",
	"main": "test.js",
	"runs": "onDemand"
};

var testComponent2 = {
	"id": testID2,
	"pkgID": testID2,
	"name": "test 2 name",
	"type": "service",
	"version": "1.0.0",
	"main": 'algo.js'
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

var test_service_filename = path.join(test_service_install_path, testComponent.main);
var test_service = function() {
	var self = {};
	self.onStart = function( cb ) { cb(undefined,new Object()); };
	self.onStop = function( cb ) { cb(); };
	return self;
};

var defaultFS = {};
defaultFS[test_service_install_path] = {
		'manifest.json': JSON.stringify(testPackage, null, 4),
		'test.js': 'module.exports = ' + test_service.toString()
};
defaultFS[test_service_data_path] = {};
defaultFS['/system/data/packages'] = {};

describe('Service', function() {
	var reg = undefined;
	var oldRequire = module.constructor.prototype.require;

	beforeEach(function() {
		reg = Mocks.init('silly');

		mockfs(defaultFS);
		var pkgs = [_.cloneDeep(testPackage)];
		reg.put( 'pkgmgr', new Mocks.PackageManager(pkgs) );
		assert.isObject( reg.get('pkgmgr') );
	});

	afterEach(function() {
		mockfs.restore();
		reg = undefined;
		Mocks.fin();

		module.constructor.prototype.require = oldRequire;
	});

	// Writes a function as a module.exports of a test_service_filename
	var writeFunction = function(fnc) {
		module.constructor.prototype.require = function(p) {
			//console.log( 'p=%s', p );
			if (p === test_service_filename) {
				return fnc;
			}
			return oldRequire(p);
		};
	}

	describe('constructor', function() {
		it('should check valid component', function() {
			writeFunction(test_service);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
		});

		it('should check service return an object', function() {
			writeFunction(function() { return 'Hello, World!!!'; });
			var fnc = function() {
				new Service(reg,testID);
			}
			assert.throw(fnc, Error, 'A service must return an object. id='+testID);
		});

        it('should check main path is valid', function() {
			fs.unlinkSync( test_service_filename );
			var fnc = function() {
				new Service(reg,testID);
			}
            assert.throw(fnc, Error, 'Main must be a file: main='+test_service_filename);
        });

		it('should check basic service adapter', function() {
			writeFunction(test_service);
			var srv = new Service(reg,testID);
			assert.isObject(srv);
			var adapter = srv._adapter;
			assert.isObject(adapter);
			assert.isFunction(adapter.load);
			assert.isFunction(adapter.save);
			assert.deepEqual( adapter.registry(), reg );
			assert.deepEqual( srv.info, adapter.info() );
			assert.isTrue( adapter.getInstallPath() === test_service_install_path );
			assert.isTrue( adapter.getDataPath() === test_service_data_path );
		});

		it('should check that type is service', function() {
			writeFunction(test_service);
			var manif = _.cloneDeep(testPackage);
			manif.components[0].type = 'app';
			reg.get('pkgmgr').setPkgs( [manif] );
			function fnc() { new Service(reg,testID); }
			assert.throw(fnc,Error,'Property type is not service');
		});

		it('should check that service file can be compiled', function() {
			writeFunction([]);
			function fnc() { new Service(reg,testID); }
			assert.throw(fnc,Error,'require(...) is not a function');
		});

		it('should check icon', function() {
			writeFunction(test_service);
			var manif = _.cloneDeep(testPackage);
			manif.components[0].icon = 'pepe';
			reg.get('pkgmgr').setPkgs( [manif] );
			var srv = new Service(reg, testID);
			assert.isObject(srv);
			assert.isUndefined( srv.info.icon );
		});

		it('should check that onStart method exists', function() {
			writeFunction(function() {
				return { pepe: function() {} };
			});
			function fnc() { new Service(reg,testID); }
			assert.throw(fnc,Error,'A Service must define onStart/onStop method. id=ar.edu.unlp.info.lifia.tvd.test');
		});

		it('should check that onStop method exists', function() {
			writeFunction(function() {
				return { onStart: function() {} };
			});
			function fnc() { new Service(reg,testID); }
			assert.throw(fnc,Error,'A Service must define onStart/onStop method. id=ar.edu.unlp.info.lifia.tvd.test');
		});

		it('should check that parse object', function() {
			writeFunction('');
			function fnc() { new Service(reg,testID); }
			assert.throw(fnc,Error,'require(...) is not a function');
		});
	});

	describe('run', function() {
		it('should check start is ok', function(done) {
			writeFunction(test_service);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				assert.isTrue(comp.isRunning());
				comp.stop().then(function(st) {
					assert.equal(st,true);
					done();
				});
			});
		});

		it('should start and increment references and stop only when references got 0', function(done) {
			writeFunction(test_service);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				assert.isTrue(comp.isRunning());
				comp.run().then(function() {
					assert.isTrue(comp.isRunning());
					assert.isTrue(comp.ref === 2);
					comp.stop().then(function(st) {
						assert.equal(st,false);
						assert.isTrue(comp.isRunning());
						comp.stop().then(function(st2) {
							assert.equal(st2,true);
							done();
						});
					});
				});
			});
		});

		it('should start and increment references and force stop should reject', function(done) {
			writeFunction(test_service);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				assert.isTrue(comp.isRunning());
				comp.run().then(function() {
					assert.isTrue(comp.isRunning());
					assert.isTrue(comp.ref === 2);
					comp.stop(true).catch(function(err) {
						assert.equal(err.message,'Cannot stop service; service is still referenced: id=ar.edu.unlp.info.lifia.tvd.test, references=1');
						assert.isTrue(comp.isRunning());
						comp.stop().then(function(st2) {
							assert.equal(st2,true);
							done();
						});
					});
				});
			});
		});

		it('should start in parallel', function(done) {
			var test = function() {
				var self = {};
				self.onStart = function( cb ) {
					setTimeout(function() {
						cb(undefined,new Object());
					}, 1000 );
				};
				self.onStop = function( cb ) { cb(); };
				return self;
			};

			writeFunction(test);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			var starts = [];
			var stops = [];
			var count=5;
			for (var i=0; i<count; i++) {
				starts.push( comp.run() );
			}
			bPromise.all(starts)
				.then(function() {
					assert.isTrue(comp.isRunning());
					assert.isTrue(comp.ref === 5);

					for (var i=0; i<count; i++) {
						stops.push( comp.stop() );
					}
					bPromise.all(stops).then(function() {
						assert.isFalse(comp.isRunning());
						done();
					});
				});
		});

		it('should start/stop/start ok', function(done) {
			writeFunction(test_service);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				assert.isTrue(comp.isRunning());
				comp.stop().then(function(st) {
					assert.equal(st,true);
					assert.isFalse(comp.isRunning());
					comp.run().then(function() {
						assert.isTrue(comp.isRunning());
						done();
					});
				});
			});
		});

		it('should check api', function(done) {
			writeFunction(function() {
				var self = {};
				self.onStart = function(cb) { cb(); };
				self.onStop = function(cb) { cb(); };
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().catch(function(err) {
				assert.isFalse(comp.isRunning());
				assert( err !== undefined );
				assert.equal( err, 'Error: OnStart must return a object' );
				done();
			});
		});

		it('should start with timeout', function(done) {
			this.timeout(25000);
			writeFunction(function() {
				var self = {};
				self.onStart = function(cb) {};
				self.onStop = function(cb) { cb(); };
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().catch(function(err) {
				assert.isFalse(comp.isRunning());
				assert( err !== undefined );
				assert.equal( err.message, 'Service not started; timeout: id=ar.edu.unlp.info.lifia.tvd.test' );
				done();
			});
		});

		it('should check catch onStart exceptions', function(done) {
			writeFunction(function() {
				var self = {};
				self.onStart = function() { throw Error('Catch me!'); };
				self.onStop = function(cb) { cb(); };
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().catch(function(err) {
				assert.isFalse(comp.isRunning());
				assert( err !== undefined );
				assert.equal( err, 'Error: Catch me!' );
				done();
			});
		});

		it('should check onStart check for error', function(done) {
			writeFunction(function() {
				var self = {};
				self.onStart = function(cb) { cb( new Error('pepe') ); },
				self.onStop = function(cb) { cb(); };
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().catch(function(err) {
				assert.isFalse(comp.isRunning());
				assert( err !== undefined );
				assert.equal( err, 'Error: pepe' );
				done();
			});
		});

		it('should check onStart returns api and getWebAPI exists and it is a function', function(done) {
			writeFunction(function() {
				var self = {}
				self.onStopCalled = false;
				self.onStart = function(cb) {
					var apis = {};
					apis.pepe = function () { return true; }
					apis.getWebAPI = 10;
					cb(undefined,apis);
				}
				self.onStop = function(cb) { self.onStopCalled = true; cb(); }
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().catch(function(err) {
				assert.isFalse(comp.isRunning());
				assert.isTrue(comp._wrapped.onStopCalled);
				assert( err !== undefined );
				assert.equal( err, 'Error: getWebAPI must be a function: srvID=' + testID );
				done();
			});
		});

		it('should check onStart returns api as object', function(done) {
			writeFunction(function() {
				var self = {}
				self.onStopCalled = false;
				self.onStart = function(cb) {
					cb(undefined,'pepe');
				}
				self.onStop = function(cb) { self.onStopCalled = true; cb(); }
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().catch(function(err) {
				assert.isFalse(comp.isRunning());
				assert.isTrue(comp._wrapped.onStopCalled);
				assert( err !== undefined );
				assert.equal( err.message, 'OnStart must return a object' );
				done();
			});
		});

		it('should check onStart returns api and getWebAPI function returns name property', function(done) {
			writeFunction(function() {
				var self = {}
				self.onStopCalled = false;
				self.onStart = function(cb) {
					var apis = {};
					apis.pepe = function () { return true; }
					apis.getWebAPI = function () { return {}; }
					cb( undefined, apis );
				}
				self.onStop = function(cb) { self.onStopCalled = true; cb(); }
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().catch(function(err) {
				assert.isFalse(comp.isRunning());
				assert.isTrue(comp._wrapped.onStopCalled);
				assert( err !== undefined );
				assert.equal( err, 'Error: getWebAPI must return a object with a name property: srvID=' + testID );
				done();
			});
		});

		it('should check onStart returns api and getWebAPI function throw error', function(done) {
			writeFunction(function() {
				var self = {}
				self.onStopCalled = false;
				self.onStart = function(cb) {
					var apis = {};
					apis.pepe = function () { return true; }
					apis.getWebAPI = function () { throw new Error('alguno'); }
					cb( undefined, apis );
				}
				self.onStop = function(cb) { self.onStopCalled = true; cb; }
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().catch(function(err) {
				assert.isFalse(comp.isRunning());
				assert.isTrue(comp._wrapped.onStopCalled);
				assert( err !== undefined );
				assert.equal( err, 'Error: alguno' );
				done();
			});
		});

		it('should check onStart returns api and getWebAPI function with multiple apis returns name property', function(done) {
			writeFunction(function() {
				var self = {}
				self.onStopCalled = false;
				self.onStart = function(cb) {
					var apis = {
						_hasMultiple: true,
						_main: {
							mainMethod: function () { return true; },
							getWebAPI: function () { return { name:'main' } }
						},
						api1: {
							pepeMethod: function () { return true; },
							getWebAPI: function () { return { name:'api1' } }
						},
						api2: {
							otroMethod: function () { return true; },
							getWebAPI: function () { return { name:'api2' } }
						}
					};

					cb( undefined, apis );
				}
				self.onStop = function(cb) { self.onStopCalled = true; cb(); }
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				assert.isTrue(comp.isRunning());
				assert.isFalse(comp._wrapped.onStopCalled);

				let mainAPI = reg.get(testID);
				assert(mainAPI);
				assert.isFunction( mainAPI.mainMethod );

				let api1 = reg.get(testID+'.api1');
				assert(api1);
				assert.isFunction( api1.pepeMethod );

				let api2 = reg.get(testID+'.api2');
				assert(api2);
				assert.isFunction( api2.otroMethod );
				done();
			});
		});

		it('should check onStart returns api and getWebAPI function with multiple apis with errors', function(done) {
			writeFunction(function() {
				var self = {}
				self.onStopCalled = false;
				self.onStart = function(cb) {
					var apis = {
						_hasMultiple: true,
						api1: {
							pepeMethod: function () { return true; },
							getWebAPI: function () { return { name:'api1' } }
						},
						api2: undefined
					};

					cb( undefined, apis );
				}
				self.onStop = function(cb) { self.onStopCalled = true; cb(); }
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().catch(function(err) {
				assert.equal(err.message,'OnStart must return a object: srvID=' + testID );
				done();
			});
		});

		it('should check onStart returns ok and onStop throw error', function(done) {
			writeFunction(function() {
				var self = {}
				self.onStopCalled = false;
				self.onStart = function(cb) {
					var apis = {};
					cb( undefined, apis );
				}
				self.onStop = function(cb) { throw new Error('alguno'); cb(); }
				return self;
			});
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				assert.isTrue(comp.isRunning());
				comp.stop().catch(function(err) {
					assert( err !== undefined );
					assert.equal( err.message, 'alguno' );
					assert.isFalse(comp._wrapped.onStopCalled);
					done();
				});
			});
		});

		it('should run his dependencies', function(done) {
			writeFunction(test_service);
			var pkgInfo = _.cloneDeep(testPackage);
			pkgInfo.components[0].dependencies = [testID2];
			pkgInfo.components.push(_.cloneDeep(testComponent2));
			var pkgMgr = reg.get( 'pkgmgr' );
			var srvMgr = new Mocks.ServiceManager();
			reg.put( 'srvmgr', srvMgr );
			pkgMgr.setPkgs( [pkgInfo] );
			var comp = new Service(reg,testID);
			comp.run().then(function() {
				assert.isTrue( srvMgr.result.runs.length === 1 );
				assert.isTrue( srvMgr.result.runs[0] === testID2 );
				done();
			});
		});

		it('should stop his dependencies if fail', function(done) {
			var test = function() {
				var self = {};
				self.onStopCalled = false;
				self.onStart = function(cb) {
					var apis = {};
					apis.pepe = function () { return true; }
					apis.getWebAPI = 10;
					cb( undefined, apis );
				};
				self.onStop = function(cb) { self.onStopCalled = true; cb(); }
				return self;
			};
			writeFunction(test);
			var pkgInfo = _.cloneDeep(testPackage);
			pkgInfo.components[0].dependencies = [testID2];
			pkgInfo.components.push(_.cloneDeep(testComponent2));
			var pkgMgr = reg.get( 'pkgmgr' );
			var srvMgr = new Mocks.ServiceManager();
			reg.put( 'srvmgr', srvMgr );
			pkgMgr.setPkgs( [pkgInfo] );
			var comp = new Service(reg,testID);
			comp.run().catch(function(err) {
				assert.isTrue( srvMgr.result.runs.length === 1 );
				assert.isTrue( srvMgr.result.runs[0] === testID2 );
				assert.isTrue( srvMgr.result.stops.length === 1 );
				assert.isTrue( srvMgr.result.stops[0] === testID2 );
				done();
			});
		});

		it('should not run if any of his dependencies cannot be satisfied', function(done) {
			writeFunction(test_service);
			var pkgInfo = _.cloneDeep(testPackage);
			pkgInfo.components[0].dependencies = [testID2];
			pkgInfo.components.push(_.cloneDeep(testComponent2));
			var pkgMgr = reg.get( 'pkgmgr' );
			var srvMgr = new Mocks.ServiceManager();
			srvMgr.run = function() { return new bPromise.reject( new Error('pepe') ); }
			reg.put( 'srvmgr', srvMgr );
			pkgMgr.setPkgs( [pkgInfo] );
			var comp = new Service(reg,testID);
			comp.run().catch(function(err) {
				assert.isTrue( err !== undefined );
				assert.equal( err, 'Error: pepe' );
				done();
			});
		});
	});

	describe('stop', function() {
		it('should check stop is called when started', function(done) {
			var test = function() {
				var self = {};
				self.onStopCalled = false;
				self.onStart = function(cb) { cb( undefined, new Object() ); };
				self.onStop = function(cb) { self.onStopCalled = true; cb(); }
				return self;
			};

			writeFunction(test);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				assert.isTrue(comp.isRunning());
				comp.stop().then(function() {
					assert.isFalse(comp.isRunning());
					assert.isTrue(comp._wrapped.onStopCalled);
					done();
				});
			});
		});

		it('should return error on stop', function(done) {
			var test = function() {
				var self = {};
				self.onStopCalled = false;
				self.onStart = function(cb) { cb( undefined, new Object() ); };
				self.onStop = function(cb) { self.onStopCalled = true; cb( new Error( 'alguno' ) ); }
				return self;
			};

			writeFunction(test);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function(err) {
				assert.isTrue(comp.isRunning());
				comp.stop().catch(function(err) {
					assert.isFalse(err === undefined);
					assert.isFalse(comp.isRunning());
					assert.isTrue(comp._wrapped.onStopCalled);
					assert.equal( err.message, 'alguno' );
					done();
				});
			});
		});

		it('should not run if stop in curse', function(done) {
			var start_count=0;
			var stop_count=0;

			var test = function() {
				var self = {};
				self.onStart = function(cb) { cb( undefined, new Object() ); };
				self.onStop = function(cb) {
					setTimeout(function() {
						cb( undefined );
					}, 300 );
				};
				return self;
			};

			writeFunction(test);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function(err) {
				start_count++;
				assert.isTrue(comp.isRunning());
				comp.stop().then(function() {
					stop_count++;
				});
				comp.run().then(function(err) {
					start_count++;
					assert(!err);
					assert.equal(start_count,2);
					assert.equal(stop_count,1);
					done();
				});
			});
		});

		it('should timeout if onStop called and does not return', function(done) {
			this.timeout(5000);
			var test = function() {
				var self = {};
				self.onStopCalled = false;
				self.onStart = function(cb) { cb( undefined, new Object() ); };
				self.onStop = function(cb) { self.onStopCalled = true; };
				return self;
			};

			writeFunction(test);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function(err) {
				assert.isTrue(comp.isRunning());
				comp.stop().catch(function(err) {
					assert.isFalse(err === undefined);
					assert.isFalse(comp.isRunning());
					assert.isTrue(comp._wrapped.onStopCalled);
					assert.equal( err.message, 'Service stopped by timeout' );
					done();
				});
			});
		});

		it('should not call onStop if onStart fail', function(done) {
			var test = function() {
				var self = {};
				self.onStopCalled = false;
				self.onStart = function() { throw new Error('pepe'); };
				self.onStop = function(cb) { self.onStopCalled = true; cb(); }
				return self;
			};

			writeFunction(test);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().catch(function() {
				assert.isFalse(comp.isRunning());
				comp.stop().then(function() {
					assert.isFalse(comp._wrapped.onStopCalled);
					done();
				});
			});
		});
	});

	describe('runs/autostart', function() {
		it('should parse runs property from component or onStartup as default', function() {
			writeFunction(test_service);
			var pkgs = reg.get('pkgmgr').getPkgs();
			pkgs[0].components[0].runs = undefined;
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			assert.equal( comp.info.runs, 'onStartup' );
		});

		it('should change runs method (autostart)', function() {
			writeFunction(test_service);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			assert.equal( comp.info.runs, 'onDemand' );
			assert.isFalse( comp.autoStart() );
			comp.runs( 'onStartup' );
			assert.isTrue( comp.autoStart() );
			comp.runs( 'onDemand' );
			assert.isFalse( comp.autoStart() );
		});
	});

	describe('resetData', function() {
		it('should reset data if not started', function(done) {
			writeFunction(test_service);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			assert.isFalse(comp.isRunning());
			comp.resetData().then(function() {
				assert.isFalse(comp.isRunning());
				done();
			});
		});

		it('should reset data if is started', function(done) {
			writeFunction(test_service);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				comp.resetData().then(function() {
					assert.isTrue(comp.isRunning());
					comp.resetData();
					done();
				});
			});
		});
	});

	describe('dump', function() {
		var dumpData = {
			"id":"ar.edu.unlp.info.lifia.tvd.test",
			"pkgID":"ar.edu.unlp.info.lifia.tvd.test",
			"runs": "onDemand",
			"name":"Test Service",
			"configure": undefined,
			"description": undefined,
			"version":"1.0.0",
			"system": true,
			"type": "service",
			"user": true,
			"vendor":"LIFIA",
			"vendor-url":"http://lifia.info.unlp.edu.ar/",
			"dependencies":[],
			"main":"/system/original/packages/ar.edu.unlp.info.lifia.tvd.test/test.js"
		};

		it('should dump service state', function(done) {
			var test = function() {
				var api = {};
				api.dump = function() {
					return { state: true };
				};
				var self = {};
				self.onStart = function(cb) { cb(undefined,api); };
				self.onStop = function(cb) { cb(); }
				return self;
			};
			writeFunction(test);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				var info = comp.dump();
				var dumpedData = _.clone(dumpData);
				dumpedData["state"] = { "state": true };
				assert.deepEqual( info, dumpedData );
				done();
			});
		});

		it('should dump generic service information if service not started', function() {
			var test = function() {
				var self = {};
				self.onStart = function() { cb( new Error('pepe') ); },
				self.onStop = function(cb) { cb(); }
				return self;
			};
			writeFunction(test);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			var info = comp.dump();
			assert.deepEqual( info, dumpData );
		});

		it('should catch any dump error', function(done) {
			var test = function() {
				var self = {};
				self.onStart = function(cb) {
					var api = {};
					api.dump = function() {
						throw new Error('alguno');
					};
					cb(undefined,api);
				};
				self.onStop = function(cb) { cb(); }
				return self;
			};
			writeFunction(test);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				var info = comp.dump();
				assert.deepEqual( info, dumpData );
				done();
			});
		});

		it('should dump generic service information if api defined but dump not defined', function(done) {
			var test = function() {
				var api = {};
				var self = {};
				self.onStart = function(cb) { cb(undefined,api); };
				self.onStop = function(cb) { cb(); }
				return self;
			};
			writeFunction(test);
			var comp = new Service(reg,testID);
			assert.isObject(comp);
			comp.run().then(function() {
				var info = comp.dump();
				assert.deepEqual( info, dumpData );
				done();
			});
		});
	});

	describe('ServiceAdapter', function() {

		function createDataConfig(config) {
			fs.writeFileSync( path.join(test_service_data_path, 'config.json'), JSON.stringify(config, null, 4) );
		}

		var adapter = undefined;

		var test_adapter_service = function(adapter) {
			var self = {}
			self.onStart = function(cb) {
				var api = {getAdapter: function() { return adapter; } };
				cb(undefined,api);
			}
			self.onStop = function() {};
			return self;
		}

		beforeEach(function(done) {
			mockfs(defaultFS);
			writeFunction(test_adapter_service);
			var srv = new Service(reg, testID);
			srv.run().then(function() {
				adapter = reg.get(testID).getAdapter();
				done();
			});
		});

		describe('info', function() {
			var data = {
				"id":"ar.edu.unlp.info.lifia.tvd.test",
				"pkgID":"ar.edu.unlp.info.lifia.tvd.test",
				"runs": "onDemand",
				"name":"Test Service",
				"configure": undefined,
				"description": undefined,
				"version":"1.0.0",
				"system": true,
				"type": "service",
				"user": true,
				"vendor":"LIFIA",
				"vendor-url":"http://lifia.info.unlp.edu.ar/",
				"dependencies":[],
				"main":"/system/original/packages/ar.edu.unlp.info.lifia.tvd.test/test.js"
			};

			it('should return the static info of the service', function() {
				assert.deepEqual( adapter.info(), data );
			});
		});

		describe('resetData', function() {
			it('should reset data', function(done) {
				adapter.save( 'myconfig.conf', { "pepe": "otro" } );
				fs.mkdirSync( path.join( adapter.getDataPath(), "un_dir" ) );
				var filesBefore = fs.readdirSync(adapter.getDataPath());
				assert.isTrue( filesBefore.length > 0 );
				adapter.resetData().then(function() {
					var filesAfter = fs.readdirSync(adapter.getDataPath());
					assert.isTrue( filesAfter.length === 0 );
					done();
				});
			});
		});

		describe('getInstallPath', function() {
			it('should return the service install path', function() {
				assert.isTrue( adapter.getInstallPath() === "/system/original/packages/ar.edu.unlp.info.lifia.tvd.test" );
			});

			it('should return the service install path + component', function() {
				assert.isTrue( adapter.getInstallPath('pepe') === "/system/original/packages/ar.edu.unlp.info.lifia.tvd.test/pepe" );
			});

			it('should return the service install path + components', function() {
				assert.isTrue( adapter.getInstallPath('pepe','otro') === "/system/original/packages/ar.edu.unlp.info.lifia.tvd.test/pepe/otro" );
			});
		});

		describe('getDataPath', function() {
			it('should return the service data path', function() {
				assert.isTrue( adapter.getDataPath() === "/system/data/config/ar.edu.unlp.info.lifia.tvd.test" );
			});

			it('should return the service data path + component', function() {
				assert.isTrue( adapter.getDataPath('pepe') === "/system/data/config/ar.edu.unlp.info.lifia.tvd.test/pepe" );
			});

			it('should return the service data path + components', function() {
				assert.isTrue( adapter.getDataPath('pepe','otro') === "/system/data/config/ar.edu.unlp.info.lifia.tvd.test/pepe/otro" );
			});
		});

		describe('getDataRootPath', function() {
			it('should return the service data path', function() {
				assert.equal( adapter.getInstallRootPath(), '/system/original/' );
				assert.equal( adapter.getDataRootPath(), "/system/data/" );
			});

			it('should return the service data path without components', function() {
				assert.equal( adapter.getInstallRootPath('pepe'), '/system/original/' );
				assert.equal( adapter.getDataRootPath('pepe'), "/system/data/" );
			});
		});

		describe('getSharePath', function() {
			it('should return the shared path', function() {
				assert.equal( adapter.getDataRootPath(), "/system/data/" );
				assert.equal( adapter.getSharePath(), "/system/data/share" );
			});
		});

		describe('getTemporaryPath', function() {
			it('should return temporary folder path', function() {
				assert.isTrue( adapter.getTemporaryPath() === "/tmp" );
			});
		});

		describe('load', function() {
			it('should return the defaults value when there is no config', function() {
				var defaults = {
					'opt': 'value1',
					'opt2': 2
				}
				var def2 = _.cloneDeep(defaults);
				def2._version = '1.0.0';
				assert.deepEqual(def2, adapter.load('config.json',defaults));
			});

			it('should return the defaults value when there is no config and dont call convert config', function() {
				var defaults = {
					'opt': 'value1',
					'opt2': 2
				}
				var def2 = _.cloneDeep(defaults);
				def2._version = '1.0.0';
				assert.deepEqual(def2, adapter.load('config.json',defaults, function(){
					assert.fail();
				}));
			});

			it('should return the merge of defaults value and the installed config when there is one', function() {
				var config = { 'opt3': 'value' };
				var defaults = { 'opt': 'value1', 'opt2': 2	};
				fs.writeFileSync( path.join(test_service_install_path, 'config.json'), JSON.stringify(config, null, 4) );

				assert.deepEqual(adapter.load('config.json',defaults), {
					'_version': '1.0.0',
					'opt': 'value1',
					'opt2': 2,
					'opt3': 'value'
				});
			});

			it('should return the merge of defaults value and the data config when there is one', function() {
				createDataConfig({ '_version': '1.0.0', 'opt3': 'value' });
				var defaults = { 'opt': 'value1', 'opt2': 2	};

				assert.deepEqual(adapter.load('config.json',defaults), {
					'_version': '1.0.0',
					'opt': 'value1',
					'opt2': 2,
					'opt3': 'value'
				});
			});

			it('should return the merge of defaults value and the data config when there is one (b)', function() {
				var data = { '_version': '1.0.0', 'opt4': true };
				var install = { 'opt3': 'value' };
				var defaults = { 'opt': 'value1', 'opt2': 2	};

				fs.writeFileSync( path.join(test_service_install_path, 'config.json'), JSON.stringify(install, null, 4) );
				createDataConfig(data);

				assert.deepEqual(adapter.load('config.json',defaults), {
					'_version': '1.0.0',
					'opt': 'value1',
					'opt2': 2,
					'opt4': true,
					'opt3': 'value'
				});
			});

			it('should return the defaults values overrides by the data config values', function() {
				createDataConfig({ '_version': '1.0.0', 'opt': 'dataValue' });
				var defaults = { 'opt': 'value1', 'opt2': 2	};

				assert.deepEqual(adapter.load('config.json',defaults), {
					'_version': '1.0.0',
					'opt': 'dataValue',
					'opt2': 2,
				});
			});

			it('should return the defaults values overrides by the install config values', function() {
				var config = { 'opt': 'dataValue' };
				var defaults = { 'opt': 'value1', 'opt2': 2	};

				fs.writeFileSync( path.join(test_service_install_path, 'config.json'), JSON.stringify(config, null, 4) );

				assert.deepEqual(adapter.load('config.json',defaults), {
					'_version': '1.0.0',
					'opt': 'dataValue',
					'opt2': 2,
				});
			});

			it('should return the defaults values overrides by the install config values overrides by the data config values', function() {
				var data = { '_version': '1.0.0', 'opt': 'value3', 'dataOpt': 'data' };
				var install = { 'opt': 'value2', 'opt2': 3, 'installOpt': 'install' };
				var defaults = { 'opt': 'value1', 'opt2': 2, 'defualtsOpt': 'defaults' };

				fs.writeFileSync( path.join(test_service_install_path, 'config.json'), JSON.stringify(install, null, 4) );
				createDataConfig(data);

				assert.deepEqual(adapter.load('config.json',defaults), {
					'_version': '1.0.0',
					'opt': 'value3',
					'opt2': 3,
					'dataOpt': 'data',
					'installOpt': 'install',
					'defualtsOpt': 'defaults'
				});
			});

			it('should return the defaults values if version mismatch (no present)', function() {
				var data = { 'opt': 'value3', 'dataOpt': 'data' };
				var install = { 'opt': 'value2', 'opt2': 3, 'installOpt': 'install' };
				var defaults = { 'opt': 'value1', 'opt2': 2, 'defualtsOpt': 'defaults' };

				fs.writeFileSync( path.join(test_service_install_path, 'config.json'), JSON.stringify(install, null, 4) );
				createDataConfig(data);

				//	Some file
				fs.writeFileSync( path.join(test_service_data_path, 'pepe.json'), 'pepe data' );

				assert.deepEqual(adapter.load('config.json',defaults), {
					'_version': '1.0.0',
					'opt': 'value2',
					'opt2': 3,
					'installOpt': 'install',
					'defualtsOpt': 'defaults'
				});

				var files = fs.readdirSync(adapter.getDataPath());
				assert.equal( files.length, 0 );
			});

			it('should return the defaults values if version mismatch (present but differ)', function() {
				var data = { '_version': '2.0.0', 'opt': 'value3', 'dataOpt': 'data' };
				var install = { 'opt': 'value2', 'opt2': 3, 'installOpt': 'install' };
				var defaults = { 'opt': 'value1', 'opt2': 2, 'defualtsOpt': 'defaults' };

				fs.writeFileSync( path.join(test_service_install_path, 'config.json'), JSON.stringify(install, null, 4) );
				createDataConfig(data);

				//	Some file
				fs.writeFileSync( path.join(test_service_data_path, 'pepe.json'), 'pepe data' );

				assert.deepEqual(adapter.load('config.json',defaults), {
					'_version': '1.0.0',
					'opt': 'value2',
					'opt2': 3,
					'installOpt': 'install',
					'defualtsOpt': 'defaults'
				});

				var files = fs.readdirSync(adapter.getDataPath());
				assert.equal( files.length, 0 );
			});

			it('should return the updated values if version mismatch and convert function exists', function() {
				var data = { 'opt': 'value3', 'dataOpt': 'data' };
				var install = { 'opt': 'value2', 'opt2': 3, 'installOpt': 'install' };
				var defaults = { 'opt': 'value1', 'opt2': 2, 'defualtsOpt': 'defaults' };

				fs.writeFileSync( path.join(test_service_install_path, 'config.json'), JSON.stringify(install, null, 4) );
				createDataConfig(data);

				var convert = function(data) {
					return data;
				};

				assert.deepEqual(adapter.load('config.json',defaults,convert), {
					'_version': '1.0.0',
					'opt': 'value3',
					'dataOpt': 'data',
					'opt2': 3,
					'installOpt': 'install',
					'defualtsOpt': 'defaults'
				});
			});
		});

		describe('save', function() {
			it('should save the new values', function() {
				var values = { 'opt': 'value1', 'opt2': 2 };
				adapter.save('config.json',values);
				var config = JSON.parse(fs.readFileSync(path.join(test_service_data_path, 'config.json')));

				assert.deepEqual(config, values);
			});
		});
	});
});
