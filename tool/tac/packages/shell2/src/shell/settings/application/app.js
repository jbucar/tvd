/* exported ApplicationSetting */
/* globals model, SettingView */
function ApplicationSetting(res) {

	function onRun( appInfo ) {
		log.info( 'ApplicationSetting', 'onRun' );
		apiCall( appmgr.run, appInfo.id );
	}

	function onToggleFav(btn, appInfo,enable) {
		log.info( 'ApplicationSetting', 'onToggleFav: isFav=%s, newState=%s', appInfo.isFavorite, enable );
		assert( enable !== appInfo.isFavorite, 'Favorite toggle state differ from app favorite state');
		model.get('apps').toogleFavorite(appInfo.id)
			.finally( () => btn.setActive( appInfo.isFavorite ) );
		return true;
	}

	function onUninstall(ractive, appInfo) {
		log.info( 'ApplicationSetting', 'onUninstall' );
		model.get('apps').uninstall( appInfo.id );
		ractive.parent.fire('goToMenu');
	}

	let prot = {};

	prot.oninit = function() {
		log.info( 'ApplicationSetting', 'oninit' );

		// Call SettingView.oninit
		this._super();

		let appInfo = this.get('screenParams');
		let svm = ui.getModels(SettingView);

		//	Setup action panel
		let actPanel = new svm.ActionPanel();

		{	//	Run button
			let appRun = new svm.Button( res.txt('btnRun'), res.img('run') );
			appRun.setAction( $(onRun,appInfo) );
			actPanel.addWidget( appRun );
		}

		{	//	Fav button
			let appFav = new svm.ToggleButton( res.txt('btnFav'), res.img('fav') );
			appFav.setAction( $(onToggleFav,appFav,appInfo) );
			appFav.setActive( appInfo.isFavorite );
			actPanel.addWidget( appFav );
		}

		if (appInfo.canUninstall) {
			let appDel = new svm.Button( res.txt('btnUninstall'), res.img('uninstall') );
			appDel.setAction( $(onUninstall,this,appInfo) );
			appDel.setDialog( 'confirm', res.txt('uninstall_app_title'), res.txt('uninstall_app_text') );
			actPanel.addWidget( appDel );
		}

		{	//	Left panal
			let leftPanel = this.model.getLeftPanel();
			leftPanel.setTitle( appInfo.name );
			leftPanel.setIcon( appInfo.icon );
			leftPanel.addDescription([
				res.txt('vendor') + ': ' + appInfo.vendor,
				res.txt('version') + ': ' + appInfo.version,
			]);
			leftPanel.addDescription([appInfo.description]);
		}

		this.model.pushActionPanel(actPanel);
	};

	return {
		extends: SettingView,
		proto: prot
	};
}
