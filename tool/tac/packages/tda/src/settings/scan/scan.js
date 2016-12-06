/* exported Scan */
/* globals SettingView */
function Scan(res) {

	function initScanProgress( btn, ractive ) {
		let channels = [];
		let percentage;
		let network;

		function updateProgress() {
			if (percentage !== undefined && network !== undefined) {
				let text = percentage + '% (' + network + 'Mhz) - ' + channels.length + ' canal(es) encontrados';
				let opts = {
					scanProgressShow: true,
					scanProgressPercentage: percentage,
					scanProgressText: text,
					scanProgressChannels: channels
				};
				ractive.set(opts);
			}
		}

		tvd.on('channelAdded', function(chID) {
			apiCallAsync( tvd.channel.get, chID )
				.then(function (ch) {
					channels.push(ch);
					updateProgress();
				});
		});

		tvd.on( 'scanChanged', function(evt) {
			if (evt.state === 'network') {
				percentage = evt.percentage;
				network = evt.network;
				updateProgress();
			}
			else if (evt.state === 'end') {
				//	TODO: Remove listener
				ractive.set({scanProgressShow: false});

				toggleScanButton( btn, ractive, true, channels.length > 0 );
			}
		});
	}

	function onScan( btn, ractive ) {
		log.info( 'Scan', 'onScan' );
		assert(btn);

		initScanProgress( btn, ractive );

		apiCallAsync( tvd.tuner.startScan )
			.finally( () => toggleScanButton( btn, ractive, false ) );

		return true;
	}

	function onCancelScan( btn ) {
		log.info( 'Scan', 'onCancelScan' );
		assert(btn);
		apiCall( tvd.tuner.cancelScan );
		return true;
	}

	function toggleScanButton( btn, ractive, showScan, needConfirm ) {
		// Update name and title
		let resName;
		let resIcon;
		let fncAction;
		let title;
		let desc;

		btn.actionCompleted();

		if (showScan) {
			resName = 'btnScan';
			resIcon = 'scan';
			fncAction = $(onScan,btn,ractive);
			needConfirm = needConfirm;
			title = res.txt('carrouselName').toUpperCase();
			desc = res.txt('description');
		}
		else {
			resName = 'btnCancel';
			resIcon = 'cancel';
			fncAction = $(onCancelScan,btn);
			title = res.txt('scan_title');
		}

		let name = res.txt( resName );
		let img = res.img( resIcon );
		btn.setName( name );
		btn.setIcon( img );
		btn.setAction( fncAction );
		if (needConfirm) {
			btn.setDialog( 'confirm', res.txt('scan_confirm_title'), res.txt('scan_confirm_text') );
		}

		{	//	Change left panel text
			let panel = ractive.model.getLeftPanel();
			panel.setTitle( title );
			panel.clearDescriptions();
			if (desc) {
				panel.addDescription([desc]);
			}
		}
	}

	function updateVisibles( ractive, isEnabled, isBlocked ) {
		log.verbose( 'Restrictions', 'updateVisibles: isEnabled=%s, isBlocked=%s', isEnabled, isBlocked );

		ractive.buttons.initSession.setVisible( isBlocked );
		ractive.buttons.scan.setVisible( !(isEnabled && isBlocked) );

		ractive.resetFocus();
	}

	var prot = {
		css: res.style('progress')
	};

	prot.oninit = function(opts) {
		log.info( 'Scan', 'oninit' );

		// Call SettingView.oninit
		this._super(opts);

		let session = model.get('session');
		let svm = ui.getModels(SettingView);
		let actPanel = new svm.ActionPanel();
		this.buttons = {};

		{	// Create initialize session
			let btn = new svm.Button( session.btnText(), session.btnImage() );
			btn.setAction( () => session.unblock() );
			this.buttons.initSession = btn;
			actPanel.addWidget( btn );
		}

		{	// Create scan button
			let btn = new svm.Button( res.txt('btnScan'), res.img('scan') );
			btn.setAction( $(onScan,btn) );
			let needConfirm = model.get('channels').getAll().length > 0;
			if (needConfirm) {
				btn.setDialog( 'confirm', res.txt('scan_confirm_title'), res.txt('scan_confirm_text') );
			}
			this.buttons.scan = btn;
			actPanel.addWidget( btn );

			//	Update state
			apiCallAsync( tvd.tuner.isScanning )
				.then( (isScanning) => toggleScanButton( btn, this, !isScanning, needConfirm ) );
		}

		{	//	Left panal
			let leftPanel = this.model.getLeftPanel();
			leftPanel.setTitle( res.txt('carrouselName').toUpperCase() );
			leftPanel.setIcon( res.img('scan') );
			leftPanel.addDescription([res.txt('description')]);
			leftPanel.setExtraContent( res.tpl('progress') );
		}

		this.model.pushActionPanel(actPanel);

		{	//	Session handling
			updateVisibles( this, session.isEnabled(), session.isBlocked() );
			session.on('changed', $(updateVisibles,this) );
		}
	};

	prot.onteardown = function() {	//	TODO: Esto solo debe llamarse cuando se destruye (una unica vez)
		log.info( 'Scan', 'onteardown' );

		// Call SettingView
		this._super();

		//	Cancel scan if necesary
		apiCall( tvd.tuner.cancelScan );
	};

	return {
		extends: SettingView,
		proto: prot
	};
}
