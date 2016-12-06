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

function TorrentMock() {
    // hash: 'd.get_hash',
    // torrent: 'd.get_tied_to_file',
    // torrentsession: 'd.get_loaded_file',
    // path: 'd.get_base_path',
    // name: 'd.get_base_filename',
    // size: 'd.get_size_bytes',
    // skip: 'd.get_skip_total',
    // completed: 'd.get_completed_bytes',
    // down_rate: 'd.get_down_rate',
    // down_total: 'd.get_down_total',
    // up_rate: 'd.get_up_rate',
    // up_total: 'd.get_up_total',
    // message: 'd.get_message',
    // bitfield: 'd.get_bitfield',
    // chunk_size: 'd.get_chunk_size',
    // chunk_completed: 'd.get_completed_chunks',
    // createdAt: 'd.creation_date',
    // active: 'd.is_active',
    // open: 'd.is_open',
    // complete: 'd.get_complete',
    // hashing: 'd.is_hash_checking',
    // hashed: 'd.is_hash_checked',
    // leechers: 'd.get_peers_accounted',
    // seeders: 'd.get_peers_complete',
    // free_disk_space: 'd.free_diskspace',
	var self = {};
	self.getAll = function() {
		return bPromise.resolve([]);
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
			var web = new WebModule( new TorrentMock() );
			assert(web);
		});
	});

	describe('start/stop', function() {
		it( 'basic', function() {
			var remote = new RemoteApiMock();

			var web = new WebModule( new TorrentMock() );
			assert(web);

			remote.start( web );
			assert.equal( remote.handlers().length, 1 );

			remote.stop( web );
		});
	});

	describe('methods', function() {
		var remote;
		var web;
		var torrentClient;

		beforeEach(function() {
			remote = new RemoteApiMock();
			assert(remote);

			torrentClient = new TorrentMock();
			web = new WebModule( torrentClient );
			assert(web);

			remote.start(web);
		});

		afterEach(function() {
			remote.stop(web);
		});

		function createTorrent() {
			return {
				name: 'pepe',
				size: 328525,
				completed: 32525,
				up_rate: 2,
				down_rate: 25
			};
		}

		describe('handle GET /torrent request', function() {
			it( 'should handle when no torrents', function(done) {
				//	Check handler added
				var handler = remote.getHandler( 'GET', '/torrent' );
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

			it( 'should handle error', function(done) {
				torrentClient.getAll = function() {
					return bPromise.reject( new Error('pepe') );
				};
				
				//	Check handler added
				var handler = remote.getHandler( 'GET', '/torrent' );
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

			it( 'should handle url when there are torrents', function(done) {
				let torrents = [createTorrent()];
				torrentClient.getAll = function() {
					return bPromise.resolve( torrents );
				};
				
				//	Check handler added
				var handler = remote.getHandler( 'GET', '/torrent' );
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
	});
})
