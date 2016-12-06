'use strict';

var http = require('http');
var Url = require ('url');
var tvdutil = require('tvdutil');
var util = require('util');
var bPromise = require('bluebird');

var Mocks = require('mocks');
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;

var WebModule = require('../src/webmodule');

function RemoteApiMock() {
	var self = {};
	var _handlers = [];
	var defaultPort = 2000;

	function getHandler( req, path ) {
		return _handlers.find(function(mod) {
			return mod.req === req && mod.path === path;
		});
	}

	function setup(mod) {
		mod.port = function() {
			return 2000;
		};
		mod.addHandler = function( req, path, cb ) {
			//console.log( 'addHandler: req=%s, path=%s', req, path );
			_handlers.push( { req:req, path:path, cb: cb } );
		};
		mod.sendResult = function( res, st, mime, xml ) {
		};
		mod.reg = function() { return _reg; }
		mod.getBaseUrl = function() {
			return 'http://pepe:' + defaultPort;
		};
	};

	self.start = function(mod) {
		_handlers = [];
		setup(mod);
		mod.start();
	};

	self.stop = function(mod) {
		mod.stop();
	}

	self.handlers = function() {
		return _handlers;
	};

	self.getHandler = function( req, path ) {
		return _handlers.find(function(mod) {
			return mod.req === req && mod.path === path;
		});
	};

	return self;
}

describe('WebModule', function() {
	var _reg = null;
	var _adapter = null;

	beforeEach(function() {
		_reg = Mocks.init('silly');
	});

	afterEach(function() {
		_reg = null;
	});

	describe('constructor', function() {
		it( 'basic constructor', function() {
			var web = new WebModule();
			assert(web);
		});
	});

	describe('start/stop', function() {
		it( 'basic', function() {
			var remote = new RemoteApiMock();

			var web = new WebModule();
			assert(web);

			remote.start( web );
			assert.equal( remote.handlers().length, 2 );

			remote.stop( web );
		});
	});

	describe('methods', function() {
		var remote;
		var web;

		beforeEach(function() {
			remote = new RemoteApiMock();
			assert(remote);

			web = new WebModule();
			assert(web);

			remote.start(web);
		});

		afterEach(function() {
			remote.stop(web);
		});

		describe('handle GET /youtube request', function() {
			it( 'should handle url', function() {
				//	Check handler added
				var handler = remote.getHandler( 'GET', '/youtube' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				web.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 200 );
				};
				handler.cb( {}, {} );
				assert.equal( sendResult, 1 );
			});
		});

		describe('handle POST /youtube/{opts}', function() {
			it( 'should handle url', function(done) {
				//	Check handler added
				var handler = remote.getHandler( 'POST', '/youtube' );
				assert(handler);
				assert(handler.cb);

				let oldRequest = tvdutil.doRequestAsync;
				tvdutil.doRequestAsync = function(opts) {
					return bPromise.resolve();
				};

				var req = {
					data: 'uri=pepe'
				};
				web.sendResult = function( res, st, mime, xml ) {
					assert.equal( st, 200 );
					tvdutil.doRequestAsync = oldRequest;
					done();
				};
				handler.cb( req, {} );
			});

			it( 'should handle url with error', function(done) {
				//	Check handler added
				var handler = remote.getHandler( 'POST', '/youtube' );
				assert(handler);
				assert(handler.cb);

				let oldRequest = tvdutil.doRequestAsync;
				tvdutil.doRequestAsync = function(opts) {
					return bPromise.reject(new Error('pepe'));
				};

				var req = {
					data: 'uri=pepe'
				};
				web.sendResult = function( res, st, mime, xml ) {
					assert.equal( st, 200 );
					tvdutil.doRequestAsync = oldRequest;
					done();
				};
				handler.cb( req, {} );
			});
		});
	});
})
