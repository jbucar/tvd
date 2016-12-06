(function() {

	enyo.kind({
		name: 'RestoreView',
		kind: 'components.ViewContent',
		help: {txt: msg.help.config.system.restore, img: 'singleoption.png'},
		components: [
			{
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left',
				components: [
					{
						name: 'app',
						kind: 'UpdateDetailOptions',
						classes: 'apps-infoaction-view',
						icon: assets('restablecer.png'),
					},
					{
						name: 'restoreOption',
						kind: Shell.SimpleButton,
						content: 'RESTABLECER',
						onSpotlightSelect: 'restore'
					}
				]
			},
			{
				classes: 'viewcomponent-layout-right',
				components: [
					{ name: 'title', content: 'RESTAURAR SISTEMA', classes: 'info-details-title' },
					{ name: 'description', content: msg.restore, classes: 'info-details-description' }
				]
			},
		],
		restore: function() {
			enyo.Spotlight.States.push('focus', this.$.restoreOption);
			dialogs.warn({desc: msg.popup.warn.refactoryReset, owner: this}).then(function(ok) {
				if (ok) {
					system.factoryReset();
				}
			});
		}
	});

})();
