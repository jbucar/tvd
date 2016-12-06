'use strict';

var assert = require('assert');
var EventEmitter = require('events').EventEmitter;
var tvdutil = require('tvdutil');

var defaultVolume = 50;

function defaultCfg() {
	return {
		volume: defaultVolume,
		muted: false,
	};
}

function FakeAudio() {
	var self = this;

	self.name = function() {
		return "Dummy";
	};

	self.setVolume = function( vol, cb ) {
		cb(null);
	};

	self.mute = function( st, cb ) {
		cb(null);
	};

	return Object.freeze(self);
}

function AudioApi(impl) {
	var _muted = false;
	var _volume = defaultVolume;
	var _events = new EventEmitter();

	function incVolume(cur,count) {
		var newVol = cur + count;
		if (newVol < 0 ) {
			newVol = 0;
		}
		else if (newVol > 100) {
			newVol = 100;
		}
		return newVol;
	}

	function emitChanged() {
		_events.emit('changed',{ volume: _volume, isMuted: _muted });
	}

	var self = this;

	//	Events
	tvdutil.forwardEventEmitter(self,_events);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	self.setVolume = function( vol, cb ) {
		log.verbose('Audio', 'Set volume: vol=%d', vol );
		impl.setVolume( vol, function(err) {
			if (err) {
				log.warn('Audio', 'Fail to set volume: vol=%d, err=%s', vol, err.message);
				if (cb) {
					cb(err);
				}
			}
			else {
				_volume = vol;
				_muted = false;
				emitChanged();
				if (cb) {
					cb();
				}
			}
		});
	};

	self.getVolume = function() {
		return _volume;
	};

	self.mute = function( set, cb ) {
		log.verbose('Audio', 'Mute: state=%d', set );
		impl.mute( set, function(err) {
			if (err) {
				log.warn('Audio', 'Fail to toggle mute: state=%d, err=%s', set, err.message );
				if (cb) {
					cb(err);
				}
			}
			else {
				_muted = set;
				emitChanged();
				if (cb) {
					cb(null);
				}
			}
		});
	};

	self.isMuted = function() {
		return _muted;
	};

	self.toggleMute = function(cb) {
		self.mute( !self.isMuted(), cb );
	};

	self.volumeUp = function(cb) {
		self.setVolume( incVolume( self.getVolume(), 5 ), cb );
	};

	self.volumeDown = function(cb) {
		self.setVolume( incVolume( self.getVolume(), -5 ), cb );
	};

	self.getWebAPI = function() {
		return {
			'name': 'audio',
			'public': ['setVolume','getVolume','volumeUp', 'volumeDown','mute', 'isMuted', 'toggleMute', 'on'],
		};
	};

	return Object.freeze(self);
}

// Export module hooks
function createModule( adapter ) {
	var self = {};
	var _api = null;
	var _config = null;

	self.onStart = function(cb) {
		_config = adapter.load('audio.json', defaultCfg());
		var factory = adapter.registry().get('platform');
		assert(factory);
		assert(factory.createAudio);

		//	Create implementation
		var impl = factory.createAudio();
		if (!impl) {
			impl = new FakeAudio();
		}

		//	Create API
		assert(impl.mute);
		assert(impl.setVolume);
		assert(impl.name);
		_api = new AudioApi(impl);

		log.info( 'Audio', 'Using audio implementation: name=%s', impl.name() );

		//	Setup initial volume
		_api.setVolume(_config.volume, function(err) {
			if (err) {
				log.warn( 'Audio', 'Fail to setup initial volume, err=%s', err.message );
			}
			_api.mute( _config.muted, function(err) {
				if (err) {
					log.warn( 'Audio', 'Fail to setup initial mute state, err=%s', err.message );
				}

				cb( undefined, _api );
			});
		});
	};

	self.onStop = function( callback ) {
		_config.volume = _api.getVolume();
		_config.muted = _api.isMuted();
		adapter.save('audio.json', _config);
		_api = null;
		callback();
	};

	return Object.freeze(self);
}

module.exports = createModule;

