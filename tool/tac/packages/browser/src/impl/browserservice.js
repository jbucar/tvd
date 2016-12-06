'use strict';

var Browser = require('./browser');
var EventEmitter = require('events').EventEmitter;
var assert = require('assert');
var util = require('util');
var fs = require('fs');
var path = require('path');
var _ = require('lodash');

function BrowserService( remote ) {
	var self = this;
	EventEmitter.call(self);

	self.shellRemote = remote;
	self.currentBrowsers = [];
	self.keyboardLayout = {};
	self.connected = false;
	self.crashObserver = null;
	self.unsupportedCodecs = '[]';

	self.start = function(cfg, crashObserver, keyboardLayout) {
		assert( crashObserver );
		self.crashObserver = crashObserver;
		self.keyboardLayout = keyboardLayout;
		self.unsupportedCodecs = JSON.stringify(cfg.htmlshell.unsupportedCodecs);

		var htmlshellDir = path.join(cfg.htmlshell.workPath, 'htmlshell');
		if (!fs.existsSync(htmlshellDir)) {
			fs.mkdirSync(htmlshellDir);
		}
		var result = self.crashObserver.init(path.join(htmlshellDir, 'crashes'));

		if (self.shellRemote.start(cfg, onRemoteEvent)) {
			self.shellRemote.on('connected', onRemoteConnected);
			self.shellRemote.on('disconnected', onRemoteDisconnected);
			self.shellRemote.on('error', onRemoteError);
			log.verbose('BrowserService', 'Loaded successfully');
		} else {
			log.error('BrowserService', 'Fail to load browser service!');
			result = false;
		}
		return result;
	};

	self.stop = function(done) {
		log.verbose('BrowserService', 'On stop');
		if (existOpenBrowsers()) {
			log.warn('BrowserService', 'Not all browsers closed');
		}
		self.shellRemote.stop(function(err) {
			self.currentBrowsers = [];
			self.crashObserver.reportCrashes();
			done(err);
		});
	};

	self.isReady = function() {
		return self.connected;
	};

	self.launchBrowser = function( url, options, onEvent ) {
		var browser = new Browser(self);
		var id = -1;
		if (self.connected && browser.load(url, options, onEvent)) {
			self.currentBrowsers.push(browser);
			id = browser.id;
			log.silly('BrowserService', 'Launching browser: id=%d, url=%s, options=%j', id, url, options);
		} else {
			log.warn('BrowserService', 'Fail to launch browser! url=%s', url);
		}
		return id;
	};

	self.closeBrowser = function( id ) {
		var browser = getBrowserById(id);
		if (self.connected && browser) {
			log.verbose('BrowserService', 'Closing browser id=%d', id);
			return browser.close();
		}
		return false;
	};

	self.showBrowser = function( id, show, focus ) {
		var browser = getBrowserById(id);
		if (self.connected && browser) {
			return browser.show(show, focus);
		}
		return false;
	};

	self.dumpState = function() {
		var state = {
			'browsers': dumpBrowsersState(),
			'crashes': self.crashObserver.getEncodedMinidumps()
		};
		self.crashObserver.cleanCrashDirectory();
		return state;
	};

	// Called by browserstate.js
	self.sendLaunchBrowser = function(url, opts, id) {
		return self.shellRemote.launchBrowser(url, opts, id);
	};

	self.sendCloseBrowser = function(id) {
		return self.shellRemote.closeBrowser(id);
	};

	self.sendShowBrowser = function(id, show, focus) {
		return self.shellRemote.showBrowser(id, show, focus);
	};

	self.sendResponse = function(resp) {
		self.shellRemote.sendResponse(resp);
	};

	function dumpBrowsersState() {
		return self.currentBrowsers.map(function(browser) {
			return browser.dumpState();
		});
	}

	function onApiCall( evt ) {
		var browser = getBrowserById(evt.browserID);
		if (browser) {
			return browser.onApiCall(evt);
		} else {
			self.sendResponse({
				browserID: evt.browserID,
				queryID: evt.queryID,
				error: 'Api call arrived from unexistent browser with id=' + evt.browserID,
				params: '',
				isSignal: false,
			});
		}
		return false;
	}

	function browserLaunched( id ) {
		log.verbose('BrowserService', 'Browser launched! id=%d', id);
		var browser = getBrowserById(id);
		if (browser) {
			browser.onLaunched();
		} else {
			log.warn('BrowserService', 'Browser launched for inexisten id=%d', id);
			self.emit('BrowserServiceError', new Error('Browser launched for inexisten id=' + id));
		}
	}

	function browserLoaded( id ) {
		log.verbose('BrowserService', 'Browser loaded! id=%d', id);
		var browser = getBrowserById(id);
		if (browser) {
			browser.onLoad();
		} else {
			log.warn('BrowserService', 'Received browser loaded event for inexisten id=%d', id);
			self.emit('BrowserServiceError', new Error('Received browser loaded event for inexisten id=' + id));
		}
	}

	function browserClosed( id, errorStr ) {
		log.verbose('BrowserService', 'Browser closed! id=%d', id);
		if (!self.currentBrowsers.some(function (browser, idx, array) {
			if (browser.id === id) {
				if (errorStr) {
					self.crashObserver.reportCrashes();
				}
				browser.onClose(errorStr);
				array.splice(idx,1);
				return true;
			}
		})) {
			log.warn('BrowserService', 'Browser with id=%d not found in current browsers', id);
		}
	}

	function onKeyboardEvent( id, code, isUp ) {
		log.silly('BrowserService', 'On key event! browserID=%d, code=%d, isUp=%d', id, code, isUp);
		var browser = getBrowserById(id);
		if (browser) {
			browser.onKey(code, isUp);
		} else {
			self.emit('BrowserServiceError', new Error('Received key event from invalid browser with id=' + id));
		}
	}

	function onBrowserLog( id, logData ) {
		var browser = getBrowserById(id);
		if (browser) {
			browser.onWebLog(logData);
		} else {
			self.emit('BrowserServiceError', new Error('Received weblog event from invalid browser with id=' + id));
		}
	}

	function getBrowserById( id ) {
		var tmp = null;
		self.currentBrowsers.some(function (browser) {
			if (browser.id === id) {
				tmp = browser;
				return true;
			}
		});
		if (!tmp) {
			log.warn('BrowserService', 'Browser with id=%d not found', id);
		}
		return tmp;
	}

	function existOpenBrowsers() {
		return self.currentBrowsers.some(function (browser) {
			return browser.state.name !== 'closing' && browser.state.name !== 'closed';
		});
	}

	function onRemoteEvent( evt ) {
		log.silly('BrowserService', 'On remote event: %j', evt);
		switch (evt.type) {
			case 'onLaunched': browserLaunched(evt.browserID); break;
			case 'onClosed': browserClosed(evt.browserID, evt.error); break;
			case 'onLoaded': browserLoaded(evt.browserID); break;
			case 'onAPI': onApiCall(evt); break;
			case 'onKey': onKeyboardEvent(evt.browserID, evt.keyCode, evt.isUp); break;
			case 'onWebLog': onBrowserLog(evt.browserID, evt.logData); break;
			default:
				log.warn('BrowserService', 'Received invalid event from htmlshellremote: %j', evt);
				self.emit('BrowserServiceError', new Error('Received invalid message from htmlshell'));
		}
	}

	function onRemoteConnected() {
		log.verbose('BrowserService', 'HtmlShellRemote connected');
		self.connected = true;
		self.emit('ready', self.connected);
	}

	function onRemoteDisconnected() {
		log.verbose('BrowserService', 'HtmlShellRemote disconnected! browsers=%s', self.currentBrowsers.length);
		self.connected = false;
		self.emit('ready', self.connected);

		var tmpBrowsers = _.clone(self.currentBrowsers);
		tmpBrowsers.forEach(function (browser) {
			browserClosed(browser.id, 'Htmlshell disconnected');
		});
	}

	function onRemoteError( error ) {
		throw new RecoveryError('[BrowserService] Received remote error: ' + error.message);
	}

	return self;
}

util.inherits(BrowserService, EventEmitter);

// Export BrowserService constructor
module.exports = BrowserService;
