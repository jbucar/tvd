enyo.kind({
	name: 'NavigationBar',
	spotlight: 'container',
	memory: true,
	classes: 'navBar',
	components: [
		{
			name: 'home',
			kind: 'NavBarTabBtn',
			colorClass: 'home-color',
			help: {txt: msg.help.navbar.home, img: 'navbar.png'},
			label: 'INICIO',
			icon: 'home.png',
			selectedIcon: 'home-selected.png',
			defaultSpotlightLeft: 'config',
			tooltipEnabled: false
		},
		{
			name: 'tv',
			kind: 'NavBarTabBtn',
			colorClass: 'tv-color',
			help: {txt: msg.help.navbar.ch, img: 'navbar.png'},
			label: 'TDA',
			icon: 'tv.png',
			selectedIcon: 'tv-selected.png',
		},
		{
			name: 'imgs',
			kind: 'NavBarTabBtn',
			colorClass: 'img-color',
			help: {txt: msg.help.navbar.img, img: 'navbar.png'},
			label: 'IMÁGENES',
			icon: 'imgn-blanco.png',
			selectedIcon: 'imgn-color.png',
		},
		{
			name: 'apps',
			kind: 'NavBarTabBtn',
			colorClass: 'apps-color',
			help: {txt: msg.help.navbar.apps, img: 'navbar.png'},
			label: 'APLICACIONES',
			icon: 'apps.png',
			selectedIcon: 'apps-selected.png',
		},
		{
			name: 'config',
			kind: 'NavBarTabBtn',
			colorClass: 'config-color',
			label: 'CONFIGURACIÓN',
			help: {txt: msg.help.navbar.config, img: 'navbar.png'},
			icon: 'config.png',
			selectedIcon: 'config-selected.png',
			defaultSpotlightRight: 'home',
		},
		{ name: 'animator', kind: 'Shell.Collapser' }
	],
	create: enyo.inherit(function(sup) {
		return function() {
			sup.apply(this, arguments);
			// Initialy expanded by CSS.
			this.$.animator.set('state', 'expand');
		};
	}),
	select: function(targetName) {
		if (this.$[targetName]) {
			enyo.Spotlight.spot(this.$[targetName]);
		}
	},
	expand: function() {
		this.$.animator.expand(this, '7px', '55px');
	},
	collapse: function(anim) {
		this.$.animator.collapse(this, '55px', '7px', !anim);
	}
});

/**
 * Kind NavBarTabBtn
 */
enyo.kind({
	name: 'NavBarTabBtn',
	kind: Shell.Button,
	tooltipStyle: 'tacmenu',
	tooltipMsg: msg.tooltip.home,
	tooltipEnabled: true
});
