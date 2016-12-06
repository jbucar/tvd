// Helpers
function uniq(a) {
	return a.sort().filter(function(item, pos) {
		return !pos || item != a[pos - 1];
	});
}

function flat(a) {
	return a.reduce(function(b, c) { return b.concat(c); });
}

function compact(a) {
	return a.filter(function(item) { return item !== undefined && item !== null; });
}

// DB

var marketApp = {
	name: 'Tienda TAC',
	icon: mockAssets('imgs/apps/hangouts.png'),
	pkgID: "id.webapps",
	id: 'id.webapps',
	system: true,
	vendor: 'UNQ',
	categories: ['OTROS'],
	description: 'En la tienda de aplicaciones vas a encontrar todas las aplicaciones que necesitas,\
juegos, utilidades, entretenimiento y mucho más. Ingresa y empezá a instalar tus aplicaciones preferidas.',
};

var apps = [
	{ name: "Shell", description: "Shell", id: "app.id.shell" },
	{ name: 'Gmail', icon: mockAssets('imgs/apps/gmail.png'), description: '', pkgID: "pkg.1", id: 'app.1', categories: ['COMUNICACION'], vendor: 'Google' },
	{ name: 'XBMC', icon: mockAssets('imgs/apps/xbmc.png'), description: 'Una descripción', pkgID: "pkg.2", id: 'app.2', categories: ['OTROS'] },
	{ name: 'iBeLight', icon: mockAssets('imgs/apps/ibelight.png'), description: 'Una descripción', pkgID: "pkg.3", id: 'app.3', categories: ['OTROS'] },
	{ name: 'WhatsApp', icon: mockAssets('imgs/apps/whatsapp.png'), description: 'Una descripción', pkgID: "pkg.4", id: 'app.4', categories: ['COMUNICACION'] },
	{ name: 'Nombre largooooooooooo', icon: mockAssets('imgs/whatsapp.png'), description: 'Una descripción', pkgID: "pkg.5", id: 'app.5', categories: ['JUEGOS'] },
	{ name: 'Hangouts', icon: mockAssets('imgs/apps/hangouts.png'), description: 'Una descripción', pkgID: "pkg.6", id: 'app.6', categories: ['COMUNICACION'] },
	{ name: 'Hangouts', icon: mockAssets('imgs/apps/hangouts.png'), description: 'Una descripción', pkgID: "pkg.7", id: 'app.7', categories: ['GINGA'] },
	marketApp
];

var favs = ['app.1'];
var installed = ['app.id.shell', 'id.webapps', 'app.1', 'app.2', 'app.7'];

// APIs

var appmgr = {
	getAll: function(fnc) {
		setTimeout(fnc.bind(undefined, undefined, installed), 1000);
	},
	run: function(appID, fnc) {
		fnc(undefined, true);
	},
	getShell: function(fnc) {
		fnc(undefined, "app.id.shell");
	},
	get: function( id, fnc ) {
		fnc(undefined, apps.find(function(app) { return app.id === id; }));
	},
	getWebApps: function(callback) {
		callback(undefined, 'id.webapps');
	},
	getCategories: function(callback) {
		var cats = uniq(flat(compact(apps.filter(function(app) {
			return installed.indexOf(app.id) !== -1;
		}).map(function(app) {
			return app.categories;
		}))));
		cats.unshift('FAVORITOS');
		callback(undefined, cats);
	},
	toggleFavorite: function(appId) {
		if (favs.indexOf(appId) === -1) {
			favs.push(appId);
		}
		else {
			delete favs[appId];
		}
	},
	isFavorite: function(appId, callback) {
		callback(undefined, favs.indexOf(appId) > -1);
	},
	configureService: function(srvId, callback) {
		callback(undefined, true);
	},
	on: function(signal, callback) {
		if (signal === 'install') {
			this.installCbk = callback;
		}
	},
};

var update = {
	version: "1.0.1",
	url: "www.update.com/v/1.0.1",
	name: "Lechuga",
	description: "Cambios en la seguridad",
	size: '15960'
};

var system = {
	licenseAccepted: function(cbk) {
		console.log('SystemMock licenseAccepted!');
		setTimeout(cbk.bind(undefined, undefined, 2000), 100);
	},
	getRemotePort: function(cbk) {
		setTimeout(cbk.bind(undefined, undefined, 2000), 100);
	},
	isFirstBoot: function(cbk) {
		setTimeout(cbk.bind(undefined, undefined, false), 100);
	},
	powerOff: function() {
		console.log('SystemMock powerOff!');
	},
	reboot: function() {
		console.log('SystemMock reboot!');
	},
	factoryReset: function() {
		console.log('SystemMock Factory reset');
	},
	getInfo: function(fnc) {
		fnc(undefined,
			{
				id: '1', system: {
					api: '1',
					version: '1.0.0',
					commit: 'gae80c12'
				},
				build: {
					name: 'dummy',
					type: 'developer',
					config: 'mock'
				},
				platform: {
					name: 'PC',
					version: '1.0',
					commit: 'gae80c12',
				}
			}
		);
	},
	updated: false,
	checkFirmwareUpdate: function() {
		this.updated = true;
	},
	firmwareUpdate: function(callback) {
		callback(undefined, this.updated ? update : undefined);
	},
	on: function(signal, callback) {
	},
	disableDevelopmentMode: function() {
		DevMode.set('enabled', false);
	},
	getLogConfig: function(fnc) {
		fnc(undefined,
			{
				enabled: false,
				maxHistorySize: 1000,
				maxWidth: null, // Disable maxWidth
				level: 'info',
				port:3045
			}
		);
	},
	setLogConfig: function(cfg, fnc) {
		console.log('SystemMock, Config log changed.');
		fnc(undefined, 'Config log changed.' );
	},
	isDevelopmentMode: function(fnc) {
		// return true if log level is Silly or verbose.
		fnc(undefined, true);
	},
	enableGeolocation: function(value) {
		return value;
	},
	isGeolocationEnabled: function(callback) {
		callback(undefined, true);
	}
};

var pkgmgr = {
	uninstall: function(opts, callback) {
		var app = apps.find(function(app) { return app.pkgID === opts.id; });
		if (app && delete installed[installed.indexOf(app.id)]) {
			setTimeout(function() {
				installed = compact(installed); // Remove the undefined leaved from the delete op
				callback(undefined);
				this.uninstallCbk(opts.id);
			}.bind(this), 100);
		}
		else {
			callback(new Error('app not installed'));
		}
	},
	on: function(signal, callback ) {
		if (signal === 'uninstall') {
			this.uninstallCbk = callback;
		}
	}
};

var updatemgr = {
	on: function(signal, cbk) {
		console.log( 'SystemMock, listening for updates' );
		if (signal === 'UpdateAvailable') {
			updatemgr.updateCbk = cbk;
		}
		if (signal === 'FirmwareDownloadProgress') {
			updatemgr.progresscbk = cbk;
		}
	},
	updateFirmware: function(url) {
		console.log('SystemMock, installing update. URL: {0}'.format(url));
	},
	fetchFirmwareUpdate: function(updatepkg, cbk) {
		console.log( 'SystemMock, fetch firm update' );

		var fnc = function(i) {
			if (updatemgr.progresscbk) {
				updatemgr.progresscbk(i);
			}

			if (i < 100) {
				setTimeout(fnc, 1000, i + 10);
			}
			else {
				if (cbk) {
					error = new Error( 'Error downloading firmware' );
					// cbk(undefined, undefined);
					cbk(undefined, updatepkg.url);
					// cbk(error, updatepkg.url);
					// cbk(error, undefined);
				}
			}
		};
		setTimeout(fnc, 1000, 0);
	},
	downloadFirmwareUpdate: function() {},
	checkFirmwareUpdate: function() {
		if (updatemgr.updateCbk) {
			setTimeout(updatemgr.updateCbk.bind(undefined, update), 5000);
		}
	}
};

// Simulate a mandatory update sended from the server
function sendMandatoryUpdate() {
	updatemgr.fetchFirmwareUpdate(update, function(err, url) {
		notification.updateCbk.forEach(function(f) {
			f({type: 'firmware', description: 'Firmware download completed'});
		});
	});
}
// setTimeout(sendMandatoryUpdate, 6000);

function emitEventsOn(cbks, evt) {
	cbks.forEach(function(cbk) {
		cbk(evt);
	});
}

var servicesList = [
	{ name: 'Service0', description: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras sagittis eget eros quis sollicitudin. Nam viverra luctus mauris et efficitur. Vestibulum a lectus ac velit facilisis efficitur et non lacus. Pellentesque porttitor justo quis massa vulputate blandit. Donec blandit urna et libero elementum, id tincidunt nulla condimentum. Duis posuere sit amet augue nec aliquet. Integer at dui et eros scelerisque rutrum vitae a tellus. Duis posuere sit amet augue nec aliquet. Integer at dui et eros scelerisque rutrum vitae a tellus. Duis posuere sit amet augue nec aliquet. Integer at dui et eros scelerisque rutrum vitae a tellus.', id: 'srv.0', canConfigure: true, canUninstall: true, status: 'disabled', running: false },
	{ name: 'Service1', description: 'Dummy1', id: 'srv.1', canConfigure: false, canUninstall: false, status: 'enabled', running: true },
	{ name: 'Service2', description: 'Una descripción', id: 'srv.2', canConfigure: false, canUninstall: false, status: 'enabled', running: true }
];

var servicesInstalled = ['srv.0', 'srv.1', 'srv.2'];

var srvmgr = {
	getAll: function(callback) {
		setTimeout(callback.bind(undefined, undefined, servicesInstalled), 1000);
	},
	get: function(id, callback) {
		callback(undefined, servicesList.find(function(srv) { return srv.id === id; }));
	},
	resetConfig: function(id, callback) {
		setTimeout(callback.bind(undefined, undefined, true), 3000);
	},
	uninstall: function(opts, callback) {
		var srv = servicesInstalled.find(function(srvID) { return srvID === opts.id; });
		if (srv && delete servicesInstalled[servicesInstalled.indexOf(srv.id)]) {
			setTimeout(function() {
				servicesInstalled = compact(servicesInstalled); // Remove the undefined leaved from the delete op
				callback(undefined);
				this.uninstallCbk(opts.id);
			}.bind(this), 100);
		}
		else {
			callback(new Error('Error uninstalling service'));
		}
	},
	isEnabled: function(id, callback) {
		callback(undefined, true);
	},
	enable: function(id, boolean, callback) {
		callback(undefined, true);
	},
	isRunning: function(id, callback) {
		callback(undefined, (servicesList.find( function(srv) { return srv.id === id; })).running );
	}
};

var network = {
	// HELPERS/IMPL/DEBUG ONLY
	_changedCbks: [],
	_successWifiConnection: true,
	_isConnected: false,
	_connDB: {
		wired: { name:'eth0', type:'wired', ip: '127.0.0.1', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'dns', domain: 'domain' },
		wireless: { name:'WiFi - TVD', type:'wireless', ip: '127.0.0.1', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'dns', domain: 'domain' },
	},
	_connections: [],
	_wifies: [
		{
			name: 'WiFi - TVD',
			signal: '100',
			isSecured: true,
			type:'wireless', ip: '127.0.0.1', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'dns', domain: 'domain'
		},
		{
			name: 'Tac wifi',
			signal: '50',
			isSecured: true,
			type:'wireless', ip: '127.0.0.1', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'dns', domain: 'domain'
		},
		{
			name: 'LIFIA',
			signal: '10',
			isSecured: false,
			type:'wireless', ip: '127.0.0.1', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'dns', domain: 'domain'
		}
	],
	_addConns: function() {
		this._connections.push(this._connDB.wired);
		this._connections.push(this._connDB.wireless);
	},
	_disconnectAllConnections: function() {
		log.info('Mock::networkmgr', '_disconnectAllConnections');
		for(var i=0; i < this._connections.length; i++) {
			var connected = (i !== this._connections.length-1); // last connection
			var conn = this._connections[i];
			emitEventsOn(this._changedCbks, {type: 'connection lost', state: connected, connection: conn});
		}
		this._connections.length = 0;
	},
	_connectWire: function() {
		log.info('Mock::networkmgr', '_connectWire');
		var conn = this._connDB.wired;
		emitEventsOn(this._changedCbks, {type: 'connected', state: true, connection: conn});
		if (this._connections.indexOf(conn) === -1) {
			this._connections.push(conn);
		}
	},
	_wiredConnected: function() {
		return this._connections.indexOf(this._connDB.wired) !== -1;
	},
	_connectWifi: function() {
		log.info('Mock::networkmgr', '_connectWifi');
		var conn = this._connDB.wireless;
		emitEventsOn(this._changedCbks, {type: 'connected', state: true, connection: this._wiredConnected() ? this._connDB.wired : conn });
		if (this._connections.indexOf(conn) === -1) {
			this._connections.push(conn);
		}
	},
	// Mocked API
	on: function(evt, cbk) {
		if (evt === 'changed') {
			this._changedCbks.push(cbk);
		}
		else {
			log.error('Mock::networkmgr', 'on', 'No event named %s', evt);
		}
	},
	isConnected: function(cbk) {
		cbk(undefined, this._isConnected);
	},
	activeConnection: function(cbk) {
		var conn = this._connections.find(function(c) {
			return c.type === 'wired';
		});

		if (!conn) {
			conn = this._connections.find(function(c) {
				return c.type === 'wireless';
			});
		}

		log.info('Mock::networkmgr', 'activeConnection', ' active connection=%O', conn);
		cbk(undefined, conn);
	},
	getConnections: function(cbk) {
		cbk(undefined, this._connections);
	},
	getWifiList: function(cbk) {
		cbk(undefined, this._wifies);
	},
	connectWifi: function(ssid, pass, autoconnect, cbk) {
		log.info('Mock::networkmgr', 'connectWifi', 'ssid=%s', ssid);
		setTimeout(function() {
			var err;

			if (this._successWifiConnection) {
				var conn = this._wifies.find(function(w) { return w.name === ssid; });
				emitEventsOn(this._changedCbks, {type: 'connected', state: true, connection: this._wiredConnected() ? this._connDB.wired : conn });
			}
			else {
				err = new Error('Invalid Pass');
			}

			cbk(err, err ? false : true);
		}.bind(this), 6000);
	},
	disconnectWifi: function (ssid, cbk) {
		log.info('Mock::networkmgr', 'disconnectWifi', 'ssid=%s', ssid);
		fnc( undefined );
		this.onNMStateChange('NM_STATE_DISCONNECTED');
		this.onNMDeviceStateChange({state: 'disconnected', type: 'wifi', device: {}});
	}
};

var _channels = [
	{
		id: 0,
		channel: '12.01',
		name: 'Telefe',
		logo: mockAssets('imgs/channels/telesur.png'),
		isMobile: false,
		isProtected: true,
		isBlocked: false,
		isFavorite: false,
		category: 0,
		parentalAge: 16,
		parentalContent: 'Drugs',
		info: {
			type: 'dvb',
			frecuency: 497,
			nitID: 1934,
			tsID: 1934,
			srvID:61888
		}
	},
	{
		id: 1,
		channel: '14.05',
		name: 'Aqua Federal',
		logo: mockAssets('imgs/channels/aqua_federal.png'),
		isMobile: false,
		isProtected: true,
		isBlocked: true,
		isFavorite: false,
		category: 1,
		parentalAge: 16,
		parentalContent: 'Drugs',
		info: {
			type: 'dvb',
			frecuency: 503,
			nitID: 1934,
			tsID: 1934,
			srvID:61888
		}
	},
	{
		id: 2,
		channel: '14.06',
		name: 'TV Pública',
		logo: mockAssets('imgs/channels/tv_publica.png'),
		isMobile: false,
		isProtected: true,
		isBlocked: true,
		isFavorite: false,
		category: 1,
		parentalAge: 16,
		parentalContent: '',
		info: {
			type: 'dvb',
			frecuency: 497,
			nitID: 1934,
			tsID: 1934,
			srvID:61888
		}
	},
	{
		id: 3,
		channel: '22.01',
		name: 'DeportTV',
		logo: mockAssets('imgs/channels/deportv.png'),
		isMobile: false,
		isProtected: true,
		isBlocked: false,
		isFavorite: false,
		category: 1,
		parentalAge: 12,
		parentalContent: '',
		info: {
			type: 'dvb',
			frecuency: 522,
			nitID: 1934,
			tsID: 1934,
			srvID:61888
		}
	},
	{
		id: 4,
		channel: '23.01',
		name: 'America',
		logo: mockAssets('imgs/channels/deportv.png'),
		isMobile: false,
		isProtected: true,
		isBlocked: false,
		isFavorite: false,
		category: 1,
		parentalAge: 12,
		parentalContent: '',
		info: {
			type: 'dvb',
			frecuency: 522,
			nitID: 1934,
			tsID: 1934,
			srvID:61888
		}
	},
	{
		id: 5,
		channel: '24.01',
		name: '99',
		logo: mockAssets('imgs/channels/deportv.png'),
		isMobile: false,
		isProtected: true,
		isBlocked: false,
		isFavorite: false,
		category: 1,
		parentalAge: 12,
		parentalContent: '',
		info: {
			type: 'dvb',
			frecuency: 522,
			nitID: 1934,
			tsID: 1934,
			srvID:61888
		}
	},
	{
		id: 6,
		channel: '25.01',
		name: 'Cinecanal',
		logo: mockAssets('imgs/channels/deportv.png'),
		isMobile: false,
		isProtected: true,
		isBlocked: false,
		isFavorite: false,
		category: 1,
		parentalAge: 12,
		parentalContent: '',
		info: {
			type: 'dvb',
			frecuency: 522,
			nitID: 1934,
			tsID: 1934,
			srvID:61888
		}
	}
];
var t_day = new Date();
var _shows = [
	{
		name: 'Dummy',
		description: 'A dummy program',
		startTime: t_day,
		endTime: t_day,
		category: 0,
		parentalContent: '',
		parentalAge: 'ATP'
	},
	{
		name: 'Show with a very large text!!!',
		description: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean non tincidunt erat. Mauris sodales, turpis non aliquam cursus, urna lacus scelerisque libero, eget pellentesque felis augue id felis. Ut eget sodales lorem. Etiam pretium tincidunt nibh ac efficitur. Donec quis congue neque. Nunc eros mi, vestibulum sit amet est sit amet, aliquet hendrerit nisi. Ut nunc tellus, aliquet et risus at, scelerisque rhoncus nulla. Proin mattis, nibh sed feugiat facilisis, ante sapien faucibus massa, ut posuere ipsum ex vitae eros. Mauris vel dictum ligula, eu laoreet erat. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Curabitur at tincidunt quam. Sed blandit in mauris fringilla aliquet.',
		startTime: t_day,
		endTime:  t_day,
		category: 0,
		parentalContent: '',
		parentalAge: 'ATP'
	},
	{
		name: 'Casado con hijos',
		description: 'Programa popular',
		startTime: t_day,
		endTime:  t_day,
		category: 1,
		parentalContent: '',
		parentalAge: 'ATP'
	},
	{
		name: 'Dummy',
		description: 'A dummy program',
		startTime: t_day,
		endTime:  t_day,
		category: 0,
		parentalContent: '',
		parentalAge: 'ATP'
	}
];

var scanCbk = [];
var sessionChangedCbk = [];
var sessionEnabledCbk = [];
var playerChangedCbk = [];
var showsListeners = [];
var middlewareListeners = [];
var pass = '';
var isSessionOpened = false;
var age = 0;
var sex = true;
var drugs = true;
var violence = false;

var tvd = {
	on: function(signal, callback) {
		if (signal == 'scanChanged' ) {
			scanCbk.push( callback );
		} else if (signal == 'sessionChanged') {
			sessionChangedCbk.push( callback );
		} else if (signal == 'sessionEnabled') {
			sessionEnabledCbk.push( callback );
		} else if(signal == 'playerProtectedChanged') {
			playerChangedCbk.push( callback );
		} else if (signal == 'showsChanged' ) {
			showsListeners.push( callback );
			setTimeout(showsListeners[0], 5000, 0);
			setTimeout(showsListeners[0], 7000, 1);
		} else if(signal === 'middleware') {
			middlewareListeners.push(callback);
			setTimeout(middlewareListeners[0], 1000, {type: 'added', id: 1, app: {name: 'pepe'}});
			setTimeout(middlewareListeners[0], 1000, {type: 'added', id: 2, app: {name: 'pepe2'}});
			setTimeout(middlewareListeners[0], 1000, {type: 'added', id: 3, app: {name: 'pepe3'}});
		}
	},
	tuner: {
		isScanning: function( fnc ) {
			fnc( undefined, false );
		},
		startScan: function() {
			console.log( 'TvdMock, start scan' );
			scanCbk.forEach( function( cbk ) {
				setTimeout(cbk, 10, 'begin');
				setTimeout(cbk, 1000, 'network', 497, 25  );
				setTimeout(cbk, 3000, 'network', 503, 50  );
				setTimeout(cbk, 5000, 'network', 507, 75  );
				setTimeout(cbk, 7000, 'end', 100 );
			});
		},
		cancelScan: function( fnc ) {
			console.log( 'TvdMock, cancel scan' );

			scanCbk.forEach( function( cbk ) {
				cbk( 'end', 100 );
			});
		}
	},
	db: {
		count: function(fnc) {
			fnc( undefined, 0 );
		},
		getAll: function(fnc) {
			var ids = _channels.map( function( ch ) {
				return ch.id;
			});
			fnc( undefined, ids );
		},
		remove: function(chID, fnc) {
			fnc( undefined, true );
		}
	},
	channel: {
		get: function(chId, fnc) {
			var channel = _channels.filter( function( ch ) {
				return ch.id === chId;
			});
			fnc( undefined, channel[0] );
		},
		toggleFavorite: function(chId, fnc) {
			console.log( 'TvdMock, toggleFavorite' );
			fnc();
		},
		toggleBlocked: function(chId, fnc) {
			console.log( 'TvdMock, toggleBlocked' );
			fnc();
		},
		getShow: function(chId, fnc) {
			fnc( undefined, _shows[chId%4] );
		},
		getShowsBetween: function(chId, first, last, fnc) {
			var shows = chId % 2 === 0 ? _shows : [];
			fnc( undefined, shows);
		}
	},
	player: {
		//	Player: status
		current: function(fnc) {
			fnc( undefined, 1 );
		},
		isProtected: function(fnc) {
			if(fnc) {
				fnc( undefined, false );
			}
		},
		status: function(fnc) {
			fnc( undefined, { signal:86, quality:63 } );
		},
		//	Player: change
		change: function(chId, fnc) {
			fnc( undefined, chId );
			if (chId !== -1) {
				this.get(chId, function(err, model) {
					if (model.isBlocked) {
						playerChangedCbk.forEach( function( cbk ) {
							cbk(true);
						});
					}
				});
			}
		},
		nextChannel: function(first, factor, fnc) {
			fnc( undefined, 2 );
		},
		nextFavorite: function(first, factor, fnc) {
			fnc( undefined, 2 );
		},
		showMobile: function(fnc) {
			fnc( undefined, false );
		},
		//	Player: media
		nextVideo: function(fnc) {
			fnc( undefined, 2 );
		},
		videoInfo: function(fnc) {
			fnc( undefined, { pid: 3000,  width: 720, height: 576, fps: 25} );
		},
		nextAudio: function(fnc) {
			fnc( undefined, 2 );
		},
		audioInfo: function( fnc ) {
			fnc( undefined, { count: 1, lang: 'spanish', pid: 4000,  channels: '2.1', rate: 45000} );
		},
		nextSubtitle: function(fnc) {
			fnc( undefined, 2 );
		},
		subtitleInfo: function(fnc) {
			fnc( undefined, {
				count: 0,
				pid: 33,
				codec: 'utf'
			});
		}
	},
	session: {
		enableSession: function( password ) {
			pass = password;
			isSessionOpened = pass !== '';
			sessionEnabledCbk.forEach( function( cbk ) {
				cbk( pass !== '' );
			});
		},
		isSessionEnabled: function(fnc) {
			fnc( undefined, pass !== '' );
		},
		checkSession: function( password, fnc ) {
			var isValid = pass === password;
			if (isValid) {
				isSessionOpened = true;
			}
			fnc( undefined, isValid );
			sessionChangedCbk.forEach( function( cbk ) {
				cbk( isValid );
			});
		},
		isSessionBlocked: function(fnc) {
			fnc ( undefined, isSessionOpened );
		},
		expireSession: function() {
			isSessionOpened = false;
			sessionChangedCbk.forEach( function( cbk ) {
				cbk( isSessionOpened );
			});
		},
		getSessionTimeExpiration: function(fnc) {
			fnc( undefined, 60 );
		},
		setSessionTimeExpiration: function( time ) {
		},
		restrictSex: function( restrict ) {
			sex = restrict;
		},
		isSexRestricted: function(fnc) {
			fnc( undefined, sex );
		},
		restrictViolence: function( restrict ) {
			violence = restrict;
		},
		isViolenceRestricted: function(fnc) {
			fnc( undefined, violence );
		},
		restrictDrugs: function( restrict ) {
			drugs = restrict;
		},
		isDrugsRestricted: function(fnc) {
			fnc( undefined, drugs );
		},
		restrictAge: function( restrict ) {
			age = restrict;
		},
		isAgeRestricted: function(fnc) {
			fnc( undefined, age );
		}
	},
	middleware: {
		enableMiddleware: function(enable) {
			//do nothing
		},
		disableApplications: function(disable) {
			//do nothing
		},
		isMiddlewareEnabled: function(cbk) {
			cbk(undefined, true);
		}
	}
};

var audio = {
	getVolume: function() {}
};

var notification = {
	updateCbk: [],
	on: function(signal, cbk) {
		console.log( 'SystemMock, listening for notifications' );
		if (signal === 'Application') {
			setTimeout(cbk.bind(undefined, {state: 'loading'}), 1000);
			setTimeout(cbk.bind(undefined, {state: 'loaded'}), 2000);
		}
		if (signal === 'Update') {
			this.updateCbk.push(cbk);
		}
	},
	once: function(signal, cbk) {
		notification.on(signal, cbk);
	},
	emit: function(name, evt) {
		log.info('MOCK', 'Notification', 'EventName=%s event=%O', name, evt);
	}
};

var media = {};
media.image = mockImageAPI();

var tacKeyboardLayout = tacKeyboardLayout || {
	"VK_PAUSE":-1,
	"VK_HELP":65, //Tecla A, solo para probar
	"VK_FAST_FWD":-1,
	"VK_TRACK_NEXT":-1,
	"VK_NEXT_FAVORITE_CHANNEL":-1,
	"VK_REWIND":-1,
	"VK_PLAY":83,
	"VK_SUBTITLE":-1,
	"VK_BACK":8,
	"VK_ENTER":13,
	"VK_EXIT":27,
	"VK_PAGE_UP":33,
	"VK_PAGE_DOWN":34,
	"VK_LEFT":37,
	"VK_UP":38,
	"VK_RIGHT":39,
	"VK_DOWN":40,
	"VK_0":48,
	"VK_1":49,
	"VK_2":50,
	"VK_3":51,
	"VK_4":52,
	"VK_5":53,
	"VK_6":54,
	"VK_7":55,
	"VK_8":56,
	"VK_9":57,
	"VK_POWER":123,
	"VK_HOME":164,
	"VK_MUTE":106,
	"VK_VOLUME_DOWN":109, // -
	"VK_VOLUME_UP":107, // +
	"VK_RED":112, // F1
	"VK_GREEN":113, // F2
	"VK_YELLOW":114, // F3
	"VK_BLUE":115, // F4
	"VK_CHANNEL_UP":116, // F5
	"VK_CHANNEL_DOWN":117, // F6
	"VK_LIST":118, // F7
	"VK_RECALL_FAVORITE_0":119, // F8
	"VK_GUIDE":120, // F9
	"VK_INFO":121, // F10
	"VK_LAST":122, // F11
	"VK_GO_TO_START":36,
	"VK_GO_TO_END":35
};
