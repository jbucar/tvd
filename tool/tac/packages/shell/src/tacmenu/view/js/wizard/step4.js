"use strict";

(function() {
	var logStartError = log.error.bind(log, 'UpdateChannelsView', 'startScan');

	enyo.kind({
		name: 'Step4',
		kind: 'WizardPanel',
		next: 'INICIAR TAC TV',
		stepIndex: 2,
		components: [
			{ name: 'scanChannels', kind: 'WizardScanChannels' }
		],
		transitionStarted: function() {
			this.$.scanChannels.resetScan();
		},
		transitionFinished: function(info) {
			if (info.to === info.index) {
				this.$.scanChannels.startScan();
			}
		},
		leftBtnAction: enyo.inherit(function(sup) {
			return function(oSender, oEvent) {
				sup.apply(this, arguments);
				this.$.scanChannels.resetScan();
			}
		}),
		rightBtnAction: enyo.inherit(function(sup) {
			return function(oSender, oEvent) {
				sup.apply(this, arguments);
				this.$.scanChannels.resetScan();
				enyo.Signals.send('onScanInfoViewed');
				enyo.Signals.send('onWizardRequest', {on:false});
			}
		})
	});

	/**
	 * WizardScanChannels
	 */
	enyo.kind({
		name: 'WizardScanChannels',
		classes: 'wizard-network-view',
		model: scanModel,
		spotlight: false,
		observers: [
			{ method: 'scanningStateChanged', path: 'model.isScanning' }
		],
		components: [
			{ name: 'title', classes: 'wizard-title', content: 'BUSCAR CANALES'},
			{ name: 'description', classes: 'wizard-content', content: 'Esta búsqueda puede tardar unos minutos, por favor espere hasta que haya finalizado.', style: 'height: 60px' },
			{ name: 'scanningInfo', kind: 'scanChannels.impl.scanningInfo', style: 'text-align: center;', showing: true },
		],
		scanningStateChanged: function() {
			if (scanModel.get('isScanning')) {
				this.$.scanningInfo.set('chCollection', channels);
			}
		},
		startScan: function(sender, evt) {
			scanModel.start();
		},
		resetScan: function() {
			scanModel.stop();
			this.$.scanningInfo.reset();
		}
	});
})();