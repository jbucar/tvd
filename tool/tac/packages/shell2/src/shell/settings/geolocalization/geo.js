/* exported Geolocalization */
/* globals SettingView */
function Geolocalization(res) {

	function onEnable( newState ) {
		assert(this);
		log.info( 'Geolocalization', 'onEnable: newState=%s', newState );
		apiCallAsync( system.enableGeolocation, newState )
			.then( () => notification.emit('geoEnabled', newState) )
			.finally( () => this.setActive(newState) );
		return true;
	}

	var prot = {};

	prot.onconstruct = function(opts) {
		log.info( 'Geolocalization', 'onconstruct' );

		// Call SettingView.onconstruct
		this._super(opts);

		let svm = ui.getModels(SettingView);
		let actPanel = new svm.ActionPanel();

		{	// Create about
			let icon = res.img('geolocalization');
			let geoBtn = new svm.ToggleButton( res.txt('enabled'), icon );
			geoBtn.setAction( onEnable.bind(geoBtn) );
			geoBtn.setLeftPanel( new svm.Panel(res.txt('title'), icon) );
			geoBtn.getLeftPanel().addDescription([res.txt('description')]);
			actPanel.addWidget( geoBtn );
			apiCall( system.isGeolocationEnabled, enabled => geoBtn.setActive(enabled) );
		}

		this.model.pushActionPanel(actPanel);
	};

	return {
		extends: SettingView,
		proto: prot
	};
}
