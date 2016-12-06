 // Helpers
applications = {};

applications.run = function(model) {
	var id = model.get('id');
	log.trace('Applications', 'run', 'Run application id=%s', id);
	appmgr.run(id, util.catchError(function(result) {
		var st = result ? ' sucess' : ' fails';
		log.debug('Applications', 'run', 'Run application id=%s status=%s', id, st);
	}));
};

(function() {

	enyo.kind({
		name: 'AppView',
		kind: 'CarouselItem',
		bindings: [
			{ from: '.model.name', to: '.$.txt.content' },
			{ from: '.model.icon', to: '.$.img.src' },
			{ from: '.model.favorite', to: '.$.fav.showing', transform: Boolean },
		],
		iconBar: [
			{ name: 'fav', kind: enyo.Image, src: assets('fav_lista.png'), classes: 'fav-icon', showing: false }
		],
		handlers: {
			onSpotlightSelect: '_selected',
		},
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.iconsBar.addClass('app-iconbar');
				this.$.txt.set('showing', true);
				this.$.txt.addClass('appsname');
			}
		}),
		_selected: function() {
			applications.run(this.get('model'));
			return true;
		},
		onBlur: enyo.inherit(function(sup) {
			return function() {
				this.$.txt.stopMarquee();
				this.$.txt.removeClass('selected {0}'.format(Context.current()));
			}
		})
	});

	function getCollection(mdl) {
		var cat = mdl.get('name');
		switch(cat) {
			case 'TODAS':
				return Applications;
			case 'FAVORITOS':
				return FavoriteApplications;
			default:
				var collection = new ApplicationCategoryCollection();
				collection.setCategory(cat);
				collection.setCollection(Applications);
				return collection;
		}
	}

	enyo.kind({
		name: 'AppsContainer',
		kind: View,
		showing: true,
		context: 'apps',
		help: { txt: msg.help.submenu.apps, img: 'submenu.png' },
		mixins: [MenuCollectionSupport],
		menuOptions: { classes: 'apps-submenu view-in-view-menu' },
		collection: ApplicationsCategoriesCollection,
		menuKindOption: 'AppsTabBtnCategory',
		contentOptions: {
			kind: 'CarouselDetail',
			bindings: [
				{ from: 'model', to: 'collection', transform: getCollection }
			],
			carouselItemView: 'AppView',
			detailItemView: 'AppsInfo',
			help: {
				'carousel': { txt: msg.help.carousel.apps, img: 'carousel-apps.png' },
				'detail': { txt: msg.help.details.apps, img: 'details-apps.png' }
			}
		},
		handlers: {
			onSpotLost: '_spotLosted'
		},
		_spotLosted: function(sender, evt) {
			return enyo.Spotlight.spot(this.$.menu);
		}
	});

	enyo.kind({
		name: 'App',
		kind: View,
		black: true,
		context: 'apps',
		help: {txt: msg.help.menu.apps, img: 'menu.png'},
		menuOptions: { classes: 'apps-menu' },
		tabBindings: {
			apps: 'appsContainer',
			market: 'marketDetail',
			back: 'none'
		},
		handlers: {
			onBack: 'onBack',
			onSpotlightSelect: 'onSelect'
		},
		menuComponents: [
			{
				name: 'apps',
				kind: 'AppsTabBtn',
				label: 'APLICACIONES',
				icon: 'apps.png'
			},
			{
				name: 'market',
				kind: 'AppsTabBtn',
				label: 'TIENDA TAC',
				icon: 'menu_tienda.png',
			},
			{
				name: 'back',
				kind: 'AppsTabBtn',
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
			},
		],
		components: [
			{
				classes: 'apps-main-wrapper',
				components: [
					{
						name: 'appsContainer',
						kind: AppsContainer,
					},
					{
						name: 'contentMarket',
						components: [
							{
								name: 'marketDetail',
								kind: 'MarketInfo',
								classes: 'market-detail',
								help: {txt: msg.help.marketdetail.apps, img: 'singleoption.png'},
								showing: false,
								onSpotlightContainerEnter: 'onMarketDetailEnter',
							}
						]
					}
				]
			}
		],
		showingChanged: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				// Hack: force view adjust when it is always visible
				this.$.appsContainer.adjust();
			}
		}),
		onMarketDetailEnter: function(sender, evt) {
			Context.setHelp(sender.help);
		},
		onBackFocused: function(sender, evt) {
			Context.setHelp(sender.help);
		},
		onBackBlur: function() {
			Context.setHelp(this.help);
		},
		handlers: {
			onSpotLost: '_spotLosted'
		},
		_spotLosted: function(sender, evt) {
			if (this.$.menu.length > 0 && evt.originator.isDescendantOf(this.tabSelected)) {
				enyo.Spotlight.spot(this.$.menu);
				return true;
			}
		}
	});

	enyo.kind({
		name: 'AppsTabBtn',
		kind: Shell.TabButton,
		colorClass: 'apps-button'
	});

	function getIcon(cat) {
		var res = 'app_default.png';
		if (cat === 'TODAS') {
			res = 'submenu_todos.png';
		}
		else if (cat === 'FAVORITOS') {
			res = 'submenu_fav.png';
		}

		return util.asset(res);
	};

	enyo.kind({
		name: 'AppsTabBtnCategory',
		kind: 'AppsTabBtn',
		bindings: [
			{ from: 'model.name', to: '.label', transform: util.toUpperCase },
			{ from: 'model.name', to: '.$.icon.src', transform: getIcon },
		]
	});

})();
