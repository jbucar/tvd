/*
 * Kind WatchTVView
 * This is the sub menu for the WatchTv tab.
 */
enyo.kind({
	name: 'WatchTVView',
	kind: View,
	context: 'config',
	layerClass: 'config-layer',
	help: {txt: msg.help.config.tda.submenu, img: 'submenu.png'},
	tabBindings: {
		ginga: 'gingaControl',
		channels: 'updateChannels',
		parental: 'parentalControl'
	},
	handlers: {
		onMenuRequest: 'menuRequested'
	},
	menuOptions: { classes: 'config-submenu view-in-view-menu' },
	menuComponents: [
		{
			name: 'channels',
			kind: 'ConfigTabBtn',
			label: 'BUSCAR',
			icon: 'buscar_tda.png'
		},
		{
			name: 'parental',
			kind: 'ConfigTabBtn',
			label: 'RESTRICCIONES',
			icon: 'submenu_restricciones.png',
			onSpotlightSelect: 'checkEnter',
			onSpotlightDown: 'checkEnter',
		},
		{
			name: 'ginga',
			kind: 'ConfigTabBtn',
			label: 'GINGA.AR',
			icon: 'notification_ginga.png'
		}
	],
	components: [
		{ name: 'gingaControl', kind: 'GingaView', showing: false },
		{ name: 'updateChannels', kind: 'UpdateChannelsView', channels: channels },
		{ name: 'parentalControl', kind: 'ParentalControlView', showing: false },
	],
	checkEnter: function() {
		if (session.shouldRequestPass()) {
			security.openSession().then(function(open) {
				if (open) {
					// Hack to Spotlight to avoid memory about the last 5wayLastEvent, this is caused by a Spotlight bug
					enyo.Spotlight.onSpotlightSelect({});
					enyo.Spotlight.spot(this.$.wrapper, 'DOWN');
				}
			}.bind(this)).catch(log.error.bind(log, 'WatchTVView', 'checkEnter'));

			return true;
		}

		return false;
	},
	menuRequested: function() {
		enyo.Spotlight.spot(this.$.menu);
		return true;
	}
});
