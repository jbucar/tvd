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

function ArgenteamMock() {
	var self = {};
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
			var web = new WebModule( new ArgenteamMock() );
			assert(web);
		});
	});

	describe('start/stop', function() {
		it( 'basic', function() {
			var remote = new RemoteApiMock();

			var web = new WebModule( new ArgenteamMock() );
			assert(web);

			remote.start( web );
			assert.equal( remote.handlers().length, 3 );

			remote.stop( web );
		});
	});

	describe('methods', function() {
		var remote;
		var web;
		var argenteamImpl;

		beforeEach(function() {
			remote = new RemoteApiMock();
			assert(remote);

			argenteamImpl = new WebModule( new ArgenteamMock() );
			web = new WebModule( argenteamImpl );
			assert(web);

			remote.start(web);
		});

		afterEach(function() {
			remote.stop(web);
		});

		describe('handle GET /argenteam request', function() {
			it( 'should handle get', function(done) {
				//	Check handler added
				var handler = remote.getHandler( 'GET', '/argenteam' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				web.sendResult = function( res, st, mime, html ) {
					sendResult++;
					assert.equal( st, 200 );
					done();
				};
				handler.cb( {}, {} );
			});
		});

		describe('handle POST requests', function() {
			it( 'should handle refresh', function(done) {
				let checkCount=0;
				argenteamImpl.checkFeeds = function() {
					checkCount++;
				};

				//	Check handler added
				var handler = remote.getHandler( 'POST', '/argenteam/checkFeeds' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				web.sendResult = function( res, st, mime, html ) {
					sendResult++;
					assert.equal( st, 200 );
					done();
				};
				handler.cb( {}, {} );
				assert.equal( checkCount, 1 );
			});

			it( 'should handle addID', function(done) {
				let checkCount=0;
				argenteamImpl.addID = function(type,id) {
					assert.equal( type, 'movie' );
					assert.equal( id, 'pepe' );
					checkCount++;
					return bPromise.resolve();
				};

				//	Check handler added
				var handler = remote.getHandler( 'POST', '/argenteam/addID' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				web.sendResult = function( res, st, mime, html ) {
					sendResult++;
					assert.equal( st, 200 );
					done();
				};
				var req = {
					data: 'type=movie&id=pepe'
				};
				handler.cb( req, {} );
				assert.equal( checkCount, 1 );
			});

			it( 'should handle and fail addID', function(done) {
				let checkCount=0;
				argenteamImpl.addID = function(type,id) {
					assert.equal( type, 'serie' );
					assert.equal( id, 'pepe' );
					checkCount++;
					return bPromise.reject(new Error('pepe'));
				};

				//	Check handler added
				var handler = remote.getHandler( 'POST', '/argenteam/addID' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				web.sendResult = function( res, st, mime, html ) {
					sendResult++;
					assert.equal( st, 200 );
					done();
				};
				var req = {
					data: 'type=serie&id=pepe'
				};
				handler.cb( req, {} );
				assert.equal( checkCount, 1 );
			});
		});
	});
})
