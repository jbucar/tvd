(function() {
	'use strict';

	function format(val, opt) { return val ? val : ( opt ? opt : '-'); }

	enyo.kind({
		name: 'ServicesInfo',
		kind: 'components.ViewContent',
		help: { txt: msg.help.config.services.details, img: 'submenu.png' },
		classes: 'serviceDetails',
		published: {
			model: null
		},
		handlers: {
			onSpotlightRight: 'onRight',
			onSpotlightLeft: 'onLeft'
		},
		bindings: [
			{
				from: '.model.name',
				to: '.$.title.content',
				transform: function(name) {
					return format(name, 'Unknown service').toUpperCase();
				}
			},
			{ from: '.model.description', to: '.$.description.content' },
			{ from: '.model.mode', to: '.$.startModePopup.modeSelected', oneWay: false },
			{ from: '.model', to: '.$.infoOptions.model', oneWay: false }
		],
		components: [
			{
				classes: 'details-actions',
				components: [
					{ name: 'infoOptions', kind: 'ServicesActions', center: false }
				]
			},
			{
				classes: 'details-desc',
				components: [
					{ name: 'title', classes: 'info-details-title' },
					{ name: 'description', content: msg.system.update, classes: 'info-details-description', allowHtml: true }
				]
			}
		],
		onContentEnter: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this,arguments);
				Context.setHelp(this.help);
			}
		}),
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

	function enablingOrDisabling(st) { return (st === 'enabling' || st === 'disabling'); }
	function isResetting(st) { return st === 'resetting'; }

	enyo.kind({
		name: 'ServicesActions',
		kind: 'components.flexLayout.Vertical',
		spotlight: 'container',
		handlers: {
			onSpotlightContainerEnter: 'onEnter'
		},
		bindings: [
			{ from: '.model.canConfigure', to: '.$.config.showing' },
			{ from: '.model.canUninstall', to: '.$.uninstall.showing' },

			{ from: '.model.status', to: '.$.status.flipped', transform: enablingOrDisabling },
			{ from: '.model.status', to: '.$.statusTransition.progressing', transform: enablingOrDisabling },
			{ from: '.model.status', to: '.$.statusTransition.content', transform: function(st) { return st === 'enabling' ? 'HABILITANDO' : 'DESHABILITANDO'; } },

			{ from: '.model.running', to: '.$.resetButton.showing', transform: util.negate },
			{ from: '.model.status', to: '.$.resetTransition.progressing', transform: isResetting },
			{ from: '.model.status', to: '.$.resetButton.flipped', transform: isResetting },

			{
				from: '.model.icon', to: '.$.img.icon',
				transform: function(icon) {
					return icon ? icon : assets('servicios-tac.png');
				}
			}
		],
		observers: [
			{ method: 'runningChanged', path: 'model.running' },
			{ method: 'statusChanged', path: 'model.status' }
		],
		components: [
			{ name: 'statusIcon', classes: 'services-actions-status-icon' },
			{
				name: 'img',
				kind: 'UpdateDetailOptions',
				classes: 'apps-infoaction-view system-img',
				icon: assets('servicios-tac.png'),
				spotlightDisabled: true, spotlight: true,
				onSpotlightBlur: 'onImgBlur',
			},
			{
				kind: 'components.VerticalMenu',
				style: 'min-width: 100%',
				components: [
				{
					name: 'status',
					kind: Shell.SimpleButton,
					mixins: [components.FlipperSupport],
					spotlight: 'container',
					components: [
						{
							name: 'statusChanger',
							spotlight: true,
							onSpotlightSelect: 'toggleEnabled'
						},
						{
							name: 'statusTransition',
							kind: components.Progress,
							bgColor: 'rgb(144, 38, 30)',
							classes: 'download-wrapper',
							style: 'font-size: inherit', // Patch download wrapper class
						}
					]
				},
				{
					name: 'config',
					kind: Shell.SimpleButton,
					content: 'EDITAR',
					onSpotlightSelect: 'config',
				},
				{
					name: 'resetButton',
					kind: Shell.SimpleButton,
					mixins: [components.FlipperSupport],
					spotlight: 'container',
					components: [
						{
							name: 'button',
							spotlight: true,
							defaultSpotlightDisappear: 'status',
							content: 'BORRAR DATOS',
							style: 'text-align: center; width:100%',
							onSpotlightSelect: 'reset',
						},
						{
							name: 'resetTransition',
							kind: components.Progress,
							content: 'BORRANDO',
							bgColor: 'rgb(144, 38, 30)',
							classes: 'download-wrapper',
							style: 'font-size: inherit', // Patch download wrapper class
						}
					]
				},
				{
					name: 'uninstall',
					kind: Shell.SimpleButton,
					content: 'DESINSTALAR',
					onSpotlightSelect: 'uninstall',
				}
			]}
		],
		onEnter: function(sender, evt) {
			if (evt.originator === this) {
				this.model.fetchState();
			}
		},
		runningChanged: function() {
			var running = this.model.get('running');
			this.$.statusIcon.addRemoveClass('stoped', !running);
			this.$.statusIcon.addRemoveClass('running', running);
		},
		statusChanged: function() {
			var st = this.model.get('status');
			if ((st === 'enabled' || st === 'disabled')) {
				this.$.statusChanger.setContent(st === 'enabled' ? 'DESHABILITAR' : 'HABILITAR');
			}
		},
		toggleEnabled: function() {
			if (this.allowAction()) {
				// If there is a button below, then spotit
				var thereIsReset = this.$.resetButton.showing && this.model.get('status') === 'disabled';
				if (this.$.config.showing || this.$.uninstall.showing || thereIsReset ) {
					enyo.Spotlight.onSpotlightEvent({type: 'onSpotlightDown'});
				}
				else {
					this.$.img.set('spotlightDisabled', false);
					enyo.Spotlight.spot(this.$.img);
				}
				this.model.toggleMode();
			}
			return true;
		},
		onImgBlur: function() {
			if (enyo.Spotlight.getCurrent() !== this.$.img) {
				this.$.img.set('spotlightDisabled', true);
			}
		},
		config: function() {
			if (this.allowAction()) {
				this.model.config();
			}
			return true;
		},
		allowAction: function() {
			var st = this.model.get('status');
			return !(enablingOrDisabling(st) || isResetting(st));
		},
		reset: function() {
			if (this.allowAction()) {
				enyo.Spotlight.States.push('focus');
				dialogs.warn({desc: msg.popup.warn.resetCongigService, owner: this}).then(function(ok) {
					if (ok) {
						enyo.Spotlight.onSpotlightEvent({type: 'onSpotlightUp'});
						this.model.resetConfig();
					}
				}.bind(this));
			}

			return true;
		},
		uninstall: function() {
			if (this.allowAction()) {
				enyo.Spotlight.States.push('focus');
				dialogs.warn({desc: msg.popup.warn.uninstallService, owner: this}).then(function(ok) {
					if (ok) {
						this.model.uninstall();
					}
				}.bind(this));
			}

			return true;
		}
	});

})();
