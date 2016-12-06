'use strict';

var path = require('path');
var Component = require('component');

var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;

describe('util.js', function() {
	var comp = new Component( path.join(__dirname,'../src/components/util/util.js'), [
		'defaultErrorHandler',
		'assert',
		'$',
		'apiCall',
		'apiCallAsync',
		'registerNotification',
		'getURIParameter',
		'forwardEventEmitter'
	]);
	var util;

	beforeEach(function(done) {
		global.document = {
			location: {
				search: ''
			}
		};
		global.notification = {
			emit: function() {}
		};
		global.log = {
			error: (/*cat, msg*/) => {}
		};

		comp.create()
			.then(function(ctor) {
				assert(ctor);
				util = ctor;
				done();
			});
	});

	afterEach(function() {
		comp.destroy();
		util = null;

		delete global.document;
		delete global.notification;
		delete global.log;
	});

	describe('basic', function() {
		it('should valid methods', function() {
			assert.equal( Object.keys(util).length, 8 );
			assert( util.defaultErrorHandler );
			assert( util.assert );
			assert( util.apiCall );
			assert( util.apiCallAsync );
			assert( util.registerNotification );
			assert( util.getURIParameter );
			assert( util.forwardEventEmitter );
			assert( typeof util.$ === 'function' );
		});
	});

	describe('defaultErrorHandler', function() {
		it('should log error message', function() {
			var count = 0;
			global.log.error = function( cat, msg ) {
				count++;
				assert.equal( cat, 'util' );
				assert.equal( msg, 'Generic error: err=%s' );
			};

			util.defaultErrorHandler( new Error('pepe') );
			assert.equal( count, 1 );
		});
	});


	describe( 'assert', function() {
		it( 'should call to default error handler', function() {
			var count = 0;
			global.log.error = () => count++;

			util.assert( false );
			util.assert( true );
			assert.equal( count, 1 );
		});
	});

	describe( '$ (_.partial)', function() {
		it( 'should partial a function with one param', function() {
			let val1 = 2;
			let called = 0;
			let fnc = function(p1) {
				assert.equal(p1,val1);
				called++;
			};
			util.$( fnc, val1 )();
			assert.equal( called, 1 );
		});

		it( 'should partial a function with 3 params', function() {
			let val1 = 2;
			let val2 = 5;
			let val3 = 11;
			let called = 0;
			let fnc = function(p1,p2,p3) {
				assert.equal(p1,val1);
				assert.equal(p2,val2);
				assert.equal(p3,val3);
				called++;
			};
			util.$( fnc, val1, val2, val3 )();
			assert.equal( called, 1 );
		});

		it( 'should partial a function with param and extra arguments', function() {
			let val = 1;
			let extra1 = 2;
			let extra2 = 3;
			let called = 0;
			let fnc = function(p1,p2,p3) {
				assert.isUndefined(this);
				assert.equal(p1,val);
				assert.equal(p2,extra1);
				assert.equal(p3,extra2);
				called++;
			};
			util.$( fnc, val )(extra1,extra2);
			assert.equal( called, 1 );
		});

		it( 'should partial a function with param and extra argument with this changed', function() {
			let obj = {};
			let val = 1;
			let extra1 = 2;
			let extra2 = 3;
			let called = 0;
			let fnc = function(p1,p2,p3) {
				assert.equal(this, obj);
				assert.equal(p1,val);
				assert.equal(p2,extra1);
				assert.equal(p3,extra2);
				called++;
			};
			let p = util.$( fnc, val );
			p.call(obj, extra1, extra2);
			assert.equal( called, 1 );
		});
	});

	describe('apiCall', function() {
		function AppMgr() {
			var self = {};
			self.apps = {};
			self.getAll = cb => cb( undefined, Object.keys(self.apps) );
			return self;
		}

		it( 'basic without parameters', function(done) {
			var errorCount = 0;
			util.defaultErrorHandler = () => errorCount++;

			var appmgr = new AppMgr();
			appmgr.apps.pepeID = {
				name: 'Pepe'
			};
			util.apiCall( appmgr.getAll, function(apps) {
				assert.equal( apps.length, 1 );
				assert.equal( apps[0], 'pepeID' );

				assert.equal( errorCount, 0 );
				done();
			});
		});

		it( 'basic signal', function(done) {
			var errorCount = 0;
			util.defaultErrorHandler = () => errorCount++;

			var appmgr = new AppMgr();
			appmgr.apps.pepeID = {
				name: 'Pepe'
			};
			appmgr.on = function( sigName, cb ) {
				cb( Object.keys(appmgr.apps) );
			};
			appmgr.on.isSignal = true;

			util.apiCall( appmgr.on, 'pepe', function(apps) {
				assert.equal( apps.length, 1 );
				assert.equal( apps[0], 'pepeID' );

				assert.equal( errorCount, 0 );
				done();
			});
		});

		it( 'basic signal, without cb', function() {
			var errorCount = 0;
			util.defaultErrorHandler = () => errorCount++;

			var appmgr = new AppMgr();
			appmgr.apps.pepeID = {
				name: 'Pepe'
			};

			var onCalled=0;
			appmgr.on = function( sigName, cb ) {
				cb( Object.keys(appmgr.apps) );
				onCalled++;
			};
			appmgr.on.isSignal = true;

			util.apiCall( appmgr.on, 'pepe' );
			assert.equal( errorCount, 0 );
			assert.equal( onCalled, 1 );
		});

		it( 'without parameters and cb', function() {
			var appmgr = new AppMgr();
			var count=0;
			appmgr.getAll = () => count++;
			util.apiCall( appmgr.getAll );
			assert.equal( count, 1 );
		});

		it( 'without parameters, but with error', function(done) {
			var appsCount = 0;
			var count = 0;
			global.log = {
				error: function() {
					count++;
					done();
				}
			};

			var appmgr = new AppMgr();
			appmgr.getAll = function(cb) {
				cb( new Error('pepe'), [] );
			};
			appmgr.apps.pepeID = {
				name: 'Pepe'
			};

			util.apiCall( appmgr.getAll, function(apps) {
				appsCount = apps.length;
			});
		});
	});
});
