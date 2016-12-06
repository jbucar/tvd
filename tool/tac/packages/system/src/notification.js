'use strict';

var assert = require('assert');
var EventEmitter = require('events').EventEmitter;
var tvdutil = require('tvdutil');

function NotificationServiceApi( impl ) {
	var self = this;

	tvdutil.forwardEventEmitter(self, impl.events);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;
	self.once.isSignal = true;

	self.forwardEvents = function( api, signal, sigName ) {
		impl.forwardEvents( api, signal, sigName );
	};

	self.getWebAPI = function() {
		return {
			'name': 'notification',
			'public': ['on', 'once', 'emit', 'forwardEvents'],
		};
	};

	return self;
}

function NotificationImpl( reg ) {
	assert(reg);
	var _forwards = {};
	var _events = new EventEmitter();

	function registerApi( srv, fwd ) {
		srv.on( fwd.signal, fwd.fncRegister );
	}

	function onServiceAdded( apiID ) {
		var fwd = _forwards[apiID];
		if (fwd) {
			registerApi( reg.get( fwd.apiID ), fwd );
		}
	}

	function onRemoveService( apiID ) {
		var fwd = _forwards[apiID];
		if (fwd) {
			reg.get( fwd.apiID ).removeListener( fwd.signal, fwd.fncRegister );
		}
	}

	var self = this;
	self.events = _events;

	self.start = function(cb) {
		reg.on( 'added', onServiceAdded );
		reg.on( 'remove', onRemoveService );
		cb( undefined, new NotificationServiceApi(self) );
	};

	self.stop = function() {
		reg.removeListener( 'added', onServiceAdded );
		reg.removeListener( 'remove', onRemoveService );
		self.events.removeAllListeners();
	};

	self.forwardEvents = function( api, signal, sigName ) {
		var fwd = {
			apiID: api,
			signal: signal,
			name: sigName,
			fncRegister: function() {
				var params = Array.prototype.slice.call(arguments, 0);
				params.unshift(sigName);
				_events.emit.apply( _events, params );
			}
		};
		_forwards[api] = fwd;

		var srv = reg.get( api );
		if (srv) {
			registerApi( srv, fwd );
		}
	};

	return self;
}

//	Service entry point
function NotificationService(adapter) {
	var self = {};
	var impl = null;

	self.onStart = function(cb) {
		assert(cb);
		impl = new NotificationImpl( adapter.registry() );
		impl.start(cb);
	};

	self.onStop = function(cb) {
		assert(cb);
		impl.stop();
		cb();
	};

	return Object.freeze(self);
}

module.exports = NotificationService;
