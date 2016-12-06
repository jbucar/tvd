'use strict';

var assert = require('chai').assert;
var EventEmitter = require('events').EventEmitter;
var tvdutil = require('tvdutil');
var Mocks = require('mocks');

var _setupCEC = null;

function LibCecMock() {
	var _open = false;
	var _powerStatus = 1;
	var _events = new EventEmitter();

	var self = {};
	tvdutil.forwardEventEmitter(self, _events);

	self.detectAdapters = function() {
		return [{ portName: "test" }];
	};
	self.open = function( port, cb ) {
		_open = true;
		cb( undefined, true );
	};
	self.close = function(cb) {
		_open = false;
		cb();
	};
	self.isOpen = function() {
		return _open;
	};
	self.getLogicalAddress = function(cb) {
		cb(undefined,1);
	};
	self.isActiveSource = function(addr,cb) {
		cb( undefined, true );
	};
	self.setActiveSource = function(addr,cb) {
		cb( undefined, true );
	};

	self.changeSourceActive = function( addr, st ) {
		var evt = {
			laddr: addr,
			active: st
		};
		_events.emit( 'sourceActivated', evt );
	};

	self.setDevicePowerStatus = function( st ) {
		_powerStatus = st;
	};

	self.getDevicePowerStatus = function( addr, cb ) {
		cb( undefined, _powerStatus );
	};

	self.sendKey = function( code, duration ) {
		var evt = {
			keyCode: code,
			duration: duration
		};
		_events.emit( 'keyPress', evt );
	};

	if (_setupCEC) {
		_setupCEC(self);
	}
	//console.log( 'self=%j, libCec=%j', self, _setupCEC );

	return self;
}

describe('cec', function() {
	var _reg = null;
	var _adapter = null;
	var _module = null;
	beforeEach( function() {
		_reg = Mocks.init('verbose');
		_reg.put('screen', new Mocks.Screen() );
		_reg.put('input', new Mocks.Input() );
		_adapter = new Mocks.ServiceAdapter(_reg);
		_adapter.load = function( file, defaultsValues ) {
			defaultsValues.powerTimeout = 1000;
			return defaultsValues;
		};
		assert(_adapter);
		_setupCEC = null;
		_module = Mocks.mockRequire( require, '../src/main', { './cec': LibCecMock } )(_adapter);
		assert( _module );
	});

	afterEach( function () {
		_module = null;
		Mocks.fin();
		_adapter = null;
		_reg = null;
	});

	it('should return tvd api on start', function(done) {
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert.isObject(api);
			_module.onStop(done);
		});
	});

	it('should update config from v1', function(done) {
		_adapter.load = function( file, defaultsValues, convertFnc ) {
			let cfgv1 = { powerTimeout: 1000 };
			let newCfg = convertFnc(cfgv1);
			assert( newCfg.keys );
			assert( newCfg.keys.length > 0 );
			return newCfg;
		};

		_module.onStart(function(err,api) {
			assert.equal(err);
			assert.isObject(api);
			_module.onStop(done);
		});
	});

	it('should update config from v2', function(done) {
		_adapter.load = function( file, defaultsValues, convertFnc ) {
			let newCfg = convertFnc(defaultsValues);
			assert( newCfg.keys );
			assert( newCfg.keys.length > 0 );
			return newCfg;
		};

		_module.onStart(function(err,api) {
			assert.equal(err);
			assert.isObject(api);
			_module.onStop(done);
		});
	});

	it('should return error if no cec device found', function(done) {
		_setupCEC = function(obj) {
			obj.detectAdapters = function() { return []; };
		};
		_module.onStart(function(err,api) {
			assert.instanceOf(err, Error, 'some message about it')
			assert.isUndefined( api );
			assert.equal( err.message, 'No device available' );
			_module.onStop( done );
		});
	});

	it('should check error on getLogicalAddress', function(done) {
		_setupCEC = function(obj) {
			obj.getLogicalAddress = function( cb ) {
				cb( new Error('pepe') );
			};
		};
		_module.onStart(function(err,api) {
			assert.isUndefined(api);
			assert.instanceOf(err, Error, 'pepe');
			assert.equal(err.message, 'pepe');
			_module.onStop(done);
		});
	});

	it('should handle error on getLogicalAddress', function(done) {
		_setupCEC = function(obj) {
			obj.getLogicalAddress = function( cb ) {
				cb( undefined, -1 );
			};
		};
		_module.onStart(function(err,api) {
			assert.instanceOf(err, Error, 'Invalid logical address: lAddr=-1');
			assert.isUndefined(api);
			_module.onStop(done);
		});
	});

	it('should check error on isActiveSource', function(done) {
		_setupCEC = function(obj) {
			obj.isActiveSource = function( addr, cb ) {
				cb( new Error('pepe') );
			};
		};
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert.isObject(api);
			_module.onStop(done);
		});
	});

	it('should fail if open fail with error', function(done) {
		_setupCEC = function(obj) {
			obj.open = function( port, cb ) {
				cb( new Error('pepe') );
			};
		};
		_module.onStart(function(err,api) {
			assert.isUndefined(api);
			assert.instanceOf(err, Error, 'Cannot open CEC device: port=test');
			_module.onStop(done);
		});
	});

	it('should fail if open fail without error', function(done) {
		_setupCEC = function(obj) {
			obj.open = function( port, cb ) {
				cb( undefined, false );
			};
		};
		_module.onStart(function(err,api) {
			assert.isUndefined(api);
			assert.instanceOf(err, Error, 'Cannot open CEC device: port=test');
			_module.onStop(done);
		});
	});

	it('should check error on setActiveSource', function(done) {
		_setupCEC = function(obj) {
			obj.setActiveSource = function( lAddr, cb ) {
				cb( new Error('pepe') );
			};
		};
		_module.onStart(function(err,api) {
			assert.isUndefined(api);
			assert.instanceOf(err, Error, 'pepe');
			_module.onStop(done);
		});
	});

	it('should fail if setActiveSource fail', function(done) {
		_setupCEC = function(obj) {
			obj.setActiveSource = function( lAddr, cb ) {
				cb( undefined, false );
			};
		}
		_module.onStart(function(err,api) {
			assert.isUndefined(api);
			assert.instanceOf(err, Error, 'Cannot set device as active source: lAddr=1');
			_module.onStop(done);
		});
	});

	describe( 'events', function() {
		var _cec = null;
		beforeEach(function(done) {
			_setupCEC = function(obj) {
				_cec = obj;
			};

			_module.onStart(function(err,api) {
				assert.isUndefined(err);
				assert.isObject(api);
				done();
			});
		});

		afterEach( function (done) {
			_module.onStop(done);
		});

		it('Wait power status', function(done) {
			this.timeout(3000);
			var count=-1;
			_reg.get('screen').on( 'changed', function(st) {
				count++;
				if (count === 0) {
					assert.equal( st, true );
					//	Set to off
					_cec.setDevicePowerStatus( 1 );
				}
				else if (count === 1) {
					assert.equal( st, false );
					done();
				}
			});

			//	Set to on
			_cec.setDevicePowerStatus( 0 );
		});

		it('source active', function(done) {
			_cec.setDevicePowerStatus( 0 );

			var count=-1;
			_reg.get('screen').on( 'changed', function(st) {
				count++;
				if (count === 0) {
					assert.equal( st, true );

					assert.equal( _reg.get('screen').isActive(), true );
					_cec.changeSourceActive( 1, true );
					assert.equal( _reg.get('screen').isActive(), true );
					_cec.changeSourceActive( 1, false );
					assert.equal( _reg.get('screen').isActive(), false );
					_cec.changeSourceActive( 1, false );
					assert.equal( _reg.get('screen').isActive(), false );

					_cec.changeSourceActive( 1, true );

					assert.equal( _reg.get('screen').isActive(), true );
					_cec.changeSourceActive( 2, true );
					assert.equal( _reg.get('screen').isActive(), false );
					_cec.changeSourceActive( 2, false );
					assert.equal( _reg.get('screen').isActive(), false );
					_cec.changeSourceActive( 1, true );
					assert.equal( _reg.get('screen').isActive(), true );

					done();
				}
			});
		});

		it('key event', function() {
			var input = _reg.get('input');
			var inputs=[];

			function testKey( keyID, keyCode ) {
				inputs = [];
				_cec.sendKey( keyID, 0 );
				_cec.sendKey( keyID, 100 );
				assert.equal( inputs.length, 2 );
				assert.equal( inputs[0].code, keyCode );
				assert.equal( inputs[0].isUp, false );
				assert.equal( inputs[1].code, keyCode );
				assert.equal( inputs[1].isUp, true );
			}

			input.sendKey = function(code,isUp) {
				inputs.push( {
					code: code,
					isUp: isUp
				});
			};

			testKey( 0, 'VK_ENTER' );
			testKey( 1, 'VK_UP' );
			testKey( 2, 'VK_DOWN' );
			testKey( 3, 'VK_LEFT' );
			testKey( 4, 'VK_RIGHT' );
			testKey( 9, 'VK_MENU' );
			testKey( 0xd, 'VK_EXIT' );

			testKey( 0x30, 'VK_CHANNEL_UP' );
			testKey( 0x31, 'VK_CHANNEL_DOWN' );

			testKey( 0x71, 'VK_BLUE' );
			testKey( 0x72, 'VK_RED' );
			testKey( 0x73, 'VK_GREEN' );
			testKey( 0x74, 'VK_YELLOW' );

			testKey( 0x91, 'VK_EXIT' );
			testKey( 0x96, 'VK_LIST' );
			testKey( 0x32, 'VK_RECALL_FAVORITE_0' );

			testKey( 0x20, 'VK_0' );
			testKey( 0x21, 'VK_1' );
			testKey( 0x22, 'VK_2' );
			testKey( 0x23, 'VK_3' );
			testKey( 0x24, 'VK_4' );
			testKey( 0x25, 'VK_5' );
			testKey( 0x26, 'VK_6' );
			testKey( 0x27, 'VK_7' );
			testKey( 0x28, 'VK_8' );
			testKey( 0x29, 'VK_9' );

			_cec.sendKey( 10, 0 );
		});
	});
});
