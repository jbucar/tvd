enyo.kind({
	name: 'Splash',
	components: [
		{
			kind: 'enyo.Image',
			src: assets('splashbg.jpg'),
			sizing: 'cover',
			classes: 'enyo-fit splash',
			components: [
				{
					classes: 'wrapper',
					components: [
						{
							kind: 'enyo.Image',
							classes: 'img',
							src: assets('logo_preload.png')
						},
						{ content: 'Cargando...', classes: 'txt' },
						{ kind: 'Shell.BallPreloader' }
					]
				}
			]
		}
	]
});

/*
 * Shell.Notification kind.
*/
enyo.kind({
	name: 'Shell.Notification',
	classes: 'enyo-fit',
	handlers: {
		'onVanish': 'onVanish'
	},
	components: [
		{ name: 'splash', kind: 'Splash', showing: false },
	],
	tools: [
		{ name: 'notifications', kind: 'NotificationStack' },
		{
			name: 'keyboard',
			kind: 'VirtualKeyboardPopup',
			onAccept: 'onInputDone',
			onCancel: 'onInputDone'
		},
		{
			name: 'volume',
			kind: 'Volume',
			showing: false,
		},
		{
			name: 'subtitle',
			kind: 'ClosedCaption',
			showing: false,
		},
	],
	_needShowLayer: 0,
	_hasFocus: false,
	_reqFrameId: null,
	enabled: true,
	create: function() {
		this.inherited(arguments);
		var self = this;

		// Create tools
		self.createChrome(self.tools);

		// Virtual Keyboard.
		notification.on('VirtualKeyboard', this.bindSafely('onVirtualKeyboard'));

		// Volume.
		notification.forwardEvents( 'ar.edu.unlp.info.lifia.tvd.audio', 'changed', 'onMastVolumeChanged' );
		notification.on('onMastVolumeChanged', function(evt) {
			// Sets volume value or sets mute on/off.
			self.$.volume.setVolume(evt.volume);
			self.$.volume.setMute(evt.isMuted);

			// Shows volume widget.
			if (!self.$.volume.showing) {
				self.showLayer();
			}
			self.$.volume.show();
		});

		// Notification status.
		notification.on('NotificationStatus', function(evt) {
			self.enabled = evt.enabled;
			if (!self.enabled) {
				self.$.notifications.destroyClientControls();
			}
		});

		// Apps notifications.
		notification.forwardEvents( 'ar.edu.unlp.info.lifia.tvd.appmgr', 'application', 'onApplicationStateChanged' );
		notification.on('onApplicationStateChanged', function(evt) {
			switch (evt.state) {
				case 'starting':
					self.$.splash.show();
					self.showLayer();
					// NOTE: Postpone to the next frame so enyo can show its widgets.
					enyo.requestAnimationFrame(function() {
					enyo.requestAnimationFrame(function() {
						notification.emit('ShowApp', evt.id);
					});});
					break;
				case 'loaded':
					if (evt.id != 'ar.edu.unlp.info.lifia.tvd.shell.system_layer') {
						self.$.splash.hide();
						self.hideLayer();
					}
					break;
				case 'stopped':
					// App loading canceled or app fail to load
					if (self.$.splash.showing) {
						if (evt.error) {
							self.showNotify({
								icon: NotificationBox.icons.error,
								group: 'APPLICATION_GROUP_ID',
								content: 'ERROR EN LA APLICACIÓN',
								timeout: 3000,
							});
						}
						self.$.splash.hide();
						self.hideLayer();
					}
					break;
			}
		});

		notification.forwardEvents( 'ar.edu.unlp.info.lifia.tvd.pkgmgr', 'install', 'onPackageInstalled' );
		notification.on('onPackageInstalled', function() {
			self.showNotify({
				icon: NotificationBox.icons.check,
				content: 'APLICACIÓN INSTALADA',
				timeout: 3000,
			});
		});

		notification.forwardEvents( 'ar.edu.unlp.info.lifia.tvd.pkgmgr', 'updated', 'onPackageUpdated' );
		notification.on('onPackageUpdated', function(id) {
			self.showNotify({
				icon: NotificationBox.icons.check,
				content: id.toUpperCase() + ' ACTUALIZADA',
				sizeFixed: false,
				timeout: 3000,
			});
		});

		notification.on('uninstallApp', function(evt) {
			if (evt.state === 'uninstalled') {
				self.showNotify({
					content: 'APLICACIÓN DESINSTALADA',
					group: 'APPLICATION_GROUP_ID',
					replace: true,
					timeout: 3000,
					icon: NotificationBox.icons.check
				});
			}
		});

		notification.on('favoriteApp', function(isFavorite) {
			self.showNotify({
				group: 'APPLICATION_GROUP_ID',
				replace: true,
				content: isFavorite ? 'AGREGADO A FAVORITOS' : 'QUITADO DE FAVORITOS',
				icon: NotificationBox.icons.favorite,
				timeout: 3000,
			});
		});

		// Channels notifications.
		notification.on('favoriteChannel', function(isFavorite) {
			self.showNotify({
				group: 'CHANNEL_GROUP_ID',
				content: isFavorite? 'AGREGADO A FAVORITOS' : 'QUITADO DE FAVORITOS',
				replace: true,
				icon: isFavorite ? NotificationBox.icons.favorite : NotificationBox.icons.unfavorite,
				timeout: 3000,
			});
		});

		// Channels scan.
		notification.forwardEvents( 'ar.edu.unlp.info.lifia.tvd.tvd', 'scanChanged', 'onTvScanChanged' );
		notification.on('onTvScanChanged', function(evt) {
			var notification = {
				group: 'SCAN_CHANGED_GROUP_ID',
				replace: true,
				icon: NotificationBox.icons.search
			};

			if (evt.state === 'network') {
				notification.content = 'BUSCANDO CANALES...({0}Mhz)'.format(evt.network);
				self.showNotify(notification);
			}
			else if (evt.state === 'end') {
				notification.timeout = 6000;
				notification.content = evt.count + ' CANALES ENCONTRADOS';
				notification.icon = NotificationBox.icons.channels;
				self.showNotify(notification);
			}
		});

		notification.on('ShellError', function(evt) {
			self.showNotify({
				group: 'SHELL_ERROR_GROUP_ID',
				replace: true,
				content: 'ERROR: ' + evt.description.toUpperCase(),
				icon: NotificationBox.icons.error,
				timeout: 10000,
				sizeFixed: false
			});
		});

		// Block channel.
		notification.on('blockedChannel', function(evt) {
			self.showNotify({
				group: 'CHANNEL_BLOCKED_GROUP_ID',
				replace: true,
				content: evt.isBlocked? 'CANAL BLOQUEADO' : 'CANAL DESBLOQUEADO',
				icon: evt.isBlocked? NotificationBox.icons.locked : NotificationBox.icons.unlocked,
				timeout: 3000,
			});
		});

		// Session.
		notification.on('session', function(evt) {
			var not = {
				icon: NotificationBox.icons.parental,
				group: 'PARENTAL_GROUP_ID',
				replace: true,
				timeout: 3000,
			};

			switch(evt.property){
				case 'pass':
					not.content = 'CONTRASEÑA MODIFICADA';
					break;
				case 'delete':
					not.content = 'CONTRASEÑA ELIMINADA';
					break;
				case 'sessionOpen':
					not.group = 'PARENTAL_GROUP_STATUS_ID';
					not.content = 'SESIÓN INICIADA';
					break;
				case 'sessionClosed':
					not.group = 'PARENTAL_GROUP_STATUS_ID';
					not.content = 'SESIÓN FINALIZADA';
					break;
				default:
					not.content = 'RESTRICCIONES MODIFICADAS';
					break;
			}

			self.showNotify(not);
		});

		// Updates.
		notification.forwardEvents( 'ar.edu.unlp.info.lifia.tvd.update', 'Firmware', 'onCheckFirmwareChanged' );
		notification.on('onCheckFirmwareChanged', function (evt) {
			var noti = {
				icon: NotificationBox.icons.search,
				group: 'UPDATE_GROUP_ID',
			};

			switch(evt.status){
				case 'available':
					noti.content = 'ACTUALIZACIÓN DISPONIBLE';
					noti.timeout = 3000;
					noti.replace = true;
					noti.icon = NotificationBox.icons.check;
					break;
				case 'downloaded':
					noti.content = 'DESCARGA FINALIZADA';
					noti.icon = NotificationBox.icons.downloaded;
					noti.timeout = 3000;
					break;
				case 'error':
					noti.content = 'ERROR EN LA ACTUALIZACIÓN';
					noti.timeout = 5000;
					noti.icon = NotificationBox.icons.error;
					break;
				default:
					noti = undefined;
					break;
			}

			if (noti) {
				self.showNotify(noti);
			}
		});

		notification.on('InternetConnection', function(evt) {
			var noti = {
				icon: assets('notification_search.png'),
				group: 'INET_GROUP_ID',
				timeout: 3000,
			};

			switch(evt.name){
				case 'fetch':
					noti.replace = true;
					if (evt.status === 'begin') {
						noti.content = 'BUSCANDO REDES';
						noti.icon = NotificationBox.icons.loading;
					}
					else if (evt.status === 'end') {
						noti.content = evt.result === 0? 'NO HAY REDES DISPONIBLES' : evt.result + ' REDES ENCONTRADAS';
						noti.icon = (evt.result === 0)? NotificationBox.icons.error : NotificationBox.icons.check;
					}
					else if (evt.status === 'error') {
						noti.content = 'ERROR BUSCANDO REDES';
						noti.icon = NotificationBox.icons.error;
					}
					break;
				case 'connecting':
					noti.timeout = undefined; // erase timeout
					noti.content = 'CONECTANDO A RED';
					noti.icon = NotificationBox.icons.loading;
					break;
				case 'connected':
					noti.discardOnOverlap = true;
					noti.group += '_CONNECTION';
					noti.content = 'CONECTADO A RED';
					noti.icon = NotificationBox.icons.check;
					break;
				case 'disconnected':
					noti.content = 'RED DESCONECTADA';
					noti.icon = NotificationBox.icons.check;
					break;
				case 'noConnectivity':
					noti.content = 'SIN ACCESO A INTERNET';
					noti.icon = NotificationBox.icons.error;
					break;
				case 'error':
					noti.content = 'FALLÓ LA CONEXIÓN A RED';
					noti.icon = NotificationBox.icons.error;
					break;
			}

			self.showNotify(noti);
		});

		// Closed Caption.
		notification.on('CC', function(evt) {
			self.$.subtitle.setCC( evt.id );
			self.$.subtitle.show();
			self.showLayer();
		});

		// Development Mode Enabled
		notification.forwardEvents( 'ar.edu.unlp.info.lifia.tvd.system', 'DevelopmentModeEnabled', 'onDevelopmentModeEnabled' );
		notification.on('onDevelopmentModeEnabled', function(enable) {
			self.showNotify({
				content: enable? 'AHORA SOS PROGRAMADOR' : 'HAS SALIDO DEL MODO PROGRAMADOR',
				icon: NotificationBox.icons.check,
				group: 'SYSTEM_GROUP_ID',
				timeout: 3000
			});
		});

		// Ginga notifications.
		notification.on('gingaAvailable', function(appsAvailable) {
			self.showNotify({
				group: 'GINGA',
				content: 'PROGRAMACIÓN INTERACTIVA <br> Presione botón rojo',
				replace: true,
				sizeFixed: false,
				icon: NotificationBox.icons.ginga,
				timeout: 3000,
			});
		});

		notification.on('gingaEnabled', function(enabled) {
			var msg = enabled ? 'HABILITADO' : 'DESHABILITADO';
			self.showNotify({
				group: 'GINGA',
				content: 'GINGA.AR ' + msg,
				replace: true,
				sizeFixed: true,
				icon: NotificationBox.icons.ginga,
				timeout: 3000,
			});
		});

		// USB notifications.
		notification.forwardEvents( 'ar.edu.unlp.info.lifia.tvd.diskmgr', 'mountChanged', 'mountChanged' );
		notification.on('mountChanged', function(mountInfo) {
			var mounted = mountInfo.isMounted ? 'ENCONTRADO' : 'DESCONECTADO';
			var notification = {
				group: 'DISKMGR',
				content: 'DISPOSITIVO ' + mounted,
				replace: true,
				icon: NotificationBox.icons.usb,
				timeout: 3000,
			};
			self.showNotify(notification);
		});

		// GeoLocal notifications.
		notification.on('geolocalEnabled', function(enabled) {
			var msg = enabled ? 'HABILITADA' : 'DESHABILITADA';
			self.showNotify({
				group: 'LOCALIZACION',
				content: 'LOCALIZACION ' + msg,
				replace: true,
				sizeFixed: true,
				icon: NotificationBox.icons.geolocal,
				timeout: 3000,
			});
		});
	},
	onVanish: function() {
		this.hideLayer();
	},
	onVirtualKeyboard: function(evt, opts) {
		switch (evt) {
			case 'show':
				this.$.keyboard.show(opts);
				enyo.Spotlight.spot(this.$.keyboard);
				this.showLayer(true);
				break;
			case 'hide':
				this.$.keyboard.hide();
				this.hideLayer(true);
				break;
		}
	},
	onInputDone: function(oSender, oEvent) {
		notification.emit('VirtualKeyboardInput', {
			status: oEvent.type === 'onAccept' ? 'accepted' : 'cancelled',
			input: oEvent.value
		});
		this.onVirtualKeyboard('hide');
	},
	showLayer: function(needFocus) {
		needFocus = needFocus === true;
		if ((this._needShowLayer++ === 0) || (needFocus && !this._hasFocus)) {
			this._hasFocus = needFocus;
			notification.emit('ShowSystemLayer', true, needFocus);
			if (this._reqFrameId) {
				enyo.cancelRequestAnimationFrame(this._reqFrameId);
				this._reqFrameId = null;
			}
		}
	},
	hideLayer: function(removeFocus) {
		if (--this._needShowLayer <= 0) {
			this._needShowLayer = 0;
			this._hasFocus = false;

			// NOTE: Postpone to the next frame so enyo can hide its widgets.
			// This is because requestAnimationFrame callbacks are not called
			// once the chromium aura::Window is hidden until it is shown again.
			var self = this;
			function enqueAnimationFrame(cb) {
				self._reqFrameId = enyo.requestAnimationFrame(function() {
					self._reqFrameId = enyo.requestAnimationFrame(cb);
				});
			}
			enqueAnimationFrame( function() {
				notification.emit('ShowSystemLayer', false, false);
				self._reqFrameId = null;
			});
		}
		else if (removeFocus === true && this._hasFocus) {
			// If don't need keyboard focus anymore but cannot hide because other widgets
			// are being displayed => show the layer again but without keyboard focus
			this._hasFocus = false;
			notification.emit('ShowSystemLayer', true, false);
		}
	},
	showNotify: function(c) {
		if (this.enabled) {
			this.$.notifications.stack(
				c,
				{owner: this.getInstanceOwner(), kind: 'NotificationBox'},
				this.showLayer.bind(this)
			);
		}
	},
});
