'use strict';

var path = require('path');
var assert = require('chai').assert;
var Mocks = require('mocks');
var Platform = require('../src/main');

describe('Platform', function() {
	var _reg = null;
	var _adapter = null;

	function setupAdapter(platJS, platJSON, buildJSON) {
		_adapter.getInstallPath = function(p) {
			switch (p) {
				case 'platform.js': return path.join(__dirname, platJS);
				case 'platform.json': return path.join(__dirname, platJSON);
				case 'build.json': return path.join(__dirname, buildJSON);
				default: return __dirname;
			}
		};
	}

	beforeEach( function() {
		_reg = Mocks.init('silly');
		_adapter = new Mocks.ServiceAdapter(_reg);
		assert(_adapter);
	});

	afterEach( function () {
		_adapter = null;
		Mocks.fin();
		_reg = null;
	});

	describe( 'constructor', function() {
		it('should create', function() {
			assert( new Platform() );
		});

		it('should create without new', function() {
			assert( Platform() );
		});

		it('should start', function(done) {
			setupAdapter( 'impl.js', 'platform.json', 'build.json' );
			var plat = new Platform(_adapter);
			plat.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject( api );
				plat.onStop(done);
			});
		});

		it('should not load if platform implementation not found', function(done) {
			setupAdapter( 'implx.js', 'platform.json', 'build.json' );
			var plat = new Platform(_adapter);
			plat.onStart(function(err,api) {
				assert.equal(err.message, "Cannot find module '" + __dirname + "/implx.js'");
				assert.equal( api );
				done();
			});
		});

		it('should not load if platform implementation is invalid', function(done) {
			setupAdapter( 'fail4.js', 'platform.json', 'build.json' );
			var plat = new Platform(_adapter);
			plat.onStart(function(err,api) {
				assert.equal(err.message, "Invalid platform implementation");
				assert.equal( api );
				done();
			});
		});

		it('should not load if platform config file not found', function(done) {
			var plat = new Platform(_adapter);
			setupAdapter( 'impl.js', 'platformx.json', 'build.json' );
			plat.onStart(function(err,api) {
				assert.equal(err.message, "ENOENT: no such file or directory, open '" + __dirname + "/platformx.json'");
				assert.equal( api );
				done();
			});
		});

		it('should not load if build config file not found', function(done) {
			var plat = new Platform(_adapter);
			setupAdapter( 'impl.js', 'platform.json', 'buildx.json' );
			plat.onStart(function(err,api) {
				assert.equal(err.message, "ENOENT: no such file or directory, open '" + __dirname + "/buildx.json'");
				assert.equal( api );
				done();
			});
		});

		it('should start/stop', function(done) {
			var plat = new Platform(_adapter);
			setupAdapter( 'impl.js', 'platform.json', 'build.json' );
			plat.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject( api );
				assert( api.createAudio );
				plat.onStop(done);
			});
		});

		it('should not load no implementation', function(done) {
			setupAdapter( 'fail.js', 'platform.json', 'build.json' );
			var plat = new Platform(_adapter);
			plat.onStart(function(err,api) {
				assert.equal(err.message, "require(...) is not a function");
				assert.equal( api );
				done();
			});
		});

		it('should not load if implementation is incomplete (audio)', function(done) {
			setupAdapter( 'fail2.js', 'platform.json', 'build.json' );
			var plat = new Platform(_adapter);
			plat.onStart(function(err,api) {
				assert.equal(err.message, "Factory createAudio is missing");
				assert.equal( api );
				done();
			});
		});

		it('should not load if implementation is incomplete (input)', function(done) {
			var plat = new Platform(_adapter);
			setupAdapter( 'no_input.js', 'platform.json', 'build.json' );
			plat.onStart(function(err,api) {
				assert.equal(err.message, 'Factory createInput is missing');
				assert.equal( api );
				done();
			});
		});

		it('should not load if implementation is incomplete (screen)', function(done) {
			var plat = new Platform(_adapter);
			setupAdapter( 'fail3.js', 'platform.json', 'build.json' );
			plat.onStart(function(err,api) {
				assert.equal(err.message, 'Factory createInput is missing');
				assert.equal( api );
				done();
			});
		});
	});

	describe( 'methods', function() {
		var api = null;

		beforeEach( function(done) {
			var plat = new Platform(_adapter);
			setupAdapter( 'impl.js', 'platform.json', 'build.json' );
			plat.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert.isObject(api);
				done();
			});
		});

		it('should create audio', function() {
			assert( api.createAudio() );
		});

		it('should create input', function() {
			assert( api.createInput() );
		});

		it('should not get invalid pkg cfg', function() {
			assert( !api.getConfig('invalid') );
		});

		it('should get valid cfgs', function() {
			assert.equal( api.getConfig('backendURL'), 'http://www.pepe.com' );
			assert.equal( api.getConfig('htmlshell').cmd, 'htmlshell' );
			assert.equal( api.getConfig('keyboardLayout').VK_ENTER.virtual, 13 );
			assert.equal( api.getConfig('keyboardLayout').VK_ENTER.raw, 28 );
		});
	});
});
