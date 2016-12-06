

(function() {

	enyo.kind({
		name: 'channels.CarouselItem',
		kind: 'CarouselItem',
		handlers: {
			onSpotlightSelect: '_selected'
		},
		bindings: [
			{ from: '.model.name', to: '.$.chName.content' },
			{ from: '.model.id', to: '.chID' },
			{ from: '.model.channel', to: '.$.chNumber.content' },
			{ from: '.model.isBlocked', to: '.$.blockedIcon.showing' },
			{ from: '.highlighted', to: '.$.blockedIcon.enabled' },
			{ from: '.model.logo', to: '.$.img.src' },
			{ from: '.model.currentShow', to: '.currentShow' },
			{
				from: '.currentShow.name', to: '$.txt.content',
				transform: function( name ) {
					return name ? name : '---';
				}
			},
		],
		iconBar: [
			{ name: 'chNumber', kind: 'ChTitle', classes: 'carousel-item-txt chnumber' },
			{
				name: 'blockedIcon', kind: 'components.StateImage',
				classes: 'blockedIcon', showOnSelected: true,
				enabledSrc: assets('unlock-tda.png'),
				disabledSrc: assets('unlock-tda-unselected.png')
			},
			{ name: 'chName', kind: 'ChTitle', classes: 'carousel-item-txt chname' }
		],
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.iconsBar.addClass('carousel-iconbar-ch');
				this.$.chNumber.set('style', 'margin-right: ' + (this.model.get('isBlocked') ? 11 : 40) + 'px');
			};
		}),
		_selected: function() {
			tvdadapter.change(this.chID).then(
				enyo.Signals.send.bind(enyo.Signals, 'onTVMode', {mode: 'fullscreen'})
			);
		},
		onFocus: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.model.fetchShow();
				this.$.chNumber.set('style', 'margin-right: ' + (this.model.get('isBlocked') ? 40 : 71) + 'px; font-size: 16pt');
				this.$.chName.set('style', 'font-size: 16pt');
				this.$.chName.start();
			};
		}),
		onBlur: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.chNumber.set('style', 'margin-right: ' + (this.model.get('isBlocked') ? 11 : 40) + 'px; font-size: 12pt');
				this.$.chName.set('style', 'font-size: 12pt');
				this.$.chName.stop();
			};
		})
	});

	enyo.kind({
		name: 'ChTitle',
		bindings: [
			{ from: '.content', to:'.$.title.content' }
		],
		mixins: ['moon.MarqueeSupport'],
		marqueeSpeed: 50,
		components: [
			{
				name: 'title',
				kind: 'moon.MarqueeText',
				classes: 'ch-marquee-text',
			},
		],
		start: function() {
			this.startMarquee();
		},
		stop: function() {
			this.stopMarquee();
		}
	});

	function getCollection(mdl) {
		var cat = mdl.get('name');
		switch(cat) {
			case 'FAVORITOS':
				return chFavs;
			case 'BLOQUEADOS':
				return bckChs;
			default:
				return availableChannels;
		}
	}

	enyo.kind({
		name: 'ChContainer',
		kind: View,
		showing: true,
		context: 'ch',
		help: { txt: msg.help.submenu.ch, img: 'submenu.png' },
		handlers: {
			onSpotLost: '_spotLosted',
		},
		bindings: [
			{ from: '^scanModel.isScanning', to: '.$.wrapper.showing', transform: util.negate },
		],
		mixins: [MenuCollectionSupport],
		menuOptions: { classes: 'ch-submenu view-in-view-menu' },
		collection: ChannelCategories,
		menuKindOption: 'ChannelsTabBtnCategory',
		contentOptions: {
			kind: 'CarouselDetail',
			bindings: [
				{ from: 'model', to: 'collection', transform: getCollection }
			],
			sortedBy: 'id',
			carouselItemView: 'channels.CarouselItem',
			detailItemView: 'ChInfo',
			help: {
				'carousel': { txt: msg.help.carousel.ch, img: 'carousel-tda.png' },
				'detail': { txt: msg.help.details.ch, img: 'details-tda.png' }
			}
		},
		spotCh: function(chID) {
			var todos = this.getClientControls().find(function(c) {
				var m = c.get('model');
				return m? m.get('name') === 'TODOS' : false;
			});

			var channel = todos.$.carousel.$.container.getClientControls().find(function(c) {
				return c.model.get('id') === chID;
			});

			if (channel) {
				enyo.Spotlight.spot(channel);
			}
		},
		_spotLosted: function(sender, evt) {
			return enyo.Spotlight.spot(this.$.menu);
		}
	});

	enyo.kind({
		name: 'Channels',
		kind: View,
		black: true,
		context: 'ch',
		help: {txt: msg.help.menu.ch, img: 'menu.png'},
		handlers: {
			onSelectView: 'selectView'
		},
		tabBindings: {
			watchTV: 'chContainer',
			scan: 'scanPage',
			guide: 'guideView',
			back: 'none'
		},
		menuOptions: { classes: 'channels-menu' },
		menuComponents: [
			{
				name: 'watchTV',
				kind: 'ChannelsTabBtn',
				label: 'TDA',
				icon: 'menu-config_tv.png',
			},
			{
				name: 'guide',
				kind: 'ChannelsTabBtn',
				label: 'GUÍA',
				icon: 'epg.png',
			},
			{
				name: 'scan',
				kind: 'ChannelsTabBtn',
				label: 'BUSCAR',
				icon: 'buscar_tda.png',
			},
			{
				name: 'back',
				kind: 'ChannelsTabBtn',
				label: 'MENÚ',
				icon: 'menu_menu.png',
				help: {txt: msg.help.menu.backToNavbar, img: 'back-to-navbar.png'},
				onSpotlightFocused: 'onBackFocused',
				onSpotlightBlur: 'onBackBlur',
				onSpotlightSelect: 'viewClose',
				onSpotlightDown: 'viewClose',
				tooltipStyle: 'menu',
				tooltipMsg: msg.tooltip.menu,
				tooltipEnabled: true
			}
		],
		components: [
			{
				classes: 'ch-main-wrapper',
				components: [
					{
						name: 'chContainer',
						kind: ChContainer,
					},
					{
						name: 'guideView',
						kind: Guide,
						showing: false
					},
					{
						name: 'scanPage',
						kind: 'UpdateChannelsView',
						showing: false
					}
				]
			}
		],
		showingChanged: function() {
			this.inherited(arguments);
			// Hack: force view adjust when it is always visible
			this.$.chContainer.adjust();
		},
		selectView: function() {
			enyo.Spotlight.spot( this.$.watchTV );
			this.resetMenu();
			return true;
		},
		onBackFocused: function(oSender, oEvent) {
			Context.setHelp(oSender.help);
		},
		onBackBlur: function() {
			Context.setHelp(this.help);
		}
	});

	enyo.kind({
		name: 'ChannelsTabBtn',
		kind: Shell.TabButton,
		selectedIcon: 'apps-selected.png',
		colorClass: 'ch-button'
	});

	function getIcon(cat) {
		var res = 'submenu_fav.png';
		if (cat === 'TODOS') {
			res = 'submenu_todos.png';
		}
		else if (cat === 'FAVORITOS') {
			res = 'submenu_fav.png';
		}
		else if (cat === 'BLOQUEADOS') {
			res = 'block.png';
		}

		return util.asset(res);
	}

	enyo.kind({
		name: 'ChannelsTabBtnCategory',
		kind: 'ChannelsTabBtn',
		bindings: [
			{ from: 'model.name', to: '.label', transform: util.toUpperCase },
			{ from: 'model.name', to: '.$.icon.src', transform: getIcon },
		]
	});

})();
