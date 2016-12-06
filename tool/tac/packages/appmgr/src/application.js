'use strict';

var assert = require('assert');
var _ = require('lodash');
var util = require('util');
var bPromise = require("bluebird");
var tvdutil = require('tvdutil');
var Component = require('component');
var EventEmitter = require('events').EventEmitter;

var states = { 'stopped': 0, 'stopping': 1, 'starting': 2, 'launching': 3, 'launched': 4, 'loaded': 5 };
var states_r = _.invert(states);	//	Do no touch this

function OnBrowserEvent( evt ) {
	if (this._status === states.stopped) {
		log.warn('Application', 'Browser event in invalid state: state=%s', states_r[this.status] );
		return;
	}

	assert(evt.name);
	//log.silly('Application', 'On browser event: evt=%s', evt.name );

	switch (evt.name) {
	case 'onKey':
		this.onKey(evt);
		break;
	case 'onAPI':
		this.onAPI(evt);
		break;
	case 'onClose':
		this.stopApplication(evt.error, false);
		break;
	case 'onLaunched':
		this.setStatus( 'launched' );
		break;
	case 'onLoaded':
		this.setStatus( 'loaded' );
		break;
	case 'onWebLog':
		this.onWebLog(evt);
		break;
	default:
		log.warn('Application', 'App %s received unknown event: %j', this.info.id, evt);
	}
}

function OnVkInput( evt ) {
	log.verbose( 'Application', 'On virtual keyboard input: %j', evt );
	if (this.hasKeyboardOpen()) {
		this._onVkInputCb(undefined, evt);
		this._onVkInputCb = null;
	} else {
		log.error( 'Application', 'Received virtual keyboard input but callback was not setted' );
	}
}

function getPermission( app ) {
	let sec = {
		all: app.info.system,
	};

	if (app._cfgService) {
		sec.api = [app._cfgService];
	}

	return sec;
}

function Application( reg, id ) {
	//	Call to Component constuctor
	Application.super_.call(this,reg,id);

	//	Check property type
	if (this.info.type != 'app') {
		throw new Error( 'Property type is not application' );
	}

	//	Get package manager
	var pkgMgr = reg.get('pkgmgr');
	var info = pkgMgr.getComponentInfo( id );

	//	Check main
	this.info.main = this.resolveResource(pkgMgr,this.info,'main');
	this.info.bg_transparent = info.bg_transparent === true;
	this.info.enable_mouse = info.enable_mouse === true;
	this.info.enable_geolocation = info.enable_geolocation === true;
	this.info.hw_codecs_only = info.hw_codecs_only === true;
	this.info.extra_ua = info.extra_ua;
	this.info.plugins = _.isArray(info.plugins) ? _.cloneDeep( info.plugins ) : [];

	//	Process categories: Always upper case ....
	this.info.categories = [];
	if (info.categories) {
		for (var i=0; i<info.categories.length; i++) {
			this.info.categories.push( info.categories[i].toUpperCase() );
		}
	}

	this._status = 0;
	this._browserID = null;
	this._signalUnregs = [];
	this._events = new EventEmitter();
	this._zIndex = 5; // Apps zIndex by default
	this._restart = false;
	this._canExit = true;
	this._focus = true;
	this._hasFocus = false;

	// Virtual keyboard settings:
	this._vkCfg = {}; // VK settings (title, bounds, background, etc)
	this._onVkInput = _.bind(OnVkInput,this); // Called when user accepts or cancel the VK input
	this._onVkInputCb = null; // Callback for sending the user input back to the application

	tvdutil.forwardEventEmitter(this,this._events);
}

util.inherits(Application, Component);

Application.prototype.onAPI = function( evt ) {
	evt.unregisterSignal = _.bind(this.addSignalUnregister, this);
	evt.appID = this.info.id;
	evt.security = getPermission(this);
	this._registry.runAPI(evt);
};

Application.prototype.onWebLog = function( evt ) {
	var index = this.info.id.indexOf( 'ar.edu.unlp.info.lifia.tvd' );
	var id = (index === 0) ? this.info.id.substring( this.info.id.lastIndexOf('.')+1 ) : this.info.id;
	switch (evt.logData.level) {
	case 2:
		log.error(id, '%s, (%s : %d)', evt.logData.message, evt.logData.source, evt.logData.line);
		break;
	case 1:
		log.warn(id, '%s, (%s : %d)', evt.logData.message, evt.logData.source, evt.logData.line);
		break;
	case 0:
		log.verbose(id, evt.logData.message);
		break;
	case -1:
		log.silly(id, evt.logData.message);
		break;
	}
};

Application.prototype.onKey = function( evt ) {
	if (evt.isUp) {
		//	see $DEPOT/tool/tac/packages/browser/keyboardlayout_[pc|aml].json

		//	Handle key
		switch (evt.code) {
		case 'VK_EXIT':
			if (this._canExit) {
				this.stop();
			}
			break;
		case 'VK_MUTE':
			this._registry.get('audio').toggleMute();
			break;
		case 'VK_VOLUME_DOWN':
			this._registry.get('audio').volumeDown();
			break;
		case 'VK_VOLUME_UP':
			this._registry.get('audio').volumeUp();
			break;
		case 'VK_POWER':
			this._registry.get('system').powerOff();
			break;
		}
	}
};

Application.prototype.setStatus = function( state, extra ) {
	var st = states[state];
	log.silly( 'Application', 'Set status: id=%s, old=%s, new=%s', this.info.id, this._status, state );
	if (this._status != st) {
		this._status = st;
		var evt = { state: state, id: this.info.id };
		this._events.emit( 'changed', _.merge(evt,extra) );
	}
};

Application.prototype.getStatus = function() {
	return states_r[this._status];
};

Application.prototype.isRunning = function() {
	return this._status > states.stopping;
};

Application.prototype.isStopped = function() {
	return this._status === states.stopped;
};

Application.prototype.runApplication = function() {
	log.verbose( 'Application', 'Launch application: id=%s', this.info.id );

	//	Launch browser
	var opts = {
		apis: this._registry.getWebApis( this.dependencies(), getPermission(this) ),
		bgTransparent: this.info.bg_transparent,
		zIndex: this._zIndex,
		focus: this._focus,
		enableMouse: this.info.enable_mouse,
		enableGeolocation: this.info.enable_geolocation && this._registry.get('system').isGeolocationEnabled(),
		hwCodecsOnly: this.info.hw_codecs_only,
		extraUA: this.info.extra_ua,
		plugins: _.cloneDeep( this.info.plugins ),
		visible: false
	};

	//	Setup URL
	var url = this.info.main;
	if (this.parameters) {
		url += '?' + this.parameters;
	}

	//	Launch browser
	this._browserID = this._registry.get('browser').launchBrowser( url, opts, _.bind(OnBrowserEvent,this) );
	if (this._browserID === -1) {
		this.setStatus( 'stopping' );
		this.stopApplication( 'Launch browser error', true );
	}
	else {
		this.setStatus( 'launching' );
	}

	return (this._browserID !== -1);
};

Application.prototype.run = function(params) {
	log.verbose( 'Application', 'Run application: id=%s', this.info.id );
	assert(this._status === 0);

	this.setStatus( 'starting' );

	//	Run dependencies
	var self=this;
	return this.runDependencies()
		.then(function() {
			self.parameters = params;
			if (!self.runApplication()) {
				throw new Error( "Cannot launch browser" );
			}
		},function(err) {
			self.setStatus( 'stopped' );
			throw err;
		});
};

Application.prototype.stopApplication = function( err, isFatal ) {
	log.silly( 'Application', 'Stop application recieved: id=%s, err=%s', this.info.id, err );
	this.unregisterAllSignals();
	if (err && this._restart && this._registry.get('appmgr').isReady() && !isFatal) {
		log.warn( 'Application', 'Closed with error; restart!: id=%s', this.info.id );
		this.runApplication();
	}
	else {
		var app = this;
		this.stopDependencies().then(function() {
			var extra = {};
			if (err) {
				extra.error = err;
			}

			// Reset VK settings
			app._vkCfg = {};
			app._onVkInputCb = null;

			app.setStatus( 'stopped', extra );
		});
	}
};

Application.prototype.stop = function() {
	var app=this;
	return new bPromise(function(resolve) {
		if (app._status < states.stopping) {
			log.silly( 'Application', 'Cannot stop application; already stopped' );
			resolve();
			return;
		}

		if (app._status > states.stopping) {
			log.verbose( 'Application', 'Stop application: id=%s', app.info.id );
			app.setStatus( 'stopping' );
			app._registry.get('browser').closeBrowser( app._browserID );
		}

		log.silly( 'Application', 'Wait application stopped: id=%s', app.info.id );

		//	Wait for exit
		var waitForExit = function(evt) {
			if (evt.state === 'stopped') {
				app.removeListener( 'changed', waitForExit );
				resolve();
			}
		};
		app.on( 'changed', waitForExit );
	});
};

Application.prototype.show = function(needShow, needFocus) {
	if (this.isRunning()) {
		log.verbose('Application', 'Show application: needShow=%d', needShow );
		this._registry.get('browser').showBrowser(this._browserID, needShow, needFocus);
	}
	else {
		log.warn('Application', 'Cannot show application; invalid status: status=%s', states_r[this._status] );
	}
};

Application.prototype.addSignalUnregister = function( unregister ) {
	if (this.isRunning()) {
		this._signalUnregs.push(unregister);
	}
	else {
		log.warn('Application', 'Cannot register signal; invalid status: status=%s', states_r[this._status] );
	}
};

Application.prototype.unregisterAllSignals = function() {
	this._signalUnregs.forEach(function (unregister) {
		unregister();
	});
	this._signalUnregs = [];
};

Application.prototype.showVirtualKeyboard = function( cfg, cb ) {
	log.verbose('Application', 'Show virtual keyboard! cfg=%j', cfg);
	if (this.hasKeyboardOpen()) {
		log.warn( 'ApplicationManager', 'Fail to show virtual keyboard, a VK instance is already showing' );
		cb(new Error('A virtual keyboard instance is already showing'));
	} else {
		this._vkCfg = cfg;
		this._onVkInputCb = cb;
		if (this._hasFocus) {
			// This is the current active application => allow showing keyboard
			var notif = this._registry.get('notification');
			notif.once('VirtualKeyboardInput', this._onVkInput);
			notif.emit('VirtualKeyboard', 'show', cfg);
		}
	}
};

Application.prototype.hasKeyboardOpen = function() {
	return this._onVkInputCb !== null;
};

Application.prototype.hasFocus = function() {
	return this._hasFocus;
};

Application.prototype.onFocus = function() {
	log.verbose( this.info.id, 'On focus' );

	assert( !this._hasFocus );
	this._hasFocus = true;

	if (this.hasKeyboardOpen()) {
		// Se recupero el foco y previamente se estaba mostrando el VK => volver a mostrar
		var cb = this._onVkInputCb;
		this._onVkInputCb = null;
		this.showVirtualKeyboard(this._vkCfg, cb);
	}
};

Application.prototype.onBlur = function() {
	log.verbose( this.info.id, 'On blur' );

	assert( this._hasFocus );
	this._hasFocus = false;

	if (this.hasKeyboardOpen()) {
		// Ocultar el VK (Se volvera a mostrar cuando la aplicacion recupera el foco)
		var notif = this._registry.get('notification');
		notif.removeListener('VirtualKeyboardInput', this._onVkInput);
		notif.emit('VirtualKeyboard', 'hide');
	}
};

module.exports = Application;
