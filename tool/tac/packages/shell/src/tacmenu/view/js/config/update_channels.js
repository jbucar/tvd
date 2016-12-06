
(function() {

	var logStartError = log.error.bind(log, 'UpdateChannelsView', 'startScan');

	/**
	 * scanChannels.impl.ChannelItem
	 */
	enyo.kind({
		name: 'scanChannels.impl.ChannelItem',
		style: 'border-bottom: 1px solid rgba(255, 255, 255, .3); font-family: Oswald-light; font-size: 15pt; text-indent: 10px; height: 30px', // TODO
		showing: false,
		bindings: [
			{ from: '.model.name', to: '.content' },
		],
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.bubble('onItemCreated', this);
			}
		}),
	});

	/**
	 * UpdateChannelsView
	 */
	enyo.kind({
		name: 'UpdateChannelsView',
		kind: 'components.ViewContent',
		showing: false,
		model: scanModel,
		help: {txt: msg.help.scan.ch, img: 'singleoption.png'},
		bindings: [
			{ from: '.model.isScanning', to: '.$.flipper.flipped' }
		],
		observers: [
			{ method: 'scanningStateChanged', path: 'model.isScanning' }
		],
		components: [
			{
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left',
				components: [
					{
						name: 'app',
						kind: 'UpdateDetailOptions',
						classes: 'apps-infoaction-view',
						icon: assets('tda_buscar.png'),
					},
					{
						name: 'flipper',
						kind: components.Flipper,
						style: 'width: 100%; margin-top: 10px; display: flex; justify-content: center',
						height: 45, // 41px + 4 border
						components: [
							{
								name: 'scan',
								kind: Shell.SimpleButton,
								onSpotlightSelect: 'scanPressed',
								content: 'BUSCAR',
								style: 'margin: 0',
							},
							{
								name: 'progress',
								kind: Shell.SimpleButton,
								onSpotlightSelect: 'cancelPressed',
								content: 'CANCELAR BÚSQUEDA',
								style: 'margin: 0',
							}
						]
					}
				]
			},
			{
				classes: 'viewcomponent-layout-right',
				components: [
					{ name: 'title', classes: 'info-details-title', content: 'BUSCAR CANALES'},
					{ name: 'description', classes: 'info-details-description', content: 'Si desea encontrar nuevos canales presione el botón BUSCAR.', style: 'height: 60px' },
					{ name: 'scanningInfo', kind: 'scanChannels.impl.scanningInfo' },
				]
			},
			{ kind: 'Signals', onScanInfoViewed: 'onViewed' }
		],
		startScan: function() {
			security.openSession().then(function(open) {
				if (open) {
					this.$.scanningInfo.reset();
					scanModel.start();
				}
			}.bind(this)).catch(logStartError);

			return true;
		},
		scanningStateChanged: function() {
			if (scanModel.get('isScanning')) {
				this.$.scanningInfo.set('chCollection', channels);
				this.$.scanningInfo.show();
			}
		},
		scanPressed: function(sender, evt) {
			if (channels.length) {
				enyo.Spotlight.States.push('focus', this.$.flipper);
				dialogs.warn({desc: msg.popup.warn.scanChannels, owner: this}).then(function(ok) {
					if (ok) {
						this.startScan();
					}
				}.bind(this));
			}
			else {
				this.startScan();
			}
		},
		cancelPressed: function(sender, evt) {
			this.$.scanningInfo.stop();
			scanModel.stop();
			return true;
		},
		onContentEnter: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.flipper.setOrigin(22.5);
				Context.setHelp(this.help);
			}
		}),
		onContentLeave: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.flipper.setOrigin(15);

				if (!scanModel.get('isScanning')) {
					enyo.Signals.send('onScanInfoViewed');
				}
			}
		}),
		onViewed: function() {
			this.$.scanningInfo.hide();
			this.$.scanningInfo.reset();
		}
	});

	/**
	 * scanChannels.impl.scanningInfo
	 */
	enyo.kind({
		name: 'scanChannels.impl.scanningInfo',
		style: 'width: 613px',
		showing: false,
		handlers: {
			onItemCreated: 'itemCreated'
		},
		components: [
			{ name: 'scanProgress', kind: 'ScanProgress', style: 'height: 50px' },
			{
				style: 'height: 155px; margin-top: 30px; overflow: hidden',
				components: [
					{
						name: 'channels',
						kind: 'enyo.DataRepeater',
						containerOptions: { style: 'display: flex; flex-direction: column-reverse; flex-wrap: wrap;' },
						style: 'position: relative; top: -30px',
						components: [
							{ kind: 'scanChannels.impl.ChannelItem' }
						]
					}
				]
			},
			{ kind: 'Animator', duration: 600, onStep: 'animatorStep', onEnd: 'animate' },
		],
		bindings: [
			{ from: '.chCollection', to: '.$.channels.collection' }
		],
		//private
		_animsQueue: [],
		animatorStep: function(sender) {
			this.$.channels.applyStyle('transform', 'translateY(' + sender.value + 'px)');
		},
		animate: function() {
			if (this._animsQueue.length > 0 && !this.$.animator.isAnimating()) {
				var item = this._animsQueue.shift();
				item.show();
				this.$.animator.play({
					startValue: 0,
					endValue: 30,
					node: this.hasNode()
				});
			}
		},
		itemCreated: function(sender, item) {
			this._animsQueue.push(item);
			this.animate();
		},
		reset: function() {
			this._animsQueue = [];
			this.$.scanProgress.reset();
			this.$.channels.destroyClientControls();
		},
		stop: function() {
			this._animsQueue = [];
		}
	});

})();