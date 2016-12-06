(function() {

	enyo.kind({
		name: 'AboutSystem',
		kind: 'components.ViewContent',
		help: {txt: msg.help.config.system.about, img: 'singleoption-about.png'},
		components: [
			{
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left',
				components: [
					{
						kind: 'UpdateDetailOptions', classes: 'apps-infoaction-view system-img', spotlight: true,
						icon: assets('update-tac.png'),
					}
				]
			},
			{
				classes: 'viewcomponent-layout-right',
				components: [
					{ name: 'title', classes: 'info-details-title', content: msg.about.title },
					{ name: 'version', kind: 'components.Detail', style: 'margin-top: 15px', title: 'VERSIÓN:' },
					{ name: 'config', kind: 'components.Detail', title: 'CONFIGURACIÓN:' },
					{ name: 'api_level', kind: 'components.Detail', title: 'API LEVEL:' },
					{ name: 'id_', kind: 'components.Detail', title: 'ID:' },
					{ name: 'platform', kind: 'components.Detail', title: 'PLATAFORMA:' },
					{ name: 'about', classes: 'info-details-description', content: msg.about.description},
				]
			},
		],
		populate: function() {
			system.getInfo(util.catchError(function(info) {
				this.$.version.set('info', '{0} ({1}).'.format(info.system.version, info.system.commit));
				this.$.config.set('info', '{0} ({1}, {2}).'.format(info.build.name, info.build.type, info.build.config));
				this.$.api_level.set('info', '{0}.'.format(info.system.api));
				this.$.id_.set('info', '{0}'.format(info.id));
				this.$.platform.set('info', '{0}, versión: {1} ({2}).'.format(
						info.platform.name,
						info.platform.version,
						info.platform.commit
					)
				);
			}.bind(this)));
		}
	});

})();
