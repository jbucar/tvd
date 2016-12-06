/* global ui, SettingView */
/* exported Inet */
function Inet(res) {
	let svm = ui.getModels(SettingView);

	function onActiveConnectionInfo(panel, con) {
		log.verbose( 'Inet', 'onActiveConnectionInfo: con=%j', con );
		if (con.state === 'connected' ) {
			panel.setDescription(0, [
				sprintf('%s: %s', res.txt('connectedTo'), res.txt(con.type)),
				sprintf('%s: %s', res.txt('ip'), con.ip),
				sprintf('%s: %s', res.txt('mac'), con.mac),
				sprintf('%s: %s', res.txt('gw'), con.gw),
				sprintf('%s: %s', res.txt('dns'), con.dns),
			]);
		}
	}

	function doConnectWiFi(btn, ractive, ssid, pass) {
		log.verbose( 'Inet', 'doConnectWiFi: ssid=%s, pass=%s', ssid, pass);
		let opts = {
			ssid: ssid,
			pass: pass,
			autoconnect: true
		};
		apiCallAsync( network.connectWifi, opts )
			.catch( err => log.error( 'Inet', 'Cannot connect to wifi: err=%s', err.message ) )
			.finally(function() {
				btn.actionCompleted();
				ractive.model.popActionPanel();
			});
	}

	function onPassword(btn, ractive, ssid, data) {
		switch (data.status) {
			case 'accepted':
				doConnectWiFi(btn, ractive, ssid, data.input);
				break;
			case 'cancelled':
				btn.actionCompleted();
				break;
		}
	}

	function getKeyboardParams(net) {
		return {
			title: net.name,
			placeholder: res.txt('password_placeholder'),
			type: 'password',
			bgColor: 'rgb(26,46,53)',
			bgImage: res.img('../background'),
		};
	}

	function onConnectWifi( btn, ractive, net ) {
		assert(btn);
		assert(ractive);
		assert(net);
		assert(net.name);
		log.verbose( 'Inet', 'Connect to wifi: net=%s', net.name );
		if (net.isSecured) {
			apiCallAsync( appmgr.showVirtualKeyboard, getKeyboardParams(net) )
				.then( data => onPassword( btn, ractive, net.name, data ) );
		} else {
			doConnectWiFi(btn, ractive, net.name);
		}

		return true;
	}

	function createWifiBtn( ractive, net ) {
		assert(ractive);
		assert(net);
		assert(net.name);
		let icon = 'icons/wifi';
		let signal = parseInt(net.signal) - 1;
		icon += parseInt(signal / 20);
		if (net.isSecured) {
			icon += 'b';
		}
		let btn = new svm.Button( net.name, res.img(icon) );
		btn.setAction( $(onConnectWifi,btn,ractive,net) );
		return btn;
	}

	function createWifiListPanel() {
		let actPanel = new svm.ActionPanel();
		{  // Empty list
			let nfound = 'wifiNotFound';
			let widgetlab = new svm.Label( res.txt(nfound), res.img(nfound), true );
			widgetlab.getLeftPanel().addDescription( [res.txt(nfound + 'Description')]);
			actPanel.addWidget( widgetlab );
		}

		{	//	Create left panel
			let leftPanel = new svm.Panel( res.txt('wifiAvailables'), res.img('wifi') );
			leftPanel.addDescription([res.txt('wifiAvailablesDescription')]);
			actPanel.setLeftPanel( leftPanel );
		}

		return actPanel;
	}

	function btnOnSearch( btn, ractive ) {
		log.verbose( 'Inet', 'On search networks' );
		assert(btn);
		assert(ractive);

		apiCallAsync( network.getWifiList )
			.then(function(list) {
				btn.actionPanel.truncateWidgets(1);
				if (list.length > 0) {
					list.filter( net => net.name && net.name.length )
						.forEach( net => btn.actionPanel.addWidget( createWifiBtn(ractive, net) ) );
					btn.actionPanel.widgets[0].setVisible(false);
				} else {
					btn.actionPanel.widgets[0].setVisible(true);
				}
				btn.actionCompleted();
			});

		return true;
	}

	function btnOnAdd( btn, ractive, params ) {
		log.verbose( 'Inet', 'On add network: ssid=%s pass=%s', params.ssid, params.password );
		apiCallAsync( network.connectWifi, {
			ssid: params.ssid,
			pass: params.password,
			autoconnect: true,
		})
		.finally(function() {
			btn.actionCompleted();
			ractive.model.popActionPanel();
		});

		return true;
	}

	function btnOnDisconnect( btn, ractive ) {
		log.verbose( 'Inet', 'On disconnect' );

		function done() {
			btn.actionCompleted();
			ractive.focus(0);
		}

		apiCallAsync( network.getConnections ).then( function(conns) {
			let wifiConn = conns.find( conn => conn.type === 'wireless' );
			if (wifiConn) {
				apiCallAsync( network.disconnectWifi, wifiConn.name ).finally( done );
			} else {
				done();
			}
		});

		return true;
	}

	function createWifiPanel( ractive ) {
		let actPanel = new svm.ActionPanel();

		let currCategory = ractive.parent.headerData.category;
		actPanel.setCloseAction(() => ractive.parent.headerData.category = currCategory);

		{	//	Search
			let btnSearch = new svm.Button( res.txt('searchTitle'), res.img('wifi'), true );
			btnSearch.setAction( $(btnOnSearch,btnSearch,ractive) );
			btnSearch.setActionPanel( createWifiListPanel() );
			btnSearch.getLeftPanel().addDescription([res.txt('searchDescription')]);
			actPanel.addWidget( btnSearch );
		}

		{	//	Add
			let btnAdd = new svm.Button( res.txt('addTitle'), res.img('add'), true );
			btnAdd.setAction( $(btnOnAdd,btnAdd,ractive) );
			let inputs = [{
				name: 'ssid',
				label: res.txt('ssid'),
				placeholder: res.txt('network_name_input'),
			},{
				name: 'password',
				label: 'Contraseña',
				placeholder: res.txt('password_placeholder'),
				type: 'password',
			}];
			btnAdd.setDialog( 'input', res.txt('addTitle').toUpperCase(), inputs, res.txt('connect'));
			btnAdd.getLeftPanel().addDescription([res.txt('addDescription')]);
			actPanel.addWidget( btnAdd );
		}

		{	// Disconnect
			let disconnectBtn = new svm.Button( res.txt('disconnectTitle'), res.img('disconnect'), true );
			disconnectBtn.setAction( $(btnOnDisconnect,disconnectBtn,ractive) );
			disconnectBtn.getLeftPanel().addDescription([res.txt('disconnectDescription')]);
			disconnectBtn.setVisible(false);
			actPanel.addWidget( disconnectBtn );
			ractive.wifiDisconnectBtn = disconnectBtn;
		}

		return actPanel;
	}

	function updateInfo(ractive, panel, evt) {
		if (evt !== undefined && evt.type === 'state') {
			log.verbose( 'Inet', 'Update information: evt=%j', evt );
			apiCallAsync( network.isConnected ).then( connected => {
				if (connected) {
					apiCallAsync( network.activeConnection ).then($(onActiveConnectionInfo,panel));
				} else {
					panel.setDescription(0, [res.txt('disconnected')]);
				}
			});
			//	Update wifi disconnect btn state
			apiCallAsync( network.getConnections ).then(conns => ractive.wifiDisconnectBtn.setVisible(conns.some(conn => conn.type === 'wireless')));
		}
	}

	function onEnterWifi(ractive) {
		ractive.parent.headerData.category = ractive.parent.headerData.category + ' > ' + res.txt('wireless');
	}

	let prot = {};

	prot.oninit = function() {
		log.info( 'Inet', 'oninit' );

		// Call SettingView.oninit()
		this._super();

		let actPanel = new svm.ActionPanel();
		let netInfoPanel;

		{	//	Add info
			let wgt = new svm.Label( res.txt('title'), res.img('inet'), true );
			actPanel.addWidget( wgt );
			netInfoPanel = wgt.getLeftPanel();
			netInfoPanel.addDescription([res.txt('disconnected')]);
		}

		{	//	Wifi
			let wifiBtn = new svm.Button( res.txt('wireless'), res.img('wifi'), true );
			wifiBtn.setAction( $(onEnterWifi, this) );
			wifiBtn.setActionPanel( createWifiPanel(this) );
			wifiBtn.getLeftPanel().addDescription([res.txt('wifiDescription')]);
			actPanel.addWidget( wifiBtn );
		}

		this.model.pushActionPanel(actPanel);

		{	// Update network information
			updateInfo(this, netInfoPanel, {type: 'state'});
			registerNotification( 'ar.edu.unlp.info.lifia.tvd.network', 'connection', 'connectionChanged', $(updateInfo, this, netInfoPanel) );
		}
	};

	return {
		extends: SettingView,
		proto: prot
	};
}
