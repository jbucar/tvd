'use strict';

var path = require('path');
var assert = require('assert');
var Application = require('./application');

var modeDevFatality = [ 'VK_RED', 'VK_3', 'VK_3', 'VK_2', 'VK_8', 'VK_4' ];

function instanceName( name, id ) {
	return name + '.' + id;
}

function loadShell( appmgr, instanceID ) {
	var shellID = appmgr.getShell();
	var app = appmgr.loadApp( shellID, instanceName(shellID,instanceID) );
	app._restart = true;
	app._canExit = false;
	return app;
}

function createShellApp( reg, appmgr ) {
	log.silly( 'SystemLayer', 'Create shell application' );

	//	Load shell
	var app = loadShell( appmgr, 'shell' );
	app._zIndex = 0;
	app._checkMode = -1;

	//	Handle onKey browser event
	app.onKey = function( evt ) {
		if (evt.isUp) {
			//	Handle change of development mode in shell
			if (!global.inDevelopmentMode) {
				app._checkMode++;
				log.silly('SystemLayer', 'Check dev mode fatality: keyEvent=%d', app._checkMode );
				if (modeDevFatality[app._checkMode] !== evt.code) {
					app._checkMode = -1;
				}
				else if (app._checkMode === modeDevFatality.length-1) {
					reg.get('system').enableDevelopmentMode();
				}
			}
		}
		Application.prototype.onKey.call(this, evt);
	};

	return app;
}

function createNotificationApp( appmgr ) {
	log.silly( 'SystemLayer', 'Create system layer application' );

	//	Load notification
	var app = loadShell( appmgr, 'system_layer' );

	//	Change to notification.html
	var toFind = path.basename( app.info.main );
	app.info.main = app.info.main.replace( toFind, 'notification.html' );

	app._zIndex = 10;
	app._focus = false;
	app.info.dependencies = ["ar.edu.unlp.info.lifia.tvd.notification"];

	return app;
}

function SystemLayer( reg, appmgr ) {
	assert(reg);

	try {
		var _shellApp = createShellApp(reg,appmgr);
		var _layerApp = createNotificationApp(appmgr);
	} catch(err) {
		log.error( 'SystemLayer', 'Cannot create shell/layer applications' );
		throw new RecoveryError('Cannot start shell dependencies');
	}

	function onLayerStatusChanged(evt) {
		if (evt.state === 'loaded' && !_shellApp.isRunning()) {
			log.verbose('SystemLayer', 'Notification layer loaded');
			//	Once the system layer is loaded start the shell
			appmgr.startApp({ id: _shellApp.info.id })
				.catch(function() {
					throw new RecoveryError('Cannot start shell');
				});
		}
	}

	var self = this || {};

	self.start = function() {
		log.info('SystemLayer', 'Start system layer' );
		_layerApp.on( 'changed', onLayerStatusChanged );
		_layerApp.run();
	};

	self.show = function(val, focus) {
		assert(_layerApp);
		_layerApp.show(val, focus);
	};

	self.getShellID = function() {
		return instanceName(appmgr.getShell(),'shell');
	};

	return self;
}

module.exports = SystemLayer;

