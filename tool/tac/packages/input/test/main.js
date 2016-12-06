'use strict';

var http = require('http');
var tvdutil = require('tvdutil');
var Mocks = require('mocks');
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;

var InputApi = require('../src/main');

describe('input', function() {
	var _reg = null;
	var _module = null;
	var _adapter = null;

	function TestInput() {
		var self = {};

		self.events = [];
		self.sendError = null;
		self.sendKey = self.sendMouseClick = function(rawCode,isUp,cb) {
			console.log( 'sendKey impl' );
			assert(cb);
			if (self.sendError ) {
				cb( self.sendError );
			}
			else {
				self.events.push( { "rawCode": rawCode, "isUp": isUp } );
				cb();
			}
		};
		self.sendMouseMove = function(deltaX,deltaY,cb) {
			console.log( 'sendMouseMove impl' );
			assert(cb);
			if (self.sendError ) {
				cb( self.sendError );
			}
			else {
				self.events.push( { "deltaX": deltaX, "deltaY": deltaY } );
				cb();
			}
		};
		self.name = function() { return 'test'; };
		self.start = function() { return true; };
		self.stop = function(cb) { cb(); };

		return self;
	}

	function changeFactory( fact ) {
		var factory = _reg.get('platform');
		factory.createInput = function() {
			return fact;
		};
	}

	beforeEach( function() {
		_reg = Mocks.init('silly');
		_adapter = Mocks.ServiceAdapter(_reg);

		_reg.put('platform', new Mocks.Factory() );

		_module = new InputApi(_adapter);
		assert( _module );
	});

	afterEach(function() {
		Mocks.fin();
		_module = null;
		_adapter = null;
		_reg = null;
	});

	it('should start/stop', function(done) {
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert( api );
			_module.onStop(done);
		});
	});

	it('should start/stop/start/stop', function(done) {
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert( api );
			assert( api.getWebAPI );
			var webApi = api.getWebAPI();
			assert.equal( webApi.name, 'input' );
			_module.onStop(function() {
				_module.onStart(function(err,api2) {
					assert.equal(err);
					assert( api2 );
					assert( api !== api2 );
					_module.onStop(done);
				});
			});
		});
	});

	describe('impl', function() {
		it('should create a new implementation', function(done) {
			changeFactory( new TestInput() );
			_module.onStart(function(err,api) {
				assert.equal(err);
				assert( api );
				_module.onStop(done);
			});
		});

		it('should validate start method on new implementation', function(done) {
			var input = new TestInput();
			input.start = function() { return false; };
			changeFactory( input );
			_module.onStart(function(err,api) {
				assert.equal(err.message,'Invalid implementation');
				assert.equal( api );
				done();
			});
		});
	});

	describe('api_dummy', function() {
		var api = null;

		beforeEach( function(done) {
			_module.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert( api );
				done();
			});
		});

		afterEach( function (done) {
			_module.onStop(function() {
				api = null;
				done();
			});
		});

		it('should send key with dummy', function() {
			api.sendKey( 'VK_ENTER', true );
		});
	});

	describe('api_test', function() {
		var api = null;
		var _input = null;

		beforeEach( function(done) {
			_input = new TestInput();
			changeFactory( _input );
			_module.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert( api );
				done();
			});
		});

		afterEach( function (done) {
			_module.onStop(done);
			api = null;
			_input = null;
		});

		it('should send key', function(done) {
			api.sendKey( 'VK_ENTER', true, function(err) {
				assert.equal( err, undefined );
				assert.equal( _input.events.length, 1 );
				assert.equal( _input.events[0].rawCode, 28 );
				assert.equal( _input.events[0].isUp, true );
				done();
			});
		});

		it('should send key async', function(done) {
			var notifyCB = null;
			var count=-1;
			_input.sendKey = function(rawCode,isUp,cb) {
				count++;
				assert.equal( rawCode, 28 );
				assert.equal( isUp, (count === 0) ? false : true );
				notifyCB = cb;
			};

			api.sendKey( 'VK_ENTER', false, function(err) {
				assert.equal( err, undefined );
			});

			assert( notifyCB !== null );

			api.sendKey( 'VK_ENTER', true, function(err) {
				assert.equal( err, undefined );
				done();
			});

			notifyCB();
			notifyCB();
		});

		it('should not send a invalid key or not maped', function(done) {
			api.sendKey( 'VK_PEPE', true, function(err) {
				assert.equal( err.message, 'Key code not maped: keyCode=VK_PEPE' );
				assert.equal( _input.events.length, 0 );
				done();
			});
		});

		it('should print error input implementation', function() {
			_input.sendError = new Error('algo');
			api.sendKey( 'VK_ENTER', true );
		});

		it('should send a empty array of keys', function(done) {
			var aKeys = [];
			api.sendKeys( aKeys, function(err) {
				assert.equal( err, undefined );
				done();
			});
		});

		it('should send a empty array of keys without cb', function() {
			var aKeys = [];
			api.sendKeys( aKeys );
		});

		it('should send a array of keys', function(done) {
			var aKeys = [ {keyCode:'VK_ENTER', isUp: false }, {keyCode:'VK_ENTER', isUp: true } ];
			var count=0;
			api.sendKeys( aKeys, function(err) {
				count++;
				assert.equal( count, 1 );
				assert.equal( err, undefined );
				assert.equal( _input.events.length, 2 );
				assert.equal( _input.events[0].rawCode, 28 );
				assert.equal( _input.events[0].isUp, false );
				assert.equal( _input.events[1].rawCode, 28 );
				assert.equal( _input.events[1].isUp, true );
				done();
			});
		});

		it('should send mouse move', function(done) {
			api.sendMouseMove( 1, 1, function(err) {
				assert.equal( err, undefined );
				assert.equal( _input.events.length, 1 );
				assert.equal( _input.events[0].deltaX, 1 );
				assert.equal( _input.events[0].deltaY, 1 );
				done();
			});
		});

		it('should send mouse move without cb', function() {
			api.sendMouseMove( 1, 1 );
		});

		it('should send click', function(done) {
			api.sendMouseClick( 'BTN_LEFT', true, function(err) {
				assert.equal( err, undefined );
				assert.equal( _input.events.length, 1 );
				assert.equal( _input.events[0].rawCode, 272 );
				assert.equal( _input.events[0].isUp, true );
				done();
			});
		});

		it('should not send a invalid key or not maped', function(done) {
			api.sendMouseClick( 'VK_PEPE', true, function(err) {
				assert.equal( err.message, 'sendMouseClick: Key code not maped: keyCode=VK_PEPE' );
				assert.equal( _input.events.length, 0 );
				done();
			});
		});

		it('should print error input implementation', function() {
			_input.sendError = new Error('algo');
			api.sendMouseClick( 'BTN_LEFT', true );
		});
	});
});
