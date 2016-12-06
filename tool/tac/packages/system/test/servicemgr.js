'use strict';

var path = require('path');
var _ = require("lodash");
var fs = require('fs');

var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;
var mockfs = require('mock-fs');
var Mocks = require('mocks');
var Module = require('module');

var ServiceManager = require('../src/servicemgr');

var base_ro = '/system/original';
var base_rw = '/system/data';

function addSrvComponent( id, name ) {
	return {
			"id": id,
			"pkgID": id,
			"name": name,
			"type": "service",
			"version": "1.0.0",
			"vendor": "PEPE",
			"vendor-url": "http://pepe.com/",
			"description": "A description",
			"main": "main.js",
			"runs": "onStartup"
	};
}

function addSrv( id, name, components ) {
	var srv = {};

	srv.id = id;
	srv.name = name;

	srv.main = function() {
		return {
			onStart: function(cb) { cb(undefined,{}); },
			onStop: function(cb) { cb(); }
		};
	};

	if (!components) {
		components = [];
		components.push( addSrvComponent( id, name ) );
	}

	srv.manifest = {
		"id": id,
		"vendor": "LIFIA",
		"vendor-url": "http://lifia.info.unlp.edu.ar/",
		"version": "1.0.0",
		"api-level": "1.0",
		"platform": "all",
		"system": false,
		"user": false,
		"components": components
	};

	srv.path = path.join( base_ro, 'packages', id );
	srv.mainPath = path.join(srv.path, 'main.js');

	srv.exportFS = function(fs) {
		//	Export
		fs[srv.path] = {
			'manifest.json': JSON.stringify(srv.manifest, null, 4),
			'main.js': 'module.exports = ' + srv.main.toString()
		};
	};

	return srv;
}

function systemData() {
	return path.join( base_rw, 'config/ar.edu.unlp.info.lifia.tvd.system' );
}

function configFile() {
	return path.join( systemData(), 'services.json' );
}

function initFS( services, fs, srvCfg ) {
	var defaultFS = fs || {};

	defaultFS[base_rw] = {};
	defaultFS[base_ro] = {};
	defaultFS[ systemData() ] = {};
	if (srvCfg) {
		defaultFS[ systemData() ]['services.json'] = srvCfg;
	}

	services.forEach(function(srv) {
		srv.exportFS( defaultFS );
	});

	module.constructor.prototype.require = function(p) {
		//console.log( 'p=%s', p );
		let srv = services.find( (s) => s.mainPath === p );
		if (srv) {
			return srv.main;
		}
		return oldRequire(p);
	};

	mockfs(defaultFS);
}

function finMocks() {
	mockfs.restore();
	Mocks.fin();
}

function initPkgMgr( reg, services ) {
	//	Setup package manager
	var pkgs = services.map(function(srv) {
		return _.cloneDeep(srv.manifest);
	});
	reg.put( 'pkgmgr', new Mocks.PackageManager(pkgs) );
}

function configServiceManager(config) {
	fs.writeFileSync( configFile(), JSON.stringify(config, null, 4));
}

function initSrv( reg, srvMgr, cb ) {
	srvMgr.once('ready', function(st) {
		assert.equal( st, true );
		assert.equal( srvMgr.isReady(), true );
		cb(srvMgr);
	});
	srvMgr.init(reg);
}

function initSrvMgr( opts, cb ) {
	assert(opts.reg);
	assert(cb);
	if (!opts.services) {
		opts.services = [];
	}
	if (!opts.srvMgr) {
		opts.srvMgr = new ServiceManager();
	}

	//	Init mockFS
	initFS( opts.services, opts.fs, opts.srvCfg );

	//	Init package manager
	initPkgMgr( opts.reg, opts.services );

	//	Setup service manager
	initSrv( opts.reg, opts.srvMgr, cb );
}

function finSrvMgr( srvMgr, cb ) {
	srvMgr.fin().then(function() {
		finMocks();
		assert.equal( srvMgr.getAll().length, 0 );
		assert.equal( srvMgr.isReady(), false );
		cb();
	});
}

describe('ServiceManager', function() {
	describe('constructor', function() {
		it('should return a service manager', function() {
			var srv = new ServiceManager();
			assert.isObject( srv );
		});
	});

	describe('init/fin', function() {
		var _reg = null;
		var oldRequire;

		beforeEach(function() {
			_reg = Mocks.init('silly');
			var oldRequire = module.constructor.prototype.require;
		});

		afterEach(function() {
			assert.equal( _reg.get('srvmgr'), undefined );
			_reg = null;
			module.constructor.prototype.require = oldRequire;
		});

		it('should init ok', function(done) {
			initSrvMgr( { reg: _reg }, function(srvMgr) {
				assert.isObject(srvMgr);
				finSrvMgr( srvMgr, done );
			});
		});

		it('should put srvmgr api on registry', function(done) {
			initSrvMgr( { reg: _reg }, function(srvMgr) {
				assert.isObject(srvMgr);
				var api = _reg.get('srvmgr');
				assert.isObject(api);
				assert.isFunction(api.isRunning);
				assert.isFunction(api.getAllRunning);
				assert.isFunction(api.getAll);
				assert.isFunction(api.get);
				assert.isFunction(api.getConfigApp);
				assert.isFunction(api.run);
				assert.isFunction(api.stop);
				assert.isFunction(api.isReady);
				assert.isFunction(api.dump);
				assert.isFunction(api.resetConfig);
				assert.isFunction(api.init);
				assert.isFunction(api.fin);
				assert.isFunction(api.getWebAPI);
				assert.isTrue( api.getWebAPI().name === 'srvmgr' );
				finSrvMgr( srvMgr, done );
			});
		});

		it('should load config ok', function(done) {
			initSrvMgr( { reg: _reg, srvCfg: '' }, function(srvMgr) {
				assert.isObject(srvMgr);
				finSrvMgr( srvMgr, done );
			});
		});

		it('should reset config if file is corrupt', function(done) {
			initSrvMgr( { reg: _reg, srvCfg: 'jakdsfjaksdjf' }, function(srvMgr) {
				assert.isObject(srvMgr);
				finSrvMgr( srvMgr, done );
			});
		});

		it('should load services ok', function(done) {
			var srv1 = addSrv( 'srv1', 'Service1' );
			srv1.manifest.components[0].user = true;

			var srv2 = addSrv( 'srv2', 'Service2' );
			srv2.manifest.components[0].user = true;

			initSrvMgr( { reg: _reg, services: [srv1,srv2] }, function(srvMgr) {
				assert.isObject(srvMgr);
				assert.equal( srvMgr.getAll().length, 2 );
				assert.equal( srvMgr.isEnabled( srv1.id ), true );
				assert.equal( srvMgr.isEnabled( srv2.id ), true );

				assert.equal( srvMgr.isRunning( srv1.id ), true );
				assert.equal( srvMgr.isRunning( srv2.id ), true );

				finSrvMgr( srvMgr, done );
			});
		});

		it('should load a service with dependencies ok', function(done) {
			var srv1 = addSrv( 'srv1', 'Service1' );
			srv1.manifest.components[0].user = true;

			var srv2 = addSrv( 'srv2', 'Service2' );
			srv2.manifest.components[0].user = true;

			//	Add dep from srv1 -> srv2
			srv1.manifest.components[0].dependencies = [srv2.id];

			initSrvMgr( { reg: _reg, services: [srv1,srv2] }, function(srvMgr) {
				assert.isObject(srvMgr);
				assert.equal( srvMgr.getAll().length, 2 );
				assert.equal( srvMgr.isEnabled( srv1.id ), true );
				assert.equal( srvMgr.isEnabled( srv2.id ), true );

				assert.equal( srvMgr.isRunning( srv1.id ), true );
				assert.equal( srvMgr.isRunning( srv2.id ), true );

				finSrvMgr( srvMgr, done );
			});
		});

		it('should fail to load a service if some dependencies are invalid', function(done) {
			var srv1 = addSrv( 'srv1', 'Service1' );
			srv1.manifest.components[0].user = true;

			var srv2 = addSrv( 'srv2', 'Service2' );
			srv2.manifest.components[0].user = true;

			//	Add dep from srv1 -> srv2
			srv1.manifest.components[0].dependencies = ['pepe'];

			initSrvMgr( { reg: _reg, services: [srv1,srv2] }, function(srvMgr) {
				assert.isObject(srvMgr);
				assert.equal( srvMgr.getAll().length, 1 );

				assert.equal( srvMgr.isEnabled( srv1.id ), false );
				assert.equal( srvMgr.isRunning( srv1.id ), false );

				assert.equal( srvMgr.isEnabled( srv2.id ), true );
				assert.equal( srvMgr.isRunning( srv2.id ), true );

				finSrvMgr( srvMgr, done );
			});
		});

		it('should fail to start a service if some dependencies cant start', function(done) {
			var srv1 = addSrv( 'srv1', 'Service1' );
			srv1.manifest.components[0].user = true;

			var srv2 = addSrv( 'srv2', 'Service2' );
			srv2.manifest.components[0].user = true;
			srv2.main = function() {
				return {
					onStart: function(cb) { cb(new Error('pepe')); },
					onStop: function(cb) { cb(); }
				};
			};

			//	Add dep from srv1 -> srv2
			srv1.manifest.components[0].dependencies = [srv2.id];

			initSrvMgr( { reg: _reg, services: [srv1,srv2] }, function(srvMgr) {
				assert.isObject(srvMgr);
				assert.equal( srvMgr.getAll().length, 2 );
				assert.equal( srvMgr.isEnabled( srv1.id ), true );
				assert.equal( srvMgr.isRunning( srv1.id ), false );

				assert.equal( srvMgr.isEnabled( srv2.id ), true );
				assert.equal( srvMgr.isRunning( srv2.id ), false );

				finSrvMgr( srvMgr, done );
			});
		});

		it('should change service configuration when it is loaded (from onDemand to onStartup)', function(done) {
			var srv = addSrv( 'srv1', 'Service1' );
			srv.manifest.components[0].user = true;
			srv.manifest.components[0].runs = 'onDemand';

			var cfg = {
				reg: _reg,
				services: [srv],
				srvCfg: '{ "srv1": { "runs": "onStartup" } }'
			};
			initSrvMgr( cfg, function(srvMgr) {
				assert.isObject(srvMgr);
				assert.equal( srvMgr.getAll().length, 1 );
				assert.equal( srvMgr.isEnabled( srv.id ), true );
				assert.equal( srvMgr.isRunning( srv.id ), true );
				finSrvMgr( srvMgr, done );
			});
		});

		it('should change service configuration when it is loaded (from onStartup to onDemand)', function(done) {
			var srv = addSrv( 'srv1', 'Service1' );
			srv.manifest.components[0].user = true;
			srv.manifest.components[0].runs = 'onStartup';

			var cfg = {
				reg: _reg,
				services: [srv],
				srvCfg: '{ "srv1": { "runs": "onDemand" } }'
			};
			initSrvMgr( cfg, function(srvMgr) {
				assert.isObject(srvMgr);
				assert.equal( srvMgr.getAll().length, 1 );
				assert.equal( srvMgr.isEnabled( srv.id ), false );
				assert.equal( srvMgr.isRunning( srv.id ), false );
				finSrvMgr( srvMgr, done );
			});
		});

		it('should init/fin/init/fin ok', function(done) {
			var srv = addSrv( 'srv', 'Service1' );
			srv.manifest.components[0].user = true;

			var cfg = {
				reg: _reg,
				services: [srv]
			};
			initSrvMgr( cfg, function(srvMgr) {
				assert.isObject(srvMgr);
				assert.equal( srvMgr.getAll().length, 1 );
				srvMgr.fin().then(function() {
					_reg.get('pkgmgr').setPkgs([]);
					initSrv( _reg, srvMgr, function() {
						assert.equal( srvMgr.getAll().length, 0 );
						assert.isObject(srvMgr);
						finSrvMgr( srvMgr, done );
					});
				});
			});
		});

		it('should finalize when on service return error on onStop', function(done) {
			var srv = addSrv( 'srv1', 'Service1' );
			srv.main = function() {
				return {
					onStart: function(cb) { cb(undefined,{}); },
					onStop: function(cb) { cb(new Error('onStop test error')); }
				};
			};

			var cfg = {
				reg: _reg,
				services: [srv]
			};
			initSrvMgr( cfg, function(srvMgr) {
				assert.isObject(srvMgr);
				assert.equal( srvMgr.isRunning( srv.id ), true );
				finSrvMgr( srvMgr, function() {
					assert.equal( srvMgr.isRunning( srv.id ), false );
					done();
				});
			});
		});

		it('should finalize when on service not return from onStop', function(done) {
			this.timeout(5000);
			var srv = addSrv( 'srv1', 'Service1' );
			srv.main = function() {
				return {
					onStart: function(cb) { cb(undefined,{}); },
					onStop: function(cb) {}
				};
			};

			var cfg = {
				reg: _reg,
				services: [srv]
			};
			initSrvMgr( cfg, function(srvMgr) {
				assert.isObject(srvMgr);
				assert.equal( srvMgr.isRunning( srv.id ), true );

				var called=0;
				srvMgr.on('error', function(err) {
					assert( err !== undefined );
					assert.equal( err.message, 'Service stopped by timeout' );
					called++;
				});

				finSrvMgr( srvMgr, function() {
					assert.equal( srvMgr.isRunning( srv.id ), false );
					assert.equal( called, 1 );
					done();
				});
			});
		});

		it('should continue closing all services when on service not return from onStop', function(done) {
			this.timeout(5000);

			var srv1 = addSrv( 'srv1', 'Service1' );
			srv1.main = function() {
				return {
					onStart: function(cb) { cb(undefined,{}); },
					onStop: function(cb) {}
				};
			};
			var srv2 = addSrv( 'srv2', 'Service2' );

			var cfg = {
				reg: _reg,
				services: [srv1,srv2]
			};
			initSrvMgr( cfg, function(srvMgr) {
				assert.isObject(srvMgr);
				assert.equal( srvMgr.isRunning( srv1.id ), true );
				assert.equal( srvMgr.isRunning( srv2.id ), true );


				var errors=0;
				srvMgr.on('error', function(err) {
					assert.equal( errors, 0 );
					assert( err !== undefined );
					assert.equal( err.message, 'Service stopped by timeout' );
					errors++;
				});

				var called=0;
				srvMgr.on('stopped', function(id) {
					assert.equal( called, 0 );
					assert.equal( id, srv2.id );
					called++;
				});

				finSrvMgr( srvMgr, function() {
					assert.equal( srvMgr.isRunning( srv1.id ), false );
					assert.equal( srvMgr.isRunning( srv2.id ), false );
					assert.equal( called, 1 );
					assert.equal( errors, 1 );
					done();
				});
			});
		});
	});

	describe('api', function() {
		var _reg = null;
		var _srvMgr = null;

		beforeEach(function() {
			_reg = Mocks.init('silly');
		});

		afterEach(function(done) {
			finSrvMgr( _srvMgr, function() {
				assert.equal( _reg.get('srvmgr'), undefined );
				_reg = null;
				done();
			});
		});

		describe('get()', function() {
			it('should return null if service do not exists', function(done) {
				initSrvMgr( {reg: _reg}, function(obj) {
					_srvMgr = obj;
					var srv = _srvMgr.get('test');
					assert.strictEqual( null, srv );
					done();
				});
			});

			it('should return service info if service exists', function(done) {
				var srv1 = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv1]}, function(obj) {
					_srvMgr = obj;
					var srv = _srvMgr.get(srv1.id);
					assert.isObject( srv );
					assert.strictEqual( srv1.id, srv.id );
					assert.strictEqual( "Service1", srv.name );
					assert.strictEqual( "srv1", srv.pkgID );
					assert.strictEqual( "A description", srv.description );
					assert.strictEqual( "1.0.0", srv.version );
					assert.strictEqual( "PEPE", srv.vendor );
					assert.strictEqual( "http://pepe.com/", srv["vendor-url"] );
					done();
				});
			});
		});

		describe('getAll()', function() {
			it('should return an empty array if there isnt any non-user service', function(done) {
				initSrvMgr( {reg: _reg}, function(obj) {
					_srvMgr = obj;
					var srvs = _srvMgr.getAll();
					assert.isArray( srvs );
					assert.strictEqual( 0, srvs.length );
					done();
				});
			});

			it('should return all user services', function(done) {
				var srv1 = addSrv( 'srv1', 'Service1' );
				srv1.manifest.components[0].user = true;

				var srv2 = addSrv( 'srv2', 'Service2' );
				srv2.manifest.components[0].user = false;

				initSrvMgr( { reg: _reg, services: [srv1,srv2] }, function(obj) {
					_srvMgr = obj;
					var srvs = _srvMgr.getAll();
					assert.isArray( srvs );
					assert.strictEqual( 1, srvs.length );
					assert.strictEqual( srv1.id, srvs[0] );
					done();
				});
			});
		});

		describe('run', function() {
			it('should run service successfully', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					_srvMgr.on('started', function(id) {
						assert.isTrue( id === srv.id );
						assert.isTrue( _srvMgr.isRunning(srv.id) );
						done();
					});
					_srvMgr.run(srv.id);
				});
			});

			it('should not throw error if service already running', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;
					_srvMgr.run( srv.id );
					done();
				});
			});

			it('should not run when the service fail to start', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';
				srv.main = function() {
					return {
						onStart: function(cb) { cb( new Error('pepe') ); },
						onStop: function(cb) { cb(); }
					};
				};
				initSrvMgr( {reg: _reg, services:[srv]}, function(obj) {
					_srvMgr = obj;
					_srvMgr.run(srv.id).catch(function(err) {
						assert( err !== undefined );
						assert.equal( err.message, 'pepe' );
						done();
					});;
				});
			});

			it('should return false when the service not exists', function(done) {
				initSrvMgr( {reg: _reg}, function(obj) {
					_srvMgr = obj;
					_srvMgr.run('pepe').catch(function(err) {
						assert( err !== undefined );
						assert.equal( err.message, 'Service not found: id=pepe' );
						done();
					});
				});
			});
		});

		describe('stop(id)', function() {
			it('should stop running the service', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					var called = 0;
					_srvMgr.on('stopped', function(id) {
						assert.equal( id, srv.id );
						called++;
					});

					assert.isTrue( _srvMgr.isRunning(srv.id) );
					_srvMgr.stop( srv.id ).then(function() {
						assert.equal( called, 1 );
						assert.isFalse(_srvMgr.isRunning(srv.id));
						done();
					});
				});
			});

			it('should do nothing it is called twice', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					var called = 0;
					_srvMgr.on('stopped', function(id) {
						assert.equal( id, srv.id );
						called++;
					});

					assert.isTrue( _srvMgr.isRunning(srv.id) );
					_srvMgr.stop( srv.id ).then(function() {
						assert.equal( called, 1 );
						assert.isFalse(_srvMgr.isRunning(srv.id));
						_srvMgr.stop(srv.id).then(function() {
							assert.equal( called, 1 );
							assert.isFalse(_srvMgr.isRunning());
							done();
						});
					});
				});
			});

			it('should do nothing if the service is not running', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					var called = 0;
					_srvMgr.on('stopped', function(id) {
						assert.equal( id, srv.id );
						called++;
					});

					assert.isFalse( _srvMgr.isRunning(srv.id) );
					_srvMgr.stop( srv.id ).then(function() {
						assert.equal( called, 0 );
						done();
					});
				});
			});

			it('should do nothing if the service not exists', function(done) {
				initSrvMgr( {reg: _reg}, function(obj) {
					_srvMgr = obj;

					assert.isFalse( _srvMgr.isRunning('pepe') );
					_srvMgr.stop( 'pepe' ).catch(function(err) {
						assert(err !== undefined );
						done();
					});
				});
			});
		});

		describe('getDependencies()', function() {
			it('should return an empty array if service do not exist', function(done) {
				initSrvMgr( {reg: _reg}, function(obj) {
					_srvMgr = obj;
					var deps = _srvMgr.getDependencies('pepe');
					assert.isArray( deps );
					assert.strictEqual( 0, deps.length );
					done();
				});
			});

			it('should return an empty array if service do not have dependencies', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;
					assert.isTrue( _srvMgr.isRunning( srv.id ) );
					var deps = _srvMgr.getDependencies( srv.id );
					assert.isArray( deps );
					assert.strictEqual( 0, deps.length );
					done();
				});
			});

			it('should return an array with deps if service have dependencies', function(done) {
				var srv1 = addSrv( 'srv1', 'Service1' );
				srv1.manifest.components[0].user = true;

				var srv2 = addSrv( 'srv2', 'Service2' );
				srv2.manifest.components[0].user = true;

				//	Set deps srv1 -> srv2
				srv1.manifest.components[0].dependencies = [srv2.id];

				initSrvMgr( { reg: _reg, services: [srv1,srv2] }, function(obj) {
					_srvMgr = obj;

					var deps = _srvMgr.getDependencies( srv1.id );
					assert.isArray( deps );
					assert.strictEqual( 1, deps.length );
					assert.strictEqual( srv2.id, deps[0] );
					done();
				});
			});
		});

		describe('getConfigApp()', function() {
			it('should return service config app id if service exists', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].configure = "configure.html";
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					var cfgAppId = _srvMgr.getConfigApp( srv.id );
					assert.strictEqual( 'file://' + path.join(srv.path, 'configure.html'), cfgAppId );
					done();
				});
			});

			it('should return service config app id if service exists', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					var cfgAppId = _srvMgr.getConfigApp( srv.id );
					assert.strictEqual( undefined, cfgAppId );
					done();
				});
			});

			it('should return undefined if service do not exist', function(done) {
				initSrvMgr( {reg: _reg}, function(obj) {
					_srvMgr = obj;
					var cfgAppId = _srvMgr.getConfigApp('pepe');
					assert.isUndefined( cfgAppId );
					done();
				});
			});
		});

		describe('resetConfig(id)', function() {
			it('should not reset data if service is invalid', function(done) {
				initSrvMgr( {reg: _reg}, function(obj) {
					_srvMgr = obj;
					_srvMgr.resetConfig('test',function(err) {
						assert.isDefined( err );
						assert.equal( err.message, 'Invalid service' );
						done();
					});
				});
			});

			it('should not reset data if service is invalid, no callback provided', function(done) {
				initSrvMgr( {reg: _reg}, function(obj) {
					_srvMgr = obj;
					_srvMgr.resetConfig('pepe');
					done();
				});
			});

			it('should reset data if service is valid and running', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;
					assert.isTrue( _srvMgr.isRunning( srv.id ) );
					_srvMgr.resetConfig( srv.id,function() {
						assert.isTrue( _srvMgr.isRunning( srv.id ) );
						done();
					});
				});
			});
		});

		describe('getAllRunning()', function() {
			it('should return all services running', function(done) {
				var srv1 = addSrv( 'srv1', 'Service1' );
				var srv2 = addSrv( 'srv2', 'Service2' );
				srv2.manifest.components[0].runs = 'onDemand';
				initSrvMgr( {reg: _reg, services: [srv1,srv2]}, function(obj) {
					_srvMgr = obj;
					assert.equal( _srvMgr.isRunning( srv1.id ), true );
					var all = _srvMgr.getAllRunning();
					assert.equal( all.length, 1 );
					assert.equal( all[0], srv1.id );
					done();
				});
			});
		});

		describe('isRunning(id)', function() {
			it('should return true when the service is running', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;
					_srvMgr.run( srv.id ).then(function() {
						assert.isTrue( _srvMgr.isRunning(srv.id) );
						done();
					});
				});
			});

			it('should return false when the service is not running', function(done) {
				initSrvMgr( {reg: _reg}, function(obj) {
					_srvMgr = obj;
					assert.isFalse( _srvMgr.isRunning('pepe') );
					done();
				});
			});

			it('should return false when the service was stopped', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;
					_srvMgr.run( srv.id ).then(function() {
						_srvMgr.stop( srv.id ).then(function() {
							assert.isFalse( _srvMgr.isRunning(srv.id) );
							done();
						});
					});
				});
			});
		});

		describe('dump()', function() {
			it('should not return anything if service not exists', function(done) {
				initSrvMgr( {reg: _reg}, function(obj) {
					_srvMgr = obj;
					assert.equal( _srvMgr.dump('pepe'), undefined );
					done();
				});
			});

			it('should return dump service information if service exists', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;
					assert.deepEqual( _srvMgr.dump( srv.id ), {
						"id": "srv1",
						"pkgID": "srv1",
						"type": "service",
						"main": "/system/original/packages/srv1/main.js",
						"dependencies": [],
						"name": "Service1",
						"description": "A description",
						"version": "1.0.0",
						"vendor": "PEPE",
						"vendor-url": "http://pepe.com/",
						"system": false,
						"user": false,
						"runs": "onStartup",
						"configure": undefined
					});
					done();
				});
			});
		});

		describe('pkg manager', function() {
			it('should start a autoStart service if a new package added', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );

				//	Init mockFS
				initFS( [srv] );

				//	Setup package manager
				_reg.put( 'pkgmgr', new Mocks.PackageManager([]) );

				//	Setup service manager
				var srvMgr = new ServiceManager();
				initSrv( _reg, srvMgr, function(obj) {
					_srvMgr = obj;

					_srvMgr.on('started', function(id) {
						assert.isTrue( id === srv.id );
						assert.isTrue( _srvMgr.isRunning(srv.id) );
						done();
					});

					var pkgMgr = _reg.get('pkgmgr');
					pkgMgr.installEx( srv.manifest, function(err,id) {
						assert.equal( err, undefined );
					});
				});
			});

			it('should not start a no-autostart service if a new package added', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';

				//	Init mockFS
				initFS( [srv] );

				//	Setup package manager
				_reg.put( 'pkgmgr', new Mocks.PackageManager([]) );

				//	Setup service manager
				var srvMgr = new ServiceManager();
				initSrv( _reg, srvMgr, function(obj) {
					_srvMgr = obj;

					_srvMgr.on('loaded', function(id) {
						assert.isTrue( id === srv.id );
						assert.equal( _srvMgr.isRunning(srv.id), false );
						done();
					});

					var pkgMgr = _reg.get('pkgmgr');
					pkgMgr.installEx( srv.manifest, function(err,id) {
						assert.equal( err, undefined );
					});
				});
			});

			it('should not save state when uninstall a service', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';

				//	Init mockFS
				initFS( [srv] );

				//	Setup package manager
				_reg.put( 'pkgmgr', new Mocks.PackageManager([]) );

				//	Setup service manager
				var srvMgr = new ServiceManager();
				initSrv( _reg, srvMgr, function(obj) {
					_srvMgr = obj;

					var loaded=-1;
					_srvMgr.on('loaded', function(id) {
						loaded++;
						assert.equal( id, srv.id );
						assert.equal( _srvMgr.isRunning(srv.id), false );
						if (loaded === 1) {
							done();
						}
					});

					var count=-1;
					_srvMgr.on('started', function(id) {
						count++;
						assert.equal( id, srv.id );
						if (count === 0) {
							assert.equal( _srvMgr.isRunning(srv.id), true );
						}
						else {
							assert(false);
						}
					});

					var pkgMgr = _reg.get('pkgmgr');
					pkgMgr.installEx( srv.manifest, function(err,id) {
						assert.equal( err );
						_srvMgr.enable( srv.id, true, function(err) {
							assert.equal( err );
							assert.isTrue( _srvMgr.isEnabled(srv.id) );
							pkgMgr.uninstall( srv.id, function(id) {
								assert.equal( srv.id, id );
								pkgMgr.installEx( srv.manifest, function(err,id) {
									assert.isFalse( _srvMgr.isEnabled(srv.id) );
									assert.equal( err );
								});
							});
						});
					});
				});
			});

			it('should save state when update a service', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';

				//	Init mockFS
				initFS( [srv] );

				//	Setup package manager
				_reg.put( 'pkgmgr', new Mocks.PackageManager([]) );

				//	Setup service manager
				var srvMgr = new ServiceManager();
				initSrv( _reg, srvMgr, function(obj) {
					_srvMgr = obj;

					var loaded=-1;
					_srvMgr.on('loaded', function(id) {
						loaded++;
						assert.equal( id, srv.id );
						assert.equal( _srvMgr.isRunning(srv.id), false );
					});

					var count=-1;
					_srvMgr.on('started', function(id) {
						count++;
						assert.equal( id, srv.id );
						if (count === 0) {
							assert.equal( _srvMgr.isRunning(srv.id), true );
						}
						else if (count === 1) {
							done();
						}
					});

					var pkgMgr = _reg.get('pkgmgr');
					pkgMgr.installEx( srv.manifest, function(err,id) {
						assert.equal( err );
						_srvMgr.enable( srv.id, true, function(err) {
							assert.equal( _srvMgr.isEnabled(srv.id), true );
							assert.equal( err );
							pkgMgr.update( {id:srv.id,url:'pepe'}, function(err,id) {
								assert.equal( srv.id, id );
								assert.equal( _srvMgr.isEnabled(srv.id), true );
							});
						});
					});
				});
			});

			it('should not load a invalid service when a new package added', function(done) {
				initSrvMgr( { reg: _reg }, function(obj) {
					_srvMgr = obj;

					var pkgMgr = _reg.get('pkgmgr');
					var options = {
						id: 'pepe'
					};
					pkgMgr.install( options, function() {
						done();
					});
				});
			});

			it('should not load again a service', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					var pkgMgr = _reg.get('pkgmgr');
					var options = {
						id: srv.id
					};
					pkgMgr.install( options, function() {
						done();
					});
				});
			});

			it('should stop service when an already started service is uninstalled', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					assert.isTrue( _srvMgr.isRunning( srv.id ) );
					var pkgMgr = _reg.get('pkgmgr');
					pkgMgr.uninstall( srv.id, function(id) {
						assert.isTrue( id === srv.id );
						assert.isFalse( _srvMgr.isRunning(srv.id) );
						assert.isFalse( _srvMgr.getAll().indexOf(srv.id) === 0 );
						assert.isNull( _srvMgr.get(srv.id) );
						done();
					});
				});
			});

			it('should remove service when an valid service is uninstalled', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					assert.isFalse( _srvMgr.isRunning( srv.id ) );
					var pkgMgr = _reg.get('pkgmgr');
					pkgMgr.uninstall( srv.id, function(id) {
						assert.isTrue( id === srv.id );
						assert.isFalse( _srvMgr.isRunning(srv.id) );
						assert.isFalse( _srvMgr.getAll().indexOf(srv.id) === 0 );
						assert.isNull( _srvMgr.get(srv.id) );
						done();
					});
				});
			});

			it('should stop all deps before stop a service', function(done) {
				var srv1 = addSrv( 'srv1', 'Service1' );
				var srv2 = addSrv( 'srv2', 'Service2' );
				//	srv2 -> srv1
				srv2.manifest.components[0].dependencies = [srv1.id];
				initSrvMgr( {reg: _reg, services: [srv2,srv1]}, function(obj) {
					_srvMgr = obj;

					assert.isTrue( _srvMgr.isRunning( srv1.id ) );
					assert.isTrue( _srvMgr.isRunning( srv2.id ) );
					var pkgMgr = _reg.get('pkgmgr');
					pkgMgr.uninstall( srv1.id, function(id) {
						assert.isTrue( id === srv1.id );

						//	Tests on srv1
						assert.isFalse( _srvMgr.isRunning(srv1.id) );
						assert.isFalse( _srvMgr.getAll().indexOf(srv1.id) === 0 );
						assert.isNull( _srvMgr.get(srv1.id) );

						//	Tests on srv2
						assert.isFalse( _srvMgr.isRunning(srv2.id) );
						assert.isFalse( _srvMgr.getAll().indexOf(srv2.id) === 0 );
						assert.isObject( _srvMgr.get(srv2.id) );

						done();
					});
				});
			});

			it('should handle a onDemand service update stopped', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					assert.isFalse( _srvMgr.isRunning( srv.id ) );
					var pkgMgr = _reg.get('pkgmgr');
					pkgMgr.update( {id:srv.id, url: 'pepe'}, function(err,id) {
						assert.isTrue( id === srv.id );
						assert.isFalse( _srvMgr.isRunning(srv.id) );
						assert.isFalse( _srvMgr.getAll().indexOf(srv.id) === 0 );
						assert.isObject( _srvMgr.get(srv.id) );
						done();
					});
				});
			});

			it('should handle a onDemand service update started', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;
					_srvMgr.run( srv.id ).then(function() {
						assert.isTrue( _srvMgr.isRunning( srv.id ) );
						var pkgMgr = _reg.get('pkgmgr');
						pkgMgr.update( {id:srv.id, url: 'pepe'}, function(err,id) {
							assert.isTrue( id === srv.id );
							//	TODO: Deberia quedar corriendo?
							//	Si el servicio estaba corriendo porque había uno que dependía de él, debería hacer
							//	stop del servicio al recibir update y al recibir updated correrlo de vuelta
							assert.isFalse( _srvMgr.isRunning(srv.id) );
							assert.isFalse( _srvMgr.getAll().indexOf(srv.id) === 0 );
							assert.isObject( _srvMgr.get(srv.id) );
							done();
						});
					});
				});
			});

			it('should handle a onStarted service update', function(done) {
				var srv = addSrv( 'srv1', 'Service1' );
				initSrvMgr( {reg: _reg, services: [srv]}, function(obj) {
					_srvMgr = obj;

					_srvMgr.on('started', function(id) {
						assert.equal( id,  srv.id );
						assert.isTrue( _srvMgr.isRunning(srv.id) );
						assert.isFalse( _srvMgr.getAll().indexOf(srv.id) === 0 );
						assert.isObject( _srvMgr.get(srv.id) );
						done();
					});

					assert.isTrue( _srvMgr.isRunning( srv.id ) );
					var pkgMgr = _reg.get('pkgmgr');
					pkgMgr.update( {id:srv.id, url: 'pepe'}, function(err,id) {
						assert.equal( err );
						assert.equal( id,  srv.id );
					});
				});
			});
		});

		describe('enable', function() {
			it('should not enable a invalid service', function(done) {
				initSrvMgr( {reg:_reg}, function(obj) {
					_srvMgr = obj;
					assert.isFalse( _srvMgr.isEnabled( 'invalid' ) );
					_srvMgr.enable( 'invalid', true, function(err) {
						assert.instanceOf( err, Error, 'Service not found: id=invalid' );
						done();
					});
				});
			});

			it('should not disable a invalid service', function(done) {
				initSrvMgr( {reg:_reg}, function(obj) {
					_srvMgr = obj;
					_srvMgr.enable( 'invalid', false, function(err) {
						assert.instanceOf( err, Error, 'Service not found: id=invalid' );
						done();
					});
				});
			});

			it('should not throw error if enable fail to start a disabled service', function(done) {
				var srv = addSrv( 'srv', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';
				srv.main = function() {
					return {
						onStart: function(cb) { cb(new Error('pepe')); },
						onStop: function(cb) { cb(); }
					};
				};

				initSrvMgr( {reg:_reg, services:[srv]}, function(obj) {
					assert.isObject(obj);
					_srvMgr = obj;

					assert.isFalse( _srvMgr.isEnabled( srv.id ) );
					assert.isFalse( _srvMgr.isRunning( srv.id ) );
					_srvMgr.enable( srv.id, true, function(err) {
						assert.equal(err);
						assert.isFalse( _srvMgr.isRunning( srv.id ) );
						done();
					});

				});
			});

			it('should not enable a onStartup service', function(done) {
				var srv = addSrv( 'srv', 'Service1' );

				initSrvMgr( {reg:_reg, services:[srv]}, function(obj) {
					assert.isObject(obj);
					_srvMgr = obj;

					assert.isTrue( _srvMgr.isEnabled( srv.id ) );
					assert.isTrue( _srvMgr.isRunning( srv.id ) );
					_srvMgr.enable( srv.id, true, function(err) {
						assert.instanceOf( err, Error, 'Service already enabled/disabled: id=srv, state=1');
						done();
					});

				});
			});

			it('should not disable a onDemand service', function(done) {
				var srv = addSrv( 'srv', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';

				initSrvMgr( {reg:_reg, services: [srv]}, function(obj) {
					assert.isObject(obj);
					_srvMgr = obj;

					assert.isFalse( _srvMgr.isEnabled( srv.id ) );
					assert.isFalse( _srvMgr.isRunning( srv.id ) );
					_srvMgr.enable( srv.id, false, function(err) {
						assert.instanceOf( err, Error, 'Service already enabled/disabled: id=srv, state=0');
						done();
					});
				});
			});

			it('should enable a onDemand service', function(done) {
				var srv = addSrv( 'srv', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';

				initSrvMgr( {reg:_reg, services:[srv]}, function(obj) {
					assert.isObject(obj);
					_srvMgr = obj;

					assert.isFalse( _srvMgr.isEnabled( srv.id ) );
					assert.isFalse( _srvMgr.isRunning( srv.id ) );
					_srvMgr.enable( srv.id, true, function(err) {
						assert.isTrue( _srvMgr.isEnabled( srv.id ) );
						assert.isTrue( _srvMgr.isRunning( srv.id ) );
						assert.isUndefined(err);
						done();
					});
				});
			});

			it('should enable a onDemand service and save state', function(done) {
				var srv = addSrv( 'srv', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';

				initSrvMgr( {reg:_reg, services:[srv]}, function(obj) {
					assert.isObject(obj);
					_srvMgr = obj;

					//	Enable!
					assert.isFalse( _srvMgr.isEnabled( srv.id ) );
					assert.isFalse( _srvMgr.isRunning( srv.id ) );
					_srvMgr.enable( srv.id, true, function(err) {
						//	Is enabled!
						assert.isTrue( _srvMgr.isEnabled( srv.id ) );
						assert.isTrue( _srvMgr.isRunning( srv.id ) );
						assert.isUndefined(err);

						//	Stop service manager
						_srvMgr.fin().then(function() {
							//	Start service manager again!
							initSrv( _reg, _srvMgr, function() {
								assert.isTrue( _srvMgr.isRunning( srv.id ) );
								done();
							});
						});
					});
				});
			});

			it('should enable/disable/enable a onDemand service', function(done) {
				var srv = addSrv( 'srv', 'Service1' );
				srv.manifest.components[0].runs = 'onDemand';

				initSrvMgr( {reg:_reg, services:[srv]}, function(obj) {
					assert.isObject(obj);
					_srvMgr = obj;

					//	Enable service!
					assert.isFalse( _srvMgr.isEnabled( srv.id ) );
					assert.isFalse( _srvMgr.isRunning( srv.id ) );
					_srvMgr.enable( srv.id, true, function(err) {
						assert.isUndefined(err);

						//	Disable service!
						assert.isTrue( _srvMgr.isEnabled( srv.id ) );
						assert.isTrue( _srvMgr.isRunning( srv.id ) );
						_srvMgr.enable( srv.id, false, function(err) {
							assert.isUndefined(err);

							//	Enable service!
							assert.isFalse( _srvMgr.isEnabled( srv.id ) );
							assert.isFalse( _srvMgr.isRunning( srv.id ) );
							_srvMgr.enable( srv.id, true, function(err) {
								assert.isUndefined(err);
								done();
							});
						});
					});
				});
			});

			it('should disable a onStartup service', function(done) {
				var srv = addSrv( 'srv', 'Service1' );

				initSrvMgr( {reg:_reg, services: [srv]}, function(obj) {
					assert.isObject(obj);
					_srvMgr = obj;

					assert.isTrue( _srvMgr.isEnabled( srv.id ) );
					assert.isTrue( _srvMgr.isRunning( srv.id ) );
					_srvMgr.enable( srv.id, false, function(err) {
						assert.isUndefined(err);
						assert.isFalse( _srvMgr.isEnabled( srv.id ) );
						assert.isFalse( _srvMgr.isRunning( srv.id ) );
						done();
					});
				});
			});
		});
	});
});

