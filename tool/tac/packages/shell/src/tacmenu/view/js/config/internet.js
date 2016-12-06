
(function() {
	'use strict';

	enyo.kind({
		name: 'InternetView',
		kind: View,
		context: 'config',
		layerClass: 'config-layer',
		help: {txt: msg.help.config.network.submenu, img: 'submenu.png'},
		classes: 'config-internet',
		tabBindings: {
			geolocal: 'geolocalView',
			status: 'statusView',
			wifi: 'searchView'
		},
		menuOptions: { classes: 'config-submenu view-in-view-menu' },
		menuComponents: [
			{
				name: 'status',
				kind: 'ConfigTabBtn',
				label: 'INFORMACIÓN',
				icon: 'submenu_info.png',
			},
			{
				name: 'wifi',
				kind: 'ConfigTabBtn',
				label: 'WIFI',
				icon: 'submenu_wifi.png',
			},
			{
				name: 'geolocal',
				kind: 'ConfigTabBtn',
				label: 'LOCALIZACIÓN',
				icon: 'submenu_geo.png',
			}
		],
		components: [
			{ kind: 'internet.StatusView' },
			{ kind: 'internet.SearchView', showing: false },
			{ kind: 'internet.GeolocalView', showing: false }
		],
		onSelfEnter: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				ActiveConnection.enableAPIsBinding(true);
				ActiveConnection.sync();
			};
		}),
		onSelfLeave: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				ActiveConnection.enableAPIsBinding(false);
			};
		})
	});

	/*
	* GeolocalView
	*/
	enyo.kind({
		name: 'internet.GeolocalView',
		kind: 'components.ViewContent',
		bindings: [
			{
				from: '^geolocalModel.enabled', to: '.$.enableOption.content', transform: function(enabled) {
					return enabled ? 'DESHABILITAR' : 'HABILITAR';
				}
			}
		],
		components: [
			{
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left',
				components: [
					{
						name: 'image',
						kind: 'UpdateDetailOptions',
						classes: 'apps-infoaction-view',
						icon: assets('geo-gde.png'),
					},
					{
						name: 'enableOption',
						kind: Shell.SimpleButton,
						onSpotlightSelect: 'enable',
					}
				]
			},
			{
				classes: 'viewcomponent-layout-right',
				components: [
					{ name: 'title', content: 'LOCALIZACIÓN', classes: 'info-details-title' },
					{ name: 'description', content: msg.geolocal, classes: 'info-details-description' }
				]
			}
		],
		enable: function() {
			geolocalModel.enable(!geolocalModel.get('enabled'));
		}
	});

	/*
	* StatusView
	*/
	enyo.kind({
		name: 'internet.StatusView',
		kind: 'components.ViewContent',
		help: {txt: msg.help.config.network.status, img: 'singleoption-about.png'},
		bindings: [
			{ from: '^ActiveConnection.state', to: '.$.noconndesc.showing', transform: function(st) { return st !== 'connected'; } },
			{ from: '^ActiveConnection.state', to: '.$.info.showing', transform: function(st) { return st === 'connected'; } },
			{
				from: '^ActiveConnection.type', to: '.$.type.content', transform: function(type) {
					return 'CONECTADO A: ' + (type === 'wireless' ? 'WIFI' : 'CABLE DE RED');
				}
			},
			{ from: '^ActiveConnection.mac', to: '.$.mac.info' },
			{ from: '^ActiveConnection.ip', to: '.$.ip.info' },
			{ from: '^ActiveConnection.gw', to: '.$.gw.info' },
			{ from: '^ActiveConnection.dns', to: '.$.dns.info', transform: function(dns) { return dns ? dns[0] : 'No disponible'; } },
		],
		components: [
			{
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left',
				components: [
					{
						name: 'internet_info_image',
						kind: 'UpdateDetailOptions',
						classes: 'apps-infoaction-view system-img',
						icon: assets('internet_info.png'),
						spotlight: true
					}
				]
			},
			{
				classes: 'viewcomponent-layout-right',
				components: [
					{ classes: 'info-details-title', content: 'ESTADO ACTUAL DE LA RED' },
					{ name: 'noconndesc', classes: 'internet-connection-type', content: 'SIN CONEXIÓN' },
					{
						name: 'info',
						classes: 'internet-details',
						components: [
							{ name: 'type', classes: 'internet-connection-type' },
							{ name: 'ip', kind: 'components.Detail', title: 'IP:' },
							{ name: 'mac', kind: 'components.Detail', title: 'MAC:' },
							{ name: 'gw', kind: 'components.Detail', title: 'GW:' },
							{ name: 'dns', kind: 'components.Detail', title: 'DNS:' },
						]
					}
				]
			},
		]
	});

	var onNetError = log.error.bind(log, 'connectWifi');

	function connectWifi(connection, onlySecured) {
		assert(connection);
		if (connection.get('isSecured') !== false) {
			var opts = { title: 'CONECTAR A ' + connection.get('name').toUpperCase() };
			return keyboard.show(opts).then(function(result) {
				if (result.status === 'accepted') {
					network.connectWifi({ ssid: connection.get('name'), pass: result.input, autoconnect: true });
				}
			}).catch(onNetError);
		}
		else if (!onlySecured) {
			network.connectWifi({ ssid: connection.get('name'), autoconnect: true });
		}
	}

	Connections.on('connFail', function warnConnError(sender, evtName, evt) {
		var net;
		if (evt.connection) {
			net = WirelessAPs.find(function(ap) {
				return ap.get('name') === evt.connection.get('name');
			});
		}

		log.trace('Connections', 'warnConnError');
		enyo.Spotlight.States.push('focus', enyo.Spotlight.getCurrent() || Context.owner());
		dialogs.error({
			desc: msg.popup.err.connectionFail,
			owner: Context.owner()
		}).then(function() {
			if (net) {
				connectWifi(net, true);
			}
		});
	});

	enyo.kind({
		name: 'internet.HiddenConnectionDialog',
		kind: 'ConfirmDialog',
		icon: 'dialog_wifi.png',
		title: 'AGREGAR RED',
		description: '',
		acceptLabel: 'CONECTAR',
		denyLabel: 'CERRAR',
		classes: 'connect-hiddennet',
		components: [
			{
				classes: 'flex wrap horizontal center',
				style: 'width: 100%; margin-bottom: 31px;',
				components: [
					{ name: 'ssidInput', kind: 'components.TitledInput', title: 'SSID', classes: 'input' },
					{
						name: 'passInput', kind: 'components.TitledInput',
						title: 'Contraseña', classes: 'input', type: 'password'
					}
				]
			}
		],
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.descriptionContainer.destroy();

				this.$.title.applyStyle('margin-top',0);
				this.$.title.applyStyle('height','30px');
				this.on('accepted', function() {
					network.connectWifi({
						ssid: this.$.ssidInput.get('value') || '',
						pass: this.$.passInput.get('value') || '',
						autoconnect: true, mode: 'wpa'
					});

					this._clear();
				}.bind(this));

				this.on('denied', this.bindSafely('_clear'));
			}
		}),
		_clear: function() {
			this.$.ssidInput.clear();
			this.$.passInput.clear();
		},
		show: enyo.inherit(function(sup) {
			return function(sender, evt) {
				sup.apply(this, arguments);
				enyo.Spotlight.spot(this.$.ssidInput);
			}
		}),
	});

	enyo.kind({
		name: 'internet.SearchView',
		classes: 'internet-searchview',
		kind: 'components.ViewContent',
		help: { txt: msg.help.config.network.connect, img: 'details-network.png' },
		bindings: [
			{
				from: '^WirelessConnection.connName', to: '.$.title.content',
				transform: function(name) {
					return name? 'CONECTADO CON ' + name.toUpperCase() : 'REDES WIFI DISPONIBLES';
				}
			},
			{
				from: '^WirelessConnection.isConnected', to: '.$.details.content',
				transform: function(isActive) {
					return isActive? msg.network.connected : msg.network.description;
				}
			},
			{ from: '^WirelessConnection.isConnected', to: '.$.disconnect.showing'},
		],
		components: [
			{ name: 'hiddenConnect', kind: 'internet.HiddenConnectionDialog', context: 'config' },
			{
				classes: 'internet-area-left',
				components: [
					{
						kind: 'components.VerticalMenu',
						classes: 'internet-subarea-left',
						components: [
							{ name: 'networklist', kind: 'NetworkList', context: 'config' },
							{
								name: 'updateList',
								kind: Shell.SimpleButton,
								classes: 'internet-action',
								content: 'RENOVAR LISTA',
								onSpotlightSelect: 'update',
							},
							{
								name: 'connectToHidden',
								kind: Shell.SimpleButton,
								content: 'AGREGAR RED',
								classes: 'internet-action',
								onSpotlightSelect: 'openConnectDialog'
							},
							{
								name: 'disconnect',
								kind: Shell.SimpleButton,
								content: 'DESCONECTAR RED',
								classes: 'internet-action',
								onSpotlightSelect: 'disconnect',
								defaultSpotlightDisappear: 'updateList'
							}
						]
					}
				]
			},
			{
				classes: '.internet-area-right',
				components: [
					{ name: 'title', classes: 'info-details-title', content: 'REDES WIFI DISPONIBLES' },
					{ name: 'details', classes: 'info-details-description', content: msg.network.description }
				]
			},
		],
		update: WirelessAPs.sync.bind(WirelessAPs, false),
		disconnect: WirelessConnection.disconnect.bind(WirelessConnection),
		openConnectDialog: function() {
			enyo.Spotlight.States.push('focus');
			this.$.hiddenConnect.show();
		},
		onContentEnter: enyo.inherit(function(sup) {
			return function(sender, evt) {
				sup.apply(this, arguments);
				if (evt.originator === this) {
					Connections.listen(true);
					Connections.sync();
				}
			};
		}),
		onFirstEnter: function() {
			if(!WirelessAPs.models.length) {
				WirelessAPs.sync(true);
			}
		},
		onContentLeave: enyo.inherit(function(sup) {
			return function(sender, evt) {
				sup.apply(this, arguments);
				if (evt.originator === this) {
					Connections.listen(false);
				}
			};
		})
	});

	/* NetworkList */
	enyo.kind({
		name: 'NetworkList',
		classes: 'networklist',
		spotlight: 'container',
		handlers: {
			onSpotlightSelect: 'onNetSelect',
		},
		components:[
			{
				name: 'itemlist',
				kind: 'moon.Scroller',
				classes: 'scroller-class',
				horizontal: 'hidden',
				vertical: 'scroll',
				showing: false,
				spotlight: 'container'
			},
			{
				name: 'info',
				spotlight: false,
				classes: 'info',
				components: [
					{ name: 'desc', content: 'No se han buscado redes' }
				]
			}
		],
		create: enyo.inherit(function(sup) {
			return function() {
				// TODO: On/off this for performance issues
				WirelessAPs.on('add', this.bindSafely('addAPs'));
				WirelessAPs.on('fetch', this.bindSafely('fetch'));
				sup.apply(this, arguments);
			};
		}),
		onNetSelect: function(oSender, oEvent) {
			var orig = oEvent.originator;
			if (orig.isDescendantOf(this.$.itemlist)) {
				connectWifi(orig.model, false);
			}
		},
		fetch: function(oSender, oEvent, oArgs) {
			if (oArgs.status === 'begin') {
				this.$.info.show();
				this.$.itemlist.hide();
				this.$.desc.setContent('Buscando...');
			}
			else if (oArgs.status === 'error') {
				this.$.desc.setContent('Se ha producido un error.');
			}
			else if (oArgs.status === 'end' && !oArgs.networks) {
				this.$.desc.setContent('No se han encontrado redes');
			}
		},
		addAPs: function(oSender, oEvent, oAdded) {
			var itemlist = this.$.itemlist;
			itemlist.destroyClientControls();
			var models = WirelessAPs.models.slice(0);

			itemlist.createComponents(models.map(function(model) {
				return { kind: 'NetworkItem', model: model };
			}, { owner: this.getInstanceOwner() }));

			itemlist.render();
			itemlist.scrollToTop();
			itemlist.show();
			this.$.info.hide();
		}
	});

	/*
	* NetworkItem
	*/
	enyo.kind({
		name: 'NetworkItem',
		classes: 'networkitem',
		spotlight: true,
		handlers: {
			onSpotlightFocus: 'onFocus',
			onSpotlightBlur: 'onBlur'
		},
		bindings: [
			{ from: '.model.isSecured', to: '.secured' },
			{ from: '.model.isConnecting', to: '.$.netName.content', transform: 'isConnecting' },
			{ from: '.model.isConnected', to: '.$.levelIcon.connected' },
			{ from: '.model.signal', to: '.$.levelIcon.signalLevel', transform: function(l) { return Math.round(l*4/100); } },
		],
		components: [
			{ name: 'netName', marqueeSpeed: 50, marqueeOnRender: false, mixins: ['moon.MarqueeSupport', 'moon.MarqueeItem'], classes: 'networkitem-name moon-marquee-start-on-render'},
			{ name: 'levelIcon', kind: 'SignalLevel' },
			{ name: 'protectIcon', kind: 'enyo.Image', src: assets('block.png'), classes: 'networkitem-protect' },
		],
		onFocus: function() {
			this.bubble('onRequestScrollIntoView');
			this.$.netName.startMarquee();
		},
		onBlur: function() {
			this.$.netName.stopMarquee();
		},
		securedChanged: function() {
			this.$.protectIcon.applyStyle('opacity', this.secured ? 1 : 0);
		},
		isConnecting: function(isConn) {
			return (isConn? 'Conectando a ' : '') + this.model.get('name');
		}
	});

	/*
	* SignalLevel
	*/
	enyo.kind({
		name: 'SignalLevel',
		classes: 'signal-bar-container',
		published: {
			signalLevel: 0,
			connected: false,
		},
		components: [
			{ name: 'bar1', classes: 'signal-bar signal-bar1' },
			{ name: 'bar2', classes: 'signal-bar signal-bar2' },
			{ name: 'bar3', classes: 'signal-bar signal-bar3' },
			{ name: 'bar4', classes: 'signal-bar signal-bar4' },
		],
		/* private */
		_totalBars: 4,
		rendered: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				for (var i = this.signalLevel+1; i <= this._totalBars; i++) {
					this.$['bar'+i].addClass('empty');
				}
				this.connectedChanged();
			};
		}),
		connectedChanged: function() {
			this.addRemoveClass('connected', this.connected);
		}
	});


})();
