'use strict';

//	Require
var assert = require('assert');
var EventEmitter = require('events').EventEmitter;
var bPromise = require("bluebird");
var fs = bPromise.promisifyAll( require('fs') );
var tvdutil = require('tvdutil');
var Service = require('./service');

//	Local variables
var ID = 'ar.edu.unlp.info.lifia.tvd.system';

function ServiceManagerImpl() {
	var _reg = null;
	var _config = null;
	var _isReady = false;
	var _services = null;
	var _lockDatabaseCond = null;
	var self = {};
	self.events = new EventEmitter();

	function getPkgMgr() {
		return _reg.get('pkgmgr');
	}

	function getConfigPath() {
		return getPkgMgr().getDataPath(ID,'services.json');
	}

	function setReady( state ) {
		log.verbose('ServiceManager','Change service manager state: ready=%d', state );
		_isReady = state;
		if (state) {
			self.events.emit('ready', state );
		}
		else {
			return tvdutil.emitThen( self.events, 'ready', state );
		}
	}

	//	Database
	function lockDatabase( param ) {
		assert(param);
		assert(param.id);
		log.silly( 'ServiceManager', 'Wait for database: id=%s', param.id );

		//	Wait for unlock
		return new bPromise(function(resolve) {
			_lockDatabaseCond.lock(function() {
				resolve(param);
			});
		});
	}

	function unlockDatabase( param ) {
		assert(param);
		assert(param.id);
		log.silly( 'ServiceManager', 'Unlock database: id=%s', param.id );

		_lockDatabaseCond.dec();
		return bPromise.resolve(param);
	}

	function saveDatabase( param ) {
		assert(param);
		assert(param.id);
		log.silly( 'ServiceManager', 'Save database: id=%s', param.id );

		//	Write to packages.json.new
		var filename = getConfigPath();
		var newFile = filename + '.new';
		var oldFile = filename + '.old';
		return fs.writeFileAsync( newFile, JSON.stringify(_config, null, 4) )
			.then(function() {
				//	Move config.json to config.json.old
				return fs.renameAsync( filename, oldFile )
					.catch(function() {
						return bPromise.resolve();
					});
			})
			.then(function() {
				//	Move config.json.new to config.json
				fs.renameAsync( newFile, filename );
			})
			.then(function() {
				return param;
			});
	}

	//	Enable aux
	function removeServiceConfig( param ) {
		assert(param);
		assert(param.id);
		log.silly( 'ServiceManager', 'Remove service configuration: id=%s', param.id );
		delete _config[param.id];
		return param;
	}

	function enable_changeServiceStartup( param ) {
		assert(param);
		assert(param.id);
		log.silly( 'ServiceManager', 'Change service startup flag: id=%s', param.id );

		//	Change runs state in service
		var startMethod = param.state ? 'onStartup' : 'onDemand';
		param.srv.runs( startMethod );

		//	Save runs state in config list
		if (!_config[param.id]) {
			_config[param.id] = {};
		}
		_config[param.id].runs = startMethod;

		return param;
	}

	function enable_restartService( param ) {
		assert(param);
		assert(param.id);
		log.silly( 'ServiceManager', 'Reload service state: id=%s', param.id );

		var prom;
		if (param.state) {
			//	Run service, ignore error
			prom=self.runService( param.id );
		}
		else {
			//	Disable it!
			prom=self.stopService( param.id );
		}

		//	Catch any error when start/stop
		return prom.catch(function() {});
	}

	//	Init aux
	function loadConfig() {
		log.silly( 'ServiceManager', 'Load configuration: file=%s', getConfigPath() );
		_config = {};
		return fs.readFileAsync( getConfigPath() )
			.then(JSON.parse)
			.then(function(cfg) {
				_config = cfg;
			},function(err) {
				log.verbose('ServiceManager', 'Cannot load configuration file: err=%s', err.message );
			});
	}

	function removeConfig( id ) {
		//	Remove configuration
		var param = {
			id: id,
			srv: _services[id]
		};

		return lockDatabase(param)
			.then(removeServiceConfig)
			.then(saveDatabase)
			.then(unlockDatabase);
	}

	function loadService( id ) {
		return new bPromise(function(resolv,reject) {
			if (_services[id] !== undefined) {
				return resolv();
			}

			var srv;
			try {
				//	Create service from info
				srv = new Service(_reg,id);
			} catch (err) {
				log.warn( 'ServiceManager', 'Cannot create service: id=%s, err=%s', id, err.message );
				throw err;
			}

			//	Setup autostart flag
			var srvConfig = _config[id];
			if (srvConfig !== undefined) {
				srv.runs( srvConfig.runs );
			}

			log.silly( 'ServiceManager', 'Load service: id=%s, start=%d', id, srv.autoStart() );

			//	Assign
			_services[id] = srv;
			self.events.emit( 'loaded', id );

			//	If service need start, run it
			if (srv.autoStart()) {
				//	Try load && start all service dependencies
				return new bPromise.all(
					srv.dependencies().map(function(dep) {
						return loadService(dep);
					})
				).then(function() {
					return self.runService(id);
				}).then(resolv,reject);
			}

			resolv();
		});
	}

	function stopUsersOf( dep ) {
		log.silly( 'ServiceManager', 'Stop users of service: id=%s', dep );
		return bPromise.all(Object.keys(_services).reduce(function(res,id) {
			if (_services[id].depends(dep)) {
				res.push( self.stopService(id,true) );
			}
			return res;
		},[]));
	}

	function unloadService( id, persistState ) {
		log.silly( 'ServiceManager', 'Unload service: id=%s, persistState=%d', id, persistState );
		return self.stopService(id,true).then(function() {
			//	Reset state if necesary
			if (!persistState) {
				return removeConfig( id );
			}
			return bPromise.resolve();
		}).finally(function() {
			delete _services[id];
		});
	}

	function loadServices() {
		_services = {};
		var services = getPkgMgr().getAllComponentsInfo('service');
		log.silly( 'ServiceManager', 'Load all services: services=%j', services );

		return new bPromise.settle(services.map(function(srv) {
			return loadService(srv.id);
		}));
	}

	function unloadPkgServices(pkgID,isUpdate,cb) {
		log.verbose( "ServiceManager", "On package unloaded: pkgID=%s, isUpdate=%d", pkgID, isUpdate );

		//	Get all services from pkgID
		var comps = getPkgMgr().getComponents(pkgID,'service');

		//	Stop all if necesary
		return new bPromise.settle(comps.map(function(comp) {
			return unloadService(comp.id,isUpdate);
		})).then(function() {
			cb();
		}, cb );
	}

	//	Fin aux
	function findLeafsRunning() {
		var res = [];
		for (var id in _services) {
			var srv = _services[id];
			if (srv.isRunning()) {
				var isLeaf = true;
				for (var x in _services) {
					var cur = _services[x];
					if (cur.isRunning() && cur.depends( id )) {
						isLeaf = false;
					}
				}
				if (isLeaf) {
					res.push( srv.info.id );
				}
			}
		}
		return res;
	}

	function finAux() {
		//	Stop all services, sarting by leafs
		var leafs = findLeafsRunning();
		if (leafs.length > 0) {
			log.silly( 'ServiceManager', 'Leafs=%s', leafs );
			return bPromise.settle(leafs.map(function(id) {
				return self.stopService( id );
			})).then(finAux);
		}
		else {
			log.silly( 'ServiceManager', 'All services stopped!' );
			return bPromise.resolve();
		}
	}

	//	API
	self.reg = function() {
		assert(_reg);
		return _reg;
	};

	self.onPkgUpdate = function( pkgID, cb ) {
		unloadPkgServices( pkgID, true, cb );
	};
	self.onPkgUpdate.isAsync = true;

	self.onPkgUninstalled = function( pkgID, cb ) {
		unloadPkgServices( pkgID, false, cb );
	};
	self.onPkgUninstalled.isAsync = true;

	self.onPkgInstalled = function(pkgID) {
		log.verbose( 'ServiceManager', 'On package loaded: pkgID=%s', pkgID );

		//	Load services
		getPkgMgr().getComponents(pkgID,'service').forEach(function(comp) {
			loadService( comp.id, true );
		});
	};

	self.isRunning = function(id) {
		var srv = _services[id];
		return srv ? srv.isRunning() : false;
	};

	self.getAllRunning = function() {
		var res = [];
		for (var id in _services) {
			if (_services[id].isRunning()) {
				res.push(id);
			}
		}
		return res;
	};

	self.getDependencies = function(id) {
		var srv = _services[id];
		if (srv) {
			return srv.dependencies();
		}
		return [];
	};

	self.getService = function(id) {
		var srv = _services[id];
		if (srv) {
			return {
				"id": srv.info.id,
				"name": srv.info.name,
				"icon": srv.info.icon,
				"pkgID": srv.info.pkgID,
				"description": srv.info.description,
				"version": srv.info.version,
				"vendor": srv.info.vendor,
				"vendor-url": srv.info["vendor-url"],
				"isEnabled": self.isEnabled(id),
				"canConfigure": (srv.info.configure !== undefined),
				"canUninstall": getPkgMgr().canUninstall(srv.info.pkgID)
			};
		}
		return null;
	};

	self.getAll = function() {
		var res = [];
		for (var id in _services) {
			if (_services[id].info.user) {
				res.push(id);
			}
		}
		return res;
	};

	self.getConfigApp = function(id) {
		log.silly('ServiceManager', 'Get config app for service: %s', id);
		var service = _services[id];
		if (service && service.info.configure) {
			return 'file://' + getPkgMgr().getInstallPath(id, service.info.configure);
		}
	};

	self.enable = function(id,state,cb) {
		log.info('ServiceManager', 'Enable service: id=%s, state=%d', id, state);
		assert(cb);

		//	Check if service is valid
		var srv = _services[id];
		if (!srv) {
			cb( tvdutil.warnErr( 'ServiceManager', 'Service not found: id=%s', id ) );
			return;
		}

		//	Check if service is already enabled/disabled
		if (state == srv.autoStart()) {
			cb( tvdutil.warnErr( 'ServiceManager', 'Service already enabled/disabled: id=%s, state=%d', id, state) );
			return;
		}

		var param = {
			id: id,
			srv: srv,
			state: state
		};

		lockDatabase(param)
			.then(enable_changeServiceStartup)
			.then(saveDatabase)
			.then(unlockDatabase)
			.then(enable_restartService)
			.then(function() {
				cb();
			},cb);
	};

	self.isEnabled = function(id) {
		//	Check if service is valid
		var srv = _services[id];
		if (!srv) {
			log.warn('ServiceManager', 'Service not found: id=%s', id);
			return false;
		}

		return srv.autoStart();
	};

	self.runService = function(id) {
		log.silly('ServiceManager', 'run: id=%s', id);

		var srv = _services[id];
		if (!srv) {
			return bPromise.reject( tvdutil.warnErr( 'ServiceManager', 'Service not found: id=%s', id ) );
		}

		//	Run service
		return srv.run()
			.then(function() {
				//	Notify
				self.events.emit( 'started', id );
			});
	};

	self.stopService = function(id,forceStop) {
		assert(id);
		log.silly('ServiceManager', 'Stop service: id=%s', id);

		//	Get service
		var srv = _services[id];
		if (!srv) {
			return bPromise.reject( tvdutil.warnErr('ServiceManager', 'Try to stopping a invalid service. id=%s', id) );
		}

		//	Stop users of service (if necesary)
		var prom = forceStop ? stopUsersOf(id) : bPromise.resolve();
		return prom.then(function() {
			//	Stop service
			return srv.stop(forceStop);
		}).then(function(stopped) {
			if (stopped) {
				//	Send notification
				self.events.emit( 'stopped', id );
			}
		}).catch(function(err) {
			self.events.emit( 'error', err );
		});
	};

	self.dumpService = function(id) {
		assert(id);
		var srv = _services[id];
		if (srv) {
			return srv.dump();
		}
		return undefined;
	};

	self.resetData = function(id,cb) {
		assert(id);
		var srv = _services[id];
		if (srv) {
			log.info( 'ServiceManager', 'Reset service data: id=%s', id );
			srv.resetData()
				.then(function() {
					cb();
				},cb);
		}
		else if (cb) {
			cb( new Error('Invalid service') );
		}
	};
	self.resetData.isAsync = true;

	self.isReady = function() {
		return _isReady;
	};

	self.init = function( reg ) {
		log.info('ServiceManager', 'Init' );

		//	Setup variables
		assert(reg);
		_reg = reg;
		_isReady = false;
		_lockDatabaseCond = new tvdutil.BasicWaitCondition();

		//	Load service configuration
		loadConfig()
			.then(loadServices)
			.then(function() {
				//	Register to pkgMgr uninstall signal
				var pkgMgr = getPkgMgr();
				pkgMgr.on('uninstall', self.onPkgUninstalled );
				pkgMgr.on('update', self.onPkgUpdate );
				pkgMgr.on('install', self.onPkgInstalled );
				pkgMgr.on('updated', self.onPkgInstalled );

				//	Set service manager ready
				setReady( true );
			});
	};

	self.fin = function() {
		log.verbose('ServiceManager', 'Fin implementation' );
		return setReady( false )
			.then(finAux)
			.then(function() {
				//	Cleanup all services loaded
				_services = {};

				var pkgMgr = getPkgMgr();
				pkgMgr.removeListener('uninstall', self.onPkgUninstalled );
				pkgMgr.removeListener('update', self.onPkgUpdate );
				pkgMgr.removeListener('install', self.onPkgInstalled );
				pkgMgr.removeListener('updated', self.onPkgInstalled );
				return bPromise.resolve();
			});
	};

	return self;
}

//	API
function ServiceManager() {
	var _impl = new ServiceManagerImpl();
	var self = this;

	//	Service
	self.getAll = _impl.getAll.bind(_impl);
	self.getAllRunning = _impl.getAllRunning.bind(_impl);
	self.get = _impl.getService.bind(_impl);
	self.getDependencies = _impl.getDependencies.bind(_impl);
	self.getConfigApp = _impl.getConfigApp.bind(_impl);

	//	Enabled
	self.isEnabled = _impl.isEnabled.bind(_impl);
	self.enable = _impl.enable.bind(_impl);
	self.enable.isAsync = true;

	//	Running
	self.isRunning = _impl.isRunning.bind(_impl);
	self.run = _impl.runService.bind(_impl);
	self.stop = _impl.stopService.bind(_impl);

	//	State
	self.dump = _impl.dumpService.bind(_impl);
	self.resetConfig = _impl.resetData.bind(_impl);
	self.resetConfig.isAsync = true;

	//	Service manager methods
	self.isReady = _impl.isReady.bind(_impl);
	self.init = function(reg) {
		reg.put( 'srvmgr', self );
		return _impl.init(reg);
	};
	self.fin = function() {
		return _impl.fin()
			.then(function() {
				_impl.reg().remove('srvmgr');
			});
	};

	//	Events
	tvdutil.forwardEventEmitter(self,_impl.events);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	//	Web API
	self.getWebAPI = function() {
		return {
			'name': 'srvmgr',
			'public': ['getAll', 'get', 'isRunning', 'isEnabled'],
			'private': ['resetConfig', 'enable']
		};
	};

	return Object.freeze(self);
}

module.exports = ServiceManager;

