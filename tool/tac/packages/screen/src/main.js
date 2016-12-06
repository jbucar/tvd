'use strict';

var assert = require('assert');
var EventEmitter = require('events').EventEmitter;
var tvdutil = require('tvdutil');

function ScreenApi() {
	var self = this;
	var _isActive = true;
	var _provider = null;
	var _events = new EventEmitter();
	tvdutil.forwardEventEmitter(self, _events);

	self.isActive = function() {
		return _isActive;
	};

	self.setActive = function(st) {
		if (_isActive !== st) {
			_isActive = st;
			_events.emit( 'changed', st );
		}
	};

	self.turnOn = function() {
		log.info( 'Screen', 'Turn on' );
		if (_provider) {
			_provider.turnOn();
		}
		self.setActive( true );
	};

	self.turnOff = function() {
		log.info( 'Screen', 'Turn off' );
		if (_provider) {
			_provider.turnOff();
		}
		self.setActive( false );
	};

	self.setProvider = function(p) {
		if (p && p.turnOn && p.turnOff) {
			_provider = p;
		}
	};

	return Object.freeze(self);
}

//	Service entry point
function ScreenService() {
	var self = {};

	self.onStart = function(cb) {
		cb( undefined, new ScreenApi() );
	};

	self.onStop = function(cb) {
		assert(cb);
		cb();
	};

	return Object.freeze(self);
}

module.exports = ScreenService;

