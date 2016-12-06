'use strict';

var assert = require('assert');
var events = require('events');

function inherits(target, source) {
	for (var k in source.prototype)
		target.prototype[k] = source.prototype[k];
}

//	Get CEC module
var Cec = require('cecmodule').Cec;
assert(Cec);

inherits(Cec, events.EventEmitter);
module.exports = Cec;

