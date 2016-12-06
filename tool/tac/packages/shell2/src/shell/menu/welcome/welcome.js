/* globals ui, Dialog */
/* exported Welcome */
function Welcome(res) {

	function onconfig() {
		log.info( 'Welcome', 'onconfig' );

		let theme = ui.getTheme();

		ui.setDefaults(this, {
			title: res.txt('welcome'),
			description: res.txt('welcome_desc'),
			'theme.bgColor': theme.bgColor,
			'theme.fgColor': theme.fgColor,
			'theme.focusColor': theme.focusColor,
		});

		this.on( 'dialogResult', res => {
			if (res) {
				apiCall(system.licenseAccepted);
				this.parent.focusSection(0, {crsl: 0});
			}
		});
	}

	return {
		proto: {
			template: res.tpl('welcome'),
			css: res.styles('welcome'),
			components: ui.getComponents( Dialog ),
			onconfig: onconfig,
		}
	};
}
