'use strict';

var events = require('events');

var Tvd = require('./tvdmodule.node').Tvd;
inherits(Tvd, events.EventEmitter);
exports.Tvd = Tvd;

function inherits(target, source) {
  for (var k in source.prototype)
    target.prototype[k] = source.prototype[k];
}

