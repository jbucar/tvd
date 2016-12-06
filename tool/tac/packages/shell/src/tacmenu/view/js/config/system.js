
/*
 * This is the sub menu for the System tab.
 */

(function() {

	enyo.kind({
		name: 'SystemView',
		kind: View,
		help: {txt: msg.help.config.system.submenu, img: 'submenu.png'},
		context: 'config',
		layerClass: 'config-layer',
		tabBindings: {
			about: 'aboutView',
			update: 'updateView',
			restore: 'restoreView',
			wizard: 'wizardView'
		},
		menuOptions: { classes: 'config-submenu view-in-view-menu' },
		menuComponents: [
			{
				name: 'about',
				kind: 'ConfigTabBtn',
				label: 'ACERCA DE',
				icon: 'submenu_acercade.png',
			},
			{
				name: 'update',
				kind: 'ConfigTabBtn',
				label: 'ACTUALIZAR',
				icon: 'submenu_actualizar.png',
			},
			{
				name: 'wizard',
				kind: 'ConfigTabBtn',
				label: 'ASISTENTE',
				icon: 'submenu_wizard.png',
			},
			{
				name: 'restore',
				kind: 'ConfigTabBtn',
				label: 'RESTABLECER',
				icon: 'submenu_restablecer.png',
			},
		],
		bindings: [
			{ from: '^Update.status', to: '.updatest' },
		],
		components: [
			{ name: 'aboutView', kind: 'AboutSystem' },
			{ name: 'updateView', kind: 'UpdateView', showing: false },
			{ name: 'wizardView', kind: 'WizardView', showing: false },
			{ name: 'restoreView', kind: 'RestoreView', showing: false },
		],
		onFirstEnter: function() {
			this.$.aboutView.populate();
		},
		updatestChanged: function(was, is) {
			if (
				(enyo.Spotlight.getCurrent() === this.$.update) &&
				((is === 'downloaded') || (is === 'available'))
			) {
				enyo.asyncMethod(this.selectTab.bind(this, 'update', true));
			}
		}
	});

	//TODO: move update view code to update.js

	enyo.kind({
		name: 'UpdateView',
		style: 'height:100%;',
		components: [
			{ kind: 'update.DownloadView' },
			{ kind: 'update.InstallView' },
			{ kind: 'update.SearchView' }
		]
	});

	enyo.kind({
		name: 'update.SearchView',
		kind: 'components.ViewContent',
		classes: 'update-search',
		help: {txt: msg.help.config.system.update, img: 'singleoption-about.png'},
		bindings: [
			{ from: '^Update.status', to: '.showing', transform: function(st) { return st === 'idle' || st === 'checking' || st === 'error'; } },
			{ from: '^Update.status', to: '.$.flipper.flipped', transform: function(st) { return st === 'checking'; } }
		],
		components: [
			{
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left',
				components: [
					{
						name: 'app', kind: 'UpdateDetailOptions', classes: 'apps-infoaction-view',
						icon: assets('update-tac.png'),
					},
					{
						name: 'flipper',
						kind: 'components.Flipper',
						classes: 'download-flipper',
						height: 50,
						components: [
							{
								kind: Shell.SimpleButton,
								content: 'BUSCAR',
								onSpotlightSelect: 'checkUpdate',
							},
							{
								classes: 'download-wrapper download',
								content: 'BUSCANDO...',
							}
						]
					}
				],
			},
			{
				classes: 'viewcomponent-layout-right',
				components: [
					{ name: 'title', content: 'ACTUALIZACIÓN DEL SISTEMA', classes: 'info-details-title' },
					{
						name: 'lastSearch',
						classes: 'system-last-update',
						kind: 'components.Detail',
						title: 'ÚLTIMA BÚSQUEDA:',
						info: 'No disponible'
					},
					{ name: 'description', content: msg.system.update, classes: 'info-details-description' },
				]
			}
		],
		checkUpdate: function() {
			enyo.Spotlight.onSpotlightEvent({type: 'onSpotlightUp'}); // NOTE: tightly coupled to the button focused element position- Review this
			Update.search();
		},
		onContentEnter:  enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				var last = Update.get('lastSearch');
				if (last) {
					var minutesAgo = Math.round((new Date() - last) / 60000);
					this.$.lastSearch.set('info', 'Hace {0} minuto{1}.'.format(minutesAgo, minutesAgo !== 1 ? 's' : ''));
				}
			};
		})
	});

	// converts received value (bytes) to kilobytes or megabytes for showing purpose.
	function formatSize(value) {
		var converted_value = 0;
		var unit = ' KB';
		var size = '';
		if (value) {
			converted_value = Math.round(value/1024);	// Kilobytes.
			if (converted_value >= 1024) {
				converted_value = Math.round(converted_value/1024);	// Megabytes.
				unit = ' MB';
			}
			size = converted_value + unit;
		}

		return size;
	}

	enyo.kind({
		name: 'update.DownloadView',
		kind: 'components.ViewContent',
		help: {txt: msg.help.config.system.update, img: 'singleoption-about.png'},
		bindings: [
			{ from: '^Update.status', to: '.showing', transform: function(st) { return st === 'available' || st === 'downloading'; } },
			{ from: '^Update.status', to: '.$.flipper.flipped', transform: function(st) { return st === 'downloading'; } },
			{ from: '^Update.status', to: '.$.downloadbutton.spotlightDisabled', transform: function(st) { return st === 'downloading'; } },
			{ from: '.info.size', to: '.$.updateSize.info', transform: formatSize },
			{ from: '^Update.progress', to: '.downloadProgress' },
			{ from: '^Update.info', to: '.info' },
			{
				from: '.info.name', to: '.$.updateName.info',
				transform: function(value) {
					return value;
				}
			},
			{
				from: '.info.version', to: '.$.updateVersion.info',
				transform: function(value) {
					return value;
				}
			},
			{
				from: '.info.description', to: '.$.updateDescription.info',
				transform: function(value) {
					return value;
				}
			},

		],
		components: [
			{
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left',
				components: [
					{
						name: 'app', kind: 'UpdateDetailOptions', classes: 'apps-infoaction-view',
						icon: assets('download.png')
					},
					{
						name: 'flipper',
						kind: 'components.Flipper',
						classes: 'download-flipper',
						height: 45,
						components: [
							{
								name: 'downloadbutton',
								kind: Shell.SimpleButton,
								content: 'DESCARGAR',
								onSpotlightSelect: 'downloadUpdate',
							},
							{
								name: 'progress', kind: 'components.ProgressLabel', content: 'DESCARGANDO',
								barClasses: 'download-bgbar', style: 'background-color: rgba(12, 13, 13, 0.7);'
							}
						]
					}
				]
			},
			{
				classes: 'viewcomponent-layout-right',
				components: [
					{ name: 'title', classes: 'info-details-title', content: '¡NUEVA ACTUALIZACIÓN DISPONIBLE!' },
					{ name: 'updateName', kind: 'components.Detail', title: 'NOMBRE:' },
					{ name: 'updateVersion', kind: 'components.Detail', title: 'VERSIÓN:' },
					{ name: 'updateDescription', kind: 'components.Detail', title: 'DESCRIPCIÓN:' },
					{ name: 'updateSize', kind: 'components.Detail', title: 'TAMAÑO:' },
					{ name: 'about', classes: 'info-details-description', content: msg.system.download }
				]
			},
		],
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				Update.on('FirmwareUpdateFetched', this.$.progress.setProgress.bind(this.$.progress, 0));
			};
		}),
		downloadProgressChanged: function(was, is) {
			this.$.progress.animateProgressTo(is);
		},
		downloadUpdate: function() {
			enyo.Spotlight.onSpotlightEvent({type: 'onSpotlightUp'}); // NOTE: tightly coupled to the button focused element position- Review this
			Update.fetch();
		}
	});

	enyo.kind({
		name: 'update.InstallView',
		kind: 'components.ViewContent',
		help: {txt: msg.help.config.system.update, img: 'singleoption-about.png'},
		bindings: [
			{ from: '^Update.status', to: '.showing', transform: function(st) { return st === 'downloaded' || st === 'updating'; } },
			{ from: '^Update.info', to: '.info' },
			{
				from: '.info.name', to: '.$.updateName.content', transform: function(value) {
					return 'Nombre: ' + value;
				}
			},
			{
				from: '.info.version', to: '.$.updateVersion.content', transform: function(value) {
					return 'Versión: ' + value;
				}
			},
			{
				from: '.info.description', to: '.$.updateDescription.content', transform: function(value) {
					return 'Descripción: ' + value;
				}
			}
		],
		components: [
			{
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left',
				components: [
					{
						name: 'app', kind: 'UpdateDetailOptions', classes: 'apps-infoaction-view',
						icon: assets('install.png')
					},
					{
						kind: Shell.SimpleButton,
						content: 'INSTALAR',
						onSpotlightSelect: 'showWarning'
					}
				],
			},
			{
				classes: 'viewcomponent-layout-right',
				components: [
					{ name: 'title', classes: 'info-details-title', content: 'INSTALAR ACTUALIZACIÓN' },
					{ name: 'updateName', classes: 'apps-data-field' },
					{ name: 'updateVersion', classes: 'apps-data-field' },
					{ name: 'updateDescription', classes: 'apps-data-field' },
					{ name: 'about', classes: 'info-details-description', content: msg.system.install }
				]
			}
		],
		showWarning: function() {
			enyo.Spotlight.States.push('focus');
			warnInstallUpdate.show();
		}
	});

	var WarnInstallKind = enyo.kind({
		kind: 'ConfirmDialog',
		context: 'config',
		acceptLabel: 'INSTALAR',
		title: 'ADVERTENCIA',
		description: msg.popup.warn.installFirmware,
		icon: 'dialog_warn.png',
		showing: false,
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.on('accepted', function installUpdate() {
					Update.install();
				});
			};
		})
	});

	var warnInstallUpdate = new WarnInstallKind();

})();
