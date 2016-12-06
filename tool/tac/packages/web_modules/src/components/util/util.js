/* exported getURIParameter, defaultErrorHandler, assert, apiCall, apiCallAsync, registerNotification, forwardEventEmitter, currentDateTime, $ */

function getURIParameter(param, asArray) {
	return document.location.search.substring(1).split('&').reduce(function(p,c) {
		let parts = c.split('=', 2).map( param => decodeURIComponent(param) );
		if (parts.length === 0 || parts[0] != param) {
			return (p instanceof Array) && !asArray ? null : p;
		}
		let a = parts.concat(true)[1];
		return asArray ? p.concat(a) : a;
	}, []);
}

function defaultErrorHandler(err) {
	log.error( 'util', 'Generic error: err=%s', err.stack ? err.stack: err.message );
}

function assert( cond, msg ) {
	if (!cond) {
		defaultErrorHandler( new Error(msg || "Assertion failed") );
	}
}

function forwardEventEmitter(obj,evts) {
	//	Events
	obj.on = function(name, handler) {
		evts.on(name, handler);
		return () => evts.removeListener(name, handler);
	};

	obj.once = function(...args) {
		return evts.once(...args);
	};

	obj.emit = function(...args) {
		return evts.emit(...args);
	};

	obj.removeListener = function(...args) {
		return evts.removeListener(...args);
	};
}

//	This function must be removed if lodash is included
function $( fnc, ...bindArgs ) {
	return function(...args) {
		return fnc.call(this, ...bindArgs, ...args);
	};
}

function apiCall(fnc) {
	//	Remove function name
	var args = Array.prototype.slice.call(arguments, 1);
	var originalCB;
	if (typeof args[args.length-1] === 'function') {
		//	Remove function cb
		originalCB = args.pop();
	}
	//	Insert new function
	args.push(function(err) {
		if (fnc.isSignal) {
			if (originalCB) {
				originalCB.apply(undefined, arguments);
			}
		}
		else {
			if (err) {
				defaultErrorHandler(err);
			}
			else if (originalCB) {
				var args = Array.prototype.slice.call(arguments, 1); // Drop error
				originalCB.apply(undefined, args);
			}
		}
	});

	//log.verbose( 'util', 'apiCall: arguments=%d, args=%d', arguments.length, args.length );
	fnc.apply(undefined, args);
}

function apiCallAsync(fnc) {
	//	Remove function name
	var args = Array.prototype.slice.call(arguments, 1);
	return new Promise(function(resolve,reject) {
		//	Insert new function
		args.push(function(err) {
			if (fnc.isSignal) {
				resolve.apply(undefined,arguments);
			}
			else {
				if (err) {
					defaultErrorHandler(err);
					reject(err);
				}
				else {
					var args = Array.prototype.slice.call(arguments, 1); // Drop error
					resolve.apply(undefined,args);
				}
			}
		});

		fnc.apply(undefined, args);
	});
}

function registerNotification( srvID, evtName, signalName, cb ) {
	if (signalName && cb) {
		log.verbose( 'notification', 'forwardEvents: srvID=%s, evtName=%s, signalName=%s', srvID, evtName, signalName );
		apiCall( notification.forwardEvents, srvID, evtName, signalName );
	}
	if (signalName === undefined) {
		signalName = srvID;
		cb = evtName;
	}
	log.verbose( 'notification', 'registerNotification: signalName=%s', signalName );
	notification.on.isSignal = true;
	apiCall( notification.on, signalName, cb );
}

function formatDateTime( d ) {
	function zeroFill(n) {
		return ('0'+n).slice(-2);
	}
	return {
		date: zeroFill(d.getDate()) + '.' + zeroFill(d.getMonth() + 1) + '.' + d.getFullYear(),
		time: zeroFill(d.getHours()) + ':' + zeroFill(d.getMinutes())
	};
}

function currentDateTime() {
	return formatDateTime(new Date(Date.now()));

}
