'use strict';

var http = require('http');
var Url = require ('url');
var tvdutil = require('tvdutil');
var util = require('util');

var Mocks = require('mocks');
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;

var Dial = require('../src/dial');
var ssdp = require('../src/node_modules/peer-ssdp/index');

function RemoteApi() {
	var self = {};
	return self;
}

describe('DIAL', function() {
	var _reg = null;
	var _adapter = null;
	var _dial = null;

	function AppMgrMock() {
		var self = {};
		self.get = function() {
			return null;
		};
		self.getAll = function() {
			return [];
		};
		self.isRunning = function() { return false; }
		return self;
	}

	beforeEach(function() {
		_reg = Mocks.init('silly');
		_reg.put( 'appmgr', new AppMgrMock() );
	});

	afterEach(function() {
		_dial = null;
		_reg = null;
	});

	describe('constructor', function() {
		it( 'basic constructor', function() {
			_dial = new Dial();
			assert(_dial);
		});
	});

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
	}

	function start(mod) {
		_handlers = [];
		setup(mod);
		mod.start();
	}

	function stop(mod) {
		mod.stop();
	}

	describe('start/stop', function() {
		it( 'basic', function() {
			_dial = new Dial();
			assert(_dial);

			setup(_dial);

			_dial.start();
			assert.equal( _handlers.length, 5 );
			_dial.stop();
		});

		it( 'check dial search', function(done) {
			var peer = null;
			var oldCreate = ssdp.createPeer;
			ssdp.createPeer = function() {
				var self = {};
				var _signals = {};
				self.on = function( sig, cb ) {
					_signals[sig] = cb;
				};
				self.removeListener = function( sig, cb ) {
					delete _signals[sig];
				};

				self.reply = function() {};
				self.alive = function() {};
				self.byebye = function() {};

				self.start = function() {
					_signals['ready']();
				};
				self.close = function() {};

				self.search = function( headers, addrs ) {
					_signals['search']( headers, addrs );
				};

				peer = self;
				return self;
			};

			_dial = new Dial();
			assert(_dial);

			start(_dial);

			assert(peer);
			peer.search( {ST:'urn:dial-multiscreen-org:service:dial:1'} );
			peer.search( {ST:'pepe'} );

			stop(_dial);
			ssdp.createPeer = oldCreate;
			done();
		});
	});

	describe('methods', function() {
		beforeEach(function() {
			_dial = new Dial();
			assert(_dial);
			start(_dial);
		});

		afterEach(function() {
			stop(_dial);
		});

		describe('handle GET /apps and /apps/{appname} request', function() {
			it( 'should handle url', function() {
				//	Check handler added
				var handler = getHandler( 'GET', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var req = {
					routeParams: []
				};
				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 204 );
				};
				handler.cb( req, {} );
				assert.equal( sendResult, 1 );
			});

			it( 'should handle url with parameter for valid application', function() {
				_reg.get('appmgr').getAll = function() {
					return ['pepe.id', 'otro.id'];
				};
				_reg.get('appmgr').get = function(id) {
					return {
						id: id,
						name: 'pepe'
					};
				};
				_reg.get('appmgr').isRunning = function(appName) {
					return true;
				};

				//	Check handler added
				var handler = getHandler( 'GET', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 200 );
					assert( xml.length > 0 );
				};

				var req = {
					routeParams: ['pepe']
				};
				handler.cb( req, {} );
				assert.equal( sendResult, 1 );
			});

			it( 'should handle url with parameter for valid application stopped', function() {
				_reg.get('appmgr').getAll = function() {
					return ['pepe.id', 'otro.id'];
				};
				_reg.get('appmgr').get = function(id) {
					return {
						id: id,
						name: 'pepe'
					};
				};
				_reg.get('appmgr').isRunning = function(appName) {
					return false;
				};

				//	Check handler added
				var handler = getHandler( 'GET', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 200 );
					assert( xml.length > 0 );
				};

				var req = {
					routeParams: ['pepe']
				};
				handler.cb( req, {} );
				assert.equal( sendResult, 1 );
			});

			it( 'should handle url with parameter for invalid application', function() {
				//	Check handler added
				var handler = getHandler( 'GET', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 404 );
				};

				var req = {
					routeParams: ['pepe']
				};
				handler.cb( req, {} );
				assert.equal( sendResult, 1 );
			});
		});

		describe('handle POST /apps/{appname}', function() {
			it( 'should handle url', function() {
				_reg.get('appmgr').getAll = function() {
					return ['pepe.id', 'otro.id'];
				};
				_reg.get('appmgr').get = function(id) {
					return {
						id: id,
						name: 'pepe'
					};
				};
				_reg.get('appmgr').runAsync = function(opts,cb) {
					assert.equal(opts.id,'pepe.id');
					cb( undefined );
				};

				//	Check handler added
				var handler = getHandler( 'POST', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				var setHeader = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 201 );
				};

				var req = {
					routeParams: ['pepe']
				};
				var res = {
					setHeader: function() {
						setHeader++;
					}
				}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
				assert.equal( setHeader, 1 );
			});

			it( 'should handle url and app additional data', function() {
				_reg.get('appmgr').getAll = function() {
					return ['pepe.id', 'otro.id'];
				};
				_reg.get('appmgr').get = function(id) {
					return {
						id: id,
						name: 'pepe'
					};
				};
				var params = 'pairingCode=b4507f18-ab47-4230-9471-2ba48db7e86c&theme=cl&v=gS7ZCXQdsIw&t=11';
				_reg.get('appmgr').runAsync = function(opts,cb) {
					assert.equal(opts.id,'pepe.id');
					assert.equal(opts.params,JSON.stringify(params));
					cb( undefined );
				};

				//	Check handler added
				var handler = getHandler( 'POST', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				var setHeader = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 201 );
				};

				var req = {
					routeParams: ['pepe'],
					data: JSON.stringify(params)
				};
				var res = {
					setHeader: function() {
						setHeader++;
					}
				}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
				assert.equal( setHeader, 1 );
			});

			it( 'should runApp without params and fail', function() {
				//	Check handler added
				var handler = getHandler( 'POST', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 404 );
				};

				var req = {
					routeParams: []
				};
				var res = {
				}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
			});

			it( 'should runApp with invalid appname', function() {
				//	Check handler added
				var handler = getHandler( 'POST', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 404 );
				};

				var req = {
					routeParams: ['pepe']
				};
				var res = {}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
			});

			it( 'should runApp and check run errors', function() {
				_reg.get('appmgr').getAll = function() {
					return ['pepe.id', 'otro.id'];
				};
				_reg.get('appmgr').get = function(id) {
					return {
						id: id,
						name: 'pepe'
					};
				};
				_reg.get('appmgr').runAsync = function(opts,cb) {
					cb( new Error('pepe') );
				};

				//	Check handler added
				var handler = getHandler( 'POST', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 503 );
				};

				var req = {
					routeParams: ['pepe'],
				};
				var res = {
				}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
			});

			it( 'should runApp and check if already running', function() {
				_reg.get('appmgr').getAll = function() {
					return ['pepe.id', 'otro.id'];
				};
				_reg.get('appmgr').get = function(id) {
					return {
						id: id,
						name: 'pepe'
					};
				};
				_reg.get('appmgr').runAsync = function(opts,cb) {
					cb();
				};
				_reg.get('appmgr').isRunning = function() {
					return true;
				};

				//	Check handler added
				var handler = getHandler( 'POST', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 200 );
				};

				var req = {
					routeParams: ['pepe'],
				};
				var res = {}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
			});
		});

		describe('handle DELETE /apps/{appname}', function() {
			it( 'should handle url', function() {
				_reg.get('appmgr').getAll = function() {
					return ['pepe.id', 'otro.id'];
				};
				_reg.get('appmgr').get = function(id) {
					return {
						id: id,
						name: 'pepe'
					};
				};
				_reg.get('appmgr').isRunning = function(appName) {
					return true;
				};
				_reg.get('appmgr').stop = function(appName,cb) {
					cb( undefined );
				};
				//	Check handler added
				var handler = getHandler( 'DELETE', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 200 );
				};

				var req = {
					routeParams: ['pepe', '1'],
				};
				var res = {}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
			});

			it( 'should fail if no app', function() {
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/dial/apps',
					"data": {},
					"method": "DELETE"
				};

				//	Check handler added
				var handler = getHandler( 'DELETE', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 404 );
				};

				var req = {
					routeParams: [],
				};
				var res = {}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
			});

			it( 'should fail if app not found', function() {
				_reg.get('appmgr').getAll = function() {
					return ['pepe.id', 'otro.id'];
				};
				_reg.get('appmgr').get = function(id) {
					return {
						id: id,
						name: 'otro'
					};
				};
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/dial/apps/pepe',
					"data": {},
					"method": "DELETE"
				};
				//	Check handler added
				var handler = getHandler( 'DELETE', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 404 );
				};

				var req = {
					routeParams: ['pepe'],
				};
				var res = {}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
			});

			it( 'should fail if app is not running', function() {
				_reg.get('appmgr').getAll = function() {
					return ['pepe.id', 'otro.id'];
				};
				_reg.get('appmgr').get = function(id) {
					return {
						id: id,
						name: 'pepe'
					};
				};
				_reg.get('appmgr').isRunning = function(id) {
					return false;
				};
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/dial/apps/pepe',
					"data": {},
					"method": "DELETE"
				};

				//	Check handler added
				var handler = getHandler( 'DELETE', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 400 );
				};

				var req = {
					routeParams: ['pepe'],
				};
				var res = {}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
			});

			it( 'should fail if app cant stop', function() {
				_reg.get('appmgr').getAll = function() {
					return ['pepe.id', 'otro.id'];
				};
				_reg.get('appmgr').get = function(id) {
					return {
						id: id,
						name: 'pepe'
					};
				};
				_reg.get('appmgr').isRunning = function(appName) {
					return true;
				};
				_reg.get('appmgr').stop = function(appName,cb) {
					cb( new Error('pepe') );
				};

				var handler = getHandler( 'DELETE', '/dial/apps' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 400 );
				};

				var req = {
					routeParams: ['pepe'],
				};
				var res = {}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
			});
		});

		describe('handle GET /ssdp/device-desc.xml', function() {
			it( 'should handle url', function() {
				var handler = getHandler( 'GET', '/dial/ssdp/device-desc.xml' );
				assert(handler);
				assert(handler.cb);

				var setHeader = 0;
				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 200 );
				};

				var req = {};
				var res = {
					setHeader: function() {
				 		//	TODO: Check headers!
						setHeader++;
					}
				};
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
				assert.equal( setHeader, 3 );
			});
		});

		describe('handle GET /ssdp/notfound', function() {
			it( 'should handle url', function() {
				var handler = getHandler( 'GET', '/ssdp/notfound' );
				assert(handler);
				assert(handler.cb);

				var sendResult = 0;
				_dial.sendResult = function( res, st, mime, xml ) {
					sendResult++;
					assert.equal( st, 404 );
				};

				var req = {
					routeParams: ['pepe'],
				};
				var res = {}
				handler.cb( req, res );
				assert.equal( sendResult, 1 );
			});
		});
	});
})
