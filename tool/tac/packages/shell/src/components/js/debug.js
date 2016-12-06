'use strict';

(function(scope) {

// State
var current = undefined;
var kindFilter = undefined;

var loggers = {
	ERROR: console.error.bind(console),
	WARN: console.warn.bind(console),
	INFO: console.info.bind(console),
	DEBUG: console.debug.bind(console),
	TRACE: console.log.bind(console)
};

var levels = {
	ERROR: 0,
	WARN: 1,
	INFO: 2,
	DEBUG: 3,
	TRACE: 4
};

var methodStyle = 'color: DarkSlateGray; font-weight: bold;';

function lvlStyle(lvl) {
	if (lvl === levels.TRACE) {
		return 'color: SlateBlue; font-weight: bold;';
	}
	return 'font-weight: bold;';
}

var isTAC = navigator.userAgent.toLowerCase().indexOf('tac') > -1;

function doLog(lvl, kind, method, msg) {
	if (current >= levels[lvl]) {
		var args;
		var logger = loggers[lvl];
		if (!isTAC) {
			args = Array.prototype.slice.call(arguments, 3);
			args.unshift(methodStyle);
			args.unshift(lvlStyle(levels[lvl]));
			var output = sprintf('%s[%s]%s %s::%s', '%c', lvl, '%c', kind, method);
			if (msg) {
				output = output + '%c ' + msg;
			}
			args.unshift(output);
		}
		else {
			args = Array.prototype.slice.call(arguments, 4);
			var resolvedMsg = vsprintf(msg, args);
			args = [sprintf('[%s] %s::%s %s', lvl, kind, method, resolvedMsg)];
		}

		// Filtering
		if (!kindFilter || kindFilter === kind) {
			logger.apply(logger, args);
		}
	}
}

// Exports

scope.log = {
	level : levels,
	setLevel: function( level ) {
		if (typeof level === 'string') {
			level = levels[level.toUpperCase()];
		}
		current = level;
	},
	setFilter: function( kind ) {
		kindFilter = kind;
	},
	error: doLog.bind(doLog, 'ERROR'),
	warn: doLog.bind(doLog, 'WARN'),
	info: doLog.bind(doLog, 'INFO'),
	debug: doLog.bind(doLog, 'DEBUG'),
	trace: doLog.bind(doLog, 'TRACE')
};

scope.assert = function ( expr, msg ) {
	console.assert(expr, msg);
}

})(this);
