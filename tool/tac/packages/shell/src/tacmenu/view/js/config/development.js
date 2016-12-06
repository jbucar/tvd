/*
 * Kind DevelopmentView
 * This is the sub menu for the System tab.
 */
enyo.kind({
	name: 'DevelopmentView',
	kind: View,
	context: 'config',
	help: {txt: msg.help.submenu.ch, img: 'submenu.png'},
	layerClass: 'config-layer',
	tabBindings: {
		developmentDebug: 'developmentDebugView',
		developmentApps: 'developmentAppsView',
		developmentPower: 'developmentPowerView',
	},
	menuOptions: { classes: 'config-submenu view-in-view-menu' },
	menuComponents: [
		{
			name: 'developmentPower',
			kind: 'ConfigTabBtn',
			label: 'SISTEMA',
			icon: 'submenu_apagar.png',
		},
		{
			name: 'developmentDebug',
			kind: 'ConfigTabBtn',
			label: 'DEBUG',
			icon: 'submenu_debug.png',
		},
		{
			name: 'developmentApps',
			kind: 'ConfigTabBtn',
			label: 'APLICACIONES',
			icon: 'submenu_aplicacion.png',
		},
	],
	components: [
		{ name: 'developmentPowerView', kind: 'DevelopmentPowerView' },
		{ name: 'developmentAppsView', kind: 'DevelopmentAppsView', showing: false},
		{ name: 'developmentDebugView', kind: 'DevelopmentDebugView', showing: false },
	],
	onFirstEnter: function() {
		LogConfig.fetch();
		ActiveConnection.sync();
	}
});

/**
 * DevelopmentPowerView kind.
 */
enyo.kind({
	name: 'DevelopmentPowerView',
	kind: 'components.ViewContent',
	components: [
		{
			classes: 'viewcomponent-layout-left',
			components: [
				{
					kind: 'components.flexLayout.Vertical',
					components: [
						{ kind: 'components.VerticalMenu', components: [
							{
								name: 'app',
								kind: 'UpdateDetailOptions',
								classes: 'apps-infoaction-view',
								icon: assets('apagar.png'),
							},
							{
								name: 'poweroff',
								kind: Shell.SimpleButton,
								content: 'APAGAR',
								onSpotlightSelect: 'powerOff',
							},
							{
								name: 'reboot',
								kind: Shell.SimpleButton,
								content: 'REINICIAR',
								onSpotlightSelect: 'reboot',
							},
							{
								name: 'closeMode',
								kind: Shell.SimpleButton,
								content: 'SALIR DEL MODO',
								onSpotlightSelect: 'closeMode',
							}
						]},
					],
				}
			]
		},
		{
			classes: 'viewcomponent-layout-right',
			components: [
				{ content: 'SISTEMA', classes: 'info-details-title' },
				{
					classes: 'group-desc',
					components: [
						{ content: 'APAGAR', classes: 'info-details-subtitle' },
						{ content: 'Apagar el sistema', classes: 'desc' },
					]
				},
				{
					classes: 'group-desc',
					components: [
						{ content: 'REINICIAR', classes: 'info-details-subtitle' },
						{ content: 'Reiniciar el sistema', classes: 'desc' },
					]
				},
				{
					classes: 'group-desc',
					components: [
						{ content: 'SALIR DEL MODO', classes: 'info-details-subtitle' },
						{ content: 'Salir del modo desarrollador', classes: 'desc' },
					]
				},
			]
		},
	],
	powerOff: function() { system.powerOff(); },
	reboot: function() { system.reboot(); },
	closeMode: function() { system.disableDevelopmentMode(); }
});

enyo.kind({
	name: 'DevelopmentAppsView',
	kind: 'components.ViewContent',
	components: [
		{
			classes: 'viewcomponent-layout-left',
			components: [
				{
					kind: 'components.flexLayout.Vertical',
					components: [
						{
							name: 'app', kind: 'UpdateDetailOptions', classes: 'apps-infoaction-view',
							icon: assets('app.png'),
						}
					]
				}
			]
		},
		{
			classes: 'viewcomponent-layout-right',
			components: [
				{ name: 'title', content: 'APLICACIONES', classes: 'info-details-title' },
				{ name: 'urlInputTitle', content: 'Instalar TPK', classes: 'info-subtitle' },
				{
					classes: 'debug-url-container config',
					components: [
						{
							name: 'url',
							kind: 'components.Input',
							placeholder: 'Introduzca la URL del TPK',
							selectOnFocus: true,
							style: 'color: white'
						},
						{
							name: 'btnInstallTpk',
							kind: Shell.SimpleButton,
							classes: 'debug-app-install',
							content: 'INSTALAR',
							onSpotlightSelect: 'installApp'
						},
					]
				}
			]
		},
	],
	installApp: function() {
		pkgmgr.install({'url': this.$.url.get('value')});
		return true;
	}
});


/**
 * DebugExpandablePicker kind.
 */
enyo.kind({
	name: 'DebugExpandablePicker',
	kind: 'moon.ExpandablePicker',
	spotlight: 'container',
	classes: 'debug-picker',
	handlers: {
		onSpotlightContainerLeave: 'onLeave',
	},
	onLeave: function() {
		this.selectAndClose();
	}
});

/**
 * DevelopmentDebugView kind.
 */
enyo.kind({
	name: 'DevelopmentDebugView',
	kind: 'components.ViewContent',
	bindings: [
		{ from: '^LogConfig.enabled', to: '.$.logStatus.selectedIndex', transform: function(enabled) { return enabled? 0 : 1; }},
		{ from: '^LogConfig.level', to: '.$.logLevel.selectedIndex' },
		{ from: '.logInfo', to: '.$.logInfo.content' },
		{ from: '^LogConfig.port', to: 'port' },
		{ from: '^ActiveConnection.ip', to: 'ip' }
	],
	computed: [
		{ method: 'logInfo', path: ['port', 'ip'] }
	],
	components: [
		{
			classes: 'viewcomponent-layout-left',
			components: [
				{
					kind: 'components.flexLayout.Vertical',
					components: [
						{
							name: 'app', kind: 'UpdateDetailOptions', classes: 'apps-infoaction-view',
							icon: assets('debug.png'),
						}
					]
				}
			]
		},
		{
			classes: 'viewcomponent-layout-right',
			onSpotlightContainerEnter: 'spotLog',
			spotlight: 'container',
			components: [
				{ name: 'title', content: 'CONFIGURACIÓN DE DEBUG', classes: 'info-details-title' },
				{
					name: 'subarea',
					classes: 'debug-area',
					components: [
						{
							components: [
								{
									name: 'logStatus',
									kind: 'DebugExpandablePicker',
									content: 'ESTADO',
									onChange: 'enableLog',
									components: [
										{ name: 'enabled', content: 'Habilitado', value: true },
										{ name: 'disabled', content: 'Deshabilitado', value: false },
									]
								},
							]
						},
						{
							components: [
								{
									name: 'logLevel',
									kind: 'DebugExpandablePicker',
									content: 'NIVEL DE LOG',
									onChange: 'changeLevel',
									components: [
										{ content: 'Error', value: 0 },
										{ content: 'Warning', value: 1 },
										{ content: 'HTTP', value: 2 },
										{ content: 'Info', value: 3 },
										{ content: 'Verbose', value: 4 },
										{ content: 'Silly', value: 5 },
									]
								}
							]
						}
					]
				},
				{ name: 'logInfo', classes: 'config-info-paragraph' }
			]
		},
	],
	logInfo: function() {
		var ip = ActiveConnection.get('ip');
		return ip? 'Visitá http://{0}:{1}/log para acceder al log'.format(ip.substring(0, ip.lastIndexOf('/')), LogConfig.get('port')) : '';
	},
	enableLog: function(oSender, oEvent) {
		LogConfig.enable(oSender.getSelected().value);
	},
	changeLevel: function(oSender, oEvent) {
		LogConfig.changeLevel(oSender.getSelected().value);
	},
	spotLog: function(oSender, oEvent) {
		if (oEvent.originator === oSender) {
			enyo.Spotlight.spot(this.$.logStatus);
		}
	},
});
