'use strict';

var assert = require('assert');
var tvdutil = require('tvdutil');
var bPromise = require('bluebird');
var Cec = require('./cec');

function defaultMap() {
	return [
		{ id: 0x00, code: 'VK_ENTER' },
		{ id: 0x01, code: 'VK_UP' },
		{ id: 0x02, code: 'VK_DOWN' },
		{ id: 0x03, code: 'VK_LEFT' },
		{ id: 0x04, code: 'VK_RIGHT' },
		{ id: 0x09, code: 'VK_MENU' },
		{ id: 0x0D, code: 'VK_EXIT' },
		{ id: 0x20, code: 'VK_0' },
		{ id: 0x21, code: 'VK_1' },
		{ id: 0x22, code: 'VK_2' },
		{ id: 0x23, code: 'VK_3' },
		{ id: 0x24, code: 'VK_4' },
		{ id: 0x25, code: 'VK_5' },
		{ id: 0x26, code: 'VK_6' },
		{ id: 0x27, code: 'VK_7' },
		{ id: 0x28, code: 'VK_8' },
		{ id: 0x29, code: 'VK_9' },
		{ id: 0x30, code: 'VK_CHANNEL_UP' },
		{ id: 0x31, code: 'VK_CHANNEL_DOWN' },
		{ id: 0x71, code: 'VK_BLUE' },
		{ id: 0x72, code: 'VK_RED' },
		{ id: 0x73, code: 'VK_GREEN' },
		{ id: 0x74, code: 'VK_YELLOW' },
		{ id: 0x91, code: 'VK_EXIT' },
		{ id: 0x96, code: 'VK_LIST' },
		{ id: 0x32, code: 'VK_RECALL_FAVORITE_0' }
	];
}

//	Service entry point
module.exports = function( adapter ) {
	var self = this || {};
	var _lAddr = -1;
	var _states = {};
	var _input = null;
	var _screen = null;
	var _timerID = null;
	var _cec = null;
	var _workCondition = null;
	var _keyMap = null;
	var _checkPowerTimeout = 10000;

	function defaultCfg() {
		return {
			powerTimeout: _checkPowerTimeout,
			keys: defaultMap()
		};
	}

	function convertCfg( oldCfg ) {
		if (!oldCfg.keys) {
			oldCfg.keys = defaultMap();
		}
		return oldCfg;
	}

	function getState() {
		return _states.active && _states.tv;
	}

	function updateState( val, st ) {
		if (_states[val] !== st) {
			log.verbose( 'cec', 'Update state: val=%s, old=%d, new=%d', val, _states[val], st );

			var oldState = getState();
			_states[val] = st;
			var newState = getState();

			if (oldState !== newState) {
				log.info( 'cec', 'Change screen active state: old=%d, new=%d', oldState, newState );

				//	Notify to screen
				_screen.setActive( newState );
			}
		}
	}

	function onKeyPress( evt ) {
		log.silly( 'cec', 'key press: evt=%j', evt );

		if (evt.duration !== 0) {
			return;
		}

		//	Translate key
		let keyMap = _keyMap.find( (map) => map.id === evt.keyCode );
		if (keyMap) {
			//	Notify to input
			_input.sendKey( keyMap.code, false );
			_input.sendKey( keyMap.code, true );
		}
		else {
			log.warn( 'cec', 'Invalid key code: %d', evt.keyCode );
		}
	}

	function onSourceActivated( evt ) {
		log.verbose( 'cec', 'source activated: _lAddr=%s, evt=%j', _lAddr, evt );
		assert(_lAddr >= 0);
		if (_lAddr !== evt.laddr) {
			updateState( 'active', false );
		}
		else {
			updateState( 'active', evt.active );
		}
	}

	function onCheckPower() {
		log.silly( 'cec', 'Check TV power status' );
		assert(_cec);
		assert(_lAddr >= 0);

		//	Mark working
		_workCondition.inc();

		//	Check TV status
		_cec.getDevicePowerStatus( 0, function(err,powerStatus) {
			log.silly( 'cec', 'Tv power status: err=%s, powerStatus=%d', err, powerStatus );

			if (powerStatus === 0) {
				//	On
				updateState( 'tv', true );
			}
			else {
				//	Anything other ... off
				updateState( 'tv', false );
			}

			_timerID = setTimeout( onCheckPower, _checkPowerTimeout );

			//	Mark work done!
			_workCondition.dec();
		});
	}

	function cecOpenAsync(port) {
		return new bPromise(function(resolve,reject) {
			_cec.open( port, function(err,result) {
				if (err || !result) {
					reject( tvdutil.warnErr( 'cec', 'Cannot open CEC device: port=%s', port ) );
				}
				else {
					resolve();
				}
			});
		});
	}

	function cecGetLogicalAddressAsync() {
		return new bPromise(function(resolve,reject) {
			_cec.getLogicalAddress(function(err,lAddr) {
				if (err) {
					reject( err );
				}
				else if (lAddr < 0) {
					reject( tvdutil.warnErr( 'cec', 'Invalid logical address: lAddr=%d', lAddr ) );
				}
				else {
					resolve(lAddr);
				}
			});
		});
	}

	function cecSetActiveSource(lAddr) {
		assert(lAddr >= 0);
		return new bPromise(function(resolve,reject) {
			_cec.setActiveSource( lAddr, function(err,result) {
				if (err) {
					reject(err);
				}
				else if (!result) {
					reject( tvdutil.warnErr( 'cec', 'Cannot set device as active source: lAddr=%d', lAddr ) );
				}
				else {
					resolve(lAddr);
				}
			});
		});
	}

	function doOpen(devices,cb) {
		return cecOpenAsync(devices[0].portName)
			.then(cecGetLogicalAddressAsync)
			.then(cecSetActiveSource)
			.then(function(lAddr) {
				assert(lAddr >= 0);

				log.verbose( 'cec', 'CEC opened: lAddr=%d', lAddr );

				_lAddr = lAddr;

				updateState( 'active', true );

				//	Register events
				_cec.on( 'keyPress', onKeyPress );
				_cec.on( 'sourceActivated', onSourceActivated );

				//	Monitor power status
				_timerID = setTimeout( onCheckPower, _checkPowerTimeout );

				//	TODO: Setup as screen provider

				cb( undefined, {} );
			},function(err) {
				log.warn( 'cec', 'Error trying open device: err=%s', err.message );
				cb( err );
			});
	}

	function doStop(cb) {
		_input = null;
		_screen = null;
		cb();
	}

	self.onStart = function(cb) {
		log.verbose('cec', 'onStart');

		//	Create cec object
		_cec = new Cec();
		assert(_cec);

		{	//	Load config
			var cfg = adapter.load( 'cec.json', defaultCfg(), convertCfg );
			_checkPowerTimeout = cfg.powerTimeout;
			_keyMap = cfg.keys;
		}

		//	Get screen and input services
		_screen = adapter.registry().get('screen');
		assert(_screen);

		_input = adapter.registry().get('input');
		assert(_input);

		//	Setup variables
		_lAddr = -1;
		_timerID = undefined;
		_states.tv = false;
		_states.active = false;
		_workCondition = new tvdutil.BasicWaitCondition();

		//	Detect devices
		var devices = _cec.detectAdapters();
		if (devices.length > 0) {
			//	Try open
			doOpen(devices,cb);
		}
		else {
			cb( new Error('No device available') );
		}
	};

	self.onStop = function( callback ) {
		log.verbose('cec', 'onStop: _lAddr=%d', _lAddr);

		//	Wait until nothing async is working
		_workCondition.wait(function() {
			if (_lAddr >= 0) {
				clearTimeout( _timerID );

				_cec.removeListener( 'keyPress', onKeyPress );
				_cec.removeListener( 'sourceActivated', onSourceActivated );
			}

			_cec.close(function() {
				doStop(callback);
			});
		});
	};

	return Object.freeze(self);
};

