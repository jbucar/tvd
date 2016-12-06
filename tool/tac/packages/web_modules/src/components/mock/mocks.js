/* globals mockImage, mockVideosMetaData, mockSeriesMetaData, mockSeries, EventEmitter */
/* exported srvmgr, tvd */

// Helpers
function compact(a) {
	return a.filter(function(item) { return item !== undefined && item !== null; });
}

function mockAssets( resource ) {
    return "components/mock/" + resource;
}

// APIs
var servicesList = [
	{ name: 'Service0', description: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras sagittis eget eros quis sollicitudin. Nam viverra luctus mauris et efficitur. Vestibulum a lectus ac velit facilisis efficitur et non lacus. Pellentesque porttitor justo quis massa vulputate blandit. Donec blandit urna et libero elementum, id tincidunt nulla condimentum. Duis posuere sit amet augue nec aliquet. Integer at dui et eros scelerisque rutrum vitae a tellus. Duis posuere sit amet augue nec aliquet. Integer at dui et eros scelerisque rutrum vitae a tellus. Duis posuere sit amet augue nec aliquet. Integer at dui et eros scelerisque rutrum vitae a tellus.', id: 'srv.0', canConfigure: true, canUninstall: true, status: 'disabled', running: false },
	{ name: 'Service1', description: 'Dummy1', id: 'srv.1', canConfigure: false, canUninstall: false, status: 'enabled', running: true },
	{ name: 'Service2', description: 'Una descripciÃ³n', id: 'srv.2', canConfigure: false, canUninstall: false, status: 'enabled', running: true }
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
		name: 'TV PÃºblica',
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
		} else if(signal == 'playerChanged') {
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
		cancelScan: function() {
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
		current: fnc => fnc( undefined, 1 ),
		isProtected: fnc => fnc( undefined, false ),
		status: fnc => fnc( undefined, { signal:86, quality:63 } ),
		//	Player: change
		change: function(chId, fnc) {
			fnc( undefined, chId );
			if (chId !== -1) {
				tvd.channel.get(chId, (/*err, model*/) => playerChangedCbk.forEach(cbk => cbk(chId)) );
			}
		},
		nextChannel: (first, factor, fnc) => fnc( undefined, 2 ),
		nextFavorite: fnc => fnc( undefined, 2 ),
		showMobile: fnc => fnc( undefined, false ),
		//	Player: media
		nextVideo: fnc => fnc( undefined, 2 ),
		videoInfo: fnc => fnc( undefined, { pid: 3000,  width: 720, height: 576, fps: 25} ),
		nextAudio: fnc => fnc( undefined, 2 ),
		audioInfo: fnc => fnc( undefined, { count: 1, lang: 'spanish', pid: 4000,  channels: '2.1', rate: 45000} ),
		nextSubtitle: fnc => fnc( undefined, 2 ),
		subtitleInfo: fnc => fnc( undefined, { count: 0, pid: 33, codec: 'utf' } ),
	},
	session: {
		enableSession: function( password ) {
			pass = password;
			isSessionOpened = pass !== '';
			sessionEnabledCbk.forEach( function( cbk ) {
				cbk( pass !== '' );
			});
		},
		isSessionEnabled: fnc => fnc( undefined, pass !== '' ),
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
		isBlocked: fnc => fnc( undefined, isSessionOpened ),
		expireSession: function() {
			isSessionOpened = false;
			sessionChangedCbk.forEach( function( cbk ) {
				cbk( isSessionOpened );
			});
		},
		getSessionTimeExpiration: fnc => fnc( undefined, 60 ),
		setSessionTimeExpiration: () => undefined,
		restrictSex: restrict => sex = restrict,
		isSexRestricted: fnc => fnc( undefined, sex ),
		restrictViolence: restrict => violence = restrict,
		isViolenceRestricted: fnc => fnc( undefined, violence ),
		restrictDrugs: restrict => drugs = restrict,
		isDrugsRestricted: fnc => fnc( undefined, drugs ),
		restrictAge: restrict => age = restrict,
		isAgeRestricted: fnc => fnc( undefined, age ),
	},
	middleware: {
		enableMiddleware: (state,cb) => cb(undefined),
		disableApplications: () => undefined,
		isMiddlewareEnabled: fnc => fnc(undefined, true),
	}
};

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

(function(globals) {
	function mockMethod(obj, method) {
		return function() {
			let args = Array.prototype.slice.apply(arguments);
			args.unshift(method);
			obj.log.apply(obj, args);
			obj[method].apply(obj, Array.prototype.slice.apply(arguments));
		};
	}

	function createMock(mockClass, param) {
		let methods = Object.getOwnPropertyNames(mockClass.prototype);
		if (mockClass.isEventEmitter()) {
			methods.push('on', 'once', 'emit', 'removeListener');
		}
		let ctorIdx = methods.indexOf('constructor');
		if (ctorIdx >= 0) {
			methods.splice(ctorIdx, 1);
		}
		let mockInstance = new mockClass(param);
		let mock = {};
		methods.forEach( m => mock[m] = mockMethod(mockInstance, m) );
		return mock;
	}

	// HELPERS
	function uniq(a) {
		return a.sort().filter(function(item, pos) {
			return !pos || item != a[pos - 1];
		});
	}

	function flat(a) {
		return a.reduce(function(b, c) { return b.concat(c); });
	}

	let _apps = [
		{ name: "Shell", description: "Shell", id: "app.id.shell", canUninstall: false },
		{ name: 'Gmail', icon: mockAssets('imgs/apps/gmail.png'), description: '', pkgID: "pkg.1", id: 'app.1', categories: ['COMUNICACION'], vendor: 'Google', canUninstall: true },
		{ name: 'XBMC', icon: mockAssets('imgs/apps/xbmc.png'), description: 'Una descripciÃ³n', pkgID: "pkg.2", id: 'app.2', categories: ['OTROS'], canUninstall: true },
		{ name: 'iBeLight', icon: mockAssets('imgs/apps/ibelight.png'), description: 'Una descripciÃ³n', pkgID: "pkg.3", id: 'app.3', categories: ['OTROS'], canUninstall: true },
		{ name: 'WhatsApp', icon: mockAssets('imgs/apps/whatsapp.png'), description: 'Una descripciÃ³n', pkgID: "pkg.4", id: 'app.4', categories: ['COMUNICACION'], canUninstall: true },
		{ name: 'Nombre largooooooooooo', icon: mockAssets('imgs/whatsapp.png'), description: 'Una descripciÃ³n', pkgID: "pkg.5", id: 'app.5', categories: ['JUEGOS'], canUninstall: true },
		{ name: 'Hangouts', icon: mockAssets('imgs/apps/hangouts.png'), description: 'Una descripciÃ³n', pkgID: "pkg.6", id: 'app.6', categories: ['COMUNICACION'], canUninstall: true },
		{ name: 'Hangouts', icon: mockAssets('imgs/apps/hangouts.png'), description: 'Una descripciÃ³n', pkgID: "pkg.7", id: 'app.7', categories: ['GINGA'], canUninstall: true },
		{ name: 'Tienda TAC', icon: mockAssets('imgs/apps/hangouts.png'), pkgID: "id.webapps", id: 'id.webapps', system: true, canUninstall: true, vendor: 'UNQ', categories: ['OTROS'], description: 'En la tienda de aplicaciones vas a encontrar todas las aplicaciones que necesitas, juegos, utilidades, entretenimiento y mucho mÃ¡s. Ingresa y empezÃ¡ a instalar tus aplicaciones preferidas.'},
	];

	let _installed = ['app.id.shell', 'id.webapps', 'app.1', 'app.2', 'app.7'];

	class Mock {
		constructor() {
			this.events = new EventEmitter();
		}

		log(fName, ...args) {
			let tmp = [...args];
			if (typeof tmp[tmp.length-1] === 'function') {
				tmp.pop();
			}
			let params = JSON.stringify(tmp);
			log.info('mocks', '%s.%s(%s)', this.constructor.name, fName, params.substring(1, params.length-1));
		}

		replyAsync(cb, ...args) {
			setTimeout(() => cb(...args), 50);
		}

		on( signal, callback ) {
			this.events.on(signal, callback);
		}

		once( signal, callback ) {
			this.events.once(signal, callback);
		}

		removeListener( signal, callback ) {
			this.events.removeListener(signal, callback);
		}

		emit( ...args ) {
			this.events.emit(...args);
		}
	}

	class appmgr extends Mock {
		constructor() {
			super();
			this.favs = ['app.1'];
		}

		static isEventEmitter() {
			return true;
		}

		getAll( fnc ) {
			setTimeout(() => fnc(undefined, _installed), 250);
		}

		get( id, fnc ) {
			let app = _apps.find( app => app.id === id );
			if (app) {
				app.isFavorite = this.favs.indexOf(id) > -1;
			}
			fnc(undefined,app);
		}


		getWebApps( fnc ) {
			fnc(undefined, 'id.webapps');
		}

		getShell( fnc ) {
			fnc(undefined, "app.id.shell");
		}

		isFavorite( appId, fnc) {
			fnc(undefined, this.favs.indexOf(appId) > -1);
		}

		run( appID, fnc ) {
			this.emit('application', {state: 'starting'});
			fnc(undefined, true);
			// setTimeout( () => this.emit('application', {state: 'starting'}), 500 );
			setTimeout( () => this.emit('application', {state: 'loading'}), 1000 );
			setTimeout( () => this.emit('application', {state: 'loaded'}), 1500 );
			setTimeout( () => this.emit('application', {state: 'stopped'}), 3000 );
		}

		configureService( srvId, fnc ) {
			fnc(undefined, true);
		}

		getCategories(fnc) {
			var cats = uniq(flat(compact(_apps.filter(function(app) {
				return _installed.indexOf(app.id) !== -1;
			}).map(function(app) {
				return app.categories;
			}))));
			cats.unshift('FAVORITOS');
			fnc(undefined, cats);
		}

		toggleFavorite(appId, fnc) {
			let idx = this.favs.indexOf(appId);
			if (idx >= 0) {
				this.favs.splice(idx, 1);
			} else {
				this.favs.push(appId);
			}
			fnc();
		}

		showVirtualKeyboard(params, cbk) {
			cbk(undefined, {status: 'accepted', input: '0303456'});
		}
	}

	class pkgmgr extends Mock {
		constructor() {
			super();
		}

		static isEventEmitter() {
			return true;
		}

		uninstall( opts, fnc ) {
			var app = _apps.find( app => app.pkgID === opts.id );
			if (app) {
				let idx = _installed.indexOf(app.id);
				if (idx >= 0) {
					_installed.splice(idx, 1);
					setTimeout( () => {
						fnc(undefined);
						this.emit('uninstall', opts.id);
					}, 100);
					return;
				}
			}
			fnc(new Error('app not installed'));
		}
	}

	class notification extends Mock {
		constructor() {
			super();
		}

		static isEventEmitter() {
			return true;
		}

		static getApiName(id) {
			const id2api = {
				'ar.edu.unlp.info.lifia.tvd.update': 'updatemgr',
			};
			if (id2api[id] !== undefined) {
				return id2api[id];
			} else {
				let dot = id.lastIndexOf('.');
				if (dot >= 0) {
					return id.substring(dot+1);
				}
			}
			return id;
		}

		forwardEvents( api, signal, sigName ) {
			let apiName = notification.getApiName(api);
			if (globals[apiName]) {
				globals[apiName].on(signal, (...args) => this.emit(sigName, ...args));
			} else {
				log.warn('mock', 'notification.forwardEvents: api not exist! api=%s', apiName);
			}
		}
	}

	class network extends Mock {
		constructor() {
			super();
			this.wifies = [
				{ name:'WiFi-TVD', type:'wireless', ip: '127.0.0.0', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'dns.wifi', domain: 'domain', signal: '100', isSecured: true},
				{ name:'LIFIA_01', type:'wireless', ip: '127.0.0.1', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'lifia.01', domain: 'domain', signal: '90', isSecured: false},
				{ name:'LIFIA_02', type:'wireless', ip: '127.0.0.2', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'lifia.02', domain: 'domain', signal: '80', isSecured: false},
				{ name:'LIFIA_03', type:'wireless', ip: '127.0.0.3', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'lifia.03', domain: 'domain', signal: '70', isSecured: false},
				{ name:'LIFIA_04', type:'wireless', ip: '127.0.0.4', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'lifia.04', domain: 'domain', signal: '60', isSecured: false},
				{ name:'LIFIA_05', type:'wireless', ip: '127.0.0.5', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'lifia.05', domain: 'domain', signal: '50', isSecured: false},
				{ name:'LIFIA_06', type:'wireless', ip: '127.0.0.6', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'lifia.06', domain: 'domain', signal: '40', isSecured: false},
				{ name:'LIFIA_07', type:'wireless', ip: '127.0.0.7', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'lifia.07', domain: 'domain', signal: '30', isSecured: false},
				{ name:'LIFIA_08', type:'wireless', ip: '127.0.0.8', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'lifia.08', domain: 'domain', signal: '20', isSecured: false},
				{ name:'LIFIA_09', type:'wireless', ip: '127.0.0.9', mac: 'AA:BB:CC:DD:EE:FF', gw: '127.0.0.1', dns: 'lifia.09', domain: 'domain', signal: '10', isSecured: false},
			];
			this.wired = { name:'eth0', type:'wired', ip: '192.168.0.3', mac: 'CC:BB:AA:FF:EE:DD', gw: '192.168.0.1', dns: 'dns.eth0', domain: 'domain_eth0', state: 'connected' };
			this.connections = [this.wired];
		}

		static isEventEmitter() {
			return true;
		}

		isConnected(cbk) {
			cbk(undefined, true);
		}

		activeConnection(cbk) {
			let conn = this.connections.find( c => c.type === 'wireless' );
			if (!conn) {
				conn = this.connections.find( c => c.type === 'wired' );
			}
			cbk(undefined, conn);
		}

		getConnections(cbk) {
			cbk(undefined, this.connections);
		}

		getWifiList(cbk) {
			cbk(undefined, this.wifies);
		}

		connectWifi(opts, cbk) {
			setTimeout(() => {
				let err;
				let conn = this.wifies.find( w => w.name === opts.ssid );
				if (conn) {
					if (conn.isSecured && opts.pass !== '0303456') {
						err = new Error('Invalid Pass');
					} else {
						conn.state = 'connected';
						this.connections.push(conn);
						this.emit('connection', {type: 'state', state: 'connected', connection: conn});
					}
				} else {
					err = new Error('Invalid ssid');
				}

				cbk(err, err === undefined);
			}, 3000);
		}

		disconnectWifi(ssid, cbk) {
			setTimeout(() => {
				let idx = -1;
				this.connections.forEach( (c, i) => { if (c.name === ssid) idx = i; } );
				if (idx >= 0) {
					this.connections.splice(idx, 1);
					this.emit('connection', {type: 'state', state: 'connected', connection: this.wired});
					cbk( undefined );
				} else {
					cbk( new Error('Not connected') );
				}
			}, 2000);
		}
	}

	let _videosBasePath = '/opt/tac/videos/';
	let _mockVideos = [
		_videosBasePath + 'Game_of_Thrones.mkv',
		_videosBasePath + 'simpsons.mp4',
		_videosBasePath + 'big_buck_bunny.mp4'
	];

	function fixVideo( metas ) {
		metas.forEach( (meta, index) => meta.video.src.uri = 'file://' + _mockVideos[index % _mockVideos.length] );
	}

	class VideoMock extends Mock {
		constructor() {
			super();
			this.meta = mockVideosMetaData();
			fixVideo(this.meta);
		}

		static isEventEmitter() {
			return true;
		}

		getAll(cbk) {
			cbk( undefined, this.meta );
		}

		getQueuedMovies(cbk) {
			cbk( undefined, this.meta );
		}

		resolveQueuedMovie( queueID, needAdd, cbk ) {
			cbk( undefined );
		}

		setCurrentTime( mId, elapsed, cbk ) {
			cbk();
		}

		markSeen(id,state,cbk) {
			cbk();
		}

		remove(id,cbk) {
			cbk();
		}
	}

	class SerieMock extends Mock {
		constructor() {
			super();
			this.meta = mockSeriesMetaData();
			this.series = mockSeries();
			this.nextEpisode = 0;
			fixVideo(this.meta);
		}

		static isEventEmitter() {
			return true;
		}

		add(opts, cbk) {
			this.replyAsync( cbk, new Error('Fail to add serie') );
		}

		getSeries(cbk) {
			this.replyAsync( cbk, undefined, this.series);
		}

		getEpisodes(serie,season,cbk) {
			this.replyAsync( cbk, undefined, this.meta );
		}

		setCurrentTime( id, elapsed, cbk ) {
			this.meta[this.nextEpisode].currentTime = elapsed;
			if (elapsed >= 2.0) {
				//	Con esto puedo emular, que se vea el boton continuar viendo
				this.nextEpisode = (this.nextEpisode + 1) % this.meta.length;
			}
			this.replyAsync( cbk );
		}

		markSeen(id,state,cbk) {
			this.meta[this.nextEpisode].seen = state;
			this.replyAsync( cbk );
		}

		getNextEpisode( serie, cbk ) {
			//	Siempre hay un next, si termino de ver toda la serie,
			//	el next es el primer episodio, de la primer temporada
			let episode = this.meta[this.nextEpisode];
			this.replyAsync( cbk, undefined, episode );
		}

		remove( id, cbk ) {
			this.replyAsync( cbk );
		}
	}

	class system extends Mock {
		constructor() {
			super();
			this.inDevelopmentMode = true;
			this.geoEnabled = false;
		}

		static isEventEmitter() {
			return true;
		}

		powerOff() {}
		reboot() {}
		factoryReset() {}

		licenseAccepted(cbk) {
			cbk();
		}

		getRemotePort(cbk) {
			cbk(undefined, 2000);
		}

		isFirstBoot(cbk) {
			cbk(undefined, false);
		}

		disableDevelopmentMode() {
			this.inDevelopmentMode = false;
		}

		isDevelopmentMode(cbk) {
			cbk(undefined, this.inDevelopmentMode);
		}

		getInfo(cbk) {
			cbk(undefined, {
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
			});
		}

		getLogConfig(cbk) {
			cbk(undefined, {
				enabled: false,
				maxHistorySize: 1000,
				maxWidth: null, // Disable maxWidth
				level: 'info',
				port:3045
			});
		}

		setLogConfig(cfg, cbk) {
			cbk(undefined, 'Config log changed!');
		}

		enableGeolocation(value) {
			this.geoEnabled = value;
			return value;
		}

		isGeolocationEnabled(cbk) {
			cbk(undefined, this.geoEnabled);
		}
	}

	class updatemgr extends Mock {
		constructor() {
			super();
			this.status = 'idle';
			this.changeStatus = (st, prog) => {
				this.status = st;
				this.emit('Firmware', {status: st, progress: prog});
			};
		}

		static isEventEmitter() {
			return true;
		}

		updateFirmware() {
			assert(this.status === 'downloaded');
			this.changeStatus('updating');
			setTimeout(() => this.changeStatus('idle'), 5000);
		}

		getStatus(cbk) {
			cbk(undefined, this.status);
		}

		downloadFirmwareUpdate() {
			function makeProgress(progress) {
				if (progress <= 100) {
					this.changeStatus('downloading', progress);
					setTimeout(makeProgress.bind(this,progress+10), 500);
				} else {
					this.changeStatus('downloaded');
				}
			}

			assert(this.status === 'available');
			setTimeout(makeProgress.bind(this,0), 500);
		}

		checkFirmwareUpdate() {
			assert(this.status === 'idle');
			this.changeStatus('checking');
			setTimeout(() => this.changeStatus('available'), 5000);
		}
	}

	class audio extends Mock {
		constructor() {
			super();
			this.volume = 50;
			this.isMuted = false;
		}

		static isEventEmitter() {
			return true;
		}

		getVolume() {
			return this.volume;
		}

		setVolume(vol) {
			this.volume = vol;
			this.emit('changed', {volume: this.volume, isMuted: this.isMuted});
		}

		volumeUp() {
			this.setVolume(this.volume + 5);
		}

		volumeDown() {
			this.setVolume(this.volume - 5);
		}

		mute(set, cb) {
			this.isMuted = set;
			this.emit('changed', {volume: this.volume, isMuted: this.isMuted});
			cb();
		}

		isMuted() {
			return this.isMuted;
		}

		toggleMute(cb) {
			this.mute(!this.isMuted, cb);
		}
	}

	class diskmgr extends Mock {
		constructor() {
			super();
		}

		static isEventEmitter() {
			return true;
		}

		enumerateDevices() {
		}

		get() {
		}
	}

	let mocks = [system, appmgr, pkgmgr, notification, network, updatemgr, audio, diskmgr];
	mocks.forEach( m => globals[m.name] = createMock(m) );
	globals.media = {
		image: mockImage(),
		movie: createMock( VideoMock ),
		serie: createMock( SerieMock )
	};
	globals.TacOnExit = () => {};
	globals.mocks = {
		sendMandatoryUpdate: () => {
			let onUpdateAvailable = function(evt) {
				if (evt.status == 'available') {
					window.updatemgr.removeListener('Firmware', onUpdateAvailable);
					window.updatemgr.downloadFirmwareUpdate();
				}
			};
			window.updatemgr.on('Firmware', onUpdateAvailable);
			window.updatemgr.checkFirmwareUpdate();
		}
	};

})(window);
