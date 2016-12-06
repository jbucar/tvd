'use strict';

var _ = require('lodash');
var bPromise = require("bluebird");
var mockfs = require('mock-fs');
var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var Mocks = require('mocks');
var Application = require('../src/application');
var Shell = require('../src/shell');

describe('Shell', function() {
	var reg=undefined;
	var appmgr=undefined;

	var shellApp = Mocks.createApp( 'shell' );
	Mocks.changeCategories( shellApp, ["system","shell"] );

	var defaultPkgs = [shellApp.manifest];

	function initFS() {
		var defaultFS = {};
		defaultFS[Mocks.rwPath()] = {};
		defaultFS[shellApp.installPath] = shellApp.installDir;
		mockfs(defaultFS);
	}

	function ApplicationManager() {
		var self = this || {};

		self.loadApp = function(appID) {
			var app = undefined;
			try {
				app = new Application( reg, appID );
				app.info.id = newID;
			} catch(err) {
			};

			return app;
		};

		self.getShell = function() {
			return shellApp.id;
		};

		self.startApp = function() {
			return bPromise.resolve();
		};

		self.isReady = function() {
			return true;
		};

		return self;
	};

	beforeEach(function() {
		reg = Mocks.init('silly');
		initFS();
		reg.put('srvmgr', new Mocks.ServiceManager() );
		reg.put('pkgmgr', new Mocks.PackageManager(_.cloneDeep(defaultPkgs)) );
		var b = new Mocks.BasicBrowser(reg);
		b.doAuto(true);
		reg.put( 'browser', b );
		appmgr = new ApplicationManager();
	});

	afterEach(function() {
		mockfs.restore();
		Mocks.fin();
		reg = undefined;
	});

	describe('constructor', function() {
		it('should basic application', function() {
			var comp = new Shell(reg, appmgr);
			assert.isObject(comp);
		});

		it('should create without using new', function() {
			var comp = Shell(reg, appmgr);
			assert.isObject(comp);
		});
	});

	describe('methods', function() {
		it('should start', function(done) {
			var l = new Shell(reg, appmgr);
			appmgr.startApp = function() {
				done();
				return bPromise.resolve();
			};
			l.start();
		});

		it('should show', function(done) {
			var l = new Shell(reg, appmgr);
			reg.get('browser').showBrowser = function( id, show ) {
				assert.isTrue(show);
				done();
			}
			appmgr.startApp = function() {
				l.show(true);
				return bPromise.resolve();
			};
			l.start();
		});

		it('should not restart shell if notification layer stop', function(done) {
			var count = -1;
			var apps = [];

			reg.put('appmgr', appmgr );

			appmgr.loadApp = function(appID,newID) {
				var app = new Application( reg, appID );
				app.info.id = newID;
				apps[newID] = app;
				var count = -1;
				app.on( 'changed', function(evt) {
					if (app.info.id === 'ar.edu.unlp.info.lifia.tvd.shell.shell' && evt.state === 'loaded') {
						reg.get('browser').closeEvent( 1, 'Alguno' );
					}

					if (app.info.id === 'ar.edu.unlp.info.lifia.tvd.shell.system_layer' && evt.state === 'loaded') {
						count++;
						if (count > 0) {
							setTimeout(done,10);
						}
					}
				});
				return app;
			};

			appmgr.startApp = function( opts ) {
				count++;
				assert.isTrue( count < 1 );
				assert.isObject( apps[opts.id] );
				var app = apps[opts.id];
				app.run();
				return bPromise.resolve();
			};

			var l = new Shell(reg, appmgr);
			l.start();
		});

		it('should change to dev mode', function(done) {
			var shell=null;
			var apps = [];
			appmgr.loadApp = function(appID,newID) {
				var app = new Application( reg, appID );
				app.info.id = newID;
				apps[newID] = app;
				return app;
			};
			appmgr.startApp = function(opts) {
				assert.isObject( apps[opts.id] );
				shell = apps[opts.id];
				shell.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						[ 'VK_RED', 'VK_3', 'VK_3', 'VK_2', 'VK_8', 'VK_4' ].forEach(function(key) {
							reg.get('browser').sendKey(2,key);
						});
					}
				});
				shell.run();
				return bPromise.resolve();
			};

			function SystemMock() {
				var self = this || {};
				self.enableDevelopmentMode = function() {
					assert.isTrue(shell !== null);
					shell.stop().then(done);
				}
				return self;
			};
			reg.put( 'system', new SystemMock() );

			var l = new Shell(reg, appmgr);
			l.start();
		});

		it('should not change to dev mode if already on dev mode', function(done) {
			var shell=null;
			var apps = [];
			appmgr.loadApp = function(appID,newID) {
				var app = new Application( reg, appID );
				app.info.id = newID;
				apps[newID] = app;
				return app;
			};
			appmgr.startApp = function(opts) {
				assert.isObject( apps[opts.id] );
				shell = apps[opts.id];
				shell.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						[ 'VK_RED', 'VK_3', 'VK_3', 'VK_2', 'VK_8', 'VK_4' ].forEach(function(key) {
							reg.get('browser').sendKey(2,key);
						});
					}
				});
				shell.run();
				return bPromise.resolve();
			};

			function SystemMock() {
				var self = this || {};
				var count = 0;
				self.enableDevelopmentMode = function() {
					count++;
					assert.strictEqual(count, 1);
					assert.isTrue(shell !== null);
					global.inDevelopmentMode = true;
					[ 'VK_RED', 'VK_3', 'VK_3', 'VK_2', 'VK_8', 'VK_4' ].forEach(function(key) {
						reg.get('browser').sendKey(2,key);
					});
					process.nextTick(function() {
						global.inDevelopmentMode = false;
						shell.stop().then(done);
					});
				};
				return self;
			};
			reg.put( 'system', new SystemMock() );

			var l = new Shell(reg, appmgr);
			l.start();
		});

		it('should ignore exit key', function(done) {
			var shell=null;
			var apps = [];
			appmgr.loadApp = function(appID,newID) {
				var app = new Application( reg, appID );
				app.info.id = newID;
				apps[newID] = app;
				return app;
			};
			appmgr.startApp = function(opts) {
				assert.isObject( apps[opts.id] );
				shell = apps[opts.id];
				var force_stop = false;
				shell.on( 'changed', function(evt) {
					if (evt.state === 'loaded') {
						reg.get('browser').sendKey(2,'VK_EXIT');
						setTimeout(
							function() { force_stop=true; shell.stop(); },
							5
						);
					}
					else if (evt.state === 'stopped') {
						if (!force_stop) {
							assert.isFalse(true);
						}
						else {
							done();
						}
					}
				});
				shell.run();
				return bPromise.resolve();
			};

			var l = new Shell(reg, appmgr);
			l.start();
		});

		it('should send recovery error if cannot load shell application', function() {
			appmgr.loadApp = function(appID) {
				throw new Error('No se cargo');
			};
			var fnc = function() { new Shell(reg, appmgr); };
			assert.throw(fnc,Error,'Cannot start shell dependencies');
		});

		it('should send recovery error if cannot create system layer application', function() {
			var pkgmgr = reg.get('pkgmgr');
			var count=-1;
			var oldFnc = pkgmgr.getComponentInfo;
			pkgmgr.getComponentInfo = function(compID) {
				count++;
				if (count == 2) {
					return null;
				}
				else {
					return oldFnc.call(pkgmgr,compID);
				}
			};

			var fnc = function() { new Shell(reg, appmgr) };
			assert.throw(fnc,Error,'Cannot start shell dependencies');
		});

		it('should send recovery error if cannot start shell', function(done) {
			appmgr.startApp = function() {
				return bPromise.reject();
			};
			bPromise.onPossiblyUnhandledRejection(function(reason, promise) {
				assert.equal( reason.message, 'Cannot start shell' );
				done();
			});
			var l = new Shell(reg, appmgr);
			l.start();
		});
	});
});

