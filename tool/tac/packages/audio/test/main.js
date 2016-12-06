'use strict';

var assert = require('assert');
var _ = require('lodash');
var Mocks = require('mocks');

var AudioModule = require('../src/main');
var AudioAlsa = require('audio_alsa');

describe('AudioModule', function() {
	var _module = null;
	var _reg = null;
	var _adapter = null;

	function AudioFactory(impl) {
		var self = this || {};

		self.createAudio = function() {
			return impl;
		};

		return self;
	}

	beforeEach( function(done) {
		_reg = Mocks.init();
		_adapter = new Mocks.ServiceAdapter(_reg);
		_module = new AudioModule(_adapter);
		assert( _module );
		done();
	});

	afterEach( function (done) {
		_module = null;
		Mocks.fin();
		_adapter = null;
		_reg = null;
		done();
	});

	it('should return audio api on start', function(done) {
		_reg.put( 'platform', new AudioFactory() );
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert( _.isFunction(api.mute) );
			assert( _.isFunction(api.isMuted) );
			assert( _.isFunction(api.setVolume) );
			assert( _.isFunction(api.getVolume) );
			assert( _.isFunction(api.getWebAPI) );
			var webApi = api.getWebAPI();
			assert( _.isObject(webApi) );
			assert.equal( webApi.name, 'audio' );
			assert.equal( webApi['public'].length, 8 );
			_module.onStop(done);
		});
	});

	it('should start/stop', function(done) {
		function TestAudio() {
			var self = this;

			self.name = function() { return "dummy"; };
			self.setVolume = function( vol, cb ) {
				cb( new Error( 'Alguno' ) );
			};
			self.mute = function( st, cb ) {
				cb( undefined );
			};

			return Object.freeze(self);
		}

		_reg.put( 'platform', new AudioFactory( new TestAudio() ) );
		_module.onStart(function(err,api) {
			assert.equal(err);
			var vol = api.getVolume();
			assert( vol >= 0 );
			api.volumeUp();
			_module.onStop(function() {
				_module.onStart(function(err,api2) {
					assert.equal(err);
					assert( api !== api2 );
					var vol1 = api2.getVolume();
					assert.equal( vol1, vol );
					_module.onStop(done);
				});
			});
		});
	});

	describe('methods', function() {
		var _api = null;

		beforeEach(function(done) {
			_reg.put( 'platform', new AudioFactory() );
			_module.onStart(function(err,impl) {
				assert.equal(err);
				_api = impl;
				assert( _api );
				done();
			});
		});

		afterEach( function (done) {
			_module.onStop(done);
			_api = null;
		});

		it('should mute/unmute master', function (done) {
			assert( !_api.isMuted() );

			var evts = [];
			_api.on('changed', function(evt) {
				evts.push( evt );
			});

			_api.mute( true, function (err) {
				assert( !err );
				assert( _api.isMuted() );
				_api.mute( false, function (err) {
					assert( !err );
					assert( !_api.isMuted() );

					assert.equal( evts.length, 2 );
					assert.equal( evts[0].isMuted, true );
					assert.equal( evts[1].isMuted, false );

					done();
				});
			});
		});

		it('should notify volume when mute', function (done) {
			assert( !_api.isMuted() );

			var evts = [];
			_api.on('changed', function(evt) {
				evts.push( evt );
			});

			_api.setVolume( 10, function (err) {
				assert( !err );
				_api.mute( true, function (err) {
					assert( !err );
					assert.equal( evts[1].volume, 10 );
					done();
				});
			});
		});

		it('should notify volume when unmute', function (done) {
			assert( !_api.isMuted() );

			var evts = [];
			_api.on('changed', function(evt) {
				evts.push( evt );
			});

			_api.setVolume( 10, function (err) {
				assert( !err );
				_api.mute( false, function (err) {
					assert( !err );
					assert.equal( evts[1].volume, 10 );
					done();
				});
			});
		});

		it('should toggle mute master', function (done) {
			assert( !_api.isMuted() );
			_api.toggleMute( function (err) {
				assert( !err );
				assert( _api.isMuted() );
				_api.toggleMute( function (err) {
					assert( !err );
					assert( !_api.isMuted() );
					done();
				});
			});
		});

		it('should support api calls without callbacks', function() {
			assert( !_api.isMuted() );
			_api.toggleMute();
			assert( _api.isMuted() );
			_api.setVolume( 10 );
			assert.equal( 10, _api.getVolume() );
		});

		it('should set/get master volume to 0%', function (done) {
			var evts = [];
			_api.on('changed', function(evt) {
				evts.push( evt );
			});

			_api.setVolume( 0, function (err) {
				assert(!err);
				assert.equal( 0, _api.getVolume() );

				assert.equal( evts.length, 1 );
				assert.equal( evts[0].volume, 0 );

				done();
			});
		});

		it('should set/get master volume to 50%', function (done) {
			_api.setVolume( 50, function (err) {
				assert(!err);
				assert.equal( 50, _api.getVolume() );
				done();
			});
		});

		it('should volumeUp/Down', function (done) {
			_api.setVolume( 50, function (err) {
				assert(!err);
				_api.volumeUp(function(err1) {
					assert(!err1);
					assert.equal( 55, _api.getVolume() );
					_api.volumeDown(function(err2) {
						assert(!err2);
						assert.equal( 50, _api.getVolume() );
						done();
					});
				});
			});
		});

		it('should volumeDown limit', function (done) {
			_api.setVolume( 0, function (err) {
				assert(!err);
				_api.volumeDown(function(err1) {
					assert(!err1);
					assert.equal( 0, _api.getVolume() );
					done();
				});
			});
		});

		it('should volumeUp limit', function (done) {
			_api.setVolume( 100, function (err) {
				assert(!err);
				_api.volumeUp(function(err1) {
					assert(!err1);
					assert.equal( 100, _api.getVolume() );
					done();
				});
			});
		});

		it('should set/get master volume to 100%', function (done) {
			_api.setVolume( 100, function (err) {
				assert(!err);
				assert.equal( 100, _api.getVolume() );
				done();
			});
		});

		it('should work without callback', function (done) {
			_api.setVolume( 100 );
			_api.mute(true);
			_api.toggleMute();
			_api.volumeUp();
			_api.volumeDown();
			done();
		});
	});

	describe('fails in platform methods', function() {
		var _api = null;

		function FailAudio() {
			var self = this;

			self.name = function() {
				return "Fail";
			};

			self.setVolume = function( vol, cb ) {
				cb( new Error( 'Alguno' ) );
			};

			self.mute = function( st, cb ) {
				cb( new Error( 'Alguno' ) );
			};

			return Object.freeze(self);
		}

		beforeEach( function(done) {
			_reg.put( 'platform', new AudioFactory( new FailAudio() ) );
			_module.onStart(function(err,impl) {
				assert.equal(err);
				_api = impl;
				assert( _api );
				done();
			});
		});

		afterEach( function (done) {
			_module.onStop(done);
			_api = null;
		});

		it('should support api calls without callbacks', function() {
			assert( !_api.isMuted() );
			_api.toggleMute();
			assert( !_api.isMuted() );
			_api.setVolume( 10 );
			assert.equal( 50, _api.getVolume() );
		});

		it('should not set volume to 100%', function (done) {
			_api.setVolume( 100, function (err) {
				assert(err);
				assert.equal( err.message, 'Alguno' );
				assert.equal( _api.getVolume(), 50 );
				done();
			});
		});

		it('should not toggle mute', function (done) {
			_api.mute( false, function (err) {
				assert(err);
				assert.equal( err.message, 'Alguno' );
				assert.equal( _api.isMuted(), false );
				done();
			});
		});
	});

	describe('methods for alsa', function() {
		var _api = null;

		beforeEach( function(done) {
			_reg.put( 'platform', new AudioFactory( new AudioAlsa() ) );
			_module.onStart(function(err,impl) {
				assert.equal(err);
				_api = impl;
				assert( _api );
				done();
			});
		});

		afterEach( function (done) {
			_module.onStop(done);
			_api = null;
		});

		it('should set/get master volume to 100%', function (done) {
			_api.setVolume( 100, function (err) {
				assert(!err);
				assert.equal( 100, _api.getVolume() );
				done();
			});
		});

		it('should mute/unmute master', function (done) {
			assert( !_api.isMuted() );
			_api.mute( true, function (err) {
				assert( !err );
				assert( _api.isMuted() );
				_api.mute( false, function (err) {
					assert( !err );
					assert( !_api.isMuted() );
					done();
				});
			});
		});
	});
});
