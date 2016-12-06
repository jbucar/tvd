'use strict';

var http = require('http');
var dgram = require('dgram');
var tvdutil = require('tvdutil');
var Mocks = require('mocks');
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;
var EventEmitter = require('events').EventEmitter;
var bPromise = require('bluebird');

var RemoteApi = require('../src/main');

describe('remoteapi', function() {
	var _reg = null;
	var _module = null;
	var _adapter = null;
	var _configLogCB = null;
	var defaultPort = 1080;

	function CreateValidModule() {
		var self = {};
		self.start = function() {};
		self.name = 'test';
		return self;
	};

	function defaultConfig() {
		return {
			enabled: false,
			maxHistorySize: 1000,
			maxWidth: null, // Disable maxWidth
			level: 'info'
		};
	}

	beforeEach( function() {
		_reg = Mocks.init('silly');
		var sys = new Mocks.System();
		sys.on = function( sig, cb ) {
			if (sig === 'LogConfigChanged') {
				_configLogCB = cb;
			}
		};
		sys.on.isSignal = true;
		sys.removeListener = function( sig, cb ) {
			if (sig === 'LogConfigChanged') {
				_configLogCB = null;
			}
		}
		_reg.put( 'system', sys );
		_reg.put( 'network', new Mocks.NetworkManager() );
		_adapter = new Mocks.ServiceAdapter(_reg);
		_module = new RemoteApi(_adapter);
		assert( _module );
	});

	afterEach(function() {
		Mocks.fin();
		_module = null;
		_adapter = null;
		_reg = null;
	});

	function startMod(cb) {
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert( api );
			assert(_configLogCB);
			cb(api);
		});
	}

	function stopMod(cb) {
		_module.onStop(function() {
			assert(!_configLogCB);
			cb();
		});
	}

	describe( 'start/stop', function() {
		it('basic start/stop', function(done) {
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
				_module.onStop(function() {
					_module.onStart(function(err1,api1) {
						assert.equal(err1);
						assert( api1 );
						assert( api !== api1 );
						_module.onStop(done);
					});
				});
			});
		});

		it('should handle error on connection', function(done) {

			var oldCreate = http.createServer;
			var cbConnection = null;
			var cbError = null;

			function CreateServer() {
				var self = {};
				self.listen = function() {};
				self.close = function(cb) { cb(); };
				self.on = function() {};
				return self;
			}

			function CreateConnection() {
				var self = {};
				self.url = 'http://localhost:' + defaultPort + '/api';
				self.method = 'POST';
				self.on = function(signal,cb) {
					if (signal === 'error') {
						cbError = cb;
					}
				};
				return self;
			};

			function CreateResult() {
				var self = {};
				self.writeHead = function( st, content ) {}
				self.end = function() {}
				return self;
			};

			http.createServer = function(cb) {
				cbConnection = cb;
				return new CreateServer();
			};

			_module.onStart(function(err,api) {
				assert.equal(err);
				assert( api );
				assert( cbConnection );

				cbConnection( new CreateConnection(), new CreateResult() );
				assert( cbError );
				cbError( new Error('algo') );

				_module.onStop(done);
				http.createServer = oldCreate;
			});
		});

		it('should handle error on server', function(done) {

			var oldCreate = http.createServer;
			var cbConnection = null;
			var cbError = null;
			var cbServerError = null;

			function CreateServer() {
				var self = {};
				self.listen = function() {};
				self.close = function(cb) { cb(); };
				self.on = function(signal,cb) {
					if (signal === 'error') {
						cbServerError = cb;
					}
				};
				return self;
			}

			function CreateConnection() {
				var self = {};
				self.url = 'http://localhost:' + defaultPort + '/api';
				self.method = 'POST';
				self.on = function(signal,cb) {
					if (signal === 'error') {
						cbError = cb;
					}
				};
				return self;
			};

			function CreateResult() {
				var self = {};
				self.writeHead = function( st, content ) {}
				self.end = function() {}
				return self;
			};

			http.createServer = function(cb) {
				cbConnection = cb;
				return new CreateServer();
			};

			_module.onStart(function(err,api) {
				assert.equal(err);
				assert( api );
				assert( cbConnection );

				cbConnection( new CreateConnection(), new CreateResult() );
				assert( cbServerError );
				cbServerError( new Error('algo') );

				_module.onStop(done);
				http.createServer = oldCreate;
			});
		});

		it('should call start and stop async if present', function(done) {
			var startCalled=0;
			var stopCalled=0;
			startMod(function(api) {
				var mod = {};
				mod.name = function() { return 'pepe'; };
				mod.startAsync = function() {
					startCalled++;
					return bPromise.resolve();
				};
				mod.stopAsync = function() {
					stopCalled++;
					return bPromise.resolve();
				};

				api.addModule( mod );

				stopMod(function() {
					assert.equal( startCalled, 1 );
					assert.equal( stopCalled, 1 );
					done();
				});
			});
		});
	});

	describe('discover', function(done) {
		it('should handle invalid connection in activeConnection', function(done) {
			var net = _reg.get('network');
			net.activeConnection = function(cb) {
				cb();
			};
			startMod(function(api) {
				stopMod(done);
			});
		});

		it('should handle invalid ip in activeConnection', function(done) {
			var net = _reg.get('network');
			net.activeConnection = function(cb) {
				cb( undefined, { ip: '127.0.0.1' } );
			};
			startMod(function() {
				stopMod(done);
			});
		});

		it('should handle error in activeConnection', function(done) {
			var net = _reg.get('network');
			net.activeConnection = function(cb) {
				cb( new Error('algo') );
			};
			startMod(function() {
				stopMod(done);
			});
		});

		it('should handle active connection changed', function(done) {
			var net = _reg.get('network');
			net.activeConnection = function(cb) {
				cb();
			};
			startMod(function() {
				net.notify();
				stopMod(done);
			});
		});

		function MockSocket() {
			var self = {};
			var events = new EventEmitter();
			tvdutil.forwardEventEmitter(self, events);
			self.bind = function() {};
			self.close = function() {};
			self.sendMsg = function(msg) {
				events.emit( 'message', msg, { address: 'alguna', port: 10000 } );
			}
			self.send = function( msg, start, len, port, addr, cb) {
				cb();
			};
			self.events = function() {
				return events;
			};
			return self;
		};

		it('should handle client on discover socket', function(done) {
			var oldMethod = dgram.createSocket;
			var sock = null;
			dgram.createSocket = function(t) {
				if (t === 'udp4') {
					if (!sock) {
						sock = new MockSocket();
						sock.send = function( msg, start, len, port, addr, cb) {
							assert.deepEqual( msg, JSON.stringify({ port: defaultPort, host: '127.0.0.1' }) );
							assert.equal( start,  0 );
							assert.equal( len,  32 );
							assert.equal( port, 10000  );
							assert.equal( addr, 'alguna' );
							cb();
						};
					}
					return sock;
				}
				else {
					return oldMethod(t);
				}
			};
			startMod(function() {
				assert(sock);
				sock.sendMsg( '{}' );
				stopMod(function() {
					dgram.createSocket = oldMethod;
					done();
				});
			});
		});

		it('should handle client socket error', function(done) {
			var oldMethod = dgram.createSocket;
			var sock = null;
			dgram.createSocket = function(t) {
				if (t === 'udp4') {
					if (!sock) {
						sock = new MockSocket();
						sock.send = function( msg, start, len, port, addr, cb) {
							assert.deepEqual( msg, JSON.stringify({ port: defaultPort, host: '127.0.0.1' }) );
							assert.equal( start,  0 );
							assert.equal( len,  32 );
							assert.equal( port, 10000  );
							assert.equal( addr, 'alguna' );
							cb( new Error('alguno'));
						};
					}
					return sock;
				}
				else {
					return oldMethod(t);
				}
			};
			startMod(function() {
				assert(sock);
				sock.sendMsg( '{}' );
				stopMod(function() {
					dgram.createSocket = oldMethod;
					done();
				});
			});
		});

		it('should not handle client if no network active', function(done) {
			var oldMethod = dgram.createSocket;
			var sock = null;
			dgram.createSocket = function(t) {
				if (t === 'udp4') {
					if (!sock) {
						sock = new MockSocket();
					}
					return sock;
				}
				else {
					return oldMethod(t);
				}
			};
			var net = _reg.get('network');
			net.activeConnection = function(cb) {
				cb();
			};
			startMod(function() {
				assert(sock);
				sock.sendMsg( '{}' );
				stopMod(function() {
					dgram.createSocket = oldMethod;
					done();
				});
			});
		});

		it('should handle error on server socket', function(done) {
			var oldMethod = dgram.createSocket;
			var sock = null;
			dgram.createSocket = function(t) {
				if (t === 'udp4') {
					if (!sock) {
						sock = new MockSocket();
					}
					return sock;
				}
				else {
					return oldMethod(t);
				}
			};
			startMod(function() {
				assert(sock);
				sock.events().emit( 'error', new Error('alguno') );
				stopMod(function() {
					dgram.createSocket = oldMethod;
					done();
				});
			});
		});
	});

	describe('methods', function() {
		var api = null;

		beforeEach( function(done) {
			startMod(function(impl) {
				api = impl;
				done();
			});
		});

		afterEach( function (done) {
			stopMod(function() {
				api = null;
				done();
			});
		});

		it('should handle favicon request', function(done) {
			var opts = {
				"url": 'http://localhost:' + defaultPort + '/favicon.ico',
				"data": {},
				"method": "GET"
			};
			tvdutil.doRequest( opts, function(err,res) {
				assert.isUndefined(err);
				assert(res);
				assert.isTrue( res.statusCode === 200 );
				done();
			});
		});

		describe('add/rm Module', function() {
			it('should throw if module invalid', function() {
				function CreateModule() {
					var self = {};
					return self;
				};
				function fnc() {
					api.addModule( new CreateModule() );
				}
				assert.throws( fnc );
			});

			it('should add valid module', function() {
				api.addModule( new CreateValidModule() );
			});

			it('should not rm a invalid module', function(done) {
				api.rmModule( 'pepe' ).catch( () => done() );
			});

			it('should add/rm valid module', function(done) {
				api.addModule( new CreateValidModule() ).then(function() {
					api.rmModule( 'test' ).then(done);
				});
			});
		});

		describe('index_module', function() {
			it('should handle default root', function(done) {
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/',
					"data": {},
					"method": "GET"
				};
				tvdutil.doRequest( opts, function(err,res) {
					assert.isUndefined(err);
					assert(res);
					assert.isTrue( res.statusCode === 200 );
					assert.equal( res.data, '<!DOCTYPE html><html><body><a target="_self" href="/log"></br>log</a></body></html>' );
					done();
				});
			});
		});

		describe('api_module', function() {
			it('should handle non api call', function(done) {
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/where',
					"data": {},
					"method": "POST"
				};
				tvdutil.doRequest( opts, function(err,res) {
					assert.isUndefined(err);
					assert(res);
					assert.isTrue( res.statusCode === 404 );
					done();
				});
			});

			it('should handle extra module call', function(done) {
				var mod = new CreateValidModule();
				mod.start = function() {
					mod.addHandler( 'POST', '/extra', function(req, res) {
						assert.equal( mod.getBaseUrl(req).indexOf( 'http://' ), 0 );

						res.writeHead( 202, { 'Content-type': 'text/html' } );
						res.end( req.data );
					})
				};

				api.addModule( mod );
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/extra',
					"data": 'algo',
					"method": "POST"
				};
				tvdutil.doRequest( opts, function(err,res) {
					assert.isUndefined(err);
					assert(res);
					assert.equal( res.statusCode, 202 );
					assert.equal( res.data, '\"algo\"' );
					done();
				});
			});

			it('should check for extra path at begin', function(done) {
				var mod = new CreateValidModule();
				mod.start = function() {
					mod.addHandler( 'POST', '/extra', function(req, res) {
						res.writeHead( 202, { 'Content-type': 'text/html' } );
						res.end( req.data );
					})
				};

				api.addModule( mod );
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/pepe/extra',
					"data": 'algo',
					"method": "POST"
				};
				tvdutil.doRequest( opts, function(err,res) {
					assert.isUndefined(err);
					assert(res);
					assert.equal( res.statusCode, 404 );
					done();
				});
			});

			it('should process UTF8 for service', function(done) {
				var mod = new CreateValidModule();
				mod.useUTF8 = true;
				mod.start = function() {
					mod.addHandler( 'POST', '/extra', function(req, res) {
						res.writeHead( 200, { 'Content-type': 'text/html' } );
						res.end( req.data );
					});
				};

				api.addModule( mod );
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/extra',
					"data": 'algo',
					"method": "POST"
				};
				tvdutil.doRequest( opts, function(err,res) {
					assert.isUndefined(err);
					assert(res);
					assert.equal( res.statusCode, 200 );
					done();
				});
			});

			it('should process request', function(done) {
				var mod = new CreateValidModule();
				mod.start = function() {
					let handler = mod.addHandler( 'POST', '/extra', function(req, res) {
						res.writeHead( 200, { 'Content-type': 'text/html' } );
						res.end( req.data );
					});
					handler.handleData = true;
				};

				api.addModule( mod );
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/extra',
					"data": 'algo',
					"method": "POST"
				};
				tvdutil.doRequest( opts, function(err,res) {
					assert.isUndefined(err);
					assert(res);
					assert.equal( res.statusCode, 200 );
					done();
				});
			});

			it('should handle REST api call with params', function(done) {
				var mod = new CreateValidModule();
				mod.useREST = true;
				mod.start = function() {
					mod.addHandler( 'POST', '/extra', function(req, res) {
						assert.equal( req.routeParams.length, 2 );
						assert.equal( req.routeParams[0], 'param' );
						assert.equal( req.routeParams[1], '1' );
						res.writeHead( 200, { 'Content-type': 'text/html' } );
						res.end( req.data );
					});
				};

				api.addModule( mod );
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/extra/param/1',
					"data": 'algo',
					"method": "POST"
				};
				tvdutil.doRequest( opts, function(err,res) {
					assert.isUndefined(err);
					assert(res);
					assert.equal( res.statusCode, 200 );
					done();
				});
			});

			it('should handle REST api call without params', function(done) {
				var mod = new CreateValidModule();
				mod.useREST = true;
				mod.start = function() {
					mod.addHandler( 'POST', '/extra', function(req, res) {
						assert.equal( req.routeParams.length, 0 );
						res.writeHead( 200, { 'Content-type': 'text/html' } );
						res.end( req.data );
					});
				};

				api.addModule( mod );
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/extra',
					"data": 'algo',
					"method": "POST"
				};
				tvdutil.doRequest( opts, function(err,res) {
					assert.isUndefined(err);
					assert(res);
					assert.equal( res.statusCode, 200 );
					done();
				});
			});

			it('should handle invalid api call', function(done) {
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/api',
					"data": {
						"serviceID": "ar.edu.unlp.info.lifia.tvd.system",
					},
					"method": "POST"
				};
				tvdutil.doRequest( opts, function(err,res) {
					assert.isUndefined(err);
					assert(res);
					assert.isTrue( res.statusCode === 404 );
					done();
				});
			});

			it('should handle invalid data on api call', function(done) {
				var dataString = '[1, 2, 3, 4, ]';
				var opts = {
					hostname: 'localhost',
					port: defaultPort,
					path:  '/api',
					method: 'POST',
					headers: {
						'Content-Type': 'application/json',
						'Content-Length': Buffer.byteLength(dataString)
					}
				};
				var req=http.request(opts, function(res) {
					assert(res);
					assert( res.statusCode === 404 );
					done();
				});

				req.on('error', function(err) {
					assert(false);
				});
				req.write(dataString);
				req.end();
			});

			it('should exec system.info api call', function(done) {
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/api',
					"data": {
						"serviceID": "ar.edu.unlp.info.lifia.tvd.system",
						"method": "getInfo"
					},
					"method": "POST"
				};

				api.addModule( new CreateValidModule() );

				tvdutil.doRequest( opts, function(err,res) {
					assert(res);
					assert.isUndefined(err);
					assert.isTrue( res.statusCode === 200 );
					assert(res.data.system);
					assert(res.data.platform);
					assert.deepEqual( res.data, _reg.get('system').getInfo() );
					done();
				});
			});

			it('should exec system.info async api call', function(done) {
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/api',
					"data": {
						"serviceID": "ar.edu.unlp.info.lifia.tvd.system",
						"method": "getInfoAsync"
					},
					"method": "POST"
				};

				_reg.get('system').getInfoAsync = function(cb) {
					cb( undefined, _reg.get('system').getInfo() );
				};
				_reg.get('system').getInfoAsync.isAsync = true;
				_reg.get('system').getWebAPI = function() {
					return {
						'name': 'system',
						'public': ['getInfo', 'on', 'getInfoAsync']
					}
				};

				tvdutil.doRequest( opts, function(err,res) {
					assert(res);
					assert.isUndefined(err);
					assert.isTrue( res.statusCode === 200 );
					assert(res.data.system);
					assert(res.data.platform);
					assert.deepEqual( res.data, _reg.get('system').getInfo() );
					done();
				});
			});

			it('should exec system.empty api call', function(done) {
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/api',
					"data": {
						"serviceID": "ar.edu.unlp.info.lifia.tvd.system",
						"method": "empty"
					},
					"method": "POST"
				};

				_reg.get('system').empty = function() {};
				_reg.get('system').getWebAPI = function() {
					return {
						'name': 'system',
						'public': ['getInfo', 'on', 'empty']
					}
				};

				tvdutil.doRequest( opts, function(err,res) {
					assert(res);
					assert.isUndefined(err);
					assert.isTrue( res.statusCode === 200 );
					assert.isObject( res.data );
					assert.deepEqual(res.data, {});
					done();
				});
			});

			it('should exec API with parameters', function(done) {
				_reg.get('system').oneParam = function(p1) {
					assert(p1);
					return p1;
				};
				_reg.get('system').twoParam = function(p1,p2) {
					assert(p1);
					assert(p2);
					return { p1: p1, p2: p2 };
				};
				_reg.get('system').getWebAPI = function() {
					return {
						'name': 'system',
						'public': ['getInfo', 'on', 'oneParam', 'twoParam']
					}
				};

				var opts = {
					"url": 'http://localhost:' + defaultPort + '/api',
					"data": {
						"serviceID": "ar.edu.unlp.info.lifia.tvd.system",
						"method": "oneParam",
						"params": [ "test" ]
					},
					"method": "POST"
				};

				tvdutil.doRequest( opts, function(err,res) {
					assert(res);
					assert.isUndefined(err);
					assert.isTrue( res.statusCode === 200 );
					assert.equal(res.data, 'test');

					opts.data.method = 'twoParam';
					opts.data.params = [ 'test1', 'test2' ];
					tvdutil.doRequest( opts, function(err,res) {
						assert(res);
						assert.isUndefined(err);
						assert.isTrue( res.statusCode === 200 );
						assert.deepEqual(res.data, {p1: 'test1', p2: 'test2'});
						done();
					});
				});
			});

			it('should no register to a signal', function(done) {
				var opts = {
					"url": 'http://localhost:' + defaultPort + '/api',
					"data": {
						"serviceID": "ar.edu.unlp.info.lifia.tvd.system",
						"method": "on"
					},
					"method": "POST"
				};

				tvdutil.doRequest( opts, function(err,res) {
					assert(res);
					assert.isUndefined(err);
					assert.isTrue( res.statusCode === 404 );
					done();
				});
			});
		});

		describe('log_module', function() {

			function getUrlLog() {
				return 'http://localhost:' + defaultPort + '/log';
			}

			beforeEach( function() {
				var cfg = {};
				cfg.log = defaultConfig();
				cfg.log.enabled = true;
				_configLogCB( cfg.log );
			});

			it('should get log', function(done) {
				log.info( 'pepe', 'pepe message1' );
				log.info( 'pepe', 'pepe message2' );
				log.info( 'pepe', 'pepe message3' );

				http.get( getUrlLog(), function (res) {
					assert(res);
					assert.isTrue( res.statusCode === 200 );
					done();
				});
			});

			it('should ignore invalid log configuration', function() {
				var cfg = {};
				cfg.log = defaultConfig();
				cfg.log.enabled = true;
				cfg.log.maxWidth = '100';
				_configLogCB( cfg.log );
			});

			it('should use maxWidth configuration', function(done) {
				var cfg = {};
				cfg.log = defaultConfig();
				cfg.log.enabled = true;
				cfg.log.maxWidth = 100;
				_configLogCB( cfg.log );

				http.get( getUrlLog(), function (res) {
					assert(res);
					assert.isTrue( res.statusCode === 200 );
					done();
				});
			});

			it('should ignore request if not enabled', function(done) {
				var cfg = {};
				cfg.log = defaultConfig();
				cfg.log.enabled = false;
				_configLogCB( cfg.log );

				http.get( getUrlLog(), function (res) {
					assert(res);
					assert.isTrue( res.statusCode === 404 );
					done();
				});
			});

			it('should filter level and prefix', function(done) {
				http.get( getUrlLog() + '?level=silly&prefix=System', function (res) {
					assert(res);
					assert.isTrue( res.statusCode === 200 );
					done();
				});
			});
		});
	});
});
