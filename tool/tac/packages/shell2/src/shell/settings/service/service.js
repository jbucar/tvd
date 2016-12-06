/* exported ServiceSetting */
/* globals SettingView */
function ServiceSetting(res) {

	function updateServiceStatus(btn, srvInfo, enabled, panel) {
		srvInfo.isEnabled = enabled;
		btn.setActive( enabled );
		if (enabled) {
			apiCallAsync( srvmgr.isRunning, srvInfo.id )
				.then(result => panel.setDescription(0, getDescription(srvInfo, result)));
		} else {
			panel.setDescription(0, getDescription(srvInfo, false));
		}
	}

	function onToggleEnable(btn, panel, srvInfo, enable) {
		log.info( 'ServiceSetting', 'onToggleEnable: enable=%s', enable );
		apiCallAsync( srvmgr.enable, srvInfo.id, enable ).finally( () => updateServiceStatus(btn, srvInfo, enable, panel) );
		return true;
	}

	function onResetConfiguration(btn, srvInfo) {
		log.info( 'ServiceSetting', 'onResetConfiguration' );
		apiCallAsync( srvmgr.resetConfig, srvInfo.id )
			.finally( () => btn.actionCompleted() );
		return true;
	}

	function onUninstall(btn, srvInfo, ractive) {
		log.info( 'ServiceSetting', 'onUninstall' );
		apiCallAsync( pkgmgr.uninstall, {id: srvInfo.pkgID} ).finally( () => {
			btn.actionCompleted();
			ractive.parent.fire('goToMenu');
		});
		return true;
	}

	function onConfigure(srvInfo) {
		log.info( 'ServiceSetting', 'onConfigure' );
		apiCall( appmgr.configureService, srvInfo.id );
	}

	function getDescription(srvInfo, running) {
		return [
			res.txt('vendor')  + ': ' + srvInfo.vendor,
			res.txt('version') + ': ' + srvInfo.version,
			res.txt('status') + ': ' + res.txt( srvInfo.isEnabled ? (running ? 'active' : 'inactive') : 'disabled' ),
		];
	}

	var prot = {};

	prot.oninit = function() {
		log.info( 'ServiceSetting', 'oninit' );

		// Call SettingView.oninit
		this._super();

		let srvInfo = this.get('screenParams');

		let leftPanel = this.model.getLeftPanel();
		leftPanel.setTitle( srvInfo.name );
		leftPanel.setIcon( srvInfo.icon );
		leftPanel.addDescription(getDescription(srvInfo, false));
		leftPanel.addDescription([srvInfo.description]);

		let svm = ui.getModels(SettingView);

		let actPanel = new svm.ActionPanel();

		{	//	Enable/disable button
			let btn = new svm.ToggleButton( res.txt('enabled'), res.img('default') );
			btn.setAction( $(onToggleEnable, btn, leftPanel, srvInfo) );
			btn.setActive( srvInfo.isEnabled );
			actPanel.addWidget( btn );

			apiCallAsync( srvmgr.isEnabled, srvInfo.id )
				.then( isEnabled => updateServiceStatus(btn, srvInfo, isEnabled, leftPanel) );
		}

		if (srvInfo.canConfigure) {
			let btn = new svm.Button( res.txt('btnConfig'), res.img('config') );
			btn.setAction( $(onConfigure, srvInfo) );
			actPanel.addWidget( btn );
		}

		{	//	Reset button
			let btn = new svm.Button( res.txt('btnReset'), res.img('reset') );
			btn.setAction( $(onResetConfiguration,btn,srvInfo) );
			btn.setDialog( 'confirm', res.txt('btnReset').toUpperCase(), res.txt('resetText') );
			actPanel.addWidget( btn );
		}

		if (srvInfo.canUninstall) {
			let btn = new svm.Button( res.txt('btnUninstall'), res.img('delete') );
			btn.setAction( $(onUninstall,btn,srvInfo,this) );
			btn.setDialog( 'confirm', res.txt('btnUninstall').toUpperCase(), res.txt('uninstallText') );
			actPanel.addWidget( btn );
		}

		this.model.pushActionPanel(actPanel);
	};

	return {
		extends: SettingView,
		proto: prot
	};
}
