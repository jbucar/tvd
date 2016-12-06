var util = require('util');
var fs = require('fs');
var Tvd = require('./tvd').Tvd;

var cfg = {
	"dbDir": "/tmp/pepe",
	"ramDisk": "/tmp/ramDisk",
	"country": "ARG",
	"area": -1,
	"time_offset": 0,
	"enableGinga": false
};

var tvd = new Tvd();

function doStop() {
}

function scanChanged(state,network,porcentage) {
	if (state === 'end') {
		console.log( "[tvd] On end scan" );
		showChannels();
	}
	else if (state === 'network') {
		console.log( "[tvd] On scan network: network: %s, porcentage=%d", network, porcentage );
	}
	else {
		console.log( "[tvd] On begin scan" );
	}
}

function channelChanged( id ) {
	console.log( "[tvd] Channel changed!" );
}

function onChannelRemoved( id ) {
	console.log( "[tvd] Channel removed!" );
}

function onChannelAdded( id ) {
	console.log( "[tvd] Channel added!" );
}

function sessionEnabled( isEnabled ) {
	console.log( "[tvd] Session enabled: isEnabled=%d", isEnabled );
}

function sessionActiveChanged( isActive ) {
	console.log( "[tvd] Session active: isActive=%d", isActive );
}

function checkSession() {
	var ses = tvd.session();
	if (!ses.isEnabled()) {
		ses.enable( 'pepe' );
	}
	console.log( "[tvd] Start check session: isEnabled=%d", ses.isEnabled() );
	console.log( "[tvd] Check session: isEnabled=%d, isBlocked=%d, check incorrecto=%d",
				 ses.isEnabled(), ses.isBlocked(), ses.check( 'pepe1' ) );
	console.log( "[tvd] Check session: isEnabled=%d, isBlocked=%d, check correcto=%d",
				 ses.isEnabled(), ses.isBlocked(), ses.check( 'pepe' ) );
	console.log( "[tvd] Check session not blocked: isEnabled=%d, isBlocked=%d",
				 ses.isEnabled(), ses.isBlocked() );
	ses.expire();
	console.log( "[tvd] Check session expired: isEnabled=%d, isBlocked=%d",
				 ses.isEnabled(), ses.isBlocked() );

	tvd.stop();
}

function showChannels() {
	var db = tvd.channels();
	var chs = db.getAll();
	console.log( 'channels: getAll=%j', chs );

	chs.forEach(function(chID) {
		var ch = db.get( chID );
		var show = ch.getShow();
		console.log( 'channel: %j', ch );
		console.log( 'show: %j', show );
	});

	var player = tvd.createPlayer();
	if (!player) {
		console.log( 'Warning, cannot create a new player' );
		tvd.stop();
		return;
	}
	player.emit = function() {
		tvd.emit.apply(tvd,arguments);
	}
	tvd.on('channelChanged', channelChanged );

	if (player.current() === -1) {
		player.change( 0 );
	}

	setTimeout( function() {
		checkSession();
	}, 10000 );
}

if (!tvd.load( cfg )) {
	setTimeout( function() {
		console.log( 'Saliendo por error en load' );
	}, 1000 );
}
else if (tvd.start()) {
	console.log( '[tvd] tvd started' );

	//	Setup scan callback
	tvd.session().emit = function() {
		tvd.emit.apply(tvd,arguments);
	}
	tvd.channels().emit = function() {
		tvd.emit.apply(tvd,arguments);
	}
	tvd.on( 'scanChanged', scanChanged );
	tvd.on( 'sessionEnabled', sessionEnabled );
	tvd.on( 'sessionActive', sessionActiveChanged );
	tvd.on( 'channelAdded', onChannelAdded );
	tvd.on( 'channelRemoved', onChannelRemoved );

	if (tvd.session().isBlocked()) {
		tvd.session().check('pepe');
		tvd.session().enable('');
	}

	console.log( '[tvd] db count=%d', tvd.channels().count() );
	tvd.startScan();
}
