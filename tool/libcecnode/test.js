'use strict';

var assert = require('assert');
var events = require('events');

var Cec = require('./cec.node').Cec;
assert(Cec);
inherits(Cec, events.EventEmitter);
exports.Cec = Cec;

function inherits(target, source) {
	for (var k in source.prototype)
		target.prototype[k] = source.prototype[k];
}

var cecAdapter = new Cec();
assert(cecAdapter);

function printVersion() {
	var versions = {};
	versions.getClientVersion = cecAdapter.getClientVersion();
	versions.getServerVersion = cecAdapter.getServerVersion();
	versions.getFirmwareVersion = cecAdapter.getFirmwareVersion();
	console.log( 'versions=%j', versions );
}

function onKeyPress( evt ) {
	console.log( 'key press: evt=%j', evt );
	if (evt.keyCode === 0) {
		close(function() {});
	}
}

function onSourceActivated( evt ) {
	console.log( 'source activated: evt=%j', evt );
}

function open(cb) {
	var devices = cecAdapter.detectAdapters();
	if (devices.length) {
		console.log( 'devices=%j', devices );

		console.log( 'isOpen=%d', cecAdapter.isOpen() );
		cecAdapter.open( devices[0].portName, cb );
		return true;
	}
	else {
		console.log( 'No devices' );
		return false;
	}
}

function close(cb) {
	console.log( "closing!" );
	cecAdapter.removeListener( 'keyPress', onKeyPress );
	cecAdapter.removeListener( 'sourceActivated', onSourceActivated );

	cecAdapter.close(cb);
}

function scan() {
	cecAdapter.getActiveDevices(function(err,activeDevices) {
		cecAdapter.getActiveSource(function(err,activeSource) {
			console.log("CEC bus information\n===================");
			activeDevices.forEach(function(dev) {
				console.log( '\tdevice:        %s', dev );
				cecAdapter.isActiveSource( dev, function(err,src) {
					console.log( '\tactive source: %s', src );
				});
				cecAdapter.getDeviceVendorId( dev, function(err,venID) {
					console.log( '\tvendor:        %s', venID );
				});
				// console.log( '\tosd string:    %s', cecAdapter.getDeviceOSDName( dev ) );
				// console.log( '\tCEC version:   %s', cecAdapter.getDeviceCecVersion( dev ) );
				// console.log( '\tpower status:  %s', cecAdapter.getDevicePowerStatus( dev ) );
				console.log( '');
			});
		});
	});
}

function testActive() {
	cecAdapter.getLogicalAddress(function(err,lAddr) {
		console.log( 'getLogicalAddress: err=%s, laddr=%d', err, lAddr );
		if (lAddr !== -1) {
			cecAdapter.isActiveSource( lAddr, function(err,isActive) {
				console.log( 'isActive(%d)=%d', lAddr, isActive );
				if (!isActive) {
					console.log( 'setActive=%d' );
					cecAdapter.setActiveSource( lAddr, function(err,res) {
						console.log( "setActiveSource: err=%s, res=%s", err, res );
					});
				}
			});
		}
	});

	cecAdapter.getActiveSource(function(err,as) {
		console.log( 'getActiveSource: err=%s, as=%s', err, as );
	});
}

function testListeners() {
}

printVersion();

function test() {
	//scan();
	//testActive();

	var lAddr=1;
	cecAdapter.isActiveSource( lAddr, function(err,isActive) {
		console.log( 'isActive: err=%s, lAddr=%d, isActive=%d', err, lAddr, isActive );
	});

	cecAdapter.getActiveSource(function(err,as) {
		console.log( 'getActiveSource: err=%s, as=%s', err, as );
	});

	cecAdapter.getDevicePowerStatus( 0, function(err,power) {
		console.log( 'getPowerStatus: err=%s, power=%s', err, power );
		if (power === 0) {
			console.log( 'TV on' );
		}
		else if (power === 1) {
			console.log( 'TV off' );
		}

	});

	setTimeout( test, 1000 );
}

open(function(err,result) {
	console.log( 'open err=%s, result=%d, isOpen=%d', err, result, cecAdapter.isOpen() );
	if (!err && result) {
		cecAdapter.on( 'keyPress', onKeyPress );
		cecAdapter.on( 'sourceActivated', onSourceActivated );

		cecAdapter.getLogicalAddress(function(err,lAddr) {
			console.log( 'getLogicalAddress: err=%s, laddr=%d', err, lAddr );
			cecAdapter.setActiveSource( lAddr, function(err,res) {
				console.log( "setActiveSource: err=%s, res=%s", err, res );
				test();
			});
		});
	}
});

