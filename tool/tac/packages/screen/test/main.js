'use strict';

var path = require('path');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var Mocks = require('mocks');
var Screen = require('../src/main');


describe('Screen', function() {
	var reg = null;
	var adapter = null;

	function TestProvider() {
		var self = {};
		self.turnOnCount = 0;
		self.turnOffCount = 0;

		self.turnOn = function() {
			self.turnOnCount++;
		};
		self.turnOff = function() {
			self.turnOffCount++;
		};
		return self;
	}

	beforeEach(function() {
		reg = Mocks.init();
		adapter = Mocks.ServiceAdapter(reg);
	});

	afterEach(function() {
		Mocks.fin();
		adapter = null;
		reg = null;
	});

	describe('constructor', function() {
		it('should construct basic screen', function() {
			new Screen();
		});

		it('should start/stop screen', function(done) {
			var s = new Screen();
			s.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				assert.isFunction(api.isActive);
				assert.isFunction(api.setActive);
				assert.isFunction(api.setProvider);
				assert.isFunction(api.turnOn);
				assert.isFunction(api.turnOff);
				assert.isFunction(api.on);
				assert.isFunction(api.removeListener);
				s.onStop(done);
			});
		});

		it('should start/stop/start/stop screen', function(done) {
			var s = new Screen();

			s.onStart(function(err,api) {
				assert.equal(err);
				s.onStop(function() {
					s.onStart(function(err1,api1) {
						assert.equal(err1);
						assert(api1);
						assert( api !== api1 );
						s.onStop(done);
					});
				});
			});
		});
	});

	describe('methods (without provider)', function() {
		var s = null;
		var api = null;

		beforeEach(function(done) {
			s = new Screen();
			s.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert.isObject(api);
				done();
			});
		});

		afterEach(function(done) {
			s.onStop(done);
		});

		it('isActive: should return false', function() {
			assert.isTrue( api.isActive() );
		});

		it('setActive', function() {
			var _assert_state;
			api.on( 'changed', function(state) {
				assert.equal( state, _assert_state );
			});

			assert.isTrue( api.isActive() );
			_assert_state = false;
			api.setActive(false);
			api.setActive(false);

			_assert_state = true;
			api.setActive(true);
			api.setActive(true);
		});

		it('setProvider: validate provider', function() {
			api.setProvider( null );
			api.setProvider( {} );
			var obj = {};
			obj.turnOn = function() {};
			api.setProvider( obj );
			obj.turnOff = function() {};
			api.setProvider( obj );
		});

		it('turnOn: should turn on screen', function() {
			api.turnOn();
			assert.isTrue( api.isActive() );
		});

		it('turnOff: should turn off screen', function() {
			api.turnOff();
			assert.isFalse( api.isActive() );
		});
	});

	describe('methods (with provider)', function() {
		var s = null;
		var api = null;
		var _provider = null;

		beforeEach(function(done) {
			s = new Screen();
			s.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert.isObject(api);
				_provider = new TestProvider();
				api.setProvider( _provider );
				done();
			});
		});

		afterEach(function(done) {
			s.onStop(done);
			_provider = null;
			api = null;
		});

		it('turnOn: should turn on screen', function() {
			api.turnOn();
			assert.isTrue( api.isActive() );
			assert.equal( _provider.turnOnCount, 1 );
		});

		it('turnOff: should turn off screen', function() {
			api.turnOff();
			assert.isFalse( api.isActive() );
			assert.equal( _provider.turnOffCount, 1 );
		});
	});
});
