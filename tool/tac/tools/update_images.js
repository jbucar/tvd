'use strict';

var log = require('npmlog');
log.level = 'verbose';
global.log = log;

var logos = require('../packages/tdalogos/src/logos');

var opts = {
	url: 'http://ws.tda.gob.ar/rest/?method=restTac2&service_id=0&ts_id=0&on_id=0&frequency=0',
	basePath: '/tmp/',
	lastChanged: null
};

function fetch() {
	logos.fetchLogos( opts )
		.then(function(result) {
			log.verbose( 'tdalogos', 'DB fetched: len=%d, lastChanged=%s, path=%s',
						 result.logos.length, result.lastChanged, result.path );
		}, function(err) {
			log.verbose( 'tdalogos', 'Error, cannot download logos: err=%s', err.message );
		});
}

fetch()
