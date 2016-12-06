'use strict';

var assert = require('assert');
var _ = require('lodash');
var EventEmitter = require('events').EventEmitter;
var bPromise = require("bluebird");
var tvdutil = require('tvdutil');
var Application = require('./application');
var Shell = require('./shell');
var FocusManager = require('./focusmanager');

function ApplicationMgrImpl( adapter ) {
	var _cfg = null;
	var _applications = {};
	var _shell = null;
	var _ready = {};
	var _focusMgr = new FocusManager();
	var self = {};
	self.events = new EventEmitter();

	function reg() {
		assert(adapter);
		return adapter.registry();
	}

	function defaultConfig() {
		return {
			'shell' : reg().get('platform').getConfig('shell'),
			'webApps' : reg().get('platform').getConfig('store'),
			'srvCfg': 'ar.edu.unlp.info.lifia.tvd.srv_cfg',
			'favorites': []
		};
	}

	function convertConfig( oldData ) {
		return oldData;
	}

	self.resetState = function(id) {
		//	Remove favorite
		if (self.isFavorite( id )) {
			self.toggleFavorite( id );
		}
	};

	self.onAppChanged = function( evt ) {
		log.verbose( 'ApplicationManager', 'Application status changed: evt=%j', evt );
		if (evt.id === _shell.getShellID()) {
			if (evt.state === 'loaded') {
				self.events.emit( 'ready', true );
				self.showApplication( evt.id );
			}
		}

		_focusMgr.onAppChanged(_applications[evt.id], evt.state);
		self.events.emit( 'application', evt );

		//	If app is not the shell(shell/system_layer) ... and starting ... show app
		if (evt.id.indexOf(self.getShell()) !== 0 && evt.state === 'launched') {
			self.showApplication( evt.id );
		}
	};

	self.getShell = function() {
		return _cfg.shell;
	};

	self.getWebApps = function() {
		return _cfg.webApps;
	};

	//	Events
	self.isReady = function() {
		return _ready.browser && _ready.srvmgr && _ready.screen;
	};

	self.readyChanged = function(evt, isReady, cb) {
		if (_ready[evt] !== isReady) {
			//	Change event state
			var oldState = self.isReady();
			_ready[evt] = isReady;
			var newState = self.isReady();

			//	State changed?
			if (oldState !== newState) {
				log.verbose('ApplicationManager', 'Ready changed: old=%d, new=%d', oldState, newState );
				var notif = reg().get('notification');
				if (newState) {
					notif.on('ShowSystemLayer', self.showSystemLayer);
					//	Start system layer
					_shell.start();
				}
				else {
					notif.removeListener('ShowSystemLayer', self.showSystemLayer);
					self.stopAll(function() {
						self.events.emit( 'ready', false );
						if (cb) {
							cb();
						}
					});
				}
			}
		}
	};

	self.onScreenActiveChanged = function( isActive ) {
		log.silly('ApplicationManager', 'Screen active changed: isActive=%d', isActive );
		self.readyChanged('screen', isActive);
	};

	self.onBrowserReady = function( isReady ) {
		log.silly('ApplicationManager', 'BrowserService ready: isReady=%d', isReady);
		self.readyChanged('browser', isReady);
	};

	self.onSrvMgrReady = function( isReady, cb ) {
		log.info('ApplicationManager', 'Service manager ready: ready=%d, cb=%j', isReady, cb );
		self.readyChanged('srvmgr', isReady, cb);
	};
	self.onSrvMgrReady.isAsync = true;

	self.onPkgInstalled = function( pkgID ) {
		log.verbose( 'ApplicationManager', 'On package installed: pkgID=%s', pkgID );

		//	Load application
		reg().get('pkgmgr').getComponents(pkgID,'app').forEach(function(comp) {
			if (self.loadAppSafe( comp.id ) !== undefined) {
				self.events.emit( 'install', comp.id );
			}
		});
	};

	self.unloadComps = function( pkgID, isUpdate, cb ) {
		log.verbose( 'ApplicationManager', 'On package uninstalled: pkgID=%s', pkgID );

		var comps = reg().get('pkgmgr').getComponents(pkgID,'app');
		if (comps.length >= 1) {
			var id = comps[0].id;//	Only 1 app in a package is permited
			self.unloadApp( {id:id,update: isUpdate}, function() {
				self.events.emit( 'uninstall', id );
				cb();
			});
		} else {
			cb();
		}
	};

	self.onPkgUpdate = function( pkgID, cb ) {
		self.unloadComps( pkgID, true, cb );
	};
	self.onPkgUpdate.isAsync = true;

	self.onPkgUninstalled = function( pkgID, cb ) {
		self.unloadComps( pkgID, false, cb );
	};
	self.onPkgUninstalled.isAsync = true;

	self.unloadApp = function( opts, cb ) {
		log.info( 'ApplicationManager', 'Unload application: id=%s', opts.id );
		assert(opts.id);
		assert(cb);

		function unload() {
			if (!opts.update) {
				//	Reset application state
				self.resetState(opts.id);
			}

			//	Remove listener
			app.removeListener('changed', self.onAppChanged );

			delete _applications[opts.id];

			cb();
		}

		//	Check application exists
		var app = _applications[opts.id];
		if (!app) {
			cb( tvdutil.warnErr( 'ApplicationManager', 'Application not found: id=%s', opts.id ) );
			return;
		}

		//	Stop application first
		app.stop().finally(unload);
	};

	self.loadAppSafe = function( appID, newID ) {
		var app;
		try {
			app = self.loadApp( appID, newID );
		}
		catch (err) {
			log.warn( 'ApplicationManager', 'Cannot load application: appID=%s, err=%s', appID, err.message );
		}
		return app;
	};

	self.loadApp = function( appID, newID ) {
		if (!newID) {
			newID = appID;
		}

		//	Is application loaded?
		var app = _applications[newID];

		//	appID is a template, it can't be loaded as application
		if (newID !== appID && app) {
            log.warn( 'ApplicationManager', 'An instance for template app=%s with id=%s was already loaded', appID, newID );
			return app;
		}

		if (!app) {
			//	Create application, if error, catch error ...
            _applications[newID] = app = new Application( reg(), appID );
			app.info.id = newID;

			//	Listen to app loaded and closed events
			app.on('changed', self.onAppChanged );
		}
		return app;
	};

	self.startApp = function( opts ) {
		return new bPromise(function(resolve,reject) {
			log.info( 'ApplicationManager', 'Start application: id=%s', opts.id );

			//	Load app if necesary
			var app = self.loadApp( opts.id );

			//	Is app already running
			if (app.isRunning()) {
				throw tvdutil.warnErr( 'ApplicationManager', 'Application already started: id=%s', opts.id );
			}

			//	Start application
			return app.run( opts.params )
				.then(resolve,reject);
		});
	};

	self.stopApp = function( appID ) {
		log.info( 'ApplicationManager', 'Stop application: id=%s', appID );
		var app = _applications[appID];
		if (!app) {
			return bPromise.reject( tvdutil.warnErr( 'ApplicationManager', 'Application not found: id=%s', appID ) );
		}

		return app.stop();
	};

	self.getApp = function( id ) {
		var app = _applications[id];
		if (app) {
			let info = _.cloneDeep(app.info);
			info.isFavorite = self.isFavorite(id);
			info.canUninstall = reg().get('pkgmgr').canUninstall( app.info.pkgID );
			return info;
		}
		return null;
	};

	self.getApps = function() {
		var apps = [];
		for (var id in _applications) {
			var app = _applications[id];
			if (app.info.user) {
				apps.push( id );
			}
		}
		return apps;
	};

	self.getAllRunning = function() {
		var res = [];
		self.getApps().reduce(function(apps,appID) {
			if (self.isRunning(appID)) {
				apps.push( appID );
			}
			return apps;
		}, res );
		return res;
	};

	self.isRunning = function( id ) {
		log.verbose( 'ApplicationManager', 'Is application running: id=%s', id );
		var app = _applications[id];
		return app ? app.isRunning() : false;
	};

	self.toggleFavorite = function( id ) {
		var index = _cfg.favorites.indexOf( id );
		if (index === -1) {
			_cfg.favorites.push(id);
		}
		else {
			_cfg.favorites.splice(index,1);
		}
		adapter.save( 'appmgr.json', _cfg );
	};

	self.isFavorite = function( id ) {
		return (_cfg.favorites.indexOf( id ) === -1) ? false : true;
	};

	self.showSystemLayer = function( needShow, needFocus ) {
		_shell.show(needShow, needFocus);
	};

	self.showApplication = function( appId ) {
		var app = _applications[appId];
		assert(app);
		app.show(true, app._focus);
	};

	self.showVirtualKeyboard = function( appId, vkCfg, cb ) {
		var app = _applications[appId];
		if (app) {
			app.showVirtualKeyboard(vkCfg, cb);
		} else {
			log.warn( 'ApplicationManager', 'Fail to show virtual keyboard, invalid app id=%s', appId );
			cb(new Error('Fail to show virtual keyboard, invalid app id=' + appId));
		}
	};

	self.runServiceCfg = function(srvID) {
		return new bPromise(function(resolve) {
			var srvMgr = reg().get('srvmgr');
			var main = srvMgr.getConfigApp(srvID);
			if (!main) {
				throw tvdutil.warnErr( 'ApplicationManager', 'Service do not have a config app: id=%s', srvID);
			}
			log.verbose( 'ApplicationManager', 'Run service configuration: srvID=%s, url=%s', srvID, main);

			//	Do not load twice the application
			var instanceId = srvID + '.cfg';
			if (self.isRunning(instanceId)) {
				throw tvdutil.warnErr( 'ApplicationManager', 'Application already started: id=%s', instanceId);
			}

			//	Load and setup application
			var app = self.loadApp(_cfg.srvCfg, instanceId);
			app.info.main = main;
			app.info.system = false;
			app.info.dependencies = _.clone(srvMgr.getDependencies(srvID));
			app.info.dependencies.push(srvID);
			app._cfgService = srvID;

			function onCfgAppChanged(evt) {
				if (evt.state === 'stopped') {
					self.unloadApp({id:instanceId}, function() {
						app.removeListener('changed', onCfgAppChanged);
					});
				}
			}
			app.on('changed', onCfgAppChanged);

			resolve( {id:instanceId} );
		}).then(self.startApp);
	};

	self.stopAll = function( cb ) {
		log.info( 'ApplicationManager', 'Stop all applications' );

		//	Stop all apps running
		var proms = [];
		Object.keys(_applications).reduce(function(apps,appID) {
			var app = _applications[appID];
			if (!app.isStopped()) {
				apps.push( app.stop() );
			}
			return apps;
		}, proms );

		bPromise.settle(proms)
			.then(function() {
				cb();
			},cb);
	};

	self.start = function(cb) {
		assert( cb );
		assert(_.keys(_applications).length === 0 );

		//	Load settings
		_cfg = adapter.load( 'appmgr.json', defaultConfig(), convertConfig );

		//	Check packages
		var pkgMgr = reg().get( 'pkgmgr' );

		if (!pkgMgr.hasComponent( self.getShell() )) {
			log.warn( 'ApplicationManager', 'Shell application not exists, using default: shell=%s', self.getShell() );
			_cfg.shell = defaultConfig().shell;
		}

		if (!pkgMgr.hasComponent( self.getWebApps() )) {
			log.warn( 'ApplicationManager', 'Web applications not exists, using default: webapps=%s', self.getWebApps() );
			_cfg.webApps = defaultConfig().webApps;
		}

		//	Create system layer
		_shell = new Shell( reg(), self );

		{	//	Setup service manager dependency
			var srvMgr = reg().get('srvmgr');
			_ready.srvmgr = srvMgr.isReady();
			srvMgr.on('ready', self.onSrvMgrReady );
		}

		{	//	Setup browser dependency
			var browser = reg().get('browser');
			_ready.browser = browser.isReady();
			browser.on('ready', self.onBrowserReady );
		}

		{	//	Setup screen dependency
			var screen = reg().get('screen');
			_ready.screen = screen.isActive();
			screen.on('changed', self.onScreenActiveChanged );
		}

		//	Load all user applications
		pkgMgr.getAllComponentsInfo('app').forEach(function(comp) {
			if ((comp.user === undefined) || comp.user) {
				self.loadAppSafe( comp.id );
			}
		});

		{	//	Listen for package manager install/unininstall events
			pkgMgr.on('install', self.onPkgInstalled );
			pkgMgr.on('updated', self.onPkgInstalled );
			pkgMgr.on('uninstall', self.onPkgUninstalled );
			pkgMgr.on('update', self.onPkgUpdate );
		}

		cb( undefined, new ApplicationManagerApi( self ) );
	};

	self.stop = function(cb) {
		assert(cb);
		log.verbose( 'ApplicationManager', 'stop' );

		//	Stop all applications
		self.stopAll(function() {
			{	//	Remove listener for screen active changed event
				var screen = reg().get('screen');
				screen.removeListener('changed', self.onScreenActiveChanged );
			}

			{	//	Remove listener for service manager ready event
				var srvMgr = reg().get('srvmgr');
				srvMgr.removeListener('ready', self.onSrvMgrReady );
			}

			{	//	Remove listener for browser service ui ready event
				var browser = reg().get('browser');
				browser.removeListener('ready', self.onBrowserReady );
			}

			{	//	Remove listener for package manager install/unininstall events
				var pkgMgr = reg().get('pkgmgr');
				pkgMgr.removeListener('install', self.onPkgInstalled );
				pkgMgr.removeListener('updated', self.onPkgInstalled );
				pkgMgr.removeListener('uninstall', self.onPkgUninstalled );
				pkgMgr.removeListener('update', self.onPkgUpdate );
			}

			_applications = {};
			_shell = null;
			cb();
		});
	};

	return self;
}

//	API
function ApplicationManagerApi( mgr ) {
	var self = this;

	//	Events
	tvdutil.forwardEventEmitter(self,mgr.events);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	self.isReady = function() {
		return mgr.isReady();
	};

	self.addInstance = function( id, newID ) {
		return mgr.loadApp( id, newID );
	};

	self.rmInstance = function( id, cb ) {
		return mgr.unloadApp( {id:id}, cb );
	};

	self.run = function( id, cb ) {
		return self.runAsync( {id:id}, function(err) {
			cb(err, err === undefined);
		});
	};
	self.run.isAsync = true;

	self.runAsync = function( opts, cb ) {
		assert(cb);
		return mgr.startApp( opts )
			.then(function() {
				cb(undefined);
			},cb);
	};
	self.runAsync.isAsync = true;

	self.stop = function( id, fnc ) {
		mgr.stopApp( id )
			.then(function() {
				fnc();
			},fnc);
	};
	self.stop.isAsync = true;

	self.exit = function( id ) {
		self.stop( id, function() {} );
	};
	self.exit.insertId = true;

	self.get = function( id ) {
		return mgr.getApp( id );
	};

	self.getAll = function() {
		return mgr.getApps();
	};

	self.getAllRunning = function() {
		return mgr.getAllRunning();
	};

	self.isRunning = function( id ) {
		return mgr.isRunning( id );
	};

	self.getShell = function() {
		return mgr.getShell();
	};

	self.getWebApps = function() {
		return mgr.getWebApps();
	};

	self.getCategories = function() {
		var result = ['FAVORITOS'];
		mgr.getApps().forEach(function(appID) {
			var app = mgr.getApp( appID );
			for (var i=0; i<app.categories.length; i++) {
				var upper = app.categories[i].toUpperCase();
				if (result.indexOf( upper ) === -1) {
					result.push( upper );
				}
			}
		});
		return result;
	};

	self.toggleFavorite = function( id ) {
		mgr.toggleFavorite( id );
	};

	self.isFavorite = function( id ) {
		return mgr.isFavorite( id );
	};

	self.showVirtualKeyboard = function( appId, vkCfg, cb ) {
		mgr.showVirtualKeyboard(appId, vkCfg, cb);
	};
	self.showVirtualKeyboard.isAsync = true;
	self.showVirtualKeyboard.insertId = true;

	self.dump = function() {
		var state = {
			shell: self.getShell(),
			webApp: self.getWebApps(),
			apps: {}
		};
		self.getAllRunning().reduce(function(apps,appID) {
			apps[appID] = self.get(appID);
			return apps;
		}, state.apps );

		return state;
	};

	self.configureService = function(srvId,cb) {
		assert(cb);
		return mgr.runServiceCfg(srvId)
			.then(function() {
				cb();
			}, cb);
	};
	self.configureService.isAsync = true;

	self.getWebAPI = function() {
		return {
			'name': 'appmgr',
			'public': [
				'on',
				'get',
				'getAll',
				'getAllRunning',
				'isRunning',
				'getShell',
				'getWebApps',
				'getCategories',
				'isFavorite',
				'showVirtualKeyboard',
				'exit'
			],
			'private': [
				'run',
				'stop',
				'toggleFavorite',
				'configureService'
			]
		};
	};

	return Object.freeze(self);
}

//	Service entry point
function ApplicationMgr( adapter ) {
	var _mgr = new ApplicationMgrImpl( adapter );

	//	Hooks
	var self = {};

	self.onStart = function(cb) {
		_mgr.start(cb);
	};

	self.onStop = function(cb) {
		_mgr.stop(cb);
	};

	return Object.freeze(self);
}

module.exports = ApplicationMgr;

