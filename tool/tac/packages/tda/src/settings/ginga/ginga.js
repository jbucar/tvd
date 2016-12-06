/* exported Ginga */
/* globals SettingView */
function Ginga(res) {
	function toggleGinga( btn, setState ) {
		log.info( 'Ginga', 'toggle Ginga: setState:%s', setState );
		assert(btn);
		apiCallAsync( tvd.middleware.enableMiddleware, setState )
			.finally( () => btn.setActive(setState) );
		return true;
	}

	var prot = {};

	prot.init = function(opts) {
		log.info( 'Ginga', 'oninit' );

		// Call SettingView
		this._super(opts);

		let svm = ui.getModels(SettingView);

		let actPanel = new svm.ActionPanel();

		{	// Create scan button
			let icon = res.img('ginga');
			let btn = new svm.ToggleButton( res.txt('btnGinga'), icon );
			btn.setLeftPanel( new svm.Panel(res.txt('title'), icon) );
			btn.setAction( $(toggleGinga,btn) );
			btn.setActive( true );
			btn.getLeftPanel().addDescription([res.txt('description')]);
			actPanel.addWidget( btn );
			apiCall( tvd.middleware.isMiddlewareEnabled, enabled => btn.setActive(enabled) );
		}

		this.model.pushActionPanel(actPanel);
	};

	return {
		extends: SettingView,
		proto: prot
	};
}

