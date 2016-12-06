'use strict';

var _ = require('lodash');
var assert = require('assert');
var fs = require('fs');

function loadConfig( adapter, name ) {
	var file = adapter.getInstallPath( name + '.json' );
	log.silly( 'Platform', 'Loading configuration file: file=%s', file );
	return JSON.parse( fs.readFileSync( file ) );
}

function loadImpl( adapter, name ) {
	var file = adapter.getInstallPath( name + '.js' );
	log.silly( 'Platform', 'Loading implementation file: file=%s', file );

	var impl=require(file)();
	if (!impl) {
		throw new Error( "Invalid platform implementation" );
	}

	//	Check factory methods
	if (!impl.createAudio) {
		throw new Error( 'Factory createAudio is missing' );
	}

	if (!impl.createInput) {
		throw new Error( 'Factory createInput is missing' );
	}

	return impl;
}

function FactoryMethods(plat, build) {
	var self = this;
	var _cfg = {};

	// Merge platform and build configs into _cfg
	_.merge(_cfg, plat.cfg);
	_.merge(_cfg, build.cfg);

	self.createAudio = function() {
		return plat.impl.createAudio();
	};

	self.createInput = function() {
		return plat.impl.createInput();
	};

	self.getConfig = function(key) {
		return _cfg[key];

	};

	return Object.freeze(self);
}

// Export module hooks
module.exports = function(adapter) {
	var self = this || {};

	self.onStart = function( cb ) {
		assert(cb);

		try {
			var plat = {};
			var build = {};

			// Get platform dependent implementation and configs
			plat.impl = loadImpl( adapter, 'platform' );
			plat.cfg = loadConfig( adapter, 'platform' );

			// Get build type dependent implementation and configs
			build.cfg = loadConfig( adapter, 'build' );

			//	Return API
			cb( undefined, new FactoryMethods(plat, build) );
		}
		catch(err) {
			cb( err );
		}
	};

	self.onStop = function( callback ) {
		callback();
	};

	return Object.freeze(self);
};
