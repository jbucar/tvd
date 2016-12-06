'use strict';

var assert = require('assert');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );

var ioctl = require('./ioctl.node').ioctl;
assert(ioctl);

//----input.h-------------------------------------------------------------------
var EV_SYN = 0;
var EV_KEY = 1;
var EV_REL = 2;

var EV_REL_X = 0;
var EV_REL_Y = 1;

var BTN_LEFT = 0x110;

var KEY_MAX = 0x2ff;
var KEY_LEFT= 105;
var KEY_RIGHT= 106;
var KEY_ENTER=28;

//----uinput.h------------------------------------------------------------------
var uidevSize = 1116;

var UI_DEV_CREATE=21761;
var UI_DEV_DESTROY=21762;
var UI_SET_EVBIT=1074025828;
var UI_SET_KEYBIT=1074025829;
var UI_SET_RELBIT=1074025830;

var BUS_USB = 3;

// struct input_event {
// 	struct timeval time;
// 	__u16 type;
// 	__u16 code;
// 	__s32 value;
// };

// struct input_id {
// 	__u16 bustype;
// 	__u16 vendor;
// 	__u16 product;
// 	__u16 version;
// };

// #define UINPUT_MAX_NAME_SIZE	80
// struct uinput_user_dev {
// 	char name[UINPUT_MAX_NAME_SIZE];
// 	struct input_id id;
// 	__u32 ff_effects_max;
// 	__s32 absmax[ABS_CNT];
// 	__s32 absmin[ABS_CNT];
// 	__s32 absfuzz[ABS_CNT];
// 	__s32 absflat[ABS_CNT];
// };
//------------------------------------------------------------------------------


function ioctlAsync( fd, req, data ) {
	return new bPromise(function(resolv,reject) {
		var fncCallback = function(err,ret,res) {
			if (err) {
				reject(err);
			}
			else {
				assert(ret >= 0 );
				resolv(res);
			}
		};

		//console.log( 'ioctl: fd=%d, req=%d, data=%j', fd, req, data );
		if (data) {
			ioctl( fd, req, data, fncCallback );
		}
		else {
			ioctl( fd, req, fncCallback );
		}
	});
}

function initEvBuffer(type, code, value) {
	var off, structSize;
	if (process.arch === 'x64') {
		off = 16;
		structSize = 24;
	}
	else {
		off = 8;
		structSize = 16;
	}

	var buffer = new Buffer(structSize);
	buffer.fill(0);

	// var time = process.hrtime();
	// off = buffer.writeUInt32LE(time[0],off);
	// off = buffer.writeUInt32LE(Math.floor(time[1]/1000),off);
	off = buffer.writeUInt16LE(type,off);
	off = buffer.writeUInt16LE(code,off);
	off = buffer.writeInt32LE(value,off);
	return buffer;
}

function sendKeyBuffer( code, press ) {
	return initEvBuffer( EV_KEY, code, press );
}

function sendMouseRelBuffer( axis, delta ) {
	return initEvBuffer( EV_REL, axis, delta );
}

function syncBuffer() {
	return initEvBuffer( EV_SYN, 0, 0 );
}

function write(fd,buffer) {
	return fs.writeAsync( fd, buffer, 0, buffer.length );
}

function sendKey( fd, keyCode, isUp ) {
	return write( fd, sendKeyBuffer( keyCode, isUp ? false : true ) )
		.then( ()=> write( fd, syncBuffer() ) );
};

function sendMouseMove( fd, deltaX, deltaY, cb ) {
	return write( fd, sendMouseRelBuffer( EV_REL_X, deltaX ) )
		.then( ()=> write( fd, sendMouseRelBuffer( EV_REL_Y, deltaY ) ) )
		.then( ()=> write( fd, syncBuffer() ) );
};

function testKey( fd, keyCode, isUp ) {
	return sendKey( fd, keyCode, isUp );
}

function test( fd, keyCode ) {
	console.log( "test: keyCode=%d", keyCode );
	return sendKey( fd, keyCode, false )
		.delay(10)
		.then( () => sendKey( fd, keyCode, true ) );
}

function testMouse( fd ) {
	var dx = -10;
    var dy = -1;
	console.log( "testMouse: dx=%d, dy=%d", dx, dy );
	for (var i = 0; i < 20; i++) {
		sendMouseMove( fd, dx, dy )
			.delay(150)
    }
}

function doTests( fd ) {
	setTimeout( function() {
		test( fd, KEY_LEFT );
		testMouse( fd );
		doTests( fd );
	}, 5000 );
}

var _fd;
fs.openAsync( "/dev/uinput", "w+" )
	.then(function(fd) {
		_fd = fd;
	})
	.then(function() {
		//ioctl(fd, UI_SET_EVBIT, EV_KEY)
		return ioctlAsync( _fd, UI_SET_EVBIT, EV_KEY );
	})
	.then(function() {
		//ioctl(fd, UI_SET_KEYBIT, BTN_LEFT)
		return ioctlAsync( _fd, UI_SET_KEYBIT, BTN_LEFT );
	})
	.then(function() {
		var proms = [];
		for (var i=1; i<KEY_MAX; i++) {
			proms.push( ioctlAsync( _fd, UI_SET_KEYBIT, i ) );
		}
		return bPromise.all(proms);
	})
	.then(function() {
		//ioctl(fd, UI_SET_EVBIT, EV_REL)
		return ioctlAsync( _fd, UI_SET_EVBIT, EV_REL );
	})
	.then(function() {
		//ioctl(fd, UI_SET_RELBIT, REL_X)
		return ioctlAsync( _fd, UI_SET_RELBIT, EV_REL_X );
	})
	.then(function() {
		//ioctl(fd, UI_SET_RELBIT, REL_Y
		return ioctlAsync( _fd, UI_SET_RELBIT, EV_REL_Y );
	})
	.then(function() {
		var buffer = new Buffer(uidevSize);

		// memset(&uidev, 0, sizeof(uidev));
		buffer.fill(0);

		// snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
		var name = "rako sample";
		buffer.write( name, 0, name.length, 'ascii' );

		// uidev.id.bustype = BUS_USB;
		var off = buffer.writeUInt16LE(BUS_USB,80);
		// uidev.id.vendor  = 0x1;
		off = buffer.writeUInt16LE(1,off);
		// uidev.id.product = 0x1;
		off = buffer.writeUInt16LE(1,off);
		// uidev.id.version = 1;
		off = buffer.writeUInt16LE(1,off);

		// write(fd, &uidev, sizeof(uidev))
		return write( _fd, buffer );
	})
	.then(function() {
		return ioctlAsync( _fd, UI_DEV_CREATE );
	})
	.delay(2000)
	.then( ()=> doTests(_fd) )
	.delay(10000000)
	.catch(function(err) {
		console.log( err );
	})
	.finally(function() {
		console.log( 'close fd' );
		return ioctlAsync( _fd, UI_DEV_DESTROY )
			.then(function() {
				fs.close(_fd);
				_fd = null;
			});
	});

setInterval( function() {
}, 1000 );
