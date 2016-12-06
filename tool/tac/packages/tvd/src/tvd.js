'use strict';

var events = require('events');

function inherits(target, source) {
	for (var k in source.prototype)
		target.prototype[k] = source.prototype[k];
}

//	TVD setup
var Tvd = require('tvdmodule').Tvd;
inherits(Tvd, events.EventEmitter);
module.exports = Tvd;

