
(function() {

/**
 * kind AppsInfo
 */
enyo.kind({
	name: 'AppsInfo',
	kind: 'components.ViewContent',
	handlers: {
		onSpotlightRight: 'onRight',
		onSpotlightLeft: 'onLeft'
	},
	published: {
		model: null
	},
	bindings: [
		{ from: '.model', to: '.$.infoOptions.model' },
		{ from: '.model', to: '.$.infoDetails.model' },
	],
	components: [
		{
			classes: 'details-actions',
			components: [
				{ name: 'infoOptions', kind: 'AppsInfoActions', center: false }
			]
		},
		{
			classes: 'details-desc',
			components: [
				{ name: 'infoDetails', kind: 'AppsInfoDetails' }
			]
		}
	],
	onRight: function(sender, evt) {
		if (evt.originator === this) {
			this.bubble('onRequestScroll', {originator: this, dir: 'RIGHT'});
		}
	},
	onLeft: function(sender, evt) {
		if (evt.originator === this) {
			this.bubble('onRequestScroll', {originator: this, dir: 'LEFT'});
		}
	},
	destroy: enyo.inherit(function(sup) {
		return function() {
			var cur = enyo.Spotlight.getCurrent();
			if (cur && cur.isDescendantOf(this)) {
				this.bubble('onSpotLost', {originator: this});
			}
			sup.apply(this, arguments);
		}
	})
});

/**
 * kind AppsInfoActions
 */
enyo.kind({
	name: 'AppsInfoActions',
	kind: 'components.flexLayout.Vertical',
	published: {
		model: null,
	},
	bindings: [
		{ from: '.model.icon', to: '.$.app.icon' },
		{ from: '.model.favorite', to: '.$.app.favorite' },
		{ from: '.model.system', to: '.$.uninstall.showing', transform: function(sys) { return !sys; }},
		{
			from: '.model.favorite', to: '.$.favoriteBtn.content',
			transform: function(isFav) {
				return isFav ? 'QUITAR FAVORITO' : 'AGREGAR FAVORITO';
			}
		},
	],
	components: [
		{ name: 'app', kind: 'AppDetailOptions', classes: 'apps-infoaction-view' },
		{
			kind: 'components.VerticalMenu',
			style: 'min-width: 100%',
			components: [
				{
					name: 'run',
					kind: Shell.SimpleButton,
					content: 'INICIAR',
					onSpotlightSelect: 'run'
				},
				{
					name: 'favoriteBtn',
					kind: Shell.SimpleButton,
					content: 'QUITAR FAVORITO',
					onSpotlightSelect: 'toggleFav'
				},
				{
					name: 'uninstall',
					kind: Shell.SimpleButton,
					content: 'DESINSTALAR',
					onSpotlightSelect: 'showPopup',
				}
			]
		}
	],
	create: function() {
		this.inherited(arguments);
		this.$.app.removeClass('appview-wrapper');
	},
	run: function() {
		applications.run(this.model);
		return true;
	},
	toggleFav: function() {
		this.model.toggleFavorite();
		return true;
	},
	showPopup: function() {
		enyo.Spotlight.States.push('focus', this);
		dialogs.warn({desc: msg.popup.warn.uninstallApp, owner: this}).then(function(ok) {
			if (ok) {
				this.model.uninstall();
			}
		}.bind(this));
	}
});

/**
 * MarketInfo kind
 */
enyo.kind({
	name: 'MarketInfo',
	kind: 'components.ViewContent',
	bindings: [
		{ from: '.model', to: '.$.infoOptions.model' },
		{ from: '.model', to: '.$.infoDetails.model' },
	],
	components: [
		{
			classes: 'viewcomponent-layout-left',
			components: [
				{ name: 'infoOptions', kind: 'MarketActions' }
			]
		},
		{
			classes: 'viewcomponent-layout-right',
			components: [
				{ name: 'infoDetails', kind: 'AppsInfoDetails' }
			]
		}
	],
	create: enyo.inherit(function(sup) {
		return function() {
			sup.apply(this, arguments);
			var set = this.set.bind(this, 'model');
			Applications.addListener('loaded', function() {
				appmgr.getWebApps(util.catchError(function(id) {
					set(Applications.find(function(app) { return app.get('id') === id; }));
				}));
			});
		};
	})
});

/**
 * MarketActions kind.
*/
enyo.kind({
	name: 'MarketActions',
	kind: 'AppsInfoActions',
	create: function() {
		this.inherited(arguments);
		this.$.favoriteBtn.setShowing(false);
		this.$.uninstall.setShowing(false);
	}
});

/*
* AppDetailOptions
*/
enyo.kind({
	name: 'AppDetailOptions',
	classes: 'app-detail-options',
	bindings: [
		{ from: '.icon', to: '.$.img.src' },
		{ from: '.favorite', to: '.$.fav.showing', transform: Boolean },
	],
	components: [
		{
			kind: 'IconsBar',
			classes: 'app-iconbar',
			components: [
				{ name: 'fav', kind: enyo.Image, src: assets('fav.png'), classes: 'fav-icon', showing: false },
			]
		},
		{ name: 'img', kind: enyo.Image, classes: 'img' },
	]
});

function format(val, opt) { return val ? val : ( opt ? opt : '-'); }

enyo.kind({
	name: 'AppsInfoDetails',
	classes: 'apps-info-details',
	published: {
		model: null,
	},
	bindings: [
		{
			from: 'model.name',
			to: '.$.title.content',
			transform: function(name) {
				return format(name, 'Unknown app').toUpperCase();
			}
		},
		{
			from: 'model.vendor',
			to: '.$.vendor.info',
			transform: function(vendor) {
				return format(vendor, 'Autor desconocido');
			}
		},
		{
			from: 'model.version',
			to: '.$.version.info',
			transform: function(version) {
				return format(version, 'No específicada');
			}
		},
		{
			from: 'model.description',
			to: '.$.description.content',
			transform: function(desc) {
				return format(desc, 'No hay descripción disponible');
			}
		},
	],
	components: [
		{ name: 'title', classes: 'info-details-title apps' },
		{ name: 'vendor', kind: 'components.Detail', title: 'AUTOR:', info: 'Autor desconocido' },
		{ name: 'version', kind: 'components.Detail', title: 'VERSIÓN:', info: 'No específicada' },
		{ name: 'description', classes: 'info-details-description', allowHtml: true }
	]
});

})();
