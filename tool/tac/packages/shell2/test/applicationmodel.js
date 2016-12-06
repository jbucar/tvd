'use strict';

var path = require('path');
var Component = require('../../web_modules/test/node_modules/component');
var bPromise = require('bluebird');
var EventEmitter = require('../../web_modules/src/libs/EventEmitter.min.js');
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;

describe('applicationmodel.js', function() {
	var comp = new Component( path.join(__dirname,'../src/shell/applicationmodel.js'), 'ApplicationModel' );
	var uiComp = new Component( path.join(__dirname,'../../web_modules/src/components/util/ui.js'), 'UI' );

	var ApplicationModel;
	var UI;

	function AppMgr() {
		var self = {};

		self.apps = [];
		self.signals = {};

		self.getAll = function() {
			return self.apps.map( (app) => app.id );
		};

		self.get = function(appID) {
			return self.apps.find( (app) => app.id === appID );
		};

		self.on = function(sig,cb) {
			self.signals[sig] = cb;
		};

		self.removeListener = function(sig) {
			delete self.signals[sig];
		};

		self.install = function(app) {
			self.apps.push( app );
			let sigCB = self.signals.install;
			if (sigCB) {
				sigCB( app.id );
			}
		};

		self.remove = function(appID) {
			let index = self.apps.findIndex( (app) => app.id === appID );
			if (index >= 0) {
				self.apps.splice(index,1);
				let sigCB = self.signals.uninstall;
				if (sigCB) {
					sigCB( appID );
				}
			}
		};

		return self;
	}

	function Application( name ) {
		return {
			id: name,
			name: name
		};
	}

	function createResources() {
		return {
			data: {
				shell: {
					_templates: {},
					_styles: {},
					_strings: {},
					_images: {
						'default_app_icon': 'path/to/default_app_icon.png',
					}
				}
			}
		};
	}

	beforeEach(function(done) {
		global.EventEmitter = EventEmitter;
		global.assert = assert;
		global.log = {
			info: function(/*cat,msg*/) {
				var args = Array.prototype.slice.call(arguments, 1);
				console.log.apply(undefined,args);
			}
		};
		global.appmgr = new AppMgr();
		global.apiCallAsync = function(fnc) {
			//	Remove function name
			var args = Array.prototype.slice.call(arguments, 1);
			let result = fnc.apply(undefined,args);
			return bPromise.resolve(result);
		};
		global.forwardEventEmitter = function(obj,evts) {
			obj.on = () => evts.on.apply(evts,arguments);
			obj.once = () => evts.once.apply(evts,arguments);
			obj.emit = () => evts.emit.apply(evts,arguments);
			obj.removeListener = () => evts.removeListener.apply(evts,arguments);
		};
		comp.create()
			.then(function(ctor) {
				assert(ctor);
				ApplicationModel = ctor;
			})
			.then(() => uiComp.create())
			.then( function(ctor) {
				UI = ctor;
				global.ui = new UI(createResources());
				done();
			});
	});

	afterEach(function() {
		delete global.assert;
		delete global.log;
		delete global.appmgr;
		delete global.apiCallAsync;
		delete global.EventEmitter;
		delete global.forwardEventEmitter;
		delete global.ui;
		ApplicationModel = null;
		comp.destroy();
		UI = null;
		uiComp.destroy();
	});

	it('should construct', function() {
		var model = new ApplicationModel();
		assert(model);
		assert.equal( Object.keys(model).length, 10 );
	});

	describe( 'methods', function() {
		var model;

		beforeEach(function() {
			model = new ApplicationModel();
			assert(model);
		});

		afterEach(function() {
			model = null;
		});

		describe( 'init', function() {
			it('handle empty apps', function(done) {
				appmgr.apps = [];
				model.init().then(function() {
					let allApps = model.getAll();
					assert(allApps);
					assert.deepEqual( allApps, appmgr.apps );
					done();
				});
			});

			it('handle not empty apps', function(done) {
				appmgr.apps = [ new Application('app1'), new Application('app2') ];
				model.init().then(function() {
					let allApps = model.getAll();
					assert(allApps);
					assert.deepEqual( allApps, appmgr.apps );
					done();
				});
			});
		});

		describe( 'handle signals', function() {
			it('handle install', function(done) {
				appmgr.apps = [ new Application('app1'), new Application('app2') ];
				model.init().then(function() {
					let apps1Len = model.getAll().length;
					appmgr.install( new Application('app3') );
					setTimeout(function() {
						let apps2 = model.getAll();
						assert.notEqual( apps1Len, apps2.length );
						assert.deepEqual( apps2, appmgr.apps );
						done();
					}, 10 );
				});
			});

			it('handle remove', function(done) {
				appmgr.apps = [ new Application('app1'), new Application('app2') ];
				model.init().then(function() {
					let apps1Len = model.getAll().length;
					appmgr.remove( 'app1' );
					setTimeout(function() {
						let apps2 = model.getAll();
						assert.notEqual( apps1Len, apps2.length );
						assert.deepEqual( apps2, appmgr.apps );
						done();
					}, 10 );
				});
			});

			xit('handle remove 2', function(done) {
				appmgr.apps = [ new Application('app1'), new Application('app2') ];
				model.init().then(function() {
					assert.deepEqual( Object.keys(appmgr.signals), [ 'install', 'uninstall' ] );
					model.fin();
					assert.deepEqual( Object.keys(appmgr.signals), [] );
					done();
				});
			});
		});
	});
});
