
(function() {
    'use strict';

    enyo.kind({
        name: 'ScanProgress',
        style: 'display: flex; justify-content: space-between; flex-direction: column;',
        model: scanModel,
        bindings: [
            { from: 'progressDescription', to: '.$.progressDescription.content' }
        ],
        components: [
            { name: 'progressDescription', style: 'font-family: Oswald-Light; font-size: 15pt; margin-bottom: 10px;' },
            {
                name: 'progress', kind: 'moon.ProgressBar',
                style: 'width: 613px; height: 6px; background-color: rgba(255, 255, 255, .2); margin: 0',
            },
        ],
        computed: [
            { method: 'progressDescription', path: ['model.percentage', 'model.network', 'model.count'] }
        ],
        observers: [
            { method: 'setProgess', path: 'model.percentage' }
		],
        create: enyo.inherit(function(sup) {
            return function() {
                sup.apply(this, arguments);
                this.listener = this.bindSafely('scanEnded');
                scanModel.on('onEndScan', this.listener);
            }
        }),
        progressDescription: function() {
            var count = this.model.get('count');
            return '{0}% ({1} Mhz) - {2} {3}.'.format(
                this.model.get('percentage'), this.model.get('network'), count,
                count === 1? 'Canal encontrado' : 'Canales encontrados'
            );
        },
        setProgess: function() {
            if (scanModel.get('isScanning')) {
                this.$.progress.animateProgressTo(this.model.get('percentage'));
            }
        },
        reset: function() {
            this.$.progressDescription.setContent('0% (000 Mhz) - 0 Canales encontrados.');
            this.$.progress.$.progressAnimator.stop();
            this.$.progress.setProgress(0);
        },
        scanEnded: function(oSender, oEventName, oEvent) {
            if (oEvent.status === 'scanEnded') {
                var res = 'No se encontraron canales.';
                if (oEvent.count === 1) {
                    res = 'Se encontró 1 Canal.'
                } else if (oEvent.count > 1) {
                    res = 'Se encontraron {0} Canales.'.format(oEvent.count);
                }
                this.$.progressDescription.setContent('Búsqueda finalizada. ' + res);
            }
        }
    });

})();
