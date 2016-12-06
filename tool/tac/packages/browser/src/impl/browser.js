'use strict';

var BrowserState = require('./browserstate');

// Export Browser constructor
function Browser( browserService ) {
	var self = this || {};

	self.id = -1;
	self.url = '';
	self.service = browserService;
	self.state = new BrowserState.Closed(self);
	self.onEvent =  undefined;
	self.options = {};
	self.apis = [];

	self.load = function( url, options, onEvent ) {
		return self.state.load(url, options || {}, onEvent);
	};

	self.close = function() {
		return self.state.close();
	};

	self.show = function( needShow, keyboardInput ) {
		return self.state.show(needShow, keyboardInput);
	};

	// Signals
	self.onLaunched = function() {
		self.state.onLaunched();
	};

	self.onClose = function( errorStr ) {
		self.state.onClose(errorStr);
	};

	self.onLoad = function() {
		self.state.onLoad();
	};

	self.onApiCall = function( apiCall ) {
		return self.state.onApiCall(apiCall);
	};

	self.onApiCallResponse = function( msgId, resp ) {
		return self.state.onApiCallResponse(msgId, resp);
	};

	self.onKey = function( code, isUp ) {
		self.state.onKey(code, isUp);
	};

	self.onWebLog = function( logData ) {
		self.state.onWebLog(logData);
	};

	self.notify = function( eventName, extraParams ) {
		self.service.emit(eventName, self.id, extraParams);
	};

	self.emitEvent = function( evt ) {
		var handled = false;
		if (self.onEvent) {
			self.onEvent(evt);
			handled = true;
		}
		return handled;
	};

	self.dumpState = function() {
		return {
			id: self.id,
			status: self.state.name,
			url: self.url,
			apis: self.apis.map(function(api) {return api.name;})
		};
	};

	return self;
}

module.exports = Browser;
