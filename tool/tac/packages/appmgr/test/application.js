'use strict';

var bPromise = require("bluebird");
var mockfs = require('mock-fs');
var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var _ = require('lodash');
var Mocks = require('mocks');
var Application = require('../src/application');

var testID = 'ar.edu.unlp.info.lifia.tvd.test';
var test_install_path = '/system/original/packages/' + testID;
var test_data_path = '/system/data/config/' + testID;

var testID2 = 'ar.edu.unlp.info.lifia.tvd.test2';
var testID3 = 'ar.edu.unlp.info.lifia.tvd.test3';

var testComponent = {
	"id": testID,
	"pkgID": testID,
	"name": "Test app",
	"type": "app",
	"version": "1.0.0",
	"main": "test.html",
	"plugins": ['pepe']
};

var testComponent2 = {
	"id": testID2,
	"pkgID": testID2,
	"name": "Test app",
	"type": "service",
	"version": "1.0.0",
	"main": "main.js",
};

var testComponent3 = {
	"id": testID3,
	"pkgID": testID,
	"name": "Test app",
	"type": "app",
	"version": "1.0.0",
	"main": "test.html",
	"enable_geolocation": true,
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
	"components": [ testComponent, testComponent3 ]
};

var pepe_service = {
	funcReturn: function(param) {
		assert.isTrue( param === 'hello' );
		return 'pepe';
	},

	funcNoReturn: function(param) {
		assert.isTrue( param === 'hello' );
	},

	getWebAPI: function() {
		return {'name': 'pepe'};
	},
};

describe('Application', function() {
	var reg;

	function initFS() {
		var defaultFS = {};
		defaultFS[test_install_path] = {
			'manifest.json': JSON.stringify(testPackage, null, 4),
			'test.html': 'algo'
		};
		defaultFS[test_data_path] = {};
		defaultFS['/system/data/packages'] = {};
		defaultFS['/system/original/packages/' + testID3 + '/test.html'] = 'algo';
		mockfs(defaultFS);
	}

	beforeEach(function() {
		reg = Mocks.init('silly');
		initFS();
	});

	afterEach(function() {
		mockfs.restore();
		Mocks.fin();
		reg = undefined;
	});

	function initPkgMgr( comp, comp2 ) {
		var pkg = _.cloneDeep(testPackage);
		if (comp) {
			pkg.components = [comp];
		}
		if (comp2) {
			pkg.components.push( comp2 );
		}
		reg.put( 'pkgmgr', new Mocks.PackageManager([pkg]) );
	}

	describe('constructor', function() {
		it('should basic application', function() {
			initPkgMgr();
			var comp = new Application(reg, testID);
			assert.isObject(comp);
		});

		it('should check main is local file and not absolute', function() {
			var comp = _.cloneDeep(testComponent);
			comp.main = 'file:///system/original/packages/ar.edu.unlp.info.lifia.tvd.test/index.html';
			initPkgMgr(comp);
			function fnc() { new Application(reg, testID); }
			assert.throw(fnc,Error,'Property main is local file but not exists: main=/system/original/packages/ar.edu.unlp.info.lifia.tvd.test/system/original/packages/ar.edu.unlp.info.lifia.tvd.test/index.html');
		});

		it('should check main is valid url', function() {
			var comp = _.cloneDeep(testComponent);
			comp.main = 'pepe:///pepe/algun/otro/path/index.html';
			initPkgMgr(comp);
			function fnc() { new Application(reg, testID); }
			assert.throw(fnc,Error,'Property main must be http/https/file/chrome: main=pepe:///pepe/algun/otro/path/index.html');
		});

		it('should check main is http', function() {
			var comp = _.cloneDeep(testComponent);
			comp.main = 'http://www.google.com';
			initPkgMgr(comp);
			var app = new Application(reg, testID);
			assert.isObject(app);
			assert.isTrue( app.info.main === comp.main );
		});

		it('should check main is http', function() {
			var comp = _.cloneDeep(testComponent);
			comp.main = 'https://www.google.com';
			initPkgMgr(comp);
			var app = new Application(reg, testID);
			assert.isObject(app);
		});

		it('should application type', function() {
			var comp = _.cloneDeep(testComponent);
			comp.type = 'service';
			initPkgMgr(comp);
			function fnc() { new Application(reg, testID); }
			assert.throw(fnc,Error,'Property type is not application');
		});

		it('should application icon', function() {
			var comp = _.cloneDeep(testComponent);
			comp.icon = 'pepe';
			initPkgMgr(comp);
			var app = new Application(reg, testID);
			assert.isObject(app);
		});

		it('should set bg transparent if set', function() {
			var comp = _.cloneDeep(testComponent);
			comp.bg_transparent = true;
			initPkgMgr(comp);
			var app = new Application(reg, testID);
			assert.isObject(app);
			assert.isTrue( app.info.bg_transparent );
		});

		it('should set user application if not set', function() {
			var comp = _.cloneDeep(testComponent);
			initPkgMgr(comp);
			var app = new Application(reg, testID);
			assert.isObject(app);
			assert.isTrue( app.info.user );
		});

		it('should use user field from application', function() {
			var comp = _.cloneDeep(testComponent);
			comp.user = false;
			initPkgMgr(comp);
			var app = new Application(reg, testID);
			assert.isObject(app);
			assert.isFalse( app.info.user );
		});

		it('should uppercase categories', function() {
			var comp = _.cloneDeep(testComponent);
			comp.categories = ['pepe','Otro'];
			initPkgMgr(comp);
			var app = new Application(reg, testID);
			assert.isObject(app);
			assert.isTrue( app.info.categories.length === 2 );
			assert.isTrue( app.info.categories[0] === 'PEPE' );
			assert.isTrue( app.info.categories[1] === 'OTRO' );
		});
	});

	describe('methods', function() {
		var browser = null;

		beforeEach(function() {
			initPkgMgr();
			reg.put( 'srvmgr', new Mocks.ServiceManager() );
			browser = Mocks.BasicBrowser(reg);
			reg.put( 'browser', browser );
		});

		afterEach(function() {
			browser = null;
		});

		describe('lifetime', function() {
			it( 'should not emit changed event if there is no state change', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

				var count=-1;
				app.on( 'changed', function(evt) {
					count++;
					if (count === 0) {
						assert.equal( evt.state, 'starting' );
						assert.equal( app.getStatus(), 'starting' );
						assert.isTrue( app.isRunning() );
					}
					else if (count === 1) {
						assert.equal( evt.state, 'launching' );
						assert.isTrue( app.isRunning() );
						browser.launchedEvent(1);
					}
					else if (count === 2) {
						assert.equal( evt.state, 'launched' );
						assert.isTrue( app.isRunning() );
						browser.loadedEvent(1);
					}
					else if (count === 3) {
						assert.equal( evt.state, 'loaded' );
						assert.isTrue( app.isRunning() );
						browser.loadedEvent(1);
						process.nextTick(function() {
							app.stop();
						});
					}
					else if (count === 4) {
						assert.equal( evt.state, 'stopping' );
						assert.isFalse( app.isRunning() );
						browser.closedEvent(1);
					}
					else if (count === 5) {
						assert.equal( evt.state, 'stopped' );
						assert.isFalse( app.isRunning() );
						done();
					}
				});

				app.run();
			});

			it( 'should start and stop the application without errors', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

				var count=-1;
				app.on( 'changed', function(evt) {
					count++;
					if (count === 0) {
						assert.equal( evt.state, 'starting' );
						assert.equal( app.getStatus(), 'starting' );
						assert.isTrue( app.isRunning() );
					}
					else if (count === 1) {
						assert.equal( evt.state, 'launching' );
						assert.isTrue( app.isRunning() );
						browser.launchedEvent(1);
					}
					else if (count === 2) {
						assert.equal( evt.state, 'launched' );
						assert.isTrue( app.isRunning() );
						browser.loadedEvent(1);
					}
					else if (count === 3) {
						assert.equal( evt.state, 'loaded' );
						assert.isTrue( app.isRunning() );
						app.stop();
					}
					else if (count === 4) {
						assert.equal( evt.state, 'stopping' );
						assert.isFalse( app.isRunning() );
						browser.closedEvent(1);
					}
					else if (count === 5) {
						assert.equal( evt.state, 'stopped' );
						assert.isFalse( app.isRunning() );
						done();
					}
				});

				app.run();
			});

			it( 'should handle plugins options', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

				browser.launchBrowser = function( url, opts, cb ) {
					assert.equal( url, 'file:///system/original/packages/ar.edu.unlp.info.lifia.tvd.test/test.html' );
					assert.deepEqual( opts.plugins, ['pepe'] );
					assert.isFunction( cb );
					done();
				};

				app.run();
			});

			it( 'should run with geolocation disabled if not requested in manifest', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

				reg.put( 'system', {
					isGeolocationEnabled: function() {
						return true;
					}
				});

				browser.launchBrowser = function( url, opts, cb ) {
					assert.equal( url, 'file:///system/original/packages/ar.edu.unlp.info.lifia.tvd.test/test.html' );
					assert.isFalse( opts.enableGeolocation );
					assert.isFunction( cb );
					done();
				};

				app.run();
			});

			it( 'should run with geolocation disabled if requested in manifest but disabled in system', function(done) {
				var app = new Application(reg, testID3);
				assert.isObject(app);

				reg.put( 'system', {
					isGeolocationEnabled: function() {
						return false;
					}
				});

				browser.launchBrowser = function( url, opts, cb ) {
					assert.equal( url, 'file:///system/original/packages/ar.edu.unlp.info.lifia.tvd.test3/test.html' );
					assert.isFalse( opts.enableGeolocation );
					assert.isFunction( cb );
					done();
				};

				app.run();
			});

			it( 'should run with geolocation enabled if requested in manifest and enabled in system', function(done) {
				var app = new Application(reg, testID3);
				assert.isObject(app);

				reg.put( 'system', {
					isGeolocationEnabled: function() {
						return true;
					}
				});

				browser.launchBrowser = function( url, opts, cb ) {
					assert.equal( url, 'file:///system/original/packages/ar.edu.unlp.info.lifia.tvd.test3/test.html' );
					assert.isTrue( opts.enableGeolocation );
					assert.isFunction( cb );
					done();
				};

				app.run();
			});

			it( 'should not handle browser messages if not running', function(done) {
				var system = {
					powerOff: function() {
						assert.isFalse(true);
					}
				};
				reg.put( 'system', system );

				var app = new Application(reg, testID);
				assert.isObject(app);

				var count=-1;
				app.on( 'changed', function(evt) {
					count++;
					if (evt.state === 'stopping') {
						browser.closedEvent(1);
					}
					else if (evt.state == 'stopped') {
						var keyEvt = {
							name: 'onKey',
							isUp: true,
							code: 'VK_POWER'
						};
						browser.sendEvent( 1, keyEvt, true );	//	Sync
						done();
					}
				});

				app.run().then(function() {
					app.stop();
				});
			});

			it( 'should handle power off key', function(done) {
				var system = {
					powerOff: function() {
						done();
					}
				};
				reg.put( 'system', system );

				var app = new Application(reg, testID);
				assert.isObject(app);

				app.on( 'changed', function(evt) {
					if (evt.state == 'loaded') {
						browser.sendKey( 1, 'VK_POWER' );
					}
				});

				app.run().then(function() {
					browser.notifyUp(1);
				});
			});

			it( 'should handle stop key', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

				browser.doAuto(true);

				app.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						browser.sendKey( 1, 'VK_EXIT' );
					}
					else if (evt.state === 'stopped') {
						done();
					}
				});

				app.run();
			});

			it( 'should handle unknown event', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

				browser.doAuto(true);

				app.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						browser.sendEvent( 1, {name: 'pepe'}, true );
						done();
					}
				});

				app.run();
			});

			it( 'should handle API from browser with return value', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

		 		reg.put('pepe', pepe_service);
				browser.doAuto(true);

				app.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						browser.apiEvent( 1, 'pepe', 'funcReturn', ['hello'], function(res) {
							assert.isUndefined( res.error );
							assert.isFalse( res.isSignal );
							assert.isArray( res.data );
							assert.isTrue( res.data[0] === 'pepe' );
							done();
						});
					}
				});

				app.run();
			});

			it( 'should handle API from browser with no return value', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

		 		reg.put('pepe', pepe_service);
				browser.doAuto(true);

				app.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						browser.apiEvent( 1, 'pepe', 'funcNoReturn', ['hello'], function(res) {
							assert.isUndefined( res.error );
							assert.isUndefined( res.data );
							assert.isFalse( res.isSignal );
							done();
						});
					}
				});

				app.run();
			});

			describe( 'weblog from browser', function() {
				function sendLog( lv ) {
					browser.sendEvent( 1, {
						name: 'onWebLog',
						logData: {
							message: 'desc1',
							source: 'source1',
							line: 34,
							level: lv
						}
					}, 1 );
				}

				it( 'should receive events with diferent levels', function(done) {
					var app = new Application(reg, testID);
					assert.isObject(app);

					browser.doAuto(true);

					var count = -1;
					app.onWebLog = function(evt) {
						Application.prototype.onWebLog.call( app, evt );
						assert.equal( evt.logData.message, 'desc1' );
						assert.equal( evt.logData.source, 'source1' );
						assert.equal( evt.logData.line, 34 );
						assert.equal( evt.logData.level, count );
						app.info.id = 'testID';
						count++;
						if (count === 3) {
							done();
						}
					};

					app.on( 'changed', function(evt) {
						if (evt.state === 'loaded') {
							sendLog( -1 );
							sendLog( 0 );
							sendLog( 1 );
							sendLog( 2 );
						}
					});

					app.run();
				});
			});

			it( 'should handle audio keys', function(done) {
				var audio = {};
				audio.count = 0;
				audio.check = function() {
					audio.count++;
					if (audio.count === 3) {
						done();
					}
				};

				audio.toggleMute = function() { audio.check(); };
				audio.volumeUp = function() { audio.check(); };
				audio.volumeDown = function() { audio.check(); };

				reg.put( 'audio', audio );

				var app = new Application(reg, testID);
				assert.isObject(app);

				app.on( 'changed', function(evt) {
					if (evt.state == 'loaded') {
						browser.sendKey( 1, 'VK_MUTE' );
						browser.sendKey( 1, 'VK_VOLUME_DOWN' );
						browser.sendKey( 1, 'VK_VOLUME_UP' );
					}
				});

				app.run().then(function() {
					browser.notifyUp(1);
				});
			});

			it( 'should handle exit key', function(done) {
				browser.doAuto(true);

				var app = new Application(reg, testID);
				assert.isObject(app);

				app.on( 'changed', function(evt) {
					if (evt.state == 'loaded') {
						browser.sendKey( 1, 'VK_EXIT' );
					}
					else if (evt.state == 'stopped') {
						done();
					}
				});

				app.run();
			});

			it( 'should not handle exit if requerid', function(done) {
				browser.doAuto(true);

				var app = new Application(reg, testID);
				assert.isObject(app);
				app._canExit = false;

				app.on( 'changed', function(evt) {
					if (evt.state == 'loaded') {
						browser.sendKey( 1, 'VK_EXIT' );
						setTimeout(function() { app.stop().then(done); }, 5);
					}
				});

				app.run();
			});

			it( 'run dependencies fail', function(done) {
				//	Add dependency
				var pkg = _.cloneDeep(testPackage);
				pkg.components[0].dependencies = [testID2];
				pkg.components[1] = _.cloneDeep(testComponent2);
				reg.get( 'pkgmgr' ).setPkgs( [pkg] );

				reg.get( 'srvmgr' ).run = function(/*id*/) {
					return new bPromise.reject( new Error('pepe') );
				};

				var app = new Application(reg, testID);
				assert.isObject(app);

				var count=-1;
				app.on( 'changed', function(evt) {
					count++;
					if (count === 0) {
						assert.equal( evt.state, 'starting' );
						assert.isTrue( app.isRunning() );
					}
					else if (count === 1) {
						assert.equal( evt.state, 'stopped' );
						assert.isFalse( app.isRunning() );
						done();
					}
				});

				app.run().catch(function(err) {
					assert.equal( err, 'Error: pepe' );
				});
			});

			it( 'should start and browser returns error', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

				browser.launchBrowser = function( /*url, options, onEvent*/ ) {
					return -1;
				};

				var count=-1;
				app.on( 'changed', function(evt) {
					count++;
					if (count === 0) {
						assert.equal( evt.state, 'starting' );
						assert.isTrue( app.isRunning() );
					}
					else if (count === 1) {
						assert.equal( evt.state, 'stopping' );
						assert.isFalse( app.isRunning() );
					}
					else if (count === 2) {
						assert.equal( evt.state, 'stopped' );
						assert.isFalse( app.isRunning() );
						done();
					}
				});

				app.run().catch(function(err) {
					assert.equal( err, 'Error: Cannot launch browser' );
				});
			});

			it( 'should start with parameters', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

				browser.launchBrowser = function( url, opts, cb ) {
					assert.equal( url, 'file:///system/original/packages/ar.edu.unlp.info.lifia.tvd.test/test.html?pepeKey=pepeVal' );
					assert.isFunction( cb );
					done();
				};

				app.run('pepeKey=pepeVal');
			});
		});

		describe('start', function() {
			it( 'should assert if start twice', function(done) {
				browser.doAuto(true);
				var app = new Application(reg, testID);
				assert.isObject(app);
				app.run().then(function() {
					assert.equal( app.isRunning(), true );
					function fnc() { app.run(); }
					assert.throw( fnc );
					done();
				});
			});
		});

		describe('show', function() {
			it( 'should not show a not running application', function() {
				var app = new Application(reg, testID);
				assert.isObject(app);
				assert.isFalse( app.isRunning() );
				browser.showBrowser = function( /*id, st*/ ) {
					assert.isTrue( false );
				};
				app.show(true);
			});

			it( 'should show a running application', function(done) {
				browser.doAuto(true);
				var app = new Application(reg, testID);
				assert.isObject(app);
				app.run().then(function() {
					browser.showBrowser = function( id, st ) {
						assert.equal( id, 1 );
						assert.equal( st, true );
						app.stop().then(done);
					};
					app.show(true);
				});
			});
		});

		describe('stop', function() {
			it( 'should stop a not running application', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);
				assert.isFalse( app.isRunning() );
				app.stop().then(function() {
					assert.isFalse( app.isRunning() );
					done();
				});
			});

			it( 'should clear registered signals', function(done) {
				var app = new Application(reg, testID);
				assert.isObject(app);

				var fncUnregister = function() {
					done();
				};
				browser.doAuto(true);
				app.run().then(function() {
					app.addSignalUnregister( fncUnregister );
					app.stop().then(function() {
						assert.equal( app._events.listeners('changed').length, 0 );
					});
				});
			});

			it( 'should not add a registered signal if not running', function() {
				var app = new Application(reg, testID);
				assert.isObject(app);

				var fncUnregister = function() {
					assert(false);
				};

				browser.doAuto(true);
				app.addSignalUnregister( fncUnregister );
			});

			function AppMgrMock() {
				var self = this || {};
				self.state = true;
				self.isReady = function() {
					return self.state;
				};

				return self;
			}

			it( 'should restart app if closed with error', function(done) {
				reg.put('appmgr', new AppMgrMock() );
				var app = new Application(reg, testID);
				assert.isObject(app);

				app._restart = true;
				var active=0;
				app.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						if (active === 0) {
							browser.closeEvent(1,'Alguno');
							active++;
						}
					}

					if (evt.state !== 'loaded' && active === 1) {
						active++;
						assert.equal( evt.state, 'launching' );
						app.stop().then(done);
					}
				});

				browser.doAuto(true);
				app.run();
			});

			it( 'should notify error if closed with error', function(done) {
				reg.put('appmgr', new AppMgrMock() );
				var app = new Application(reg, testID);
				assert.isObject(app);
				app.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						browser.closeEvent(1,'Alguno');
					}
					else if (evt.state === 'stopped') {
						assert.equal( evt.error, 'Alguno' );
						done();
					}
				});

				browser.doAuto(true);
				app.run();
			});

			it( 'should handle stop on stopping state', function(done) {
				reg.put('appmgr', new AppMgrMock() );
				var app = new Application(reg, testID);
				assert.isObject(app);
				var count=0;
				app.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						app.stop().then(function() {
							count++;
							if (count === 2) {
								done();
							}
						});
					}
					else if (evt.state === 'stopping') {
						app.stop().then(function() {
							count++;
							if (count === 2) {
								done();
							}
						});
					}
				});

				browser.doAuto(true);
				app.run();
			});

			it( 'should not restart app if closed with error but appmgr is not ready', function(done) {
				var appmgr = new AppMgrMock();
				reg.put('appmgr', appmgr );
				var app = new Application(reg, testID);
				assert.isObject(app);

				app._restart = true;
				var active=0;
				app.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						if (active === 0) {
							appmgr.state = false;
							browser.closeEvent(1,'Alguno');
							active++;
						}
					}

					if (evt.state !== 'loaded' && active === 1) {
						active++;
						assert.equal( evt.state, 'stopped' );
						done();
					}
				});

				browser.doAuto(true);
				app.run();
			});
		});
	});
});

