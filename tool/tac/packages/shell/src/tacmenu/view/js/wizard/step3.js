enyo.kind({
	name: 'Step3',
	kind: 'WizardPanel',
	classes: 'wifi-step',
	stepIndex: 1,
	handlers: {
		onSpotlightContainerLeave: 'onLeave'
	},
	model: WirelessConnection,
	bindings: [
		{ from: '.model.isConnecting', to: '.$.connectTransition.progressing' },
		{ from: '.model.isConnecting', to: '.$.connectButton.flipped' },
	],
	components: [
		{ name: 'hiddenConnect', kind: 'internet.HiddenConnectionDialog', context: 'wizard' },
		{
			classes: 'wizard-network-view',
			components: [
				{ name: 'title', classes: 'wizard-title', content: 'INTERNET - WIFI' },
				{ name: 'description', classes: 'wizard-content', content: 'Seleccione la red que desee y presione OK' },
				{ name: 'wifilist', kind: 'NetworkList', context: 'wizard' },
				{
					name: 'connectButton',
					kind: 'WizardButton',
					style: 'margin-right:70px;',
					mixins: [components.FlipperSupport],
					spotlight: 'container',
					components: [
						{
							name: 'button',
							spotlight: true,
							content: 'AGREGAR RED',
							style: 'text-align: center; width: 185px; height: 32px; line-height: 40px; font-size: 22px;',
							onSpotlightSelect: 'openConnectDialog',
						},
						{
							name: 'connectTransition',
							kind: components.Progress,
							bgColor: 'rgb(147, 201, 246)',
							content: 'CONECTANDO',
							classes: 'download-wrapper',
							spotlight: false,
							style: 'width: 185px; font-size: 22px;', // Patch download wrapper class
						}
					]
				}
			]
		}
	],
	hideConnectingChanged: function(was, is) {
		if (is) {
			WirelessAPs.sync(true);
			this.$.connectButton.removeClass('spotlight');
		}
	},
	transitionFinished: function(info) {
		if (info.to === info.index) {
			Connections.listen(true);
			Connections.sync();
			WirelessAPs.sync(true);
		}
	},
	openConnectDialog: function() {
		enyo.Spotlight.States.push('focus', this);
		this.$.hiddenConnect.show();
	},
	onLeave: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			Connections.listen(false);
		}
	}
});
