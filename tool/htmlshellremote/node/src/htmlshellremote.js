'use strict';

var net = require('net');
var assert = require('assert');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var path = require('path');
var spawn = require('child_process').spawn;
var crypto = require('crypto');
var fs = require('fs');
var _ = require('lodash');
var ProtobufJS = require('protobufjs');
var tvdutil = require('tvdutil');

function makeSocketName(base) {
	var exists=true;
	var name;
	while (exists) {
		name = path.join( base, 'htmlshellremote_'+crypto.randomBytes(8).readUInt32LE(0)+'.socket' );
		exists = fs.existsSync( name );
	}
	return name;
}

function HtmlRemote() {
	var _private = {};
	_private.events = new EventEmitter();
	_private.proto = {
		'Request': null,
		'Response': null,
	};
	_private.server = null;
	_private.clientSocket = null;
	_private.cfg = null;
	_private.timers = {
		'StopShell': null,
		'StartShell': null,
		'KeepAlive': null
	};
	_private.shell = null;
	_private.error = null;
	_private.nextID = 0;
	_private.onEventCallback = null;
	_private.msgData = null;

	function killTimer(timer) {
		if (_private.timers[timer]) {
			clearTimeout( _private.timers[timer] );
			_private.timers[timer] = null;
		}
	}

	//	Protobuf handling
	function sendMsg( req ) {
		if (_private.clientSocket) {
			try {
				var sRequest = new _private.proto.Request(req);
				var proto = sRequest.toBuffer();

				var header = new Buffer(4);
				header.writeUInt32BE(proto.length+4,0);
				_private.clientSocket.write(header);
				_private.clientSocket.write(proto);
				if (req.ping === undefined) {
					log.silly( 'HtmlRemote', 'Send request: msg=%j: len=%d', req, proto.length );
				}
				return true;
			} catch(e) {
				log.warn( 'HtmlRemote', 'Cannot serialize msg: type=%d, error=%s', req.type, e.message );
			}
		}
		return false;
	}

	function onClientData( data ) {
		//	Copy data to msg data
		var msg = Buffer.concat([_private.msgData,data]);

		var parse = true;
		while (parse) {
			//	There is a header?
			if (msg.length > 4) {
				//	There is a complete msg?
		 		var msgSize = msg.readUInt32LE(0);
				if (msgSize <= msg.length) {
					//	Parse message
					parseResponse( msg.slice(4,msgSize) );

					//	Skip message
					msg = msg.slice(msgSize);
				}
				else {
					parse = false;
				}
			}
			else {
				parse = false;
			}
		}

		_private.msgData = msg;
	}

	function parseResponse( data ) {
		var parsed = _private.proto.Response.decode(data);
		if (!parsed) {
			log.warn( 'HtmlRemote', 'Cannot parse response: data=%s', data);
			exitShell();
			return;
		}

		if (parsed.data !== 'pingAck') {
			log.silly( 'HtmlRemote', 'Receive response: %s: %j', parsed.data, parsed[parsed.data] );
		}

		var evt = null;
		switch (parsed.data) {
			case 'pingAck': onPingAck(parsed); break;
			case 'version':
				log.info( 'HtmlRemote', 'Client version: major=%d, minor=%d', parsed.version.major, parsed.version.minor );
				if (parsed.version.major !== _private.cfg.version.major) {
					_private.error = util.format('Version mismatch: major=%d', parsed.version.major);
				}
				break;
			case 'bLaunched':
				assert(parsed.bLaunched.browserID >= 0);
				evt = {
					type: 'onLaunched',
					browserID: parsed.bLaunched.browserID
				};
				break;
			case 'bClosed':
				assert(parsed.bClosed.browserID >= 0);
				assert(parsed.bClosed.error !== undefined);
				evt = {
					type: 'onClosed',
					browserID: parsed.bClosed.browserID,
					error: parsed.bClosed.error
				};
				break;
			case 'bLoaded':
				assert(parsed.bLoaded.browserID >= 0);
				evt = {
					type: 'onLoaded',
					browserID: parsed.bLoaded.browserID,
				};
				break;
			case 'bKey':
				assert(parsed.bKey.browserID >= 0);
				assert(parsed.bKey.code !== undefined);
				assert(parsed.bKey.isUp !== undefined);
				evt = {
					type: 'onKey',
					browserID: parsed.bKey.browserID,
					keyCode: parsed.bKey.code,
					isUp: parsed.bKey.isUp
				};
				break;
			case 'bWebLog':
				assert(parsed.bWebLog.browserID >= 0);
				assert(parsed.bWebLog.logData.level !== undefined);
				assert(parsed.bWebLog.logData.line !== undefined);
				assert(parsed.bWebLog.logData.source !== undefined);
				assert(parsed.bWebLog.logData.message !== undefined);
				evt = {
					type: 'onWebLog',
					browserID: parsed.bWebLog.browserID,
					logData: {
						'level': parsed.bWebLog.logData.level,
						'line': parsed.bWebLog.logData.line,
						'source': parsed.bWebLog.logData.source,
						'message': parsed.bWebLog.logData.message
					}
				};
				break;
			case 'jsRequest':
				assert(parsed.jsRequest.browserID >= 0);
				assert(parsed.jsRequest.queryID >= 0);
				assert(parsed.jsRequest.params !== undefined);
				evt = {
					type: 'onAPI',
					browserID: parsed.jsRequest.browserID,
					queryID: parsed.jsRequest.queryID,
					params: parsed.jsRequest.params
				};
				break;
			default:
				log.warn( 'HtmlRemote', 'Invalid message: parsed: %j', parsed );
		}
		if (evt && _private.onEventCallback) {
			_private.onEventCallback( evt );
		}
	}

	//	Client
	function closeClientSocket() {
		if (_private.clientSocket) {
			log.verbose( 'HtmlRemote', 'Client disconnected' );
			_private.clientSocket.end();
			_private.clientSocket = null;
			_private.events.emit('disconnected');
		}
	}

	function disconnect(err) {
		log.verbose( 'HtmlRemote', 'Client socket disconnected: err=%s', err );
		closeClientSocket();
		exitShell();
	}

	function isConnected() {
		return _private.clientSocket ? true : false;
	}

	//	Keep alive
	function onPingAck( res ) {
		if (res.pingAck.id === 10) {
			killTimer( 'KeepAlive' );
			startKeepAlive();
		}
	}

	function onPingTimeout() {
		log.verbose( 'HtmlRemote', 'Ping timeout' );
		exitShell();
	}

	function sendKeepAlive() {
		sendMsg({ping: {id: 10}});
		_private.timers.KeepAlive = setTimeout( onPingTimeout, _private.cfg.htmlshell.keepAliveTimeout );
	}

	function startKeepAlive(ms) {
		if (!ms) {
			ms = _private.cfg.htmlshell.keepAliveTimeout;
		}
		_private.timers.KeepAlive = setTimeout( sendKeepAlive, ms );
	}

	//	Shell
	function startShell(url) {
		log.verbose( 'HtmlRemote', 'start shell' );

		assert(_private.shell === null);
		assert(_private.clientSocket === null);
		assert(_private.timers.StopShell === null);
		assert(_private.timers.StartShell === null);

		var cfg = _private.cfg.htmlshell;

		//	Setup spawn
		var pBinPath = path.join(cfg.installPath, 'bin');
		var pCmd = path.join(pBinPath, cfg.cmd);
		var pArgs = _.cloneDeep(cfg.args);
		pArgs.push('--manifest=' + path.resolve(cfg.installPath, 'resources', 'manifest.json'));
		pArgs.push('--sys-profile=' + path.join(cfg.workPath, 'htmlshell'));
		pArgs.push('--usr-profile=' + path.join(cfg.workPath, 'htmlshell'));
		pArgs.push('--tvd-remote-server-url=' + url);
		var pEnv = _.cloneDeep(process.env);
		pEnv.HOME = cfg.workPath;

		//	Spawn
		_private.shell = spawn( pCmd, pArgs, {cwd: pBinPath, env: pEnv, stdio:'inherit'});
		log.verbose( 'HtmlRemote', 'Launched %s, args=%j, pid=%d', pCmd, pArgs, _private.shell.pid);

		//	Setup process notifications
		_private.shell.once('error', function() {
			log.warn( 'HtmlRemote', 'Fail to spawn %s', pCmd);
			_private.error = 'Fail to spawn';
			onShellExit();
		});

		_private.shell.once('exit', function(code, signal) {
			log.verbose( 'HtmlRemote', 'HtmlShell exit with code=%d, signal=%s', code, signal);
			if (code > 0 && code <= 10) {	//	See $DEPOT/tool/htmlshell/htmlshell/src/errors.h
				_private.error = 'Initialization error';
			}
			onShellExit();
		});

		//	Start 'start timeout'
		_private.timers.StartShell = setTimeout(function() {
			log.verbose( 'HtmlRemote', 'start timeout!' );
			exitShell();
		}, _private.cfg.htmlshell.startTimeout );

		//	Emit started
		_private.events.emit('started');
	}

	function onShellExit() {
		log.verbose( 'HtmlRemote', 'On shell exit: error=%s', _private.error );

		//	Kill start timer
		killTimer('StartShell');
		//	Kill stop timer
		killTimer('StopShell');
		//	Kill keep alive timer
		killTimer('KeepAlive');

		//	Clear nextID
		_private.nextID = 0;

		//	Kill client socket
		closeClientSocket();

		//	Clear shell
		if (_private.shell) {
			_private.shell = null;
			_private.events.emit('stopped');
		}

		//	Report error
		if (_private.error) {
			_private.events.emit('error',new Error(_private.error));
		}
		else if (_private.cfg.restart) {
			//	Restart!
			_private.server.close(function() {
				initServer();
			});
			_private.server = null;
		}
	}

	function killShell() {
		if (_private.shell !== null) {
			log.verbose( 'HtmlRemote', 'kill shell' );
			_private.shell.kill('SIGKILL');
		}
	}

	function exitShell() {
		//	Check if exit/kill already sended
		if (_private.timers.StopShell === null) {
			if (isConnected()) {
				log.verbose( 'HtmlRemote', 'send exit shell' );
				_private.timers.StopShell = setTimeout( function() {
					log.verbose( 'HtmlRemote', 'stop timeout!' );
					killShell();
				}, _private.cfg.htmlshell.stopTimeout );
				sendMsg({exit: {id: 0}});
			}
			else {
				killShell();
			}
		}
	}

	function waitShellExit(fnc) {
		if (_private.shell !== null) {
			_private.events.once('stopped', fnc );
		}
		else {
			fnc();
		}
	}

	//	Server
	function initServer() {
		//	Create url on filesystem
		var url = makeSocketName( _private.cfg.tmpDirectory );
		log.verbose( 'HtmlRemote', 'Listener server on: url=%s', url );

		_private.server = net.createServer(function(c) { //'connection' listener
			log.verbose( 'HtmlRemote', 'client connected');

			//	Setup client socket
			_private.clientSocket = c;
			c.on('data', onClientData );
			c.once('end', disconnect );
			c.once('error', disconnect );

			//	Send version
			sendMsg({version: _private.cfg.version});

			//	Kill start timeout
			killTimer( 'StartShell' );

			//	Start keep alive
			startKeepAlive(0);

			//	Signal
			_private.events.emit('connected');
		});

		//	Accept clients
		_private.server.listen( url, function() {
			startShell(url);
		});
	}

	//	API
	var self = this || {};
	tvdutil.forwardEventEmitter(self,_private.events);

	//	Initialization
	self.defaultConfig = function() {
		return {
			'version': { major: 1, minor: 2 },
			'tmpDirectory': '/tmp',
			'htmlshell': {
				'workPath': '/tmp',
				'installPath': '/tmp',
				'cmd': 'run.sh',
				'args': [],
				'startTimeout': 30000,	//	Time between start && connect
				'stopTimeout': 2000,	//	Time to wait for stop
				'keepAliveTimeout': 1000
			},
			'restart': true,
			'protoFile': path.join(__dirname, 'htmlshell.proto'),
		};
	};

	self.start = function( cfg, evtCallback ) {
		log.info( 'HtmlRemote', 'start: version=(%d,%d)', cfg.version.major, cfg.version.minor );
		assert(_.isFunction(evtCallback));

		//	Setup variables
		_private.cfg = cfg;
		_private.server = null;
		_private.clientSocket = null;
		_private.shell = null;
		_private.error = null;
		_private.nextID = 0;
		_private.onEventCallback = evtCallback;
		_private.msgData = new Buffer(0);

		try {
			// Parse proto file
			var builder = ProtobufJS.loadProtoFile(cfg.protoFile);
			if (!builder) {
				log.error( 'HtmlRemote', 'Cannot open proto file: %s', cfg.protoFile );
				return false;
			}
			_private.proto.Request = builder.build('ar.edu.unlp.info.lifia.tvd.htmlshell.Request');
			_private.proto.Response = builder.build('ar.edu.unlp.info.lifia.tvd.htmlshell.Response');
		}
		catch (error) {
			log.error( 'HtmlRemote', 'Fail to read parse proto file %s, error: %s', cfg.protoFile, error.message );
			return false;
		}

		//	Start server
		initServer();

		return true;
	};

	self.stop = function(done) {
		if (_private.cfg) {
			log.info( 'HtmlRemote', 'Stop' );

			//	No restart on die
			_private.cfg.restart = false;

			//	Exit from shell
			exitShell();

			//	Wait shell exit
			waitShellExit(function() {
				//	Stop server
				if (_private.server) {
					_private.server.close(function() {
						_private.cfg = null;
						done();
					});
				} else {
					done();
				}
			});
		}
		else {
			done();
		}
	};

	//	Browser methods
	self.launchBrowser = function( url, opts, id ) {
		var browserID = (id < 0) ? _private.nextID++ : id;
		sendMsg({
			launchBrowser: {
				browserID: browserID,
				url: url,
				options: opts
			}
		});
		return browserID;
	};

	self.closeBrowser = function( browserID ) {
		return sendMsg({closeBrowser: {browserID: browserID}});
	};

	self.showBrowser = function( browserID, needShow, needFocus ) {
		return sendMsg({showBrowser: {
			browserID: browserID,
			show: needShow,
			focus: needFocus
		}});
	};

	self.sendResponse = function( res ) {
		assert(_.isObject(res));
		return sendMsg({jsResponse: res});
	};

	return Object.freeze(self);
}

module.exports = HtmlRemote;
