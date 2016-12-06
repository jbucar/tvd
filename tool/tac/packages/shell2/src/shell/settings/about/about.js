/* global SettingView */
/* exported About */
function About(res) {

	function onReset() {
		log.info( 'About', 'onReset' );
		apiCall( system.factoryReset );
	}

	function createWidget(models, name, action) {
		let label = res.txt('btn_' + name);
		let icon = res.img(name);
		let wgt;
		if (action) {
			wgt = new models.Button( label, icon, true );
			wgt.setAction( action.bind(wgt), false );
		} else {
			wgt = new models.Label( label, icon, true );
		}
		return wgt;
	}

	let prot = {};

	prot.onconstruct = function(opts) {
		log.verbose( 'About', 'onconstruct' );

		this._super(opts);

		let svm = ui.getModels(SettingView);

		let actPanel = new svm.ActionPanel();
		this.widgets = {};

		{	// Create about
			let about = createWidget(svm, 'about');
			let panel = about.getLeftPanel();
			apiCall( system.getInfo, function(info) {
				let desc = res.txt('desc_about');
				panel.addDescription([
					sprintf(desc[0], info.system.version, info.system.commit),
					sprintf(desc[1], info.build.name, info.build.type, info.build.config),
					sprintf(desc[2], info.system.api),
					sprintf(desc[3], info.id),
					sprintf(desc[4], info.platform.name, info.platform.version, info.platform.commit),
					sprintf(desc[5])
				]);
			});
			actPanel.addWidget( about );
		}

		{	// Create reset
			let reset = createWidget(svm, 'reset', onReset);
			reset.setDialog( 'confirm', res.txt('reset_confirm_title'), res.txt('reset_confirm_text') );
			reset.getLeftPanel().addDescription([res.txt('desc_reset')]);
			actPanel.addWidget( reset );
		}

		this.model.pushActionPanel(actPanel);
	};

	prot.oncomplete = function() {
		log.info( 'About', 'oncomplete' );
		this._super();
	};

	prot.onunrender = function() {
		log.info( 'About', 'onunrender' );
		this._super();
	};

	return {
		extends: SettingView,
		proto: prot
	};
}
