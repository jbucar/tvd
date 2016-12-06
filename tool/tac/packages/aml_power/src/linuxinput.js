'use strict';

var assert = require( 'assert' );
var fs = require( 'fs' );
var _ = require("lodash");

function LinuxInput( dev, cb ) {
	assert(dev);
	assert(cb);
	this.dev = dev;
	this.onEvent = cb;
	this.bufSize = 24;
	this.buf = new Buffer( this.bufSize );
	fs.open( this.dev, 'r', _.bind(this.onOpen,this) );
}

LinuxInput.prototype.onOpen = function(err,fd) {
	if (err) {
		log.error( 'LinuxInput', err );
	}
	else {
		this.fd = fd;
		this.startRead();
	}
};

LinuxInput.prototype.startRead = function() {
	fs.read( this.fd, this.buf, 0, this.bufSize, null, _.bind(this.onRead,this) );
};

LinuxInput.prototype.onRead = function(err, bytesRead) {
	if (err) {
		log.error( 'LinuxInput', err );
		this.fd = null;
	}
	else {
		//	See /usr/include/linux/input.h
		if (bytesRead == 16) {
			var event = {
				dev: this.dev,
				type: this.buf.readUInt16LE(8),
				code: this.buf.readUInt16LE(10),
				value: this.buf.readUInt16LE(12)
			};
			this.onEvent( event );
		}

		if (this.fd) {
			this.startRead();
		}
	}
};

LinuxInput.prototype.close = function(/*callback*/) {
	fs.close(this.fd,function () {});
	this.fd = undefined;
};

module.exports = LinuxInput;
