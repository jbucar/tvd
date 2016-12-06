"use strict";

var util = require('util');
var assert = require('assert');
var _ = require('lodash');
global.log = require('npmlog');
var Remote = require('../htmlshellremote');

global.log.level = 'verbose';

var testDir = __dirname;

describe('htmlremote', function() {
	describe('constructor', function() {
		it('should construct html with new', function() {
			var html = new Remote();
			assert( _.isObject( html ) );
		});

		it('should construct without new', function() {
			var html = Remote();
			assert( _.isObject( html ) );
		});

		it('should check exported methods', function() {
			var html = new Remote();
			assert( _.isObject( html ) );
			assert( _.isFunction( html.defaultConfig ) );
			assert( _.isFunction( html.launchBrowser ) );
			assert( _.isFunction( html.closeBrowser ) );
			assert( _.isFunction( html.showBrowser ) );
			assert( _.isFunction( html.on ) );
		});
	});

	describe('initialization', function() {
		var html = null;

		beforeEach(function() {
			html = new Remote();
		});

		describe('defaultConfig', function() {
			it('should check basic configuration', function() {
				var cfg = html.defaultConfig();
				assert( _.isObject( cfg ) );
			});
		});

		describe('start', function() {
			function defaultEventHandler() {
			}

			beforeEach(function() {
			});

			afterEach(function(done) {
				html.stop(done);
			});

			it('should fail with error if cmd is invalid', function(done) {
				var cfg = _.cloneDeep(html.defaultConfig());
				cfg.htmlshell.cmd = 'pepe';
				cfg.htmlshell.installPath = testDir;
				html.on('error',function(err) {
					assert( err.message == 'Fail to spawn' );
					done();
				});
				assert( html.start( cfg, defaultEventHandler ) );
			});

			it('should start and connect', function(done) {
				var cfg = _.cloneDeep(html.defaultConfig());
				cfg.htmlshell.installPath = testDir;
				cfg.htmlshell.cmd = 'mock.js';
				cfg.htmlshell.stopTimeout = 1000;

				html.on('connected', done );
				assert( html.start( cfg, defaultEventHandler ) );
			});

			it('should start/stop/start ok', function(done) {
				var cfg = _.cloneDeep(html.defaultConfig());
				cfg.htmlshell.installPath = testDir;
				cfg.htmlshell.cmd = 'mock.js';
				cfg.htmlshell.stopTimeout = 100;

				var connected=0;
				var started=0;
				var stopped=0;
				html.on('started', function() {
					started++;
				});
				html.on('stopped', function() {
					stopped++;
				});

				html.on('connected',function() {
					connected++;
					if (connected === 1) {
						html.stop(function() {
							html.start( cfg, defaultEventHandler );
						});
					}
					else {
						assert(started === 2);
						assert(stopped === 1);
						assert(connected === 2);
						done();
					}
				});

				assert( html.start( cfg, defaultEventHandler ) );
			});

			it('should start, shell connect and check invalid version', function(done) {
				var cfg = _.cloneDeep(html.defaultConfig());
				cfg.htmlshell.installPath = testDir;
				cfg.htmlshell.cmd = 'mock.js';
				cfg.version.major = 2;
				cfg.keepAliveTimeout = 20;
				cfg.htmlshell.stopTimeout = 100;
				html.on('error',function(err) {
					assert( err.message.indexOf('Version mismatch') >= 0 );
					done();
				});
				assert( html.start( cfg, defaultEventHandler ) );
			});

			it('should start, mock disconnect but not exit, kill and restart', function(done) {
				var cfg = _.cloneDeep(html.defaultConfig());
				cfg.htmlshell.installPath = testDir;
				cfg.htmlshell.cmd = 'mock.js';
				cfg.htmlshell.args.push( '--test_do_stop' );
				cfg.keepAliveTimeout = 20;

				var stopped=0;
				html.on('stopped',function() {
					stopped++;
					if (stopped == 2) {
						done();
					}
				});
				assert( html.start( cfg, defaultEventHandler ) );
			});

			it('should start, but mock must not connect and startTimeout must expire', function(done) {
				var cfg = _.cloneDeep(html.defaultConfig());
				cfg.htmlshell.installPath = testDir;
				cfg.htmlshell.cmd = 'mock.js';
				cfg.htmlshell.args.push( '--test_no_start' );
				cfg.htmlshell.startTimeout = 50;
				cfg.keepAliveTimeout = 20;
				cfg.restart = false;

				html.on('stopped', done);
				assert( html.start( cfg, defaultEventHandler ) );
			});
		});
	});

	describe('methods', function() {
		var html = null;

		function getDefaultBrowserOptions() {
			return {
				'js': '',
				'bgTransparent': true,
				'focus': false,
				'enableMouse': false,
				'enableGeolocation': true,
				'visible': true,
				'zIndex': 0,
				'extraUA': '',
				'bounds': { x:0, y:0, w:0, h:0 }
			};
		}

		function getHtmlConfig() {
			var cfg = _.cloneDeep(html.defaultConfig());
			cfg.htmlshell.installPath = testDir;
			cfg.htmlshell.cmd = 'mock.js';
			cfg.keepAliveTimeout = 1000;
			cfg.htmlshell.stopTimeout = 50;
			return cfg;
		}

		function startDefaultHtml(evt) {
			var cfg = getHtmlConfig();
			assert( html.start( cfg, evt ) );
		}

		beforeEach(function() {
			html = new Remote();
		});

		afterEach(function(done) {
			html.stop(done);
		});

		describe('launchBrowser', function() {
			it('should launch browser', function(done) {
				var browserID = null;

				function eventCallback(evt) {
					assert( evt.type === 'onLaunched' );
					assert( evt.browserID === browserID );
					done();
				}

				html.on('connected',function() {
					var opts = getDefaultBrowserOptions();
					browserID = html.launchBrowser( 'http://pepe.com', opts, -1 );
					assert( browserID >= 0 );
				});

				startDefaultHtml(eventCallback);
			});

			it('should launch browser and handle key event', function(done) {
				var browserID = -1;

				function eventCallback(evt) {
					console.log('On event:' + util.inspect(evt));
					if (evt.type === 'onKey') {
						assert( evt.browserID === browserID );
						assert( evt.keyCode === 11 );
						assert( evt.isUp === true );
						done();
					}
				}

				html.on('connected',function() {
					var opts = getDefaultBrowserOptions();
					browserID = html.launchBrowser( 'http://pepe.com', opts, -1 );
					assert( browserID >= 0 );
				});

				var cfg = getHtmlConfig();
				cfg.htmlshell.args.push( '--test_keys' );
				assert( html.start( cfg, eventCallback ) );
			});

			it('should launch browser and handle error event', function(done) {
				var browserID = null;

				function eventCallback(evt) {
					if (evt.type === 'onWebLog') {
						assert( evt.browserID === browserID );
						assert( evt.logData.level === 2 );
						assert( evt.logData.line === 12345 );
						assert( evt.logData.source === 'pepe_code' );
						assert( evt.logData.message === 'pepe_error' );
						done();
					}
				}

				html.on('connected',function() {
					var opts = getDefaultBrowserOptions();
					browserID = html.launchBrowser( 'http://pepe.com', opts, -1 );
					assert( browserID >= 0 );
				});

				var cfg = getHtmlConfig();
				cfg.htmlshell.args.push( '--test_browser_error' );
				assert( html.start( cfg, eventCallback ) );
			});

			it('should launch browser and signal on load event', function(done) {
				var browserID = null;

				function eventCallback(evt) {
					if (evt.type === 'onLoaded') {
						assert( evt.browserID === browserID );
						done();
					}
				}

				html.on('connected',function() {
					var opts = getDefaultBrowserOptions();
					browserID = html.launchBrowser( 'http://pepe.com', opts, -1 );
					assert( browserID >= 0 );
				});

				var cfg = getHtmlConfig();
				cfg.htmlshell.args.push( '--test_browser_loaded' );
				assert( html.start( cfg, eventCallback ) );
			});
		});

		describe('closeBrowser', function() {
			it('should close browser', function(done) {
				var browserID = null;
				var events = 0;

				function eventCallback(evt) {
					if (events === 0) {
						assert( evt.type === 'onLaunched' );
						assert( evt.browserID === browserID );
						html.closeBrowser( browserID );
					}
					else if (events === 1) {
						assert( evt.type === 'onClosed' );
						assert( evt.browserID === browserID );
						assert( evt.error === '' );
						done();
					}
					events++;
				}

				html.on('connected',function() {
					var opts = getDefaultBrowserOptions();
				    browserID = html.launchBrowser( 'http://pepe.com', opts, -1 );
					assert( browserID >= 0 );
				});

				startDefaultHtml(eventCallback);
			});
		});


		describe('showBrowser', function() {
			it('should show/hide browser', function(done) {
				var browserID = null;
				var events = 0;

				function eventCallback(evt) {
					if (events === 0) {
						assert( evt.type === 'onLaunched' );
						assert( evt.browserID === browserID );
						html.showBrowser( browserID, false );
						html.showBrowser( browserID, true );
						html.closeBrowser( browserID );
					}
					else if (events === 1) {
						assert( evt.type === 'onClosed' );
						assert( evt.browserID === browserID );
						assert( evt.error === '' );
						done();
					}
					events++;
				}

				html.on('connected',function() {
					var opts = getDefaultBrowserOptions();
				    browserID = html.launchBrowser( 'http://pepe.com', opts, -1 );
					assert( browserID >= 0 );
				});

				startDefaultHtml(eventCallback);
			});
		});

		describe('jsapi', function() {
			it('should test basic api ident', function(done) {
				function evtCallback(evt) {
					console.log( 'onEvent: evt=%j', evt );
					if (evt.type === 'onAPI') {
						assert( evt.browserID === 10 );
						assert( evt.queryID === 11 );
						assert( evt.params === 'pepe_params' );
						done();
					}
				}

				html.on('connected',function() {
					assert(html.sendResponse( {
						'browserID': 10,
						'queryID': 11,
						'isSignal': false,
						'error': 'pepe_error',
						'params': 'pepe_params'
					}));
				});

				var cfg = getHtmlConfig();
				assert( html.start( cfg, evtCallback ) );
			});
		});
	});

	describe('protocol_stress', function() {
		var html = null;

		function getDefaultBrowserOptions() {
			return {
				'js': '',
				'bgTransparent': true,
				'focus': false,
				'enableMouse': false,
				'enableGeolocation': true,
				'visible': true,
				'zIndex': 0,
				'extraUA': '',
				'bounds': { x:0, y:0, w:0, h:0 }
			};
		}

		beforeEach(function() {
			html = new Remote();
		});

		afterEach(function(done) {
			html.stop(done);
		});

		describe('stress', function() {
			it('should stress protocol communication', function(done) {
				var timeStress=30000;
				setTimeout( function() {
					done();
				}, timeStress-1000 );
				this.timeout(timeStress);

				function eventCallback(evt) {
					if (evt.type === 'onAPI') {
						html.sendResponse( {
							'browserID': 10,
							'queryID': 11,
							'isSignal': false,
							'error': 'pepe_error',
							'params': 'pepe_params'
						});
					}
				}

				html.on('connected',function() {
					var opts = getDefaultBrowserOptions();
					var browserID = null;
					browserID = html.launchBrowser( 'http://pepe.com', opts, -1 );
					assert( browserID >= 0 );
				});

				var cfg = _.cloneDeep(html.defaultConfig());
				cfg.htmlshell.installPath = testDir;
				cfg.htmlshell.cmd = 'mock.js';
				cfg.keepAliveTimeout = 1000;
				cfg.htmlshell.stopTimeout = 2000;
				cfg.htmlshell.args.push( '--test_stress' );
				cfg.restart = false;
				assert( html.start( cfg, eventCallback ) );
			});
		});
	});
});
