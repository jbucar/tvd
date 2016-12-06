/* globals EventEmitter */
/* exported ApplicationModel */

function ApplicationModel() {
	var self = {};
	var _apps = [];
	var _events = new EventEmitter();

	function onAppInstanstalled( appID ) {
		log.info( 'ApplicationModel', 'onAppInstanstalled: appID=%s', appID );
		apiCallAsync(appmgr.get,appID)
			.then( (appInfo) => _apps.push( appInfo ) );
	}

	function onAppUninstanstalled( appID ) {
		log.info( 'ApplicationModel', 'onAppUninstanstalled: appID=%s', appID );
		let index = _apps.findIndex( (app) => app.id === appID );
		if (index >= 0) {
			_apps.splice(index,1);
		}
	}

	function changed() {
		_events.emit( 'onChanged' );
	}

	function onKeyDown(code) {
		if (code === tacKeyboardLayout.VK_RECALL_FAVORITE_0) {
			apiCall( appmgr.run, 'ar.gob.minplan.tac.cda' );
			return false;
		}
		return true;
	}

	self.init = function() {
		log.info( 'ApplicationModel', 'Init' );

		ui.registerKeyHandler( onKeyDown );

		//	Get all apps
		return apiCallAsync( appmgr.getAll )
			.map( appID => apiCallAsync(appmgr.get,appID) )
			.then(function(apps) {
				apps.forEach(function (app) {
					if (!app.icon) {
						app.icon = ui.getResource('images','shell/default_app_icon');
					}
					_apps.push( app );
				});
				appmgr.on('install', onAppInstanstalled );
				appmgr.on('uninstall', onAppUninstanstalled );
				changed();
			});
	};

	self.fin = function() {
		ui.unregisterKeyHandler( onKeyDown );
		// TODO:
		// appmgr.removeListener( 'install', onAppInstanstalled );
		// appmgr.removeListener( 'uninstall', onAppUninstanstalled );
	};

	forwardEventEmitter( self, _events );

	self.getAll = function() {
		return _apps;
	};

	self.getApp = function(appID) {
		return _apps.find( (app) => app.id === appID );
	};

	self.toogleFavorite = function( appID ) {
		log.verbose( 'ApplicationModel', 'toggleFav: appID=%s', appID );
		let app = self.getApp(appID);
		if (app) {
			return apiCallAsync( appmgr.toggleFavorite, appID )
				.then(function() {
					app.isFavorite = !app.isFavorite;
					changed();
				});
		}
		return Promise.reject( new Error('Invalid appID') );
	};

	self.uninstall = function( appID ) {
		log.verbose( 'ApplicationModel', 'uninstall: appID=%s', appID );
		let app = self.getApp(appID);
		if (app) {
			apiCall( pkgmgr.uninstall, {id: app.pkgID} );
			_apps.splice(_apps.indexOf(app), 1);
			changed();
		}
	};

	return self;
}
