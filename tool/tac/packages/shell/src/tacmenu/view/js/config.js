/**
 * Kind Config
 */
enyo.kind({
	name: 'Config',
	kind: View,
	black: true,
	context: 'config',
	help: {txt: msg.help.menu.config, img: 'menu.png'},
	menuOptions: { classes: 'config-menu' },
	tabBindings: {
		services: 'servicesView',
		watchTv: 'watchTvView',
		system: 'systemView',
		internet: 'internetView',
		back: 'none'
	},
	bindings: [
		{ from: '^DevMode.enabled', to: '.devMode' }
	],
	menuComponents: [
		{
			name: 'system',
			kind: 'ConfigTabBtn',
			label: 'SISTEMA',
			icon: 'menu-sistema.png',
			tooltipEnabled: true
		},
		{
			name: 'watchTv',
			kind: 'ConfigTabBtn',
			label: 'CONFIG. TDA',
			icon: 'menu-config_tv.png',
			tooltipEnabled: true
		},
		{
			name: 'internet',
			kind: 'ConfigTabBtn',
			label: 'INTERNET',
			icon: 'menu_internet.png',
			tooltipEnabled: true
		},
		{
			name: 'services',
			kind: 'ConfigTabBtn',
			label: 'SERVICIOS',
			icon: 'submenu-servicios-tac.png',
			tooltipEnabled: true
		},
		{
			name: 'back',
			kind: 'ConfigTabBtn',
			label: 'MENÚ',
			icon: 'menu_menu.png',
			colorClass: 'config-button',
			help: {txt: msg.help.menu.backToNavbar, img: 'back-to-navbar.png'},
			onSpotlightFocused: 'onBackFocused',
			onSpotlightBlur: 'onBackBlur',
			onSpotlightSelect: 'viewClose',
			onSpotlightDown: 'viewClose',
			tooltipEnabled: true,
			tooltipStyle: 'menu tab',
			tooltipMsg: msg.tooltip.menu,
		},
	],
	components: [
		{
			name: 'configOptions',
			classes: 'config-wrapper',
			components: [
				{ name: 'servicesView', kind: 'ServicesView' },
				{ name: 'watchTvView', kind: 'WatchTVView' },
				{ name: 'systemView', kind: 'SystemView' },
				{ name: 'internetView', kind: 'InternetView' },
				{ name: 'developmentView', kind: 'DevelopmentView' },
			]
		}
	],
	devModeChanged: function() {
		if (this.devMode !== undefined) {
			var current = enyo.Spotlight.getCurrent();
			var spot = (current && current.isDescendantOf(this)) || this.$.developmentView.showing;
			if (this.devMode) {
				this.$.menu.createComponent({
					name: 'development',
					kind: 'ConfigTabBtn',
					label: 'DESARROLLO',
					icon: 'menu_desarrollo.png',
					tooltipEnabled: true,
				}, {owner: this}).render();

				this.tabBindings.development = 'developmentView';
			}
			else {
				var devOpt = this.$.menu.getClientControls().find(function(c) {
					return c.name === 'development';
				});

				if (devOpt) {
					this.$.menu.destroyComponent(devOpt);
					this.tabBindings.development = 'none';
				}
			}
			if (spot) {
				enyo.Spotlight.spot(this.$.menu);
			}
		}
	},
	onBackFocused: function(oSender, oEvent) {
		Context.setHelp(oSender.help);
	},
	onBackBlur: function() {
		Context.setHelp(this.help);
	}
});

/**
 * Kind ConfigTabBtn
 */
enyo.kind({
	name: 'ConfigTabBtn',
	kind: Shell.TabButton,
	colorClass: 'config-button',
	tooltipStyle: 'menu',
	tooltipMsg: msg.tooltip.config,
	tooltipEnabled: false
});

/* TODO: Rename */
enyo.kind({
	name: 'UpdateDetailOptions',
	classes: 'updates-detail-options',
	bindings: [
		{ from: '.icon', to: '.$.img_lookup.src' }
	],
	components: [
		{ name: 'img_lookup', kind: enyo.Image, classes: 'updates-detail-img' }
	],
});
