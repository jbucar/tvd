if (!String.prototype.format) {
	String.prototype.format = function() {
		var args = arguments;
		return this.replace(/{(\d+)}/g, function(match, number) {
			return typeof args[number] != 'undefined' ? args[number] : match;
		});
	};
}

if (!Array.prototype.swap) {
	Array.prototype.swap = function (x, y) {
		var b = this[x];
		this[x] = this[y];
		this[y] = b;
		return this;
	}
}

function getURIParameter(param, asArray) {
	return document.location.search.substring(1).split('&').reduce(function(p,c) {
		var parts = c.split('=', 2).map(function(param) { return decodeURIComponent(param); });
		if(parts.length == 0 || parts[0] != param) return (p instanceof Array) && !asArray ? null : p;
		return asArray ? p.concat(parts.concat(true)[1]) : parts.concat(true)[1];
	}, []);
}

util = {
	negate: function(v) { return !v; },
	identity: function (v) { return v; },
	asset: function(v) { return assets(v) },
	nop: function(v) {},
	true: function() { return true; },
};

util.identityFn = function(v) { return function() { return v; }; };

util.forceSetLasFocusedChild = function(target, child) {
	target._spotlight = this._spotlight || {};
	target._spotlight.lastFocusedChild = child;
}

/* Utility that returns a function that only can be called once, all the other calls will been ignored.
The return value from the original call  will be preserved and returned (a.k.a memoized) */
util.once = util.memoize = function(fn) {
	var returnValue, called = false;
	return function () {
		if (!called) {
			called = true;
			returnValue = fn.apply(this, arguments);
		}
		return returnValue;
	};
};

util.asyncMemoize = function(getter) {
	var cache = [];
	var waitingQueue = [];

	return function() {
		var args = Array.prototype.slice.call(arguments, 0, arguments.length-1);
		var done = arguments[arguments.length-1];

		var key = JSON.stringify(args);
		if (cache[key]) {
			done(undefined, cache[key]);
		}
		else {
			if (!waitingQueue[key]) {
				waitingQueue[key] = [];
			}
			var waiting = waitingQueue[key];
			waiting.push(done);

			if (waiting.length === 1) {
				args.push(function(err, got) {
					if (!err) {
						cache[key] = got;
					}

					waiting.forEach(function(w) { w(err, got); });
					waiting.length = 0;
				});

				getter.apply(undefined, args);
			}
		}
	}
}


// Utility to bind a function to be called only when the first argument is truthy
util.callIf = function(fn) {
	return function(condition) {
		if(condition) {
			return fn();
		}
	}
}

util.renameKeys = function(dict, keyMap) {
	return _.reduce(dict, function(newDict, val, oldKey) {
		var newKey = keyMap[oldKey] || oldKey;
		newDict[newKey] = val;
		return newDict;
	}, {});
}

// Utility to ensure the delay of a function
util.ensureDelay = function(cbk, delay) {
	var _timeoutID = undefined;
	var _lastStChange = new Date();
	delay = delay || 2000;
	return function() {
		clearTimeout(_timeoutID);
		var now = new Date();
		var diff = now - _lastStChange;
		if (diff > delay) {
			cbk.apply(undefined, arguments);
		}
		else {
			var args = Array.prototype.slice.call(arguments);
			_timeoutID = setTimeout( function () { cbk.apply(undefined, args); }, delay - diff);
		}
		_lastStChange = now;
	}
}

// Utility to promisify nodejs style functions
util.promisify = function(fn) {
	// 'use strict';
	return function() {
		var args = Array.prototype.slice.call(arguments, 0);

		return new Promise(function(resolve, reject) {
			args.push(function(err) {
				if (!err) {
					resolve.apply(undefined, Array.prototype.slice.call(arguments, 1));
				}
				else {
					reject(err);
				}
			});
			fn.apply(undefined, args);
		});
	}
}

util.chain = function(funcs, initial) {
	return funcs.reduce(function(prev, cur) {
		return prev.then(cur);
	}, initial || Promise.resolve());
}

util.toUpperCase = function (string) {
    return string.toUpperCase();
}

util.capitalize = function (string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

util.proxy = function(dep) {
	var listeners = {};

	function on(evt, cbk) {
		(listeners[evt] = listeners[evt] || []).push(cbk);
	}

	function off(evt, cbk) {
		if (listeners[evt]) {
			listeners[evt] = listeners[evt].filter(function(l) {
				return l !== cbk;
			});
		}
		return false;
	}

	function forward(evtName) {
		dep.on(evtName, function() {
			var args = arguments;
			var lts = listeners[evtName];
			if (lts) {
				lts.forEach(function(l) {
					l.apply(undefined, args);
				});
			}
		});

		return this;
	}

	return {on: on, forward: forward, off: off};
};

util.spot = function(ctrol) {
	var p = ctrol.parent;
	var n = [];
	while(p) {
		n.push(p);
		p = p.parent;
	}

	n.reverse().forEach(function(c) {
		enyo.Spotlight.Util.dispatchEvent('onSpotlightDown', {
			direction: 'DOWN'
		}, c);
	});

	enyo.Spotlight.spot(ctrol);
}

util.objectValue = function(obj) {
	return Object.keys(obj).map(function(key) {
		return obj[key];
	});
}

util.zeroFill = function(number, pad) {
	pad = pad || 2;
	return ((Array(pad+1).join('0')).toString() + number).slice(-pad);
}

util.dateToString = function(date) {
	assert( date instanceof Date, 'Trying to convert a non date object' );
	return util.zeroFill(date.getHours()) + ':' + util.zeroFill(date.getMinutes());
}

util.parseDate = function(aDate) {
	return aDate.getFullYear() + '-' + (aDate.getMonth()+1) + '-' + aDate.getDate() + ' ' + aDate.getHours() + ':' + aDate.getMinutes() + ':' + aDate.getSeconds() + '.' + aDate.getMilliseconds();
}

util.formatDate = function(d, sep) {
	sep = sep || '/';
	return util.zeroFill(d.getDate()) + sep + util.zeroFill(d.getMonth() + 1) + sep + d.getFullYear();
}

function informError(caller, err) {
	if (err) {
		notification.emit('ShellError', { description: err.message });
		console.error('API call {0}raise an error: {1}'.format(caller ? 'in ' + caller : '', err));
	}
}

util.informError = function(caller) {
	return informError.bind(undefined, caller);
}

util.catchError = function(fn) {
	return function(err) {
		if (err) {
			informError(undefined, err);
		}
		else {
			var args = Array.prototype.slice.call(arguments, 1); // Drop error
			fn.apply(undefined, args);
		}
	}
}
