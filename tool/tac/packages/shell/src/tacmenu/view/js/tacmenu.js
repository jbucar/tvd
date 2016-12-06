/**
 * Home kind.
 */
enyo.kind({
	name: 'TACMenu',
	classes: 'enyo-fit',
	spotlight: 'container',
	handlers: {
		onSpotlightContainerEnter: 'entered',
		onSpotlightContainerLeave: 'leaved',
		onSpotlightFocused: 'onFocused',
		onSpotlightUp: 'onUp',
		onSpotlightKeyDown: 'onKey',
		onTabWindowClosed: 'onCloseView'
	},
	components: [
		{
			name: 'navigation',
			kind: 'NavigationBar',
			onSpotlightSelect: 'navSelected',
			onSpotlightDown: 'onNavDown',
			classes: 'bottom-component',
		},
		{
			name: 'upperComponent',
			classes: 'home-upper-component',
			components: [
				{ name: 'userBar', kind: 'UserBar' }
			]
		},
		{
			name: 'middleComponent',
			spotlight: 'container',
			classes: 'middle-component',
			components: [
				{
					name: 'home',
					context: 'home',
					classes: 'enyo-fit',
					spotlight: false,
					showingChanged: function() {
						this.bubble('onBlurResquest', {on: !this.showing}, this);
					}
				},
				{ name: 'tv', kind: 'Channels' },
				{ name: 'imgs', kind: 'Images' },
				{ name: 'apps', kind: 'App' },
				{ name: 'config', kind: 'Config' }
			]
		}
	],
	selected: undefined, // Private
	create: function() {
		this.inherited(arguments);
		this.shortcutKeys = [];
		this.shortcutKeys[tacKeyboardLayout.VK_EXIT] = this.bindSafely('selectComponent','home');
		this.shortcutKeys[tacKeyboardLayout.VK_HOME] = this.bindSafely('selectMenu');
		this.shortcutKeys[tacKeyboardLayout.VK_GUIDE] = this.bindSafely('selectComponent','tv', {tab: 'guide', enter: true});
		this.shortcutKeys[tacKeyboardLayout.VK_HELP] = this.bindSafely('showHelpDialog');
		session.on('onHasPasswordChanged', function(oSender, oEvent, oInfo) {
			if (oInfo.open) {
				Shell.helpDialog.show({txt: msg.help.config.tda.parental.new, img: 'session.png'});
			}
		});
	},
	showHelpDialog: function() {
		if (Context.help) {
			Shell.helpDialog.show();
		} else {
			log.error('Shell.HelpDialog','show', msg.help.error);
		}
	},
	onKey: function(oSender, oEvent) {
		log.trace('TACMenu','onKey', 'key=%s', oEvent.keyCode);
		var shortcut = this.shortcutKeys[oEvent.keyCode];
		if ( shortcut ) {
			shortcut();
		}
	},
	entered: function(oSender, oEvent) {
		if(oEvent.originator === this) {
			log.trace('TacMenu', 'entered');
			Context.enter('tacmenu', this);
		}
	},
	leaved: function(oSender, oEvent) {
		if(oEvent.originator === this) {
			log.trace('TacMenu', 'leaved');
			Context.leave('tacmenu');
		}
	},
	getContext: function() {
		return this.selected.context;
	},
	navSelected: function() {
		this.selectMiddle(true);
	},
	onNavDown: util.true,
	onFocused: function(oSender, oEvent) {
		if (oEvent.originator.isDescendantOf(this.$.navigation) && oEvent.originator !== this.$.navigation) {
			Context.setHelp(oEvent.originator.help);
			var oldSelected = this.selected;
			this.selected = this.$[oEvent.originator.name];
			if (this.selected !== oldSelected) {
				if (oldSelected) {
					this.stopJob('select');
					oldSelected.hide();
				}
				if (this.selected) {
					this.startJob('select', this.bindSafely('doSelection'), 250);
				}
			}
		}
	},
	onCloseView: function(oSender, oEvent) {
		this.selectMenu();
		return true;
	},
	onUp: function(oSender, oEvent) {
		if (oEvent.originator === this.$.navigation) {
			return this.selectMiddle(true);
		}
	},
	selectMiddle: function(anim) {
		this.stopJob('select');
		if (this.selected) {
			this.selected.show();
			// TODO: Fix this hack it must not checks for the Kind
			if (this.selected.name !== 'home') {
				this.$.navigation.collapse(anim);
				this.$.navigation.spotlightDisabled = true;
				this.selected.set('animEnabled', anim);
				enyo.Spotlight.spot(this.$.middleComponent);
				this.selected.set('animEnabled', true);
				return true;
			}
		}
		return false;
	},
	selectMenu: function() {
		var navbar = this.$.navigation;
		Context.setHelp(navbar.help);
		navbar.expand();
		navbar.spotlightDisabled = false;
		enyo.Spotlight.spot(navbar);
	},
	selectComponent: function(compName, opts, fromOutside) {
		if (fromOutside) {
			Context.enter('tacmenu', this);
		}

		this.$.navigation.spotlightDisabled = false;
		enyo.Spotlight.spot(this.$.navigation);
		this.$.navigation.select(compName);

		if (this.selectMiddle(false) && opts) {
			var tab = this.selected.selectTab(opts.tab, opts.enter);
			if (tab && opts.sub) {
				tab.selectTab(opts.sub.tab, opts.sub.enter);
			}
		}
		else {
			this.$.navigation.expand();
		}
	},
	doSelection: function() {
		this.selected.show();
	}
});
