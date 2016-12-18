'use strict';

var assert = require('assert');
var http = require('http');
var Url = require ('url');
var bPromise = require('bluebird');
var tvdutil = require('tvdutil');

var ApiModule = require('./api');
var LogModule = require('./log');
var IndexModule = require('./index');
var Discover = require('./discover');

function ServiceHandler( srv, method, reqPath, cb ) {
	var self = {};

	self.module = function() {
		return srv;
	};

	self.method = function() {
		return method;
	};

	self.path = function() {
		return reqPath;
	};

	self.canHandle = function( req ) {
		if (req.method === method) {
			let pathName = req.urlParsed.pathname;
			if (pathName.indexOf(reqPath) === 0) {
				if (pathName === reqPath) {
					return Number.MAX_SAFE_INTEGER;
				}
				return pathName.length - pathName.substring(reqPath.length+1).length;
			}
		}
		return -1;
	};

	self.handleRequest = function( req, res ) {
		if (srv.useREST) {
			if (reqPath.length !== req.urlParsed.pathname.length) {
				var allParams = req.urlParsed.pathname.substring(reqPath.length+1);
				req.routeParams = allParams.split('/');
			}
			else {
				req.routeParams = [];
			}
		}

		cb( req, res );
	};

	return self;
}

function ServerProxy(adapter, server, port) {
	var self = {};

	//	Implementation
	var _handlers = [];

	assert(adapter,'Invalid adapter');
	assert(server, 'Invalid server');
	assert(port, 'Invalid port');

	function handleBody(srv,req,res) {
		assert(srv, 'Invalid service');
		var body = '';

		if (srv.module().useUTF8) {
			req.setEncoding('utf8');
		}

		if (srv.handleData) {
			srv.handleRequest( req, res );
		}
		else {
			req.on('data', function (data) {
				body += data;
			});

			req.on('end', function () {
				var data;
				if (srv.useJSON) {
					try {
						data = JSON.parse( body );
					}
					catch (err) {
						log.warn( 'remoteapi', 'Cannot parse request: url=%j, body=%s, err=%s', req.url, body, err.message );
						self.sendResult( res, 404 );
						return;
					}
				}
				else {
					data = body;
				}

				req.data = data;
				srv.handleRequest( req, res );
			});
		}
	}

	self.reg = function() {
		return adapter.registry();
	};

	self.getBaseUrl = function(req) {
		return 'http://' + req.connection.address().address + ':' + port;
	};

	self.sendResult = function( res, st, ct, body ) {
		if (ct === undefined) {
			ct = 'text/html';
		}
		if (body === undefined) {
			body = '';
		}
		var headers = {
			'Content-Type': ct,
			'Content-Length': Buffer.byteLength(body)
		};
		res.writeHead( st, headers );
		res.end( body );
	};

	self.addHandler = function( mod, method, path, cb ) {
		var srvHandler = new ServiceHandler(mod,method,path,cb);
		_handlers.push( srvHandler );
		return srvHandler;
	};

	self.currentHandlers = function() {
		return _handlers;
	};

	self.processRequest = function(req,res) {
		if (req.url === '/favicon.ico') {
			self.sendResult( res, 200, 'image/x-icon' );
			return;
		}
		req.urlParsed = Url.parse(req.url,true);

		//	Check if can handle request
		var srv;
		let maxPriority=-1;
		for (var h=0; h<_handlers.length; h++) {
			let prio = _handlers[h].canHandle( req );
			if (prio >= 0 && prio > maxPriority) {
				srv = _handlers[h];
				maxPriority = prio;
			}
		}

		if (srv) {
			handleBody( srv, req, res );
		}
		else {
			log.warn( 'remoteapi', 'Request not handled: url=%j', req.urlParsed );
			self.sendResult( res, 404 );
		}

		req.on('error', function(err) {
			log.warn( 'remoteapi', 'Pipe error: err=%s', err.message );
			self.sendResult( res, 404 );
		});
	};

	return self;
}

function RemoteApiImpl(adapter) {
	var self = {};
	var _modules = [];
	var _discover = null;
	var _server = null;
	var _proxy = null;
	var _started = false;

	function createServer( port ) {
		var app = http.createServer(function(req, res) {
			_proxy.processRequest( req, res );
		});
		app.listen( port, '0.0.0.0' );
		app.on('error', function(err) {
			log.warn( 'remoteapi', 'Error on server socket: err=%s', err.message );
		});

		return app;
	}

	self.addModule = function( mod ) {
		assert(mod,'Invalid module');
		assert(mod.name,'Invalid module name');
		assert(mod.start || mod.startAsync,'Invalid methods');
		assert(_proxy, 'Invalid proxy');

		log.info( 'remoteapi', 'Add module: %s', mod.name );
		_modules.push( mod );

		//	Inject variables and methods
		mod.server = _proxy;
		mod.sendResult = _proxy.sendResult.bind(_proxy);
		mod.addHandler = _proxy.addHandler.bind(_proxy,mod);
		mod.getBaseUrl = _proxy.getBaseUrl.bind(_proxy);
		mod.reg = _proxy.reg.bind(_proxy);
		mod.port = self.port.bind(self);

		if (_started) {
			return self.startSrv(mod);
		}
		return bPromise.resolve();
	};

	self.rmModule = function(name) {
		assert(name, 'Invalid module name');

		var index = _modules.findIndex(function(mod) {
			return (mod.name === name);
		});
		if (index < 0) {
			return bPromise.reject( tvdutil.warnErr( 'remoteapi', 'Invalid module: name=%s', name ) );
		}

		let mod = _modules[index];
		_modules.splice( index, 1 );

		return self.stopSrv( mod );
	};

	self.removeAll = function() {
		return self.stopAll()
			.then(function() {
				_modules = [];
			});
	};

	self.stopSrv = function( srv ) {
		if (srv.stopAsync) {
			return srv.stopAsync();
		}

		if (srv.stop) {
			srv.stop();
		}

		return bPromise.resolve();
	};

	self.stopAll = function() {
		return bPromise.all(_modules.map( (srv) => self.stopSrv(srv) ))
			.then(function() {
				_started = false;
			});
	};

	self.startSrv = function(srv) {
		log.verbose( 'remoteapi', 'Start module: srv=%s', srv.name );
		if (srv.startAsync) {
			return srv.startAsync();
		}

		srv.start();
		return bPromise.resolve();
	};

	self.startAll = function() {
		return bPromise.all(_modules.map(function(srv) {
			return self.startSrv(srv);
		})).then(function() {
			_started = true;
		});
	};

	self.port = function() {
		return adapter.registry().get('system').getRemotePort();
	};

	self.start = function(cb) {
		_modules = [];

		//	Get server port
		var port = self.port();
		assert( port >= 1024, 'Invalid port' );

		//	Start discover
		_discover = new Discover( adapter.registry().get('network') );
		_discover.start( port );

		//	Create server
		_server = createServer( port );
		tvdutil.addServerDestroy(_server);

		//	Add system modules
		_proxy = new ServerProxy(adapter,_server,port);
		self.addModule( new IndexModule(_proxy) );
		self.addModule( new ApiModule() );
		self.addModule( new LogModule() );

		//	Start all
		self.startAll()
			.then(function() {
				cb( undefined, new RemoteApi(self) );
			}, cb );
	};

	self.stop = function(cb) {
		self.removeAll().then(function() {
			_discover.stop(function() {
				_server.destroy(function() {
					_proxy = null;
					cb();
				});
			});
		});
	};

	return self;
}

function RemoteApi(impl) {
	var self = {};

	self.addModule = impl.addModule.bind(impl);
	self.rmModule = impl.rmModule.bind(impl);
	self.port = impl.port.bind(impl);

	return self;
}

//	Service entry point
function RemoteApiService(adapter) {
	var self = {};
	var _impl = null;

	self.onStart = function(cb) {
		log.info( 'remoteapi', 'Start' );
		_impl = new RemoteApiImpl(adapter);
		_impl.start(cb);
	};

	self.onStop = function(cb) {
		log.verbose( 'remoteapi', 'Stop' );
		_impl.stop(cb);
	};

	return Object.freeze(self);
}

// Export module hooks
module.exports = RemoteApiService;
