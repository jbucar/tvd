'use strict';

var EventEmitter = require('events').EventEmitter;
var tvdutil = require('tvdutil');
var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var Mocks = require('mocks');

var NotificationService = require('../src/notification');

describe('Notification', function() {
	var reg = null;

	beforeEach(function() {
		reg = Mocks.init();
	});

	afterEach(function() {
		reg = null;
		Mocks.fin();
	});

	function checkApi(api) {
		assert.isObject(api);
		assert.isFunction(api.on);
		assert.strictEqual( 'notification', api.getWebAPI().name );
		assert.isArray( api.getWebAPI()['public'] );
	}

	describe('constructor', function() {
		it('should construct notification service', function() {
			var ns = new NotificationService();
			assert.isObject(ns);
		});

		it('should start/stop notification service', function(done) {
			var adapter = new Mocks.ServiceAdapter( reg );
			var ns = new NotificationService(adapter);
			ns.onStart(function(err,api) {
				assert.equal(err);
				checkApi(api);
				ns.onStop(done);
			});
		});
	});

	describe('api', function() {
		var ns = null;
		var api = null;

		beforeEach(function(done) {
			var adapter = new Mocks.ServiceAdapter( reg );
			ns = new NotificationService(adapter);
			ns.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				checkApi(api);
				done();
			});

		});

		afterEach(function(done) {
			ns.onStop( function() {
				api = null;
				ns = null;
				done();
			});
		});

		it('should receive simple notification', function() {
			var count = 0;
			function onNotification( evt ) {
				assert.isObject(evt);
				assert.strictEqual( 'TestEvent', evt.name );
				count++;
			}
			api.on('TestNotification', onNotification);
			api.emit('TestNotification', {name: 'TestEvent'});
			assert.strictEqual( 1, count );
		});

		it('should receive simple notification multiple times', function() {
			var count = 0;
			function onNotification( evt ) {
				assert.isObject(evt);
				assert.strictEqual( 'TestEvent', evt.name );
				count++;
			}
			api.on('TestNotification', onNotification);
			api.emit('TestNotification', {name: 'TestEvent'});
			api.emit('TestNotification', {name: 'TestEvent'});
			api.emit('TestNotification', {name: 'TestEvent'});
			assert.strictEqual( 3, count );
		});

		it('should receive simple notification only once', function() {
			var count = 0;
			function onNotification( evt ) {
				assert.isObject(evt);
				assert.strictEqual( 'TestEvent', evt.name );
				count++;
			}
			api.once('TestNotification', onNotification);
			api.emit('TestNotification', {name: 'TestEvent'});
			api.emit('TestNotification', {name: 'TestEvent'});
			api.emit('TestNotification', {name: 'TestEvent'});
			assert.strictEqual( 1, count );
		});

		it('should not receive simple notification after removing listener', function() {
			var count = 0;
			function onNotification( evt ) {
				assert.isObject(evt);
				assert.strictEqual( 'TestEvent', evt.name );
				count++;
			}
			api.on('TestNotification', onNotification);
			api.emit('TestNotification', {name: 'TestEvent'});
			api.removeListener('TestNotification', onNotification);
			api.emit('TestNotification', {name: 'TestEvent'});
			assert.strictEqual( 1, count );
		});

		it('should forward events from a custom api', function() {
			function TestApi() {
				var self = this || {};

				self.events = new EventEmitter();
				tvdutil.forwardEventEmitter(self, self.events);

				return self;
			}

			reg.put( 'testApi', new TestApi() );

			var count=0;
			function onNotification( evt ) {
				assert.isObject(evt);
				assert.equal( evt.name, 'test' );
				count++;
			}
			api.forwardEvents( 'ar.edu.unlp.info.lifia.tvd.testApi', 'signal1', 'SignalFromTestApi' );
			api.on('SignalFromTestApi', onNotification);

			reg.get( 'testApi' ).events.emit( 'signal1', { name: 'test' } );
			assert.equal( count, 1 );
		});

		it('should not forward events if api was removed from registry', function() {
			var testApi = {
				events: new EventEmitter(),
			};
			tvdutil.forwardEventEmitter(testApi, testApi.events);

			reg.put('testApi', testApi);
			reg.remove('testApi');

			api.forwardEvents('ar.edu.unlp.info.lifia.tvd.testApi', 'signal1', 'SignalFromTestApi');
			var count=0;
			api.on('SignalFromTestApi', function(evt) {
				assert.isObject(evt);
				assert.equal( evt.name, 'test' );
				count++;
			});
			testApi.events.emit( 'signal1', { name: 'test' } );
			assert.equal( count, 0 );
		});

		it('should forward events from a custom api not in registry', function() {
			function TestApi() {
				var self = this || {};

				self.events = new EventEmitter();
				tvdutil.forwardEventEmitter(self, self.events);

				return self;
			}

			var count=0;
			function onNotification( evt ) {
				assert.isObject(evt);
				assert.equal( evt.name, 'test' );
				count++;
			}
			api.forwardEvents( 'ar.edu.unlp.info.lifia.tvd.testApi', 'signal1', 'SignalFromTestApi' );
			api.on('SignalFromTestApi', onNotification);

			var test = new TestApi();
			reg.put( 'testApi', test );
			test.events.emit( 'signal1', { name: 'test' } );
			assert.equal( count, 1 );

			reg.remove( 'testApi' );
			test.events.emit( 'signal1', { name: 'test' } );
			assert.equal( count, 1 );
		});
	});
});
