'use strict';

var assert = require('assert');
var DownloaderModule = require('./downloader');
var WebModule = require('./webmodule');

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
		downloader().addModule( new DownloaderModule() )
			.then( () => remote().addModule( new WebModule() ) )
			.then( () => cb( undefined, {} ) );
	};

	self.onStop = function(cb) {
		assert(cb,'Invalid callback');
		downloader().rmModule( 'youtube' )
			.then( () => remote().rmModule( 'youtube' ) )
			.then( () => cb(), cb );
	};

	return Object.freeze(self);
}

module.exports = createService;

