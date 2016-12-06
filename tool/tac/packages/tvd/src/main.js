'use strict';

var path = require('path');
var assert = require('assert');
var events = require('events');
var tvdutil = require('tvdutil');
var Tvd = require('./tvd');

var gingaID = 'ar.edu.unlp.info.lifia.tvd.ginga';

function getInstanceID( id ) {
	return gingaID + '.' + id;
}

function createTunerAPI( tvd, db, ev ) {
	tvd.on('scanChanged', function(state,network,percentage) {
		var evt = {
			state: state
		};

		if (state === 'end') {
			evt.count = db.count();
		}
		else if (state === 'network') {
			evt.network = network;
			evt.percentage = percentage;
		}

		log.verbose( 'tvd', 'Scan changed: evt=%j', evt );
		ev.emit( 'scanChanged', evt );
	});

	var self = {};
	self.isScanning = tvd.isScanning.bind(tvd);
	self.startScan = tvd.startScan.bind(tvd);
	self.cancelScan = tvd.cancelScan.bind(tvd);
	self.getWebAPI = function() {
		return {
			'name': 'tuner',
			'public': [],
			'private': [
				'isScanning',
				'startScan',
				'cancelScan'
			]
		};
	};

	return Object.freeze(self);
}

function createMiddlewareAPI( tvd, cfg, reg, ev ) {
	tvd.on('onMiddleware', function(evt) {
		if (evt.type !== 'download') {
			log.verbose( 'tvd', 'middleware event: evt=%j', evt );
		}

		var appMgr = reg.get('appmgr');
		assert(appMgr);
		if (evt.type === 'added') {
			var app = appMgr.addInstance( gingaID, getInstanceID(evt.id) );
			app.info.user = true;
			app._canExit = true;
		}
		else if (evt.type === 'removed') {
			appMgr.rmInstance( getInstanceID(evt.id), function() {} );
		}
		else if (evt.type === 'start') {
			var params = '';
			evt.parameters.forEach(function(param) {
				if (params.length > 0) {
					params += '&';
				}
				params += param;
			});

			var opts = {
				id:getInstanceID(evt.id),
				params:params
			};
			appMgr.runAsync( opts, function() {} );
		}
		else if (evt.type === 'stop') {
			appMgr.stop( getInstanceID(evt.id), function() {} );
		}

		ev.emit( 'middleware', evt );
	});

	var self = {};

	self.enableMiddleware = function(st) {
		tvd.enableMiddleware(st);
		cfg.enableGinga = st;
	};

	self.isMiddlewareEnabled = function() {
		return cfg.enableGinga;
	};

	self.disableApplications = function(st) {
		tvd.disableApplications(st);
	};

	self.runApplication = function( appID ) {
		log.verbose( 'tvd', 'Run application: tacID=%s', appID );
		tvd.runApplication(appID);
	};

	self.getWebAPI = function() {
		return {
			'name': 'middleware',
			'public': [],
			'private': [
				'enableMiddleware',
				'isMiddlewareEnabled',
				'runApplication',
				'disableApplications'
			]
		};
	};

	return Object.freeze(self);
}

function createDBAPI( db, ev ) {
	db.emit = function() {
		ev.emit.apply(ev,arguments);
	};

	var self = {};
	self.count = db.count.bind(db);
	self.getAll = db.getAll.bind(db);
	self.remove = db.remove.bind(db);
	self.getWebAPI = function() {
		return {
			'name': 'db',
			'public': [],
			'private': [
				'count',
				'getAll',
				'remove'
			]
		};
	};

	return Object.freeze(self);
}

function createChannelAPI( db ) {
	var self = {};

	self.get = function( chID ) {
		var ch = db.get( chID );
		if (ch) {
			return {
				"id": ch.id,
				"channel": ch.channel,
				"name": ch.name,
				"logo": ch.logo,
				"isMobile": ch.isMobile,
				"category": ch.category,
				"info": ch.info,
				"isProtected": ch.isProtected(),
				"isFavorite": ch.isFavorite(),
				"isBlocked": ch.isBlocked(),
				"parentalAge": ch.parentalAge(),
				"parentalContent": ch.parentalContent()
			};
		}
	};

	self.toggleFavorite = function( chID ) {
		var ch = db.get( chID );
		if (ch) {
			ch.toggleFavorite();
		}
	};

	self.toggleBlocked = function( chID ) {
		var ch = db.get( chID );
		if (ch) {
			return ch.toggleBlocked();
		}
	};

	self.getShow = function( chID ) {
		var ch = db.get( chID );
		if (ch) {
			return ch.getShow();
		}
	};

	self.getShowsBetween = function( chID, first, last ) {
		var ch = db.get( chID );
		if (ch) {
			return ch.getShowsBetween( first, last );
		}
	};

	self.getWebAPI = function() {
		return {
			'name': 'channel',
			'public': [],
			'private': [
				'get',
				'toggleFavorite',
				'toggleBlocked',
				'getShow',
				'getShowsBetween'
			]
		};
	};

	return Object.freeze(self);
}

function createSessionAPI( tvd, ev ) {
	var session = tvd.session();
	assert(session);

	//	Session
	session.emit = function() {
		ev.emit.apply(ev,arguments);
	};

	var self = {};
	self.enableSession = session.enable.bind(session);
	self.isSessionEnabled = session.isEnabled.bind(session);

	self.checkSession = session.check.bind(session);
	self.isBlocked = session.isBlocked.bind(session);
	self.expireSession = session.expire.bind(session);

	self.getSessionTimeExpiration = session.getTimeExpiration.bind(session);
	self.setSessionTimeExpiration = session.setTimeExpiration.bind(session);

	self.restrictSex = session.restrictSex.bind(session);
	self.isSexRestricted = session.isSexRestricted.bind(session);
	self.restrictViolence = session.restrictViolence.bind(session);
	self.isViolenceRestricted = session.isViolenceRestricted.bind(session);
	self.restrictDrugs = session.restrictDrugs.bind(session);
	self.isDrugsRestricted = session.isDrugsRestricted.bind(session);
	self.restrictAge = session.restrictAge.bind(session);
	self.ageRestricted = session.ageRestricted.bind(session);

	self.getWebAPI = function() {
		return {
			'name': 'session',
			'public': [],
			'private': [
				'enableSession',
				'isSessionEnabled',
				'checkSession',
				'isBlocked',
				'expireSession',
				'getSessionTimeExpiration',
				'setSessionTimeExpiration',
				'restrictSex',
				'isSexRestricted',
				'restrictViolence',
				'isViolenceRestricted',
				'restrictDrugs',
				'isDrugsRestricted',
				'restrictAge',
				'ageRestricted'
			]
		};
	};

	return Object.freeze(self);
}

function createPlayerAPI( tvd, ev ) {
	var player = tvd.defaultPlayer();
	assert(player);

	//	Player
	player.emit = function() {
		ev.emit.apply(ev,arguments);
	};

	var self = {};
	self.current = player.current.bind(player);
	self.isProtected = player.isProtected.bind(player);
	self.currentNetworkName = player.currentNetworkName.bind(player);
	self.status = player.status.bind(player);
	self.change = player.change.bind(player);
	self.nextChannel = player.nextChannel.bind(player);
	self.nextFavorite = player.nextFavorite.bind(player);
	self.showMobile = player.showMobile.bind(player);
	self.nextVideo = player.nextVideo.bind(player);
	self.videoInfo = player.videoInfo.bind(player);
	self.nextAudio = player.nextAudio.bind(player);
	self.audioInfo = player.audioInfo.bind(player);
	self.nextSubtitle = player.nextSubtitle.bind(player);
	self.subtitleInfo = player.subtitleInfo.bind(player);

	self.getWebAPI = function() {
		return {
			'name': 'player',
			'public': [],
			'private': [
				//	Player: status
				'current',
				'isProtected',
				'status',
				//	Player: change
				'change',
				'nextChannel',
				'nextFavorite',
				'showMobile',
				//	Player: media
				'nextVideo',
				'videoInfo',
				'nextAudio',
				'audioInfo',
				'nextSubtitle',
				'subtitleInfo'
			]
		};
	};

	return Object.freeze(self);
}

//	Wrap tvd to service
function createAPIs(tvd,reg,cfg) {
	var ev = new events.EventEmitter();
	var db = tvd.channels();
	assert(db);

	var _logosProviders = [];

	var self = {};
	tvdutil.forwardEventEmitter(self,ev);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	self.tuner = createTunerAPI(tvd,db,ev);
	self.db = createDBAPI( db, ev );
	self.channel = createChannelAPI( db );
	self.session = createSessionAPI( tvd, ev );
	self.player = createPlayerAPI( tvd, ev );
	self.middleware = createMiddlewareAPI(tvd,cfg,reg,ev);

	//	Logos
	self.addLogoProvider = function( p ) {
		_logosProviders.push( p );
	};

	self.removeLogoProvider = function( p ) {
		var index = _logosProviders.indexOf(p);
		if (index >= 0) {
			_logosProviders.splice(index,1);
		}
	};

	self.getLogo = function( ch ) {
		var best = null;
		_logosProviders.forEach(function(p) {
			try {
				var result = p( ch );
				if (result && ((best && best.priority < result.priority) || !best)) {
					best = result;
				}
			} catch(err) {
				log.warn( 'tvd', 'getLogo error: err=%s', err.message );
			}
		});
		return best;
	};

	self.updateLogos = function() {
		ev.emit( 'logosChanged' );
	};

	self.getWebAPI = function() {
		return {
			'name': 'tvd',
			'modules': ['tuner', 'db', 'channel', 'session', 'player', 'middleware'],
			'public': [],
			'private': ['on']
		};
	};

	return Object.freeze(self);
}

// Export module hooks
module.exports = function(adapter) {
	var self = this || {};
	var tvd = null;
	var _cfg = null;
	var _api = null;

	function getCfg( dataPath, tmpPath ) {
		return {
			"dbDir": path.join(dataPath,'db'),
			"ramDisk": path.join(tmpPath,'tvdRamDisk'),
			"country": "ARG",
			"area": -1,
			"time_offset": 0,
			"enableGinga": false
		};
	}

	function getLogo( ch ) {
		return _api ? _api.getLogo( ch ) : null;
	}

	function onScreenActiveChanged( isActive ) {
		log.silly('TvdService', 'Screen active changed: isActive=%d', isActive );
		if (!isActive) {
			_api.player.change( -1 );	//	Do not save last channel
		}
	}

	function convertConfig( oldData ) {
		return oldData;
	}

	self.onStart = function(cb) {
		log.verbose('TvdService', 'onStart');

		//	Load configuration
		_cfg = adapter.load('tvd.json', getCfg(adapter.getDataPath(),adapter.getTemporaryPath()), convertConfig );

		//	Load tvd module
		tvd = new Tvd();
		if (!tvd.load(_cfg)) {
			cb( new Error( 'Cannot load tvd configuration' ) );
			return;
		}

		//	Setup logo provider
		tvd.addLogoProvider( getLogo );

		if (!tvd.start()) {
			cb( new Error( 'Cannot start tvd module' ) );
			return;
		}

		//	Create api
		_api = createAPIs(tvd,adapter.registry(),_cfg);

		{	//	Setup screen dependency
			var screen = adapter.registry().get('screen');
			assert(screen);
			screen.on('changed', onScreenActiveChanged );
		}

		cb( undefined, _api );
	};

	self.onStop = function( callback ) {
		log.silly('TvdService', 'onStart');

		adapter.save( 'tvd.json', _cfg );

		{	//	Remove listener for screen active changed event
			var screen = adapter.registry().get('screen');
			screen.removeListener('changed', onScreenActiveChanged );

			//	Force stop
			onScreenActiveChanged( false );
		}

		tvd.stop();
		_api = null;
		callback();
	};

	return Object.freeze(self);
};

