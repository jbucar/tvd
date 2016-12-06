'use strict';

var assert = require('assert');
var DownloaderModule = require('./downloader');
var WebModule = require('./webmodule');
var Rtorrent = require('rtorrent');

//	Service entry point
function createService(adapter) {
	var self = {};

	function downloader() {
		return adapter.registry().get('downloader');
	}

	function remote() {
		return adapter.registry().get('remoteapi');
	}

	self.onStart = function(cb) {
		assert(cb,'Invalid callback');

 		let torrent = new Rtorrent( 'localhost', 5001 );
		
		downloader().addModule( new DownloaderModule(torrent) )
			.then( () => remote().addModule( new WebModule(torrent) ) )
			.then( () => cb( undefined, {} ) );
	};

	self.onStop = function(cb) {
		assert(cb,'Invalid callback');
		downloader().rmModule( 'torrent' )
			.then( () => remote().rmModule( 'torrent' ) )
			.then( () => cb(), cb );
	};

	return Object.freeze(self);
}

module.exports = createService;

