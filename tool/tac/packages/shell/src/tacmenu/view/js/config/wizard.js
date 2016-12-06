(function() {

	var logError = log.error.bind(log, 'WizardView', 'requestWizard');

	function requestWizard(open) {
		if (open) {
			enyo.Signals.send('onWizardRequest', {on:true, firstBoot: false});
		}
	}

	enyo.kind({
		name: 'WizardView',
		kind: 'components.ViewContent',
		help: {txt: msg.help.config.system.wizard, img: 'singleoption.png'},
		components: [
			{
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left',
				components: [
					{
						kind: 'UpdateDetailOptions', classes: 'apps-infoaction-view',
						icon: assets('wizard.png'),
					},
					{
						name: 'wizardOption',
						kind: Shell.SimpleButton,
						content: 'INICIAR ASISTENTE',
						onSpotlightSelect: 'requestWizard',
					}
				]
			},
			{
				classes: 'viewcomponent-layout-right',
				components: [
					{ name: 'title', content: 'ASISTENTE DE CONFIGURACIÓN', classes: 'info-details-title' },
					{ name: 'description', content: msg.wizard, classes: 'info-details-description' }
				]
			}
		],
		requestWizard: function(sender, evt) {
			if (channels.length) {
				enyo.Spotlight.States.push('focus');
				dialogs.warn({desc: msg.popup.warn.scanChannels, owner: this}).then(function(ok) {
					if (ok) {
						security.openSession().then(requestWizard).catch(logError);
					}
				}.bind(this));
			}
			else {
				security.openSession().then(requestWizard).catch(logError);
			}
		}
	});

})();
