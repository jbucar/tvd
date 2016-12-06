'use strict';

var assert = require('assert');
var DialModule = require('./dial');

//	Service entry point
function createService(adapter) {
	var self = {};

	function remote() {
		return adapter.registry().get('remoteapi');
	}

	self.onStart = function(cb) {
		assert(cb);
		remote().addModule( new DialModule() )
			.then( () => cb( undefined, {} ) );
	};

	self.onStop = function(cb) {
		assert(cb);
		remote().rmModule( 'dial' ).then(cb,cb);
	};

	return Object.freeze(self);
}

module.exports = createService;

