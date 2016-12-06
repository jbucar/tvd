'use strict';

var EventEmitter = require('events').EventEmitter;
var assert = require('assert');
var tvdutil = require('tvdutil');
var RequestInterval = require('./requestinterval');

function BackendImpl( adapter ) {
	var self = {};
	var _cfg = null;
	var _reqConfig = null;
	var _cfgName = 'backend.json';
	var _storeID = 'ar.edu.unq.utics.tac.tienda';

	function registry() {
 		return adapter.registry();
	}

	function sys() {
		return registry().get('system');
	}

	function defaultConfig() {
		var cfgServer = registry().get('platform').getConfig('backendURL');

		return {
			serial: 0,
			config_server: {
				'url': cfgServer + '/tacconfig/configuration',
				'retryTimeout': (5*60*1000),	//	5 minutes
				'checkTimeout': (60*60*1000)	//	1 hour
			},
			firmware_update_server: {
				'url': cfgServer + '/firmware/update',
				'retryTimeout': (5*60*1000),	//	5 minutes
				'checkTimeout': (60*60*1000)	//	1 hour
			},
			package_update_server: {
				'url': cfgServer + '/package/update',
				'retryTimeout': (5*60*1000),	//	5 minutes
				'checkTimeout': (2*60*60*1000)	//	4 hour
			},
			urlCrashServer: cfgServer + '/tacerror',
		};
	}

	function getRequestInfo() {
		var info = sys().getInfo();
		return {
			id: info.id,
			platform: info.platform.name,
			version: info.system.version
		};
	}

	function onError( crashReport ) {
		//	Check crash server URL
		var url = _cfg.urlCrashServer;
		if (!url) {
			log.warn('Backend', 'Error reporting disabled');
			return;
		}

		//	Send error information to crash server
		var opts = {
			"url": _cfg.urlCrashServer,
			"data": crashReport,
			"method": "POST"
		};
		tvdutil.doRequest( opts, function(err,res) {
			if (err) {
				log.error('Backend', 'Error report fails. Reason: %s', err.message);
			}
			else if (res && res.statusCode === 200) {
				log.info('Backend', 'Error was reported succesfully');
			} else {
				log.error('Backend', 'Error report fails. Server return statusCode=%s', res.statusCode);
			}
		});
	}

	//	Request backend config
	function onConfigResponse(err,data) {
		if (!err && data.serial && data.serial != _cfg.serial) {
			log.info( 'Backend', 'On configuration changed: serial=%s', data.serial );
			//	Update config
			_cfg.serial = data.serial;
			_cfg.config_server = data.config_server;
			_cfg.firmware_update_server = data.firmware_update_server;
			_cfg.package_update_server = data.package_update_server;
			_cfg.urlCrashServer = data.urlCrashServer;

			//	Restart log and save
			sys().setLogConfig( data.log );

			//	Update config requests
			_reqConfig.config( RequestInterval.makeOptionsFromConfig(data.config_server) );

			//	Send backend config changed!
			self.events.emit( 'ConfigChanged', data );
		}
		return true;
	}

	function registerStore() {
		var appMgr = registry().get('appmgr');
		var app = appMgr.addInstance( _storeID );
		var buildInfo = registry().get('platform').getConfig('build');
		app.main = (buildInfo.type === 'release') ? 'http://tiendatac.minplan.gob.ar/tienda' : 'http://tiendatac_dev.minplan.gob.ar/tienda';
		app.info.user = true;
	}

	function unregisterStore(cb) {
		var appMgr = registry().get('appmgr');
		appMgr.rmInstance( _storeID, cb );
	}

	//	API
	self.events = new EventEmitter();
	tvdutil.forwardEventEmitter(self,self.events);

	self.getFirmwareUpdateConfig = function() {
		return _cfg.firmware_update_server;
	};

	self.getPackageUpdateConfig = function() {
		return _cfg.package_update_server;
	};

	self.getCrashServer = function() {
		return _cfg.urlCrashServer;
	};

	self.start = function(cb) {
		//	Load config
		_cfg = adapter.load( _cfgName, defaultConfig() );

		//	Register to errors
		sys().setReportCallback( onError );

		//	Register store
		registerStore();

		//	Create and start config checker
		_reqConfig = new RequestInterval.create();
		_reqConfig.start(
			RequestInterval.makeOptionsFromConfig(_cfg.config_server),
			getRequestInfo,
			onConfigResponse
		);

		cb( undefined, new BackendApi(self) );
	};

	self.stop = function(cb) {
		//	Register to errors
		sys().setReportCallback( undefined );

		//	Remove store
		unregisterStore(function() {
			//	Remove all listeners
			self.events.removeAllListeners();

			//	Stop request config
			_reqConfig.stop();

			cb();
		});
	};

	return self;
}

function BackendApi( impl ) {
	var self = {};

	//	Events
	tvdutil.forwardEventEmitter(self,impl.events);
	self.on.isSignal = true;
    self.on.unregisterMethod = self.removeListener;

	self.getFirmwareUpdateConfig = impl.getFirmwareUpdateConfig.bind(impl);
	self.getPackageUpdateConfig = impl.getPackageUpdateConfig.bind(impl);
	self.getCrashServer = impl.getCrashServer.bind(impl);

	return self;
}

//	Service entry point
function Backend(adapter) {
	var _impl = new BackendImpl(adapter);
	var self = {};

	self.onStart = function(cb) {
		assert(cb);
		_impl.start(cb);
	};

	self.onStop = function(cb) {
		assert(cb);
		_impl.stop(cb);
	};

	return Object.freeze(self);
}

module.exports = Backend;
