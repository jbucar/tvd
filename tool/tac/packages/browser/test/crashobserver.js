'use strict';

var CrashObserver = require('../src/impl/crashobserver');
var bUtils = require('../src/util');
var Mocks = require('mocks');
var assert = require('assert');
var fs = require('fs');
var path = require('path');

describe('CrashObserver', function() {
	var _crash = null;
	var _tmpCrashDir = path.join('/tmp', 'htmlshell_minidumps');
	var _sys = {
		sendError: function() {}
	};

	function writeMinidump(name) {
		var fd = fs.openSync(path.join(_tmpCrashDir, name), 'w');
		assert( 28, fs.writeSync(fd, 'this is a test minidump file') );
		fs.closeSync(fd);
	}

	beforeEach( function() {
		Mocks.init('silly');
		_crash = new CrashObserver(_sys);
		assert( _crash );
	});

	afterEach( function() {
		_crash = undefined;
		bUtils.cleanDirectory(_tmpCrashDir, true);
		Mocks.fin();
	});

	describe('#init()', function() {
		it('should fail if given an invalid crash directory', function() {
			assert( !_crash.init() );
			assert( !_crash.init(path.join(__dirname, 'invalid', 'minidumps')) );
		});

		it('should succeed if given a valid crash directory', function() {
			fs.mkdirSync(_tmpCrashDir);
			assert( _crash.init(_tmpCrashDir) );
		});

		it('should create crash directory if not exist', function() {
			assert( _crash.init(_tmpCrashDir) );
		});
	});

	describe('#reportCrashes()', function() {
		it('should not call system.sendError() if no minidump found', function(done) {
			var failed = false;
			assert( _crash.init(path.join(_tmpCrashDir)) );
			_sys.sendError = function( error ) {
				failed = true;
				done(error);
			};
			_crash.reportCrashes();
			if (!failed) {
				done();
			}
		});

		it('should call system.sendError() if exist minidumps', function(done) {
			assert( _crash.init(_tmpCrashDir) );
			writeMinidump('chromium-renderer-minidump-a0ae424b7af0e32f.dmp');
			_sys.sendError = function( error ) {
				assert.equal( error.message, 'One or more minidump/s found in htmlshell crash directory');
				done();
			};
			_crash.reportCrashes();
		});
	});

	describe('#getEncodedMinidumps()', function() {
		it('should return an empty object if no minidump found', function() {
			assert( _crash.init(_tmpCrashDir) );
			assert.equal( 0, Object.keys(_crash.getEncodedMinidumps()).length );
		});

		it('should return an empty object if invalid minidump found', function() {
			assert( _crash.init(_tmpCrashDir) );
			writeMinidump('chromium-renderer-minidump-a0ae424b7af0e32f.blabla');
			assert.equal( 0, Object.keys(_crash.getEncodedMinidumps()).length );
		});

		it('should return an object width each minidump found', function() {
			assert( _crash.init(_tmpCrashDir) );
			writeMinidump('chromium-renderer-minidump-a0ae424b7af0e32f.dmp');
			var dumps = _crash.getEncodedMinidumps();
			var keys = Object.keys(dumps);
			assert.equal( 1, keys.length );
			var minidump = keys[0];
			assert.equal( minidump, path.join(_tmpCrashDir, 'chromium-renderer-minidump-a0ae424b7af0e32f.dmp') );
			assert.equal( dumps[minidump], 'dGhpcyBpcyBhIHRlc3QgbWluaWR1bXAgZmlsZQ==' );
		});
	});
});
