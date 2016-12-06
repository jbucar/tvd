"use strict";

var _ = require('lodash');
var assert = require('assert');
var util = require('util');
var tvdutil = require('tvdutil');
var path = require('path');
var bPromise = require("bluebird");
var fs = bPromise.promisifyAll( require('fs') );
var os = require('os');
var Component = require('component');

var serviceStopTimeout = 3000;
var serviceStartTimeout = 20000;

function loadConfig( cfgFile ) {
	try {
		return JSON.parse(fs.readFileSync(cfgFile));
	} catch(e) {
		return null;
	}
}

//	Start aux
function start_lock(srv) {
	log.silly( 'Service', 'start_lock: srv=%s', srv.info.id );
	return new bPromise(function(resolve) {
		//	Wait for unlock
		srv.startCond.lock(function() {
			resolve(srv);
		});
	});
}

function start_impl(oper) {
	log.silly( 'Service', 'start_impl: srv=%s', oper.srv.info.id );
	//	Start service
	var startAsync = bPromise.promisify(oper.srv._wrapped.onStart,oper.srv._wrapped);
	return startAsync()
		.timeout(serviceStartTimeout,util.format('Service not started; timeout: id=%s', oper.srv.info.id))
		.then(function(api) {
			oper.api = api;
			return oper;
		});
}

function checkAPI( api, id ) {
	if (!_.isObject(api)) {
		throw tvdutil.warnErr( 'Service', 'OnStart must return a object: srvID=%s', id );
	}

	//	Have web API?
	if (api.getWebAPI) {
		//	Check web API is a function
		if (!_.isFunction(api.getWebAPI)) {
			throw tvdutil.warnErr( 'Service', 'getWebAPI must be a function: srvID=%s', id );
		}

		//	Check web API name
		var webApis = api.getWebAPI();
		if (!_.isString(webApis.name)) {
			throw tvdutil.warnErr( 'Service', 'getWebAPI must return a object with a name property: srvID=%s', id );
		}
	}
}

function start_finish( oper ) {
	log.silly( 'Service', 'start_finish: srv=%s', oper.srv.info.id );

	//	Check returned object
	var api = oper.api;
	if (!api || !_.isObject(api)) {
		throw tvdutil.warnErr( 'Service', 'OnStart must return a object' );
	}

	if (api._hasMultiple) {
		Object.keys(api).forEach(function (apiName) {
			let subAPI = api[apiName];
			if (apiName !== '_hasMultiple') {
				checkAPI( subAPI, oper.srv.info.id );

				let id = oper.srv.info.id;
				if (apiName !== '_main') {
					id += '.' + apiName;
				}
				console.log( 'rako: addAPI: id=%s', id );
				oper.srv._adapter.registry().put( id , subAPI );
			}
		});
	}
	else {
		checkAPI(api, oper.srv.info.id );

		//	Publish service
		oper.srv._adapter.registry().put( oper.srv.info.id, oper.api );
	}

	return oper;
}


//	This class is the first paramter to services
function ServiceAdapter(reg,info) {
	//	API
	var self = {};

	self.info = function() {
		return info;
	};

	self.getInstallPath = function () {
		var pkgMgr = reg.get('pkgmgr');
		var params = Array.prototype.slice.call(arguments, 0);
		params.unshift(info.id);
		return pkgMgr.getInstallPath.apply( pkgMgr, params );
	};

	self.getDataPath = function() {
		var pkgMgr = reg.get('pkgmgr');
		var params = Array.prototype.slice.call(arguments, 0);
		params.unshift(info.id);
		return pkgMgr.getDataPath.apply( pkgMgr, params );
	};

	self.getInstallRootPath = function() {
		return tac.getInstallRootPath();
	};

	self.getDataRootPath = function() {
		return tac.getDataRootPath();
	};

	self.getSharePath = function() {
		return path.join(tac.getDataRootPath(),'share');
	};

	self.getTemporaryPath = function() {
		return os.tmpdir();
	};

	self.registry = function() {
		return reg;
	};

	//	Load configuration. If no config.json found, use defaultsValues
	self.load = function( cfgFile, defaultsValues, convertConfig ) {
		var config = {};
		//	Setup version
		config._version = info.version;
		//	Load defaults variables
		_.assign( config, defaultsValues );
		//	Load install config file
		_.assign( config, loadConfig(reg.get('pkgmgr').getInstallPath( info.id, cfgFile )) );
		var dataCfg = loadConfig(reg.get('pkgmgr').getDataPath( info.id, cfgFile ));
		if (!dataCfg) {
			return config;
		}

		if (dataCfg._version === info.version) {
			//	Load rw file
			_.assign( config, dataCfg );
		}
		else if (convertConfig) {
			_.assign( config, convertConfig(dataCfg) );
		}
		else {
			log.warn( 'Service', 'Reseting service configuration; No data conversion available: srvID=%s', info.id );

			//	Reset data sync
			var p = self.getDataPath();
			var files = fs.readdirSync(p);
			files.forEach(function(entry) {
				tvdutil.removeTreeSync( path.join(p,entry) );
			});
		}

		return config;
	};

	//	Save configuration
	self.save = function( cfgFile, config ) {
		//	Write data to disk
		config._version = info.version;
		fs.writeFileSync(
			reg.get('pkgmgr').getDataPath( info.id, cfgFile ),
			JSON.stringify( config, null, 4 )
		);
	};

	//	Reset data
	self.resetData = function() {
		var p = self.getDataPath();
		return fs.readdirAsync(p)
			.then(function(content) {
				var proms = [];
				content.forEach(function(entry) {
					proms.push( tvdutil.removeTreeAsync( path.join(p,entry) ) );
				});
				return bPromise.all(proms);
			})
			.catch(function() {
				//	Ignore error
			});
	};

	return self;
}

function Service(reg,id) {
	log.silly( 'Service', 'Create service: id=%s', id );

	//	Call to Component constuctor
	Service.super_.call(this,reg,id);

	//	Checks
	if (this.info.type != 'service') {
		throw new Error( 'Property type is not service' );
	}

	//	Get package manager
	var pkgMgr = reg.get('pkgmgr');
	assert(pkgMgr);

	//	Get component info from manifest
	var info = pkgMgr.getComponentInfo( id );
	assert(info);

	//	Configuration app
	this.info.runs = info.runs;
	if (this.info.runs === undefined) {
		this.info.runs = 'onStartup';
	}
	assert( this.info.runs === 'onStartup' || this.info.runs === 'onDemand' );
	this.info.configure = info.configure;

	//	Check if main is file
	var main = reg.get('pkgmgr').getInstallPath( this.info.id, this.info.main );
	if (!tvdutil.isFile(main)) {
		throw tvdutil.warnErr( 'Service', 'Main must be a file: main=%s', main );
	}
	this.info.main = main;
	log.verbose( 'Service', 'Loading module: main=%s', this.info.main );

	//	Setup references
	this.ref = 0;
	this.startCond = new tvdutil.BasicWaitCondition();

	//	Create adapter
	this._adapter = new ServiceAdapter(reg,this.info);

	//	Load service main module with a service adapter
	this._wrapped = require(this.info.main)( this._adapter );

	//	Some checks on wrapped
	if (!_.isObject(this._wrapped)) {
		throw tvdutil.warnErr( 'Service', "A service must return an object. id=%s", this.info.id );
	}

	if (!_.isFunction(this._wrapped.onStart) || !_.isFunction(this._wrapped.onStop)) {
		throw tvdutil.warnErr( 'Service', "A Service must define onStart/onStop method. id=%s", this.info.id );
	}
}
util.inherits(Service, Component);

Service.prototype.runs = function(st) {
	assert( st === 'onStartup' || st === 'onDemand' );
	log.verbose( 'Service', 'Change runs state: id=%s, st=%s', this.info.id, st );
	this.info.runs = st;
};

Service.prototype.autoStart = function() {
	return (this.info.runs === 'onStartup');
};

Service.prototype.run = function() {
	var srv = this;
	return start_lock(srv)
		.then(function() {
			assert(srv.ref >= 0);
			log.log( (srv.ref === 0) ? 'info' : 'verbose', 'Service', 'run: id=%s, references=%d', srv.info.id, srv.ref );

			//	If already started?
			if (srv.ref > 0) {
				//	resolve!
				return;
			}
			else {
				var state = { srv: srv };

				return srv.runDependencies().return(state)
					.then(start_impl)
					.then(start_finish)
					.catch(function(err) {
						//	Call to stop implementation if necesary
						if (state.api) {
							srv._wrapped.onStop(function() {});
						}

						//	Stop dependencies!
						return srv.stopDependencies()
							.then(function() {
								throw err;
							});
					});
			}
		})
		.then(function() {
			log.silly( 'Service', 'Increment reference: id=%s, references=%d', srv.info.id, srv.ref );
			//	Add reference
			srv.ref++;
			srv.startCond.dec();
		},function(err) {
			log.warn( 'Service', 'Cannot start service: id=%s, err=%s', srv.info.id, err.message );
			assert( srv.ref === 0 );
			srv.startCond.dec();
			throw err;
		});
};

function stopAsync(srv) {
	var stopAsyncImpl = bPromise.promisify(srv._wrapped.onStop,srv._wrapped);
	return stopAsyncImpl()
		.timeout(serviceStopTimeout,'Service stopped by timeout');
}

Service.prototype.stop = function(force) {
	var srv = this;
	return start_lock(srv)
		.then(function() {
			assert(srv.ref >= 0);
			log.log( (srv.ref === 1) ? 'info' : 'verbose', 'Service', 'Stop: id=%s, references=%d', srv.info.id, srv.ref );

			//	Not running?
			assert(srv.ref >= 0);
			if (srv.ref === 0) {
				log.silly( 'Service', 'Cannot stop service; service not running: id=%s', srv.info.id );
				srv.startCond.dec();
				return bPromise.resolve(false);
			}

			//	Decrement reference
			if (srv.ref > 1) {
				srv.ref--;
				if (force) {
					srv.startCond.dec();
					throw tvdutil.warnErr('Service', 'Cannot stop service; service is still referenced: id=%s, references=%d', srv.info.id, srv.ref );
				}
				else {
					log.silly( 'Service', 'Cannot stop service; service is still referenced: id=%s, references=%d', srv.info.id, srv.ref );
					srv.startCond.dec();
					return bPromise.resolve(false);
				}
			}

			//	Remove service API from registry
			srv._adapter.registry().remove( srv.info.id );

			function onServiceStopped( err ) {
				//	Remove reference
				srv.ref--;

				if (err) {
					log.warn( 'Service', 'Service stopped with error: id=%s, err=%s', srv.info.id, err.message );
				}
				else {
					log.verbose( 'Service', 'Service stopped: id=%s', srv.info.id );
				}

				//	Stop dependencies
				return srv.stopDependencies().then(function() {
					srv.startCond.dec();

					//	If error, propagate error
					if (err) {
						throw err;
					}

					return true;
				});
			}

			//	Stop implementation
			return stopAsync(srv).then(function() {
				return onServiceStopped();
			}, onServiceStopped );
		});
};

Service.prototype.isRunning = function() {
	return this.ref > 0;
};

Service.prototype.dump = function() {
	var res = _.clone(this.info);

	//	Get service API
	var api = this._adapter.registry().get( this.info.id );
	if (api && api.dump) {
		try {
			res.state = api.dump();
		} catch(err) {
			log.warn( 'Service', 'Cannot dump service: id=%s, err=%s', this.info.id, err.message );
		}
	}

	return res;
};

Service.prototype.resetData = function() {
	var srv = this;
	if (srv.isRunning()) {
		return srv.stop()
			.then(function() {
				return srv._adapter.resetData();
			}).then(function() {
				return srv.run();
			});
	}
	else {
		return srv._adapter.resetData();
	}
};

module.exports = Service;
