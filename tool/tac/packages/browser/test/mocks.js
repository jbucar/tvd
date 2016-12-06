'use strict';

var EventEmitter = require('events').EventEmitter;
var util = require('util');
var _ = require('lodash');

module.exports.CrashObserver = function() {
	var self = this || {};

	self.reports = 0;

	self.init = function(dir) {
		self.dir = dir;
		return true;
	};

	self.reportCrashes = function() {
		self.reports++;
	};

	self.getEncodedMinidumps = function() {
		return {'test.dmp': 'ABCDE'};
	};

	self.cleanCrashDirectory = function() {
	};

	return self;
};

module.exports.ServiceAdapter = function(conf, reg) {
	var self = this || {};

	self.savedFiles = {};

	self.getDataPath = function() {
		return conf.htmlshell.workPath;
	};

	self.getInstallPath = function() {
		return conf.htmlshell.installPath;
	};

	self.getTemporaryPath = function() {
		return '/tmp';
	};

	self.load = function( file, defaults ) {
		var tmp = _.cloneDeep(conf);
		_.defaults(tmp, defaults);
		return tmp;
	};

	self.save = function( file, data ) {
		self.savedFiles[file] = _.cloneDeep(data);
	};

	self.registry = function() {
		return reg;
	};

	return self;
};

module.exports.BrowserService = function() {
	var self = this || {};
	EventEmitter.call(self);

	self.loadedBrowsers = 0;
	self.closedBrowsers = 0;
	self.onBrowserLoaded = 0;
	self.onBrowserClosed = 0;
	self.apiCallsHandled = 0;
	self.apiCallErrors = 0;
	self.signalsEmited = 0;
	self.visibilityModified = 0;
	self.ready = false;
	self.keyboardLayout = {'VK_ENTER': 13};

	self.setReady = function(isReady) {
		self.ready = isReady;
		self.emit('ready', self.ready);
	};

	self.isReady = function() {
		return self.ready;
	};

	self.dumpState = function() {
		return {
			'browsers': [{
				id: 1,
				status: 'loaded',
				url: 'http://google.com',
				apis: []
			}],
			'crashes': {'test.dmp': 'ABCDE'}
		};
	};

	self.launchBrowser = function( /*url, bounds, apis, callback*/ ) {
		self.loadedBrowsers++;
		return 1;
	};

	self.closeBrowser = function( /*id*/ ) {
		self.closedBrowsers++;
		return true;
	};

	self.showBrowser = function( /*id show*/ ) {
		self.visibilityModified++;
		return true;
	};

	self.sendLaunchBrowser = function() {
		return self.loadedBrowsers++;
	};

	self.sendCloseBrowser = function() {
		self.closedBrowsers++;
	};

	self.sendShowBrowser = function() {
		self.visibilityModified++;
		return true;
	};

	self.sendResponse = function( evt ) {
		if (evt.error) {
			self.apiCallErrors++;
		}
		if (evt.params) {
			self.apiCallsHandled++;
		}
		if (evt.isSignal) {
			self.signalsEmited++;
		}
	};

	self.browserClosed = function(/*id, error*/) {};

	self.on('browserLaunched', function() {
		self.onBrowserLoaded++;
	});

	self.on('browserClosed', function() {
		self.onBrowserClosed++;
	});

	return self;
};
util.inherits(module.exports.BrowserService, EventEmitter);

module.exports.HtmlShellRemote = function() {
	var self = this || {};
	EventEmitter.call(self);

	self.realId = -1;
	self.startOk = true;
	self.stopOk = true;
	self.shouldSendResponse = true;
	self.evtCallback = null;

	self.defaultConfig = function() {
		return {};
	};

	self.start = function(cfg, evtCallback) {
		if (self.startOk) {
			self.evtCallback = evtCallback;
			self.emit('started');
			process.nextTick( function() {
				self.emit('connected');
			});
			return true;
		}
		return false;
	};

	self.stop = function(done) {
		self.emit('disconnected');
		self.emit('stopped');
		if (self.stopOk){
			done();
		} else {
			done(new Error('Fail to stop!!!'));
		}
	};

	self.launchBrowser = function( url, opts, id ) {
		var browserID = (id < 0) ? self.realId++ : id;
		self.sendMessage({
			launchBrowser: {
				browserID: browserID,
				url: url,
				options: opts
			}
		});

		if (self.shouldSendResponse) {
			process.nextTick(_.bind(self.evtCallback, self, {
				type: 'onLaunched',
				browserID: self.realId
			}));
		}
		return self.realId;
	};

	self.closeBrowser = function( browserID ) {
		self.sendMessage({closeBrowser: {'browserID': browserID}});
		if (self.shouldSendResponse) {
			process.nextTick(_.bind(self.evtCallback, self, {
				type: 'onClosed',
				browserID: browserID,
				error: undefined
			}));
		}
	};

	self.showBrowser = function( browserID, needShow ) {
		self.sendMessage({showBrowser: {
			browserID: browserID,
			show: needShow
		}});
		return true;
	};

	self.sendResponse = function( res ) {
		_.isObject(res);
		self.sendMessage({jsResponse: res});
	};

	self.sendMessage = function( /*msg*/ ) {
		// Overriden in tests to check that BrowserService methods trigger htmlshellremote messages
	};

	return self;
};

util.inherits( module.exports.HtmlShellRemote, EventEmitter );
