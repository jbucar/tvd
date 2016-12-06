'use strict';

var http = require('http');
var Url = require ('url');
var tvdutil = require('tvdutil');
var util = require('util');
var fs = require('fs');
var bPromise = require('bluebird');

var Mocks = require('mocks');
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;

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
			let obj = { req:req, path:path, cb: cb };
			_handlers.push( obj );
			return obj;
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


var TvdUtilMock = {
	unzipFile: function() {
		var dest = path.join( workDir, 'unziped.XXXXXXXX' );
		return mktemp.createDirAsync( dest );
	},
	forwardEventEmitter: tvdutil.forwardEventEmitter,
	warnErr: tvdutil.warnErr,
	findFiles: tvdutil.findFiles,
	BasicWaitCondition: tvdutil.BasicWaitCondition
};


function Stream() {
	var self = {};
	self.callbacks = {};
	self.on = function( sig, cbk ) {
		self.callbacks[sig] = cbk;
	};
	self.pipe = function() {};
	return self;
};

var busBoyMock = Stream;

describe('WebModule', function() {
	var _reg = null;
	var _adapter = null;
	var MockedWebModule;

	beforeEach(function() {
		_reg = Mocks.init('silly');
		_adapter = new Mocks.ServiceAdapter( _reg );
		_reg.put( 'system', new Mocks.System() );

		let pkgMgr = new Mocks.PackageManager([]);
		pkgMgr.install = function(options,cb) {
			if (options.url) {
				cb(undefined);
			}
		};
		_reg.put( 'pkgmgr', pkgMgr );

		MockedWebModule = Mocks.mockRequire( require, '../src/webmodule', { 'tvdutil': TvdUtilMock, 'busboy': busBoyMock } );
	});

	afterEach(function() {
		_adapter = null;
		_reg = null;
	});

	describe('constructor', function() {
		it( 'basic constructor', function() {
			var web = new MockedWebModule(_adapter);
			assert(web);
		});
	});

	describe('start/stop', function() {
		it( 'basic', function() {
			var remote = new RemoteApiMock();

			var web = new MockedWebModule(_adapter);
			assert(web);

			remote.start( web );
			assert.equal( remote.handlers().length, 3 );
			remote.stop( web );
		});
	});

	describe('methods', function() {
		var remote;
		var web;
		var module;

		beforeEach(function() {
			remote = new RemoteApiMock();
			assert(remote);

			web = new MockedWebModule( _adapter );
			assert(web);

			remote.start(web);
		});

		afterEach(function() {
			remote.stop(web);
		});

		describe('handle GET /system request', function() {
			it( 'should handle get', function(done) {
				//	Check handler added
				var handler = remote.getHandler( 'GET', '/system' );
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

		describe('handle POST /system/updateFirmware request', function() {
			var oldCreate;

			beforeEach(function() {
				oldCreate = fs.createWriteStream;
			});

			afterEach(function() {
				fs.createWriteStream = oldCreate;
			});

			it( 'should handle', function(done) {
				let checkCount=0;
				_reg.get('system').updateFirmware = function(url,now) {
					checkCount++;
				};

				//	Check handler added
				var handler = remote.getHandler( 'POST', '/system/updateFirmware' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				web.sendResult = function( res, st, mime, html ) {
					sendResult++;
					assert.equal( st, 200 );
					done();
				};

				let curBusboy;
				let req = {
					pipe: function(busBoy) {
						curBusboy = busBoy;
					}
				};
				handler.cb( req, {} );
				assert(curBusboy);

				fs.createWriteStream = function(name) {
					assert( name.indexOf('fileNamePepe') >= 0 );
					return new Stream();
				};

				let curStream;
				curBusboy.callbacks['file']( 'pepe', new Stream(), 'otra' );
				curBusboy.callbacks['file']( 'firmware', new Stream(), 'fileNamePepe' );
				curBusboy.callbacks['finish']();
				assert.equal( checkCount, 0 );
			});

			it( 'should handle error on file', function(done) {
				let checkCount=0;
				_reg.get('system').updateFirmware = function(url,now) {
					checkCount++;
				};

				//	Check handler added
				var handler = remote.getHandler( 'POST', '/system/updateFirmware' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				web.sendResult = function( res, st, mime, html ) {
					sendResult++;
					assert.equal( st, 500 );
					done();
				};

				let curBusboy;
				let req = {
					pipe: function(busBoy) {
						curBusboy = busBoy;
					}
				};
				handler.cb( req, {} );
				assert(curBusboy);

				fs.createWriteStream = function(name) {
					assert( name.indexOf('fileNamePepe') >= 0 );
					return new Stream();
				};

				let file = new Stream();
				curBusboy.callbacks['file']( 'firmware', file, 'fileNamePepe' );

				file.callbacks['error']( new Error('pepe') );
				assert.equal( checkCount, 0 );
			});

			it( 'should handle error on busboy', function(done) {
				let checkCount=0;
				_reg.get('system').updateFirmware = function(url,now) {
					checkCount++;
				};

				//	Check handler added
				var handler = remote.getHandler( 'POST', '/system/updateFirmware' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				web.sendResult = function( res, st, mime, html ) {
					sendResult++;
					assert.equal( st, 500 );
					done();
				};

				let curBusboy;
				let req = {
					pipe: function(busBoy) {
						curBusboy = busBoy;
					}
				};
				handler.cb( req, {} );
				assert(curBusboy);

				fs.createWriteStream = function(name) {
					assert( name.indexOf('fileNamePepe') >= 0 );
					return new Stream();
				};

				let file = new Stream();
				curBusboy.callbacks['error']( new Error('pepe') );
				assert.equal( checkCount, 0 );
			});
		});

		describe('handle POST /system/installPkt request', function() {
			var oldCreate;

			beforeEach(function() {
				oldCreate = fs.createWriteStream;
			});

			afterEach(function() {
				fs.createWriteStream = oldCreate;
			});

			it( 'should handle', function(done) {
				let checkCount=0;
				_reg.get('system').updateFirmware = function(url,now) {
					checkCount++;
				};

				//	Check handler added
				var handler = remote.getHandler( 'POST', '/system/installPkt' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				web.sendResult = function( res, st, mime, html ) {
					sendResult++;
					assert.equal( st, 200 );
					done();
				};

				let curBusboy;
				let req = {
					pipe: function(busBoy) {
						curBusboy = busBoy;
					}
				};
				handler.cb( req, {} );
				assert(curBusboy);

				fs.createWriteStream = function(name) {
					assert( name.indexOf('fileNamePepe') >= 0 );
					return new Stream();
				};

				curBusboy.callbacks['file']( 'pktFile', new Stream(), 'fileNamePepe' );
				curBusboy.callbacks['finish']();
				assert.equal( checkCount, 0 );
			});

			it( 'should handle error on install', function(done) {
				let checkCount=0;
				_reg.get('system').updateFirmware = function(url,now) {
					checkCount++;
				};

				//	Check handler added
				var handler = remote.getHandler( 'POST', '/system/installPkt' );
				assert(handler);
				assert(handler.cb);

				let pkgMgr = _reg.get('pkgmgr');
				pkgMgr.install = function(options,cb) {
					cb( new Error('pepe') );
				};

				var sendResult = 0;
				web.sendResult = function( res, st, mime, html ) {
					sendResult++;
					assert.equal( st, 500 );
					done();
				};

				let curBusboy;
				let req = {
					pipe: function(busBoy) {
						curBusboy = busBoy;
					}
				};
				handler.cb( req, {} );
				assert(curBusboy);

				fs.createWriteStream = function(name) {
					assert( name.indexOf('fileNamePepe') >= 0 );
					return new Stream();
				};

				curBusboy.callbacks['file']( 'pktFile', new Stream(), 'fileNamePepe' );
				curBusboy.callbacks['finish']();
				assert.equal( checkCount, 0 );
			});

			it( 'should handle error on parsePost', function(done) {
				let checkCount=0;
				_reg.get('system').updateFirmware = function(url,now) {
					checkCount++;
				};

				//	Check handler added
				var handler = remote.getHandler( 'POST', '/system/installPkt' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				web.sendResult = function( res, st, mime, html ) {
					sendResult++;
					assert.equal( st, 500 );
					done();
				};

				let curBusboy;
				let req = {
					pipe: function(busBoy) {
						curBusboy = busBoy;
					}
				};
				handler.cb( req, {} );
				assert(curBusboy);

				fs.createWriteStream = function(name) {
					assert( name.indexOf('fileNamePepe') >= 0 );
					return new Stream();
				};

				curBusboy.callbacks['error']( new Error('pepe') );
				assert.equal( checkCount, 0 );
			});
		});
	});
})
