
(function() {

	enyo.kind({
		name: 'GingaView',
		kind: 'components.ViewContent',
		help: {txt: msg.help.config.tda.ginga, img: 'singleoption.png'},
		bindings: [
			{
				from: '^gingaModel.enabled', to: '.$.enableOption.content', transform: function(enabled) {
					return enabled ? 'DESHABILITAR' : 'HABILITAR';
				}
			}
		],
		components: [
			{
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left',
				components: [
					{
						name: 'image',
						kind: 'UpdateDetailOptions',
						classes: 'apps-infoaction-view',
						icon: assets('Ginga_config.png'),
					},
					{
						name: 'enableOption',
						kind: Shell.SimpleButton,
						onSpotlightSelect: 'enable',
					}
				]
			},
			{
				classes: 'viewcomponent-layout-right',
				components: [
					{ name: 'title', content: 'GINGA.AR', classes: 'info-details-title' },
					{ name: 'description', content: msg.ginga, classes: 'info-details-description' }
				]
			}
		],
		enable: function() {
			gingaModel.enable(!gingaModel.get('enabled'));
		}
	});

})();
