'use strict';

var checks = require('../util.js');
var util = require('util');
var _ = require('lodash');

var DEFAULT_BROWSER_OPTIONS = {
	bounds: {x:0,y:0,w:0,h:0},
	js: '',
	bgTransparent: false,
	focus: true,
	visible: true,
	zIndex: 0,
	plugins: [],
	enableMouse: false,
	enableGeolocation: false,
	extraUA: ""
};

// NOTE: The following scripts were minified with http://jscompress.com/ and then replaced functions with arrow functions
var MAKE_API_MIN = 'function MakeApi(n,t,o){var e=window;t.split(".").forEach(n=>{e[n]=e[n]||{},e=e[n]}),o.forEach(u=>{e[u]=(...e)=>{var o,i=e.length;return i>0&&"function"==typeof e[--i]&&(o=e[i],e=e.slice(0,i)),HtmlShellCall("RemoteExtension",{id:n,name:t,method:u,params:e},o)}})}';
/* MAKE_API source code:
function MakeApi(apiId, apiName, methods){
	var api = window;
	apiName.split('.').forEach(function(name){
		api[name] = api[name] || {};
		api = api[name];
	});
	methods.forEach( function(method) {
		api[method] = function(){
			var params = Array.prototype.slice.call(arguments);
			var nParams = params.length;
			var callback;
			if(nParams>0 && typeof params[--nParams]==="function") {
				callback = params[nParams];
				params = params.slice(0, nParams);
			}
			return HtmlShellCall("RemoteExtension", {
				id: apiId,
				name: apiName,
				method: method,
				params: params
			}, callback)
		}
	});
}*/

var EXIT_FUNCTIONS = 'TacOnExit=c=>window.onbeforeunload=e=>{c(TacExit);e.returnValue="HTMLSHELL_ASYNC_EXIT";};' +
                     'TacExit=()=>{window.onbeforeunload=null;HtmlShellCall("htmlshell","async_exit_complete");};';

var DISABLE_VPX_CODECS_MIN = 'function DisableCodecs(e){function i(i,n){return void 0===n||e.some(function(e){return-1!=n.indexOf(e)})?"":i(n)}var n=document.createElement("video");n.__proto__.canPlayType=i.bind(this,n.canPlayType.bind(n));var o=window.MediaSource;void 0!==o&&(o.isTypeSupported=i.bind(this,o.isTypeSupported.bind(o)))}';
/* DISABLE_VPX_CODECS source code:
function DisableCodecs(codecs) {
	function checkTypeSupported(originalFn, type) {
		if ((type === undefined) || codecs.some(function(c) { return type.indexOf(c) != -1; })) return '';
		return originalFn(type);
	}

	var videoElem = document.createElement("video");
	videoElem.__proto__.canPlayType = checkTypeSupported.bind(this,videoElem.canPlayType.bind(videoElem));

	var mse = window.MediaSource;
	if (mse !== undefined) {
		mse.isTypeSupported = checkTypeSupported.bind(this,mse.isTypeSupported.bind(mse));
	}
}*/

/**
 * Generic state implementation
 * @param browser The browser instance
 */
function BrowserState( browser ) {
	var self = this;

	self.load = function( /*url, options, onEvent*/ ) {
		log.warn('BrowserService', '[Browser id=%d, state=%s] Cannot load url in current state', browser.id, self.name);
		return false;
	};

	self.close = function() {
		browser.state = new ClosingState(browser);
		return true;
	};

	self.show = function( /*needShow, keyboardInput*/ ) {
		log.warn('BrowserService', '[Browser id=%d, state=%s] Cannot change browser visibility in current state', browser.id, self.name);
		return false;
	};

	self.onLaunched = function() {
		log.warn('BrowserService', '[Browser id=%d, state=%s] Launched in unexpected state, closing...', browser.id, self.name);
		browser.service.sendCloseBrowser(browser.id);
	};

	self.onClose = function( errorStr ) {
		log.silly('BrowserService', '[Browser id=%d, state=%s] Browser closed error=%s', browser.id, self.name, errorStr ? errorStr : 'none' );
		browser.state = new ClosedState(browser);
		var bError = errorStr ? new Error(errorStr) : undefined;
		browser.emitEvent({
			name: 'onClose',
			browserID: browser.id,
			error: bError
		});
		browser.notify('browserClosed', bError);
	};

	self.onLoad = function() {
		log.verbose('BrowserService', '[Browser id=%d, state=%s] Finish loading in unexpected state, ignoring event...', browser.id, self.name);
	};

	self.onApiCall = function( apiCall ) {
		log.verbose('BrowserService', '[Browser id=%d, state=%s] Cannot call api in current state', browser.id, self.name );
		self.sendApiCallResponse(browser.id, apiCall.queryID, 'Received api call in ' + self.name + ' state', undefined, false);
		return false;
	};

	self.onApiCallResponse = function( msgId, resp ) {
		log.verbose('BrowserService', '[Browser id=%d, state=%s] Received apicall response in unexpected state! resp=%j', browser.id, self.name, resp);
	};

	self.onKey = function( code, isUp ) {
		log.verbose('BrowserService', '[Browser id=%d, state=%s] Received key event in unexpected state! code=%d isUp=%j', browser.id, self.name, code, isUp);
	};

	self.onWebLog = function( /*logData*/ ) {
		log.verbose('BrowserService', '[Browser id=%d, state=%s] Received web log in unexpected state', browser.id, self.name );
	};

	self.sendApiCallResponse = function( bID, msgID, errorStr, resp, signal ) {
		var respIsArray = _.isArray(resp);
		if (resp !== undefined && !respIsArray) {
			log.warn('BrowserService', '[Browser id=%d, state=%s] Invalid apicall response params: resp=%j', browser.id, self.name, resp);
			self.sendApiCallResponse(bID, msgID, 'Received invalid api call response', undefined, signal);
		} else {
			browser.service.sendResponse({
				browserID: bID,
				queryID: msgID,
				error: errorStr,
				params: respIsArray ? JSON.stringify(resp) : '',
				isSignal: signal
			});
		}
	};

	// protected:
	self.handleApiCall = function( apiCall ) {
		var handled = false;

		var tmp;
		try {
			tmp = JSON.parse(apiCall.params);
		} catch(err) {
			log.warn('BrowserService', 'Fail to parse api call params! error: %s', err.message);
		}
		if (_.isPlainObject(tmp) && _.isArray(tmp.params) && _.isString(tmp.id) && _.isString(tmp.method)) {
			handled = browser.emitEvent({
				name: 'onAPI',
				browserID: browser.id,
				serviceID: tmp.id,
				apiName: tmp.name,
				method: tmp.method,
				params: (tmp.params.length > 0) ? tmp.params : undefined,
				resultCB: _.partial(browser.onApiCallResponse, apiCall.queryID)
			});
			if (!handled) {
				log.warn('BrowserService', '[Browser id=%d, state=%s] Api call handler was not setted', browser.id, self.name);
				self.sendApiCallResponse(browser.id, apiCall.queryID, 'Api call not handled by browser with id=' + browser.id, undefined, false);
			}
		} else {
			log.warn('BrowserService', '[Browser id=%d, state=%s] Received invalid api call: %j', browser.id, self.name, apiCall);
			self.sendApiCallResponse(browser.id, apiCall.queryID, 'Received invalid api call', undefined, false);
		}
		return handled;
	};

	self.handleApiCallResponse = function( msgId, resp ) {
		log.silly('BrowserService', 'On apicall response: %j', resp);
		self.sendApiCallResponse(browser.id, msgId, resp.error ? resp.error.message : '', resp.data, resp.isSignal);
	};

	return self;
}

/**
 * Browser.load() was never called or onClose event was received.
 * The browser is successfully closed.
 */
function ClosedState( browser ) {
	var self = this;
	BrowserState.call(self, browser);
	self.name = 'closed';

	self.load = function( url, options, onEvent ) {
		var check = checks.isValidUrl(url);
		check &= (!onEvent) || (typeof onEvent == 'function');
		check &= _.isPlainObject(options);
		check &= checks.checkJsApis(options.apis);
		if (check) {
			browser.options = _.cloneDeep(DEFAULT_BROWSER_OPTIONS);
			updateOptions(browser.options, options);
			if (checks.areValidOptions(browser.options)) {
				browser.state = new LoadingState(browser);
				browser.url = url;
				browser.onEvent = onEvent;
				browser.id = browser.service.sendLaunchBrowser(url, browser.options, browser.id);
				return true;
			} else {
				log.warn('BrowserService', '[Browser id=%d, state=%s] Fail to load url: %s. Invalid options: %j', browser.id, self.name, url, browser.options);
			}
		} else {
			log.warn('BrowserService', '[Browser id=%d, state=%s] Fail to load url: %s', browser.id, self.name, url);
		}
		return false;
	};

	self.onClose = function( /*errorStr*/ ) {
		log.warn('BrowserService', '[Browser id=%d, state=%s] Received onClose event in unexpected state', browser.id, self.name);
	};

	self.close = function() {
		log.warn('BrowserService', '[Browser id=%d, state=%s] Received close request in unexpected state', browser.id, self.name);
		return false;
	};

	function updateOptions( options, newOpts ) {
		if (!_.isUndefined(newOpts.bounds)) {
			options.bounds = _.cloneDeep(newOpts.bounds);
		}
		if (!_.isUndefined(newOpts.bgTransparent)) {
			options.bgTransparent = newOpts.bgTransparent;
		}
		if (!_.isUndefined(newOpts.focus)) {
			options.focus = newOpts.focus;
		}
		if (!_.isUndefined(newOpts.visible)) {
			options.visible = newOpts.visible;
		}
		if (!_.isUndefined(newOpts.zIndex)) {
			options.zIndex = newOpts.zIndex;
		}
		if (!_.isUndefined(newOpts.plugins)) {
			options.plugins = _.cloneDeep(newOpts.plugins);
		}
		if (!_.isUndefined(newOpts.apis)) {
			browser.apis = newOpts.apis;
		}
		if (!_.isUndefined(newOpts.enableMouse)) {
			options.enableMouse = newOpts.enableMouse;
		}
		if (!_.isUndefined(newOpts.enableGeolocation)) {
			options.enableGeolocation = newOpts.enableGeolocation;
		}
		if (!_.isUndefined(newOpts.extraUA)) {
			options.extraUA = newOpts.extraUA;
		}
		options.js = makeInitScript(newOpts.apis || []);
		if (newOpts.hwCodecsOnly) {
			options.js += DISABLE_VPX_CODECS_MIN;
			options.js += 'DisableCodecs(' + browser.service.unsupportedCodecs + ');';
		}
	}

	function makeInitScript( apis ) {
		let initScript = EXIT_FUNCTIONS;
		initScript += (apis.length > 0) ? MAKE_API_MIN : '';
		apis.forEach( function (api) {
			initScript += 'MakeApi("' + api.id + '","' + api.name + '",[';
			var count = api.exports.length;
			api.exports.forEach( function (method, idx) {
				initScript += '"' + method + '"';
				if (idx < count-1) {
					initScript += ',';
				}
			});
			initScript += ']);';
		});
		initScript += 'window.tacKeyboardLayout=' + JSON.stringify(browser.service.keyboardLayout) + ';';
		return initScript;
	}
}

/**
 * onLaunched event was received.
 * The browser is successfully loaded (it is running).
 */
function LoadedState( browser ) {
	var self = this;
	BrowserState.call(self, browser);
	self.name = 'loaded';

	self.show = function( needShow, keyboardInput ) {
		log.silly('BrowserService', '[Browser id=%d, state=%s] Change browser visibility show=%d, keyboard=%d', browser.id, self.name, needShow, keyboardInput);
		return browser.service.sendShowBrowser(browser.id, needShow, keyboardInput);
	};

	self.onLaunched = function() {
		log.warn('BrowserService', '[Browser id=%d, state=%s] Browser already launched', browser.id, self.name);
	};

	self.onLoad = function() {
		log.silly('BrowserService', '[Browser id=%d, state=%s] Browser finish loading', browser.id, self.name);
		browser.emitEvent({
			name: 'onLoaded',
			browserID: browser.id
		});
	};

	self.onApiCall = function( apiCall ) {
		return self.handleApiCall(apiCall);
	};

	self.onApiCallResponse = function( msgId, resp ) {
		return self.handleApiCallResponse(msgId, resp);
	};

	self.onKey = function( code, isUp ) {
		var vk = _.findKey(browser.service.keyboardLayout, function(keycode) {
			return keycode === code;
		});
		browser.emitEvent({
			name: 'onKey',
			browserID: browser.id,
			code: vk ? vk : 'VK_UNKNOWN',
			isUp: isUp
		});
	};

	self.onWebLog = function( logData ) {
		browser.emitEvent({
			'name': 'onWebLog',
			'browserID': browser.id,
			'logData': logData
		});
	};

	return self;
}

/**
 * Browser.close() was called. Waiting for onClose event.
 * The browser still exists.
 */
function ClosingState( browser ) {
	var self = this;
	BrowserState.call(self, browser);
	self.name = 'closing';

	browser.service.sendCloseBrowser(browser.id);

	self.onWebLog = function( logData ) {
		browser.emitEvent({
			'name': 'onWebLog',
			'browserID': browser.id,
			'logData': logData
		});
	};

	self.onApiCall = function( apiCall ) {
		return self.handleApiCall(apiCall);
	};

	self.onApiCallResponse = function( msgId, resp ) {
		return self.handleApiCallResponse(msgId, resp);
	};

	self.close = function() {
		log.warn('BrowserService', '[Browser id=%d, state=%s] Received close request while closing', browser.id, self.name);
	};
}

/**
 * Browser.load() was called. Waiting for onLaunched event.
 * The browser exist but it is no successfully loaded yet.
 */
function LoadingState( browser ) {
	var self = this;
	BrowserState.call(self, browser);
	self.name = 'loading';

	self.show = function( needShow, keyboardInput ) {
		log.silly('BrowserService', '[Browser id=%d, state=%s] Change browser visibility show=%d', browser.id, self.name, needShow, keyboardInput);
		return browser.service.sendShowBrowser(browser.id, needShow, keyboardInput);
	};

	self.onLaunched = function() {
		log.silly('BrowserService', '[Browser id=%d, state=%s] Browser loaded successfully', browser.id, self.name);
		browser.state = new LoadedState(browser);
		browser.emitEvent({name:'onLaunched', browserID:browser.id});
		browser.notify('browserLaunched');
	};
}

util.inherits(ClosedState, BrowserState);
util.inherits(LoadingState, BrowserState);
util.inherits(LoadedState, BrowserState);
util.inherits(ClosingState, BrowserState);

module.exports.Closed = ClosedState;
module.exports.Loading = LoadingState;
module.exports.Loaded = LoadedState;
module.exports.Closing = ClosingState;
