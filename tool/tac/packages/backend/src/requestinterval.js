'use strict';

var assert = require('assert');
var tvdutil = require('tvdutil');

function RequestInterval() {
	var self = this || {};
	var _url = null;
	var _method = null;
	var _onResponseFnc = null;
	var _getRequestFnc = null;
	var _retryTimeout = (5*60*1000);
	var _checkTimeout = (60*60*1000);
	var _timer = null;
	var _running = false;

	function onRequest( err, res ) {
		if (!_running) {
			//	Ingore response
			return;
		}

		if (err || (res.statusCode != 200) || !res.data) {
			var rErr;
			if (err) {
				rErr = tvdutil.warnErr( 'RequestInterval', 'Request failed: err=%s', err.message );
			}
			else if (!res.data) {
				rErr = tvdutil.warnErr( 'RequestInterval', 'Request failed: response empty' );
			}
			else {
				rErr = tvdutil.warnErr( 'RequestInterval', 'Request failed: status code=%s', res.statusCode );
			}

			_onResponseFnc( rErr );

			//	Retry post
			self.forceRequest( _retryTimeout );
		}
		else {
			//	Process response
			if (_onResponseFnc( undefined, res.data )) {
				//	Check again
				self.forceRequest( _checkTimeout );
			}
		}
	}

	function doRequest() {
		log.verbose( 'RequestInterval', 'Request: url=%s', _url );
		_timer = null;
		var opts = {
			"url": _url,
			"data": _getRequestFnc(),
			"method": _method
		};
		tvdutil.doRequest( opts, onRequest );
	}

	function cleanup() {
		if (_timer) {
			clearTimeout(_timer);
			_timer = null;
		}
	}

	self.forceRequest = function( timeout ) {
		if (_running) {
			cleanup();
			if (timeout) {
				_timer = setTimeout( doRequest, timeout );
			}
			else {
				doRequest();
			}
		}
	};

	self.config = function(options) {
		assert(options.url);
		assert(options.method);
		_url = options.url;
		_method = options.method;
		if (options.retryTimeout) {
			_retryTimeout = options.retryTimeout;
		}
		if (options.checkTimeout) {
			_checkTimeout = options.checkTimeout;
		}
		if (_timer) {
			self.forceRequest( _retryTimeout );
		}
	};

	self.start = function( options, getRequest, onResponse ) {
		assert(options);
		assert(getRequest);
		assert(onResponse);
		log.verbose( 'RequestInterval', 'Start: url=%s', options.url );
		self.config( options );
		_getRequestFnc = getRequest;
		_onResponseFnc = onResponse;
		_running = true;
		self.forceRequest( _retryTimeout );
	};

	self.stop = function() {
		log.verbose( 'RequestInterval', 'Stop: url=%s', _url );
		if (_url) {
			cleanup();
			_url = null;
			_method = null;
			_running = false;
		}
	};

	return Object.freeze(self);
}

//	system service methods
function makeRequestOptions( cfg ) {
	var options = {};
	options.url = cfg.url;
	options.method = "GET";
	options.retryTimeout = cfg.retryTimeout;
	options.checkTimeout = cfg.checkTimeout;
	return options;
}

module.exports.create = RequestInterval;
module.exports.makeOptionsFromConfig = makeRequestOptions;

