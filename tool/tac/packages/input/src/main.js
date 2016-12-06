'use strict';

var assert = require('assert');
var tvdutil = require('tvdutil');

function DummyInput() {
	var self = {};

	self.sendKey = function( keyCode, isUp, cb ) { cb(); };
	self.name = function() { return 'dummy'; };
	self.start = function() { return true; };
	self.stop = function(cb) { cb(); };

	return self;
}

function InputApi(impl) {
	var self = {};

	self.sendKey = function( keyCode, isUp, cb ) {
		impl.sendKey( keyCode, isUp, cb );
	};
	self.isAsync = true;

	//	aKeys = [{keyCode:100,isUp:true},{keyCode:100,isUp:false}]
	self.sendKeys = function( aKeys, cb ) {
		impl.sendKeys( aKeys, cb );
	};
	self.isAsync = true;

	self.sendMouseMove = function( deltaX, deltaY, cb ) {
		impl.sendMouseMove( deltaX, deltaY, cb );
	};
	self.isAsync = true;

	self.sendMouseClick = function( keyCode, isUp, cb ) {
		impl.sendMouseClick( keyCode, isUp, cb );
	};
	self.isAsync = true;

	self.getWebAPI = function() {
		return {
			'name': 'input',
			'public': ['sendKey','sendKeys', 'sendMouseMove', 'sendMouseClick']
		};
	};

	return self;
}

function InputImpl() {
	var _kbd_layout = null;
	var _mouse_layout = null;
	var _impl = null;
	var _keys = [];
	var _waitImpl = false;
	var self = {};

	function defaultCB(err) {
		if (err) {
			log.warn( 'input', 'Error sending key: err=%s', err.message );
		}
	}

	function sendNext() {
		if (_waitImpl) {
			log.silly( 'input', "sendNext skipped, waiting impl!" );
			return;
		}

		//	Pop first element
		var keyStruct = _keys.shift();
		if (keyStruct) {
			_waitImpl = true;

			//	Send to implementation
			_impl.sendKey( keyStruct.rawKey, keyStruct.isUp, function(err) {
				_waitImpl = false;
				keyStruct.cb( err );
				sendNext();
			});
		}
	}

	self.sendKey = function( keyCode, isUp, cb ) {
		if (!cb) {
			cb = defaultCB;
		}
		var keyMap = _kbd_layout[keyCode];
		if (keyMap && keyMap.raw > 0) {
			_keys.push( { rawKey: keyMap.raw, isUp: isUp, cb: cb } );
			sendNext();
		}
		else {
			cb(tvdutil.warnErr( 'input', 'Key code not maped: keyCode=%s', keyCode ));
		}
	};

	self.sendKeys = function( aKeys, cb ) {
		if (aKeys.length > 0) {
			var useCB = null;
			for (var i=0; i<aKeys.length; i++) {
				if (i === aKeys.length-1) {
					useCB = cb;
				}
				self.sendKey( aKeys[i].keyCode, aKeys[i].isUp, useCB );
			}
		}
		else if (cb) {
			cb();
		}
	};

	self.sendMouseMove = function( deltaX, deltaY, cb ) {
		if (!cb) {
			cb = defaultCB;
		}
		_impl.sendMouseMove( deltaX, deltaY, cb );
	};

	self.sendMouseClick = function( keyCode, isUp, cb ) {
		if (!cb) {
			cb = defaultCB;
		}
		var keyMap = _mouse_layout[keyCode];
		if (keyMap && keyMap.raw > 0) {
			_impl.sendKey( keyMap.raw, isUp, cb );
		}
		else {
			cb(tvdutil.warnErr( 'input', 'sendMouseClick: Key code not maped: keyCode=%s', keyCode ));
		}
	};

	self.start = function( reg, cb ) {
		assert(reg);
		assert(cb);

		var plat = reg.get('platform');
		assert(plat);

		//	Create input from factory
		_impl = plat.createInput();
		if (!_impl) {
			_impl = new DummyInput();
		}
		assert(_impl.name);
		assert(_impl.start);
		assert(_impl.stop);
		assert(_impl.sendKey);

		//	Get keyboard layout
		_kbd_layout = plat.getConfig('keyboardLayout');
		assert(_kbd_layout);
		_mouse_layout = plat.getConfig('mouseLayout');
		assert(_mouse_layout);

		//	Start implementation
		if (_impl.start()) {
			log.info('input', 'Using input implementation from: name=%s', _impl.name());
			cb( undefined, new InputApi(self) );
		}
		else {
			cb( new Error('Invalid implementation') );
		}
	};

	self.stop = function(cb) {
		assert(cb);
		_keys = [];
		_impl.stop(cb);
	};

	return self;
}

//	Service entry point
function InputApiService(adapter) {
	var self = {};
	var _impl = null;

	self.onStart = function(cb) {
		_impl = new InputImpl();
		_impl.start(adapter.registry(),cb);
	};

	self.onStop = function(cb) {
		_impl.stop(function() {
			_impl = null;
			cb();
		});
	};

	return Object.freeze(self);
}

// Export module hooks
module.exports = InputApiService;

