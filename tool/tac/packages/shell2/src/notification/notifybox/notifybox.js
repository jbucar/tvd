/* globals PopupBox */
/* exported NotifyBox */
function NotifyBox(res) {

	function showNotify(ractive, msg, icon) {
		if (ractive.showTimeout) {
			clearTimeout(ractive.showTimeout);
			ractive.showTimeout = null;
		} else {
			ractive.parent.showLayer( true );
		}

		ractive.set({
			notifyContent: msg,
			notifyIcon: (icon.indexOf('/') === -1) ? res.img('icons/' + icon) : icon,
			showNotify: true,
		});

		ractive.showTimeout = setTimeout(() => {
			ractive.showTimeout = null;
			ractive.set('showNotify', false);
			// Wait for the animation to finish before hiding the system layer
			setTimeout(() => ractive.parent.showLayer(false), ractive.get('notifyAnimDelay'));
		}, 3000);
	}

	function registerNotifications(ractive) {
		// Geolocalization status
		registerNotification( 'geoEnabled', enabled => showNotify(ractive, 'GEOLOCALIZACIÓN ' + (enabled ? 'HABILITADA' : 'DESHABILITADA'), 'geo') );

		// Network status
		registerNotification( 'ar.edu.unlp.info.lifia.tvd.network', 'connection', 'connectionChanged', function(evt) {
			if (evt.type === 'state') {
				switch(evt.state) {
					case 'connected':
						showNotify(ractive, 'CONECTADO A RED','check');
						break;
					case 'connecting':
						showNotify(ractive, 'CONECTANDO A RED', 'loading');
						break;
					case 'disconnecting':
						showNotify(ractive, 'DESCONECTANDO RED', 'loading');
						break;
					case 'disconnected':
						showNotify(ractive, 'RED DESCONECTADA', 'check');
						break;
					case 'error':
						showNotify(ractive, 'FALLÓ LA CONEXIÓN A RED', 'error');
						break;
				}
			}
		});

		// USB mount
		registerNotification( 'ar.edu.unlp.info.lifia.tvd.diskmgr', 'mountChanged', 'mountChanged', function(mountInfo) {
			log.verbose( 'notification', 'mountChanged: mountInfo=%j', mountInfo );
			var mounted = mountInfo.isMounted ? 'ENCONTRADO' : 'DESCONECTADO';
			showNotify(ractive, 'DISPOSITIVO ' + mounted, 'usb');
		});

		// Media error
		registerNotification( 'ar.edu.unlp.info.lifia.tvd.media.main', 'error', 'onMediaError', function(err) {
			log.verbose( 'notification', 'Media error: err=%s', err.message );
			showNotify(ractive, 'MEDIA ERROR ' + err.message, 'error');
		});

		// Development mode
		registerNotification( 'ar.edu.unlp.info.lifia.tvd.system', 'DevelopmentModeEnabled', 'onDevelopmentModeEnabled', function(enable) {
			log.verbose( 'notification', 'onDevelopmentModeEnabled: enable=%d', enable );
			showNotify(ractive, (enable ? 'AHORA SOS' : 'HAS SALIDO DEL MODO') + ' PROGRAMADOR', 'check');
		});

		// Package manager
		registerNotification( 'ar.edu.unlp.info.lifia.tvd.pkgmgr', 'install', 'onPackageInstalled', function(evt) {
			log.verbose( 'notification', 'onPackageInstalled: evt=%j', evt );
			showNotify(ractive, 'APLICACIÓN INSTALADA', 'check');
		});
		registerNotification( 'ar.edu.unlp.info.lifia.tvd.pkgmgr', 'uninstall', 'onPackageUninstalled', function(evt) {
			log.verbose( 'notification', 'onPackageUninstalled: evt=%j', evt );
			showNotify(ractive, 'APLICACIÓN DESINSTALADA', 'check');
		});
		registerNotification( 'ar.edu.unlp.info.lifia.tvd.pkgmgr', 'updated', 'onPackageUpdated', function(id) {
			log.verbose( 'notification', 'onPackageUpdated: id=%s', id );
			showNotify(ractive, id.toUpperCase() + ' ACTUALIZADA', 'check');
		});

		// Firmware update
		registerNotification( 'ar.edu.unlp.info.lifia.tvd.update', 'Firmware', 'onFirmwareUpdate', function( evt ) {
			log.verbose( 'notification', 'onFirmwareUpdate: evt=%j', evt );
			switch(evt.status){
				case 'available':
					showNotify(ractive, 'ACTUALIZACIÓN DISPONIBLE', 'check');
					break;
				case 'downloaded':
					showNotify(ractive, 'DESCARGA FINALIZADA', 'download');
					break;
				case 'error':
					showNotify(ractive, 'ERROR EN LA ACTUALIZACIÓN', 'error');
					break;
				}
		});

		notification.on( 'ApplicationNotification', function( evt ) {
			log.verbose( 'notification', 'ApplicationNotification: evt=%j', evt );
			showNotify( ractive, evt.msg, evt.icon );
		});
	}

	let prot = {
		template: res.tpl('notifybox'),
		css: res.style('notifybox'),
		components: ui.getComponents(PopupBox),
	};

	prot.onconstruct = function(opts) {
		log.verbose( 'NotifyBox', 'onconstruct: opts=%j', opts );
		opts.data.showNotify = false;
		opts.data.notifyAnimDelay = 1500;
	};

	prot.oninit = function() {
		log.info( 'NotifyBox', 'oninit' );
		registerNotifications(this);
	};

	return {
		proto: prot,
	};
}
