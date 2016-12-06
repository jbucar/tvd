'use strict';

var _ = require('lodash');
var mockfs = require('mock-fs');
var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var Mocks = require('mocks');

var ApplicationManager = require('../src/applicationmgr');

var testApp = Mocks.createApp( 'testID' );
var shellApp = Mocks.createApp( 'shell' );
var srvCfgApp = Mocks.createApp( 'srv_cfg' );
srvCfgApp.component.user = false;
var usrSrvCfg = Mocks.createApp( 'usrsrv.cfg' );
usrSrvCfg.component.user = false;
var emptyApp = Mocks.createApp( 'empty', false );

Mocks.changeCategories( shellApp, ["system","shell"] );
shellApp.manifest.components[0].user = false;

var usrSrvManifest = {
	'id': 'ar.edu.unlp.info.lifia.tvd.usrsrv',
	'vendor': 'LIFIA',
	'vendor-url': 'http://lifia.info.unlp.edu.ar/',
	'version': '1.0.0',
	'api-level': '1.0',
	'platform': 'all',
	'components': [{
		id: 'ar.edu.unlp.info.lifia.tvd.usrsrv',
		type: 'service',
		main: 'main.js',
		name: 'User service',
		configure: 'config.html',
	},{
		id: 'ar.edu.unlp.info.lifia.tvd.otherusrsrv',
		type: 'service',
		main: 'main.js',
		name: 'User service',
		configure: 'config.html',
	}]
};

//	Default packages
var defaultPkgs = [testApp.manifest,shellApp.manifest,emptyApp.manifest, srvCfgApp.manifest];

describe('ApplicationManager', function() {
	var reg = null;
	var adapter = null;

	function initFS() {
		var defaultFS = {};
		defaultFS[Mocks.rwPath()] = {};
		defaultFS[testApp.installPath] = testApp.installDir;
		defaultFS[shellApp.installPath] = shellApp.installDir;
		defaultFS[emptyApp.installPath] = emptyApp.installDir;
		defaultFS[srvCfgApp.installPath] = srvCfgApp.installDir;
		defaultFS[usrSrvCfg.installPath] = usrSrvCfg.installDir;
		mockfs(defaultFS);
	}

	function getInfo(data,isFav,canUninstall){
		let info = _.cloneDeep(data);
		info.isFavorite = (isFav === undefined) ? false : isFav;
		info.canUninstall = (canUninstall === undefined) ? false : canUninstall;
		return info
	}

	beforeEach(function() {
		reg = Mocks.init('verbose');
		initFS();
		var mFactory = new Mocks.Factory();
		mFactory.addConfig( 'shell', 'ar.edu.unlp.info.lifia.tvd.shell' );
		mFactory.addConfig( 'store', 'ar.edu.unq.utics.tac.tienda' );
		reg.put( 'platform', mFactory );
		reg.put( 'screen', new Mocks.Screen() );
		reg.put( 'pkgmgr', new Mocks.PackageManager(_.cloneDeep(defaultPkgs)) );
		reg.put( 'srvmgr', new Mocks.ServiceManager() );
		reg.put( 'browser', new Mocks.BasicBrowser(reg) );
		reg.put( 'notification', new Mocks.NotificationService(reg) );
		adapter = new Mocks.ServiceAdapter(reg,'ar.edu.unlp.info.lifia.tvd.appmgr');
	});

	afterEach(function() {
		Mocks.fin();
		mockfs.restore();
		adapter = null;
		reg = null;
	});

	describe('constructor', function() {
		function checkApi(api) {
			assert.isObject(api);
			assert.isFunction(api.runAsync);
			assert.isFunction(api.stop);
			assert.isFunction(api.getAll);
			assert.isFunction(api.isRunning);
			assert.isFunction(api.getShell);
			assert.isFunction(api.getWebAPI);
			assert.isFunction(api.getCategories);
			assert.isFunction(api.toggleFavorite);
			assert.isFunction(api.isFavorite);
			assert.isTrue( api.getWebAPI().name === 'appmgr' );
			assert.isFunction(api.dump);
		}

		it('should construct basic application manager', function() {
			new ApplicationManager();
		});

		it('should start application manager and fix shell if corrupt', function(done) {
			// Corrupt shell
			adapter.load = function( file, defaultsValues ) { defaultsValues.shell = 'pepe'; return defaultsValues; };

			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal( err );
				mgr.onStop(done);
			});
		});

		it('should check api from application manager', function(done) {
			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal( err );
				checkApi(api);
				mgr.onStop(done);
			});
		});

		it('should convert old config', function(done) {
			adapter.load = function( file, defaultsValues, convert ) {
				var cfg = _.cloneDeep(defaultsValues);
				var newCfg = convert(cfg);
				assert.deepEqual( cfg, newCfg );
				return cfg;
			};
			var mgr = new ApplicationManager( adapter );
			mgr.onStart(function(err,api) {
				assert.equal( err );
				mgr.onStop(done);
			});
		});

		it('should load custom webapp', function(done) {
			adapter.load = function( file, defaultsValues ) {
				var cfg = _.cloneDeep(defaultsValues);
				cfg.webApps = testApp.id;
				return cfg;
			};
			var mgr = new ApplicationManager( adapter );
			mgr.onStart(function(err,api) {
				assert.equal( err );
				assert.equal(testApp.id, api.getWebApps());
				mgr.onStop(done);
			});
		});

		it('should start/stop application manager', function(done) {
			var mgr = new ApplicationManager( adapter );
			mgr.onStart(function(err,api) {
				assert.equal( err );
				checkApi(api);
				mgr.onStop(function() {
					assert.isFalse( api.isRunning( api.getShell() ) );
					done();
				});
			});
		});

		it('should start/stop/start/stop application manager', function(done) {
			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal(err);
				checkApi(api);
				mgr.onStop(function() {
					assert.isFalse( api.isRunning( api.getShell() ) );
					mgr.onStart(function(err,api) {
						assert.equal(err);
						checkApi(api);
						mgr.onStop(function() {
							assert.isFalse( api.isRunning( api.getShell() ) );
							done();
						});
					});
				});
			});
		});

		it('should create shell instance', function(done) {
			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal( err );
				var app = api.addInstance( api.getShell(), 'nuevoID' );
				assert.isObject(app);
				assert.equal( app.info.id, 'nuevoID' );
				assert.equal( app.info.user, false );

				var appFromApi = api.get( 'nuevoID' );
				assert.isObject( appFromApi );
				assert.deepEqual( getInfo(app.info,false,true), appFromApi );
				mgr.onStop(done);
			});
		});

		it('should not create more than one shell instance with same id', function(done) {
			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal(err);

				var app = api.addInstance( api.getShell(), 'nuevoID' );
				assert.isObject(app);
				assert.equal( app.info.id, 'nuevoID' );
				assert.equal( app.info.user, false );

				var appFromApi = api.get( 'nuevoID' );
				assert.isObject( appFromApi );
				assert.deepEqual( getInfo(app.info,false,true), appFromApi );

				assert.strictEqual( app, api.addInstance(api.getShell(), 'nuevoID') );
				mgr.onStop(done);
			});
		});

		it('should return error when remove a invalid instance', function(done) {
			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal(err);
				api.rmInstance( 'nuevoID', function(err) {
					assert.equal( err, 'Error: Application not found: id=nuevoID' );
					mgr.onStop(done);
				});
			});
		});

		it('should create/remove shell instance', function(done) {
			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal(err);
				var app = api.addInstance( api.getShell(), 'nuevoID' );
				assert.isObject(app);
				api.rmInstance( 'nuevoID', function(err) {
					assert.equal( err );
					assert.isNull( api.get('nuevoID') );
					mgr.onStop(done);
				});
			});
		});

		it('should start if a corrupted application is present', function(done) {
			var pkgMgr = reg.get('pkgmgr');
			var oldInfo = pkgMgr.getComponentInfo;
			pkgMgr.getComponentInfo = function( compID ) {
				if (compID === testApp.id) {
					return null;
				}
				return oldInfo(compID);
			};

			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal(err);
				mgr.onStop(done);
			});
		});

		it('should remove a running application: view bug #678', function(done) {
			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);

				var b = reg.get( 'browser' );
				b.doAuto(true);

				//	Replace pkgmgr?
				var pkgMgr = reg.get('pkgmgr');


				reg.get('srvmgr').ready(true);
				reg.get('screen').setActive(true);
				reg.get('browser').ready(true);

				api.on('ready', function(state) {
					api.runAsync( {id:testApp.id}, function() {} );
				});

				api.on('application', function(evt) {
					if (evt.id === testApp.id) {
						if (evt.state === 'loaded') {
							api.rmInstance( testApp.id, function() {
								api.stop( testApp.id, function() {
								});
							});
						}
						else if (evt.state === 'stopped') {
							mgr.onStop(function() {
								done();
							});
						}
					}
				});
			});
		});

		it('should start/stop with a running application', function(done) {
			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal(err);
				checkApi(api);
				api.runAsync( {id:testApp.id}, function(err) {
					assert.equal( err, undefined );

					//	Close via browser
					var b = reg.get( 'browser' );
					b.closeEvent( 1 );

					mgr.onStop(done);
				});
			});
		});

		it('should start/stop with a running application (backward compatibility)', function(done) {
			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal(err);
				checkApi(api);
				api.run( testApp.id, function(err, result) {
					assert.isUndefined( err );
					assert.isTrue( result );

					//	Close via browser
					var b = reg.get( 'browser' );
					b.closeEvent( 1 );

					mgr.onStop(done);
				});
			});
		});

		it('should restart shell if closed with error', function(done) {
			var mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,api) {
				assert.equal(err);
				checkApi(api);
				reg.put('appmgr', api);

				//	Close via browser
				var b = reg.get( 'browser' );
				reg.get('srvmgr').ready(true);
				reg.get('screen').setActive(true);
				b.ready(true);

				var fin = false;
				api.on('application', function(evt) {
					if (evt.state === 'launching' && evt.id === "ar.edu.unlp.info.lifia.tvd.shell.system_layer") {
						b.loadedEvent( 1 );
					}

					if (evt.state === 'launching' && evt.id === "ar.edu.unlp.info.lifia.tvd.shell.shell") {
						b.loadedEvent( 2 );
					}

					if (evt.state === 'loaded' && evt.id === "ar.edu.unlp.info.lifia.tvd.shell.shell") {
						if (fin) {
							done();
						} else {
							b.closeEvent( 2, 'Shell error' );
							b.loadedEvent( 2 );
							fin = true;
						}
					}
				});
			});
		});

		describe('events',function() {
			it('should unload a uninstalled application and reset state', function(done) {
				var mgr = new ApplicationManager(adapter);
				mgr.onStart(function(err,api) {
					assert.equal(err);

					//	Replace pkgmgr?
					var pkgMgr = reg.get('pkgmgr');

					assert.isObject(api);
					api.toggleFavorite( testApp.id );
					assert.isTrue( api.isFavorite( testApp.id ) );
					pkgMgr.uninstall( testApp.id, function(id) {
						assert.isTrue( id === testApp.id );
						assert.isFalse( api.isFavorite( testApp.id ) );
						mgr.onStop(function() {
							done();
						});
					});
				});
			});

			it('should unload a uninstalled application without components', function(done) {
				var mgr = new ApplicationManager(adapter);
				mgr.onStart(function(err,api) {
					assert.equal(err);

					//	Replace pkgmgr?
					var pkgMgr = reg.get('pkgmgr');
					assert.isObject(api);
					pkgMgr.uninstall( emptyApp.id, function(id) {
						assert.isTrue( id === emptyApp.id );
						mgr.onStop(done);
					});
				});
			});

			it('should unload/load a updated application and persist state', function(done) {
				var mgr = new ApplicationManager(adapter);
				mgr.onStart(function(err,api) {
					assert.equal(err);

					//	Replace pkgmgr?
					var pkgMgr = reg.get('pkgmgr');

					assert.isObject(api);
					api.toggleFavorite( testApp.id );
					assert.isTrue( api.isFavorite( testApp.id ) );
					pkgMgr.update( {id:testApp.id,url:'pepe'}, function(err,id) {
						assert.equal( id, testApp.id );
						assert.isTrue( api.isFavorite( testApp.id ) );
						mgr.onStop(function() {
							done();
						});
					});
				});
			});

			it('should load a installed application', function(done) {
				var mgr = new ApplicationManager(adapter);
				mgr.onStart(function(err,api) {
					assert.equal(err);

					//	Replace pkgmgr?
					var pkgMgr = reg.get('pkgmgr');
					assert.isObject(api);
					pkgMgr.emit( 'install', testApp.id );

					mgr.onStop(done);
				});
			});

			it('should catch any error when load a invalid new application', function(done) {
				var pkgMgr = reg.get('pkgmgr');
				var oldInfo = pkgMgr.getComponentInfo;
				var mgr = new ApplicationManager(adapter);
				mgr.onStart(function(err,api) {
					assert.equal(err);

					//	Remove instance
					api.rmInstance( testApp.id, function() {
						pkgMgr.getComponentInfo = function( compID ) {
							if (compID === testApp.id) {
								return null;
							}
							return oldInfo(compID);
						};

						//	Replace pkgmgr?
						assert.isObject(api);
						pkgMgr.emit( 'install', testApp.id );

						mgr.onStop(done);
					});
				});
			});
		});
	});

	describe('methods', function() {
		var mgr = null;
		var api = null;

		beforeEach(function(done) {
			mgr = new ApplicationManager(adapter);
			mgr.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				done();
			});
		});

		afterEach(function(done) {
			mgr.onStop(done);
		});

		describe('dump', function() {
			it('should returns state when no app running', function() {
				var state = {
					"apps": {},
					"shell": "ar.edu.unlp.info.lifia.tvd.shell",
					"webApp": "ar.edu.unq.utics.tac.tienda"
				};
				assert.deepEqual( api.dump(), state );
			});

			it('should returns state when a app running', function(done) {
				var b = reg.get( 'browser' );
				b.doAuto(true);
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					var state = {
						"apps": { "ar.edu.unlp.info.lifia.tvd.testID": getInfo(testApp.componentParsed,false,true) },
						"shell": "ar.edu.unlp.info.lifia.tvd.shell",
						"webApp": "ar.edu.unq.utics.tac.tienda"
					};
					assert.deepEqual( api.dump(), state );
					done();
				});
			});
		});

		describe('getCategories', function() {
			it('should returns a array with categories from shell and test app', function() {
				assert.deepEqual( api.getCategories(), ['FAVORITOS','PRUEBA','SYSTEM'] );
			});
		});

		describe('Favorite', function() {
			it('should returns false on test app', function() {
				assert.isFalse( api.isFavorite( testApp.id ) );
			});

			it('should returns false if app not found', function() {
				assert.isFalse( api.isFavorite( 'pepe' ) );
			});

			it('should toggle favorite', function() {
				assert.isFalse( api.isFavorite( testApp.id ) );
				api.toggleFavorite( testApp.id );
				assert.isTrue( api.isFavorite( testApp.id ) );
				api.toggleFavorite( testApp.id );
				assert.isFalse( api.isFavorite( testApp.id ) );
			});
		});

		describe('runApp', function() {
			it('should not start invalid application', function(done) {
				api.runAsync( {id:'pepe'}, function(err) {
					assert(err !== undefined);
					assert.equal( err, 'Error: Component not exists: id=pepe' );
					done();
				});
			});

			it('should not start application if browser fail', function(done) {
				var b = reg.get( 'browser' );
				b.setLauncherReturn( -1 );
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err !== undefined );
					assert.equal( err, 'Error: Cannot launch browser' );
					done();
				});
			});

			it('should not start twice a application running', function(done) {
				api.runAsync( {id:testApp.id}, function(err) {
					assert.equal( err, undefined );

					api.runAsync( {id: testApp.id}, function(err1) {
						assert( err1 !== undefined );
						assert.equal( err1, 'Error: Application already started: id=ar.edu.unlp.info.lifia.tvd.testID' );

						var b = reg.get( 'browser' );
						b.closeEvent(1);
						done();
					});
				})
			});

			it('should start/stop a valid application and receive application events', function(done) {
				var b = reg.get( 'browser' );

				var count=-1;
				api.on('application', function(evt) {
					assert.isTrue( testApp.id === evt.id );

					count++;
					if (count === 0) {
						assert.equal( evt.state, 'starting' );
					}
					else if (count === 1) {
						assert.equal( evt.state, 'launching' );
						b.launchedEvent(1);
					}
					else if (count === 2) {
						assert.equal( evt.state, 'launched' );
						b.loadedEvent(1);
					}
					else if (count === 3) {
						assert.equal( evt.state, 'loaded' );
						api.stop( testApp.id, function(err) {
							assert.isUndefined(err);
							done();
						});
					}
					else if (count === 4) {
						assert.equal( evt.state, 'stopping' );
						b.closedEvent(1);
					}
					else if (count === 5) {
						assert.equal( evt.state, 'stopped' );
					}
				});

				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
				});
			});

			it('should start a valid application and receive update package event; should stop application', function(done) {
				var b = reg.get( 'browser' );
				b.doAuto(true);
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.equal( api.isRunning(testApp.id), true );
					var pkgMgr = reg.get( 'pkgmgr' );
					pkgMgr.update( {id:testApp.id,url:'pepe'}, function(err,id) {
						assert.equal( err );
						assert.equal( id, testApp.id );
						assert.isFalse( api.isRunning(testApp.id) );
						assert.isObject( api.get(testApp.id) );
						done();
					});
				});
			});

			it('should start a valid application and receive uninstall package event', function(done) {
				var b = reg.get( 'browser' );
				b.doAuto(true);

				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.equal( api.isRunning(testApp.id), true );
					var pkgMgr = reg.get( 'pkgmgr' );
					pkgMgr.uninstall( testApp.id, function(/*id*/) {
						assert.isFalse( api.isRunning(testApp.id) );
						done();
					});
				});
			});

			it('should start/stop/start a valid application', function(done) {
				var b = reg.get( 'browser' );
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.isTrue( api.isRunning(testApp.id) );
					b.closeEvent(1);
					api.stop( testApp.id, function(err) {
						assert( err === undefined );
						assert.isFalse( api.isRunning(testApp.id) );
						api.runAsync( {id:testApp.id}, function(err) {
							assert( err === undefined );
							assert.isTrue( api.isRunning( testApp.id ) );
							b.closeEvent(1);
							api.stop( testApp.id, done );
						});
					});
				});
			});

			it('should start a valid application and check notifications', function(done) {
				var b = reg.get( 'browser' );
				b.doAuto(true);

				var count=0;
				api.on('application', function(evt) {
					if (evt.state == 'starting') {
						count++;
					}
				});

				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					api.stop( testApp.id, function() {
						assert( err === undefined );
						assert.isFalse( api.isRunning(testApp.id) );
						api.runAsync( {id:testApp.id}, function(err) {
							assert( err === undefined );
							api.stop( testApp.id, function(err) {
								assert( err === undefined );
								assert.equal( count, 2 );
								done();
							});
						});
					});
				});
			});

			it('should get api events', function(done) {
				var srvIdent = {
					'ident': function(param) { assert(this === srvIdent); return param; },
					'ident_async': function(param,cb) {
						assert(this === srvIdent);
						cb(undefined,param);
					},
					'getWebAPI': function() {
						return { 'name': 'ident' };
					}
				};
				srvIdent.ident_async.isAsync = true;
				reg.put( 'ident', srvIdent );
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.isTrue( api.isRunning(testApp.id) );
					var b = reg.get( 'browser' );
					b.notifyUp(1);
					b.apiEvent( 1, 'ident', 'ident', ['hello'], function(resp) {
						assert.isObject( resp );
						assert.isUndefined( resp.error );
						assert.isArray( resp.data );
						assert.isTrue( resp.data[0] === 'hello' );
						assert.isFalse( resp.isSignal );
						b.apiEvent( 1, 'ident', 'ident_async', ['hello'], function(resp2) {
							assert.isObject( resp2 );
							assert.isUndefined( resp2.error );
							assert.isArray( resp2.data );
							assert.isTrue( resp2.data[0] === 'hello' );
							assert.isFalse( resp2.isSignal );
							b.closeEvent(1);
							done();
						});
					});
				});
			});

			it('should check returns values on api events', function(done) {
				var srvIdent = {
					'ident': function(param) { return param; },
					'null': function() { return null; },
					'undef': function() { return undefined; },
					'getWebAPI': function() {
						return { 'name': 'ident' };
					}
				};
				reg.put( 'ident', srvIdent );
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.isTrue( api.isRunning(testApp.id) );
					var b = reg.get( 'browser' );
					b.notifyUp(1);
					b.apiEvent( 1, 'ident', 'ident', ['hello'], function(resp) {
						assert.isObject(resp);
						assert.isUndefined( resp.error );
						assert.isArray( resp.data );
						assert.isTrue( resp.data[0] === 'hello' );
						assert.isFalse( resp.isSignal );
						b.apiEvent( 1, 'ident', 'null', ['hello'], function(resp2) {
							assert.isObject(resp2);
							assert.isUndefined( resp2.error );
							assert.isArray( resp2.data );
							assert.isTrue( resp2.data[0] === null );
							assert.isFalse( resp2.isSignal );
							b.apiEvent( 1, 'ident', 'undef', ['hello'], function(resp3) {
								assert.isObject(resp3);
								assert.isUndefined( resp3.error );
								assert.isUndefined( resp3.data );
							});
							setTimeout( function() {
								//	Wait for undef callback .....
								b.closeEvent(1);
								done();
							}, 10 );
						});
					});
				});
			});

			it('should check service exists in api events', function(done) {
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.isTrue( api.isRunning(testApp.id) );
					var b = reg.get( 'browser' );
					b.notifyUp(1);
					b.apiEvent( 1, 'ident', 'ident', ['hello'], function(res) {
						assert.isTrue( res.error !== undefined );
						assert.strictEqual( res.error.message, 'Registry API call cannot be processed; appID=ar.edu.unlp.info.lifia.tvd.testID, apiID=ident, apiName=undefined, method=ident, err=Service not found');
						assert.isUndefined( res.data );
						b.closeEvent(1);
						done();
					});
				});
			});

			it('should check service method exists in api events', function(done) {
				var srvIdent = {
					'ident1': function(param) { return param; }
				};
				reg.put( 'ident', srvIdent );
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.isTrue( api.isRunning(testApp.id) );
					var b = reg.get( 'browser' );
					b.notifyUp(1);
					b.apiEvent( 1, 'ident', 'ident', ['hello'], function(res) {
						assert.isTrue( res.error !== undefined );
						assert.strictEqual( res.error.message, 'Registry API call cannot be processed; appID=ar.edu.unlp.info.lifia.tvd.testID, apiID=ident, apiName=undefined, method=ident, err=Service not exported');
						assert.isUndefined( res.data );
						b.closeEvent(1);
						done();
					});
				});
			});

			it('should check service is exported', function(done) {
				var srvIdent = {
					'identFnc': function(param) { return param; }
				};
				reg.put( 'ident', srvIdent );
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.isTrue( api.isRunning(testApp.id) );
					var b = reg.get( 'browser' );
					b.notifyUp(1);
					b.apiEvent( 1, 'ident', 'identFnc', ['hello'], function(res) {
						assert.isTrue( res.error !== undefined );
						assert.strictEqual( res.error.message, 'Registry API call cannot be processed; appID=ar.edu.unlp.info.lifia.tvd.testID, apiID=ident, apiName=undefined, method=identFnc, err=Service not exported');
						assert.isUndefined( res.data );
						b.closeEvent(1);
						done();
					});
				});
			});

			it('should check method is exported in service api', function(done) {
				var srvIdent = {
					'identFnc': function(param) { return param; },
					'identFnc1': function(param) { return param; },
					'getWebAPI': function() {
						return {
							'name': 'ident',
							'public': ['identFnc1']
						};
					}
				};
				reg.put( 'ident', srvIdent );
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.isTrue( api.isRunning(testApp.id) );
					var b = reg.get( 'browser' );
					b.notifyUp(1);
					b.apiEvent( 1, 'ident', 'identFnc', ['hello'], function(res) {
						assert.isTrue( res.error !== undefined );
						assert.strictEqual( res.error.message, 'Registry API call cannot be processed; appID=ar.edu.unlp.info.lifia.tvd.testID, apiID=ident, apiName=undefined, method=identFnc, err=Method not exists');
						assert.isUndefined( res.data );
						b.apiEvent( 1, 'ident', 'identFnc1', ['hello'], function(res2) {
							assert.isUndefined( res2.error );
							assert.isTrue( res2.data == 'hello' );
							b.closeEvent(1);
							done();
						});
					});
				});
			});

			xit('should check method is exported in service api with multiple apis', function(done) {
				var srvIdent = {
					'identFnc': function(param) { return param; },
					'identFnc1': function(param) { return param; },
					'identFnc2': function() { return 'hello2'; },
					'getWebAPI': function() {
						return [{
							'name': 'ident1',
							'public': ['identFnc1']
						},{
							'name': 'ident2',
							'public': ['identFnc2']
						}];
					}
				};
				reg.put( 'ident', srvIdent );
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.isTrue( api.isRunning(testApp.id) );
					var b = reg.get( 'browser' );
					b.notifyUp(1);
					b.apiEvent2( 1, 'ident', 'ident1', 'identFnc', ['hello'], function(res) {
						assert.isTrue( res.error !== undefined );
						assert.strictEqual( res.error.message, 'Application API call cannot be processed; msg=Method not exported, app=ar.edu.unlp.info.lifia.tvd.testID, serviceID=ident, method=identFnc');
						assert.isUndefined( res.data );
						b.apiEvent2( 1, 'ident', 'ident1', 'identFnc1', ['hello'], function(res2) {
							assert.isUndefined( res2.error );
							assert.isArray( res2.data );
							assert.strictEqual( res2.data[0], 'hello' );
							b.apiEvent2( 1, 'ident', 'ident2', 'identFnc2', [], function(res2) {
								assert.isUndefined( res2.error );
								assert.isArray( res2.data );
								assert.strictEqual( res2.data[0], 'hello2' );
								b.closeEvent(1);
								done();
							});
						});
					});
				});
			});

			it('should check any error in method call in api events', function(done) {
				var srvIdent = {
					'ident': function() { return 'pepe'; }
				};
				reg.put( 'ident', srvIdent );
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					assert.isTrue( api.isRunning(testApp.id) );
					var b = reg.get( 'browser' );
					b.notifyUp(1);
					b.apiEvent( 1, 'ident', 'ident', ['hello'], function(res) {
						assert.isTrue( res.error !== undefined );
						assert.strictEqual( res.error.message, 'Registry API call cannot be processed; appID=ar.edu.unlp.info.lifia.tvd.testID, apiID=ident, apiName=undefined, method=ident, err=Service not exported');
						assert.isUndefined( res.data );
						b.closeEvent(1);
						done();
					});
				});
			});
		});

		describe('stopApp', function() {
			it('should not stop invalid application', function(done) {
				api.stop( 'pepe', function(err) {
					assert.equal( err, 'Error: Application not found: id=pepe' );
					assert.isFalse( api.isRunning( 'pepe' ) );
					done();
				});
			});

			it('should stop valid application and call to callback if defined', function(done) {
				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					api.stop( testApp.id, function() {
						assert.isFalse( api.isRunning( testApp.id ) );
						done();
					});
					var b = reg.get( 'browser' );
					b.closeEvent(1);
				});
			});

			it('should stop valid application using exit', function(done) {
				api.on('application', function(evt) {
					if (evt.state === 'stopped') {
						done();
					}
				});

				api.runAsync( {id:testApp.id}, function(err) {
					assert( err === undefined );
					api.exit( testApp.id );
					var b = reg.get( 'browser' );
					b.closeEvent(1);
				});
			});

			it('should stop multiples valid applications and call to callback if defined', function(done) {
				api.runAsync( {id:shellApp.id}, function(err) {
					assert.equal( err );
					api.runAsync( {id:testApp.id}, function(err) {
						assert.equal( err );

						var testAppStopped = false;
						api.stop( shellApp.id, function() {
							assert.isFalse( api.isRunning( shellApp.id ) );
							assert.isTrue( testAppStopped );
							done();
						});
						api.stop( testApp.id, function() {
							assert.isFalse( api.isRunning( testApp.id ) );
							testAppStopped = true;
						});
						var b = reg.get( 'browser' );
						b.loadedEvent(1);
						b.loadedEvent(2);
						b.closeEvent(2);
						b.closeEvent(1);
					});
				});
			});
		});

		describe('getAllRunning', function() {
			it('should return all running applications', function(done) {
				api.runAsync( {id:testApp.id}, function(err) {
					assert.equal( err );
					var all = api.getAllRunning();
					assert.strictEqual( all.length, 1 );
					assert.strictEqual( all[0], testApp.id );
					api.stop( testApp.id, function() {
						assert.isFalse( api.isRunning( testApp.id ) );
						done();
					});
					var b = reg.get( 'browser' );
					b.closeEvent(1);
				});
			});
		});

		describe('getAll', function() {
			it('should returns all apps', function() {
				var apps = api.getAll();
				assert.equal( apps.length, 1 );
				assert.deepEqual( apps[0], testApp.id );
			});
		});

		describe('get', function() {
			it('should returns a app', function() {
				var app = api.get( testApp.id );
				assert.isObject( app );
				assert.deepEqual( getInfo(app), getInfo(testApp.componentParsed) );
			});

			it('should not return a invalid app', function() {
				var app = api.get( 'pepe' );
				assert.isTrue( app === null );
			});
		});

		describe('getShell', function() {
			it('should returns shell', function() {
				assert.isTrue( api.getShell() === 'ar.edu.unlp.info.lifia.tvd.shell' );
			});
		});

		describe('isReady', function() {
			it('should check ready state', function(done) {
				var b = reg.get( 'browser' );
				b.doAuto(true);

				var count=-1;
				api.on('ready', function(state) {
					count++;
					if (count === 0) {
						assert.isTrue( state );
						assert.isTrue( api.isReady() );
						assert.isTrue( api.isRunning( api.getShell() + '.shell' ) );
						assert.isTrue( api.isRunning( api.getShell() + '.system_layer' ) );
						reg.get('notification').emit( 'ShowSystemLayer', true );
						reg.get('screen').setActive(false);
					}
					else {
						assert.isFalse( state );
						done();
					}
				});

				reg.get('srvmgr').ready(true);
				reg.get('screen').setActive(true);
				reg.get('browser').ready(true);
			});

			it('should call callback when srvmgr change to not ready', function(done) {
				var b = reg.get( 'browser' );
				b.doAuto(true);

				var count = 0;
				api.on('ready', function(state) {
					count++;
					if (count === 1) {
						assert.isTrue( state );
						assert.isTrue( api.isReady() );
						assert.isTrue( api.isRunning( api.getShell() + '.shell' ) );
						assert.isTrue( api.isRunning( api.getShell() + '.system_layer' ) );
						reg.get('notification').emit( 'ShowSystemLayer', true );
						reg.get('srvmgr').ready(false, function() {
							assert.isFalse( api.isReady() );
							done();
						});
					}
					else {
						assert.isFalse( api.isReady() );
						assert.isFalse( state );
					}
				});

				reg.get('srvmgr').ready(true);
				reg.get('screen').setActive(true);
				reg.get('browser').ready(true);
			});

			it('should not emit ready event if state not change', function(done) {
				var b = reg.get( 'browser' );
				b.doAuto(true);

				var count=-1;
				api.on('ready', function(state) {
					count++;
					if (count === 0) {
						assert.isTrue( state );
						assert.isTrue( api.isReady() );
						assert.isTrue( api.isRunning( api.getShell() + '.shell' ) );
						assert.isTrue( api.isRunning( api.getShell() + '.system_layer' ) );
						reg.get('notification').emit( 'ShowSystemLayer', true );

						// Check ready event is not emited if state don't change
						reg.get('browser').ready(true);

						// Change ready state to finalize test
						reg.get('screen').setActive(false);
					}
					else if (count === 1) {
						assert.isFalse( state );
						done();
					} else {
						done(new Error('Should not emit ready event again if state not change'));
					}
				});

				reg.get('srvmgr').ready(true);
				reg.get('screen').setActive(true);
				reg.get('browser').ready(true);
			});
		});

		describe('getWebApps', function() {
			it('should returns web applications', function() {
				assert.isTrue( api.getWebApps() === 'ar.edu.unq.utics.tac.tienda' );
			});
		});

		describe('showVirtualKeyboard', function() {
			it('should open vk when app get focus', function(done) {
				reg.put('appmgr', api);
				api.runAsync( {id: testApp.id}, function(err) {
					assert.equal( err );
					var notif = reg.get('notification');
					notif.on('VirtualKeyboard', function(evt, cfg) {
						assert.strictEqual( evt, 'show' );
						assert.deepEqual( cfg, {title: 'Test VK'} );
						notif.emit('VirtualKeyboardInput', { status: 'accepted', input: 'user input' });
					});

					var b = reg.get('browser');
					b.notifyUp(1);
					b.apiEvent(1, 'ar.edu.unlp.info.lifia.tvd.appmgr', 'showVirtualKeyboard', [{title:"Test VK"}], function(res) {
						assert.isUndefined( res.error );
						assert.isArray( res.data );
						assert.strictEqual( res.data[0].status, 'accepted' );
						assert.strictEqual( res.data[0].input, 'user input' );
						b.closeEvent(1);
						done();
					});
				});
			});

			it('should hide vk when app lost focus', function(done) {
				reg.put('appmgr', api);
				api.runAsync( {id: testApp.id}, function(err) {
					assert.equal( err );

					var notif = reg.get('notification');

					notif.on('VirtualKeyboard', function(evt, cfg) {
						assert.strictEqual( evt, 'show' );
						assert.deepEqual( cfg, {title: 'Test VK'} );
						b.apiEvent(1, 'ar.edu.unlp.info.lifia.tvd.appmgr', 'showVirtualKeyboard', [{title:"Test VK"}], function(res) {
							assert.isUndefined( res.data );
							assert.strictEqual( res.error.message, 'A virtual keyboard instance is already showing' );
							notif.emit('VirtualKeyboardInput', { status: 'accepted', input: 'user input' });
						});
					});

					var b = reg.get('browser');
					b.notifyUp(1);
					b.apiEvent(1, 'ar.edu.unlp.info.lifia.tvd.appmgr', 'showVirtualKeyboard', [{title:"Test VK"}], function(res) {
						assert.isUndefined( res.error );
						assert.isArray( res.data );
						assert.strictEqual( res.data[0].status, 'accepted' );
						assert.strictEqual( res.data[0].input, 'user input' );
						b.closeEvent(1);
						done();
					});
				});
			});

			it('should hide vk when app lost focus and show again later', function(done) {
				reg.put('appmgr', api);
				var b = reg.get('browser');
				var notif = reg.get('notification');
				var count = 0;
				notif.on('VirtualKeyboard', function(evt, cfg) {
					count++;
					switch (count) {
					case 1:
						assert.strictEqual( evt, 'show' );
						assert.deepEqual( cfg, {title: 'Test VK'} );
						api.runAsync( {id: shellApp.id}, function(err) {
							assert.equal(err);
							b.notifyUp(2);
						});
						break;
					case 2:
						assert.strictEqual( evt, 'hide' );
						assert.isUndefined( cfg );
						b.closeEvent(2);
						break;
					case 3:
						assert.strictEqual( evt, 'show' );
						assert.deepEqual( cfg, {title: 'Test VK'} );
						notif.emit('VirtualKeyboardInput', { status: 'accepted', input: 'user input' });
						break;
					}
				});

				api.runAsync( {id: testApp.id}, function(err) {
					assert.equal( err );
					b.notifyUp(1);
					b.apiEvent(1, 'ar.edu.unlp.info.lifia.tvd.appmgr', 'showVirtualKeyboard', [{title:"Test VK"}], function(res) {
						assert.isUndefined( res.error );
						assert.isArray( res.data );
						assert.strictEqual( res.data[0].status, 'accepted' );
						assert.strictEqual( res.data[0].input, 'user input' );
						b.closeEvent(1);
						done();
					});
				});
			});

			it('should not show vk when app do not have focus but show vk when it gain focus later', function(done) {
				reg.put('appmgr', api);
				var b = reg.get('browser');
				var notif = reg.get('notification');
				var canShowVK = false;
				notif.on('VirtualKeyboard', function(evt, cfg) {
					assert.isTrue( canShowVK );
					assert.strictEqual( evt, 'show' );
					assert.deepEqual( cfg, {title: 'Test VK'} );
					notif.emit('VirtualKeyboardInput', { status: 'accepted', input: 'user input' });
				});

				api.runAsync( {id: testApp.id}, function(err) {
					assert.equal( err );

					api.runAsync( {id: shellApp.id}, function(err) {
						assert.equal( err );
						b.notifyUp(1);
						b.notifyUp(2);
						b.apiEvent(1, 'ar.edu.unlp.info.lifia.tvd.appmgr', 'showVirtualKeyboard', [{title:"Test VK"}], function(res) {
							assert.isUndefined( res.error );
							assert.isArray( res.data );
							assert.strictEqual( res.data[0].status, 'accepted' );
							assert.strictEqual( res.data[0].input, 'user input' );
							b.closeEvent(1);
							done();
						});
						canShowVK = true;
						b.closeEvent(2);
					});
				});
			});

			it('should not call input callback if vk is closed when input arrive', function(done) {
				reg.put('appmgr', api);
				var b = reg.get('browser');
				var notif = reg.get('notification');
				var inputCbCalled = false;
				notif.on('VirtualKeyboard', function(evt, cfg) {
					assert.strictEqual( evt, 'show' );
					assert.deepEqual( cfg, {title: 'Test VK'} );
					api.stop( testApp.id, function() {
						assert.isFalse( api.isRunning( testApp.id ) );
						notif.emit('VirtualKeyboardInput', { status: 'accepted', input: 'user input' });
						assert.isFalse( inputCbCalled );
						done();
					});
					b.closeEvent(1);
				});

				api.runAsync( {id: testApp.id}, function(err) {
					assert.equal( err );
					b.notifyUp(1);
					b.apiEvent(1, 'ar.edu.unlp.info.lifia.tvd.appmgr', 'showVirtualKeyboard', [{title:"Test VK"}], function() {
						inputCbCalled = true;
					});
				});
			});

			it('should not show vk if called from invalid app id', function(done) {
				reg.put('appmgr', api);
				var notif = reg.get('notification');
				var vkCalled = false;
				notif.on('VirtualKeyboard', function() {
					vkCalled = true;
				});

				api.showVirtualKeyboard('invalid_id', {}, function(err, resp) {
					assert.isFalse( vkCalled );
					assert.strictEqual( err.message, 'Fail to show virtual keyboard, invalid app id=invalid_id' );
					assert.isUndefined( resp );
					done();
				});
			});
		});

		describe('configureService', function() {
			it('should not start config application for invalid service', function(done) {
				api.configureService( 'usr_srv', function(err) {
					assert.equal( err, 'Error: Service do not have a config app: id=usr_srv' );
					done();
				});
			});

			it('should start config application for valid service', function(done) {
				var count = -1;
				api.on('application', function(evt) {
					count++;
					switch (count) {
						case 0:
							assert.isTrue( 'usr_srv.cfg' === evt.id );
							assert.equal( evt.state, 'starting' );
							break;
						case 1:
							assert.isTrue( 'usr_srv.cfg' === evt.id );
							assert.equal( evt.state, 'launching' );
							reg.get('browser').loadedEvent(1);
							break;
						case 2:
							assert.isTrue( 'usr_srv.cfg' === evt.id );
							assert.equal( evt.state, 'loaded' );
							api.stop( 'usr_srv.cfg', function(err) {
								assert.isUndefined(err);
								done();
							});
							break;
						case 3:
							assert.isTrue( 'usr_srv.cfg' === evt.id );
							assert.equal( evt.state, 'stopping' );
							reg.get('browser').closeEvent(1);
							break;
					}
				});
				reg.get('srvmgr').services.usr_srv = {
					id: 'usr_srv',
					name: 'User service',
					configure: 'config.html',
					dependencies: []
				};

				api.configureService( 'usr_srv', function(err) {
					assert.equal(err);
				});
			});

			it('should not start config application for valid service twice', function(done) {
				reg.get('srvmgr').services.usr_srv = {
					id: 'usr_srv',
					name: 'User service',
					configure: 'config.html',
					dependencies: []
				};
				api.configureService( 'usr_srv', function(err) {
					assert.equal(err);
					api.configureService( 'usr_srv', function(err1) {
						assert.equal( err1, 'Error: Application already started: id=usr_srv.cfg' );
						api.stop( 'usr_srv.cfg', function(err) {
							assert.isUndefined(err);
							done();
						});
						reg.get('browser').closeEvent(1);
					});
				});
			});

			it('should not start config application if cannot run app', function(done) {
				reg.get('srvmgr').services.usr_srv = {
					id: 'usr_srv',
					name: 'User service',
					configure: 'config.html',
					dependencies: []
				};
				var b = reg.get('browser');
				b.setLauncherReturn(-1);
				api.configureService( 'usr_srv', function(err) {
					assert.equal(err, 'Error: Cannot launch browser');
					done();
				});
			});

			it('should not start config application if cannot load app', function(done) {
				reg.get('srvmgr').services.usr_srv = {
					id: 'usr_srv',
					name: 'User service',
					configure: 'config.html',
					dependencies: []
				};
				var pkgMgr = reg.get('pkgmgr');
				var origFunc = pkgMgr.getComponentInfo;
				pkgMgr.getComponentInfo = function(id) {
					if (id === 'ar.edu.unlp.info.lifia.tvd.srv_cfg') {
						return null;
					} else {
						return origFunc.call(pkgMgr, id);
					}
				};
				api.configureService( 'usr_srv', function(err) {
					assert.equal(err, 'Error: Component not exists: id=ar.edu.unlp.info.lifia.tvd.srv_cfg');
					done();
				});
			});

			it('should not export cfg apis to regular application', function(done) {
				var b = reg.get('browser');
				b.launchBrowser = function( url, options, onEvent ) {
					assert.deepEqual( options.apis, [{
						name: 'usrsrv',
						id: 'ar.edu.unlp.info.lifia.tvd.otherusrsrv',
						exports: ['apiMethod']
					}]);
					onEvent({'name': 'onClose'});
					done();
				};

				reg.put('otherusrsrv', {
					apiMethod: function() {},
					cfgMethod: function() {},
					getWebAPI: function() {
						return {
							'name': 'usrsrv',
							'public': ['apiMethod'],
							'private': ['cfgMethod']
						};
					}
				});

				usrSrvCfg.manifest.system = false;
				reg.get('pkgmgr').setPkgs([usrSrvCfg.manifest, usrSrvManifest]);
				usrSrvCfg.component.dependencies = ['ar.edu.unlp.info.lifia.tvd.otherusrsrv'];
				api.runAsync({id:usrSrvCfg.id}, function(err) {
					assert.equal( err );
				});
			});

			it('should export cfg apis to system application', function(done) {
				var b = reg.get('browser');
				b.launchBrowser = function( url, options, onEvent ) {
					assert.deepEqual( options.apis, [{
						name: 'usrsrv',
						id: 'ar.edu.unlp.info.lifia.tvd.usrsrv',
						exports: ['apiMethod', 'cfgMethod']
					}]);
					onEvent({'name': 'onClose'});
					done();
				};

				reg.put('usrsrv', {
					apiMethod: function() {},
					cfgMethod: function() {},
					getWebAPI: function() {
						return {
							'name': 'usrsrv',
							'public': ['apiMethod'],
							'private': ['cfgMethod']
						};
					}
				});

				usrSrvCfg.manifest.system = true;
				usrSrvCfg.component.dependencies = ['ar.edu.unlp.info.lifia.tvd.usrsrv'];
				reg.get('pkgmgr').setPkgs([usrSrvCfg.manifest, usrSrvManifest]);
				api.runAsync({id:usrSrvCfg.id}, function(err) {
					assert.equal(err);
				});
			});

			it('should export cfg apis to config application', function(done) {
				var b = reg.get('browser');
				b.launchBrowser = function( url, options, onEvent ) {
					assert.deepEqual( options.apis, [{
						name: 'usrsrv',
						id: 'ar.edu.unlp.info.lifia.tvd.usrsrv',
						exports: ['apiMethod', 'cfgMethod']
					}]);
					onEvent({'name': 'onClose'});
					done();
				};

				reg.put('usrsrv', {
					apiMethod: function() {},
					cfgMethod: function() {},
					getWebAPI: function() {
						return {
							'name': 'usrsrv',
							'public': ['apiMethod'],
							'private': ['cfgMethod']
						};
					}
				});

				reg.get('pkgmgr').setPkgs([srvCfgApp.manifest, usrSrvCfg.manifest, usrSrvManifest]);
				reg.get('srvmgr').services['ar.edu.unlp.info.lifia.tvd.usrsrv'] = {
					id: 'ar.edu.unlp.info.lifia.tvd.usrsrv',
					name: 'User service',
					configure: 'config.html',
					dependencies: []
				};
				api.configureService( 'ar.edu.unlp.info.lifia.tvd.usrsrv', function(err) {
					assert.equal(err);
				});
			});
		});
	});

});
