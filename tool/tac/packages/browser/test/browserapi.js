'use strict';

var BrowserApi = require('../src/browserapi.js');
var BrowserMocks = require('./mocks');
var Mocks = require('mocks');
var assert = require('assert');

describe('BrowserApi', function() {
	var _browserService = null;
	var _browserApi = null;

	beforeEach( function() {
		Mocks.init('verbose');
		_browserService = new BrowserMocks.BrowserService();
		_browserApi = new BrowserApi(_browserService);
	});

	afterEach( function() {
		_browserApi = null;
		_browserService = null;
		Mocks.fin();
	});

	describe('#launchBrowser()', function() {
		it('should forward message to browser service', function() {
			assert.equal( 1, _browserApi.launchBrowser() );
			assert.equal( 1, _browserService.loadedBrowsers );
		});
	});

	describe('#closeBrowser()', function() {
		it('should forward message to browser service', function() {
			assert( _browserApi.closeBrowser(1) );
			assert.equal( 1, _browserService.closedBrowsers );
		});
	});

	describe('#showBrowser()', function() {
		it('should forward message to browser service', function() {
			assert.equal( 0, _browserService.visibilityModified );
			assert( _browserApi.showBrowser(1, true) );
			assert.equal( 1, _browserService.visibilityModified );
		});
	});

	describe('browserLoaded', function() {
		it('should receive browserLoaded events', function() {
			var count = 0;
			_browserApi.on('browserLoaded', function (id) {
				count++;
				assert.equal( count, id );
			});
			_browserService.emit('browserLoaded', 1);
			_browserService.emit('browserLoaded', 2);
			assert.equal( 2, count );
		});

		it('should receive browserLoaded event only once', function() {
			var count = 0;
			_browserApi.once('browserLoaded', function (id) {
				count++;
				assert.equal( 1, id );
			});
			_browserService.emit('browserLoaded', 1);
			_browserService.emit('browserLoaded', 2);
			assert.equal( 1, count );
		});

		it('should stop receiveing browserLoaded events after unregister', function() {
			var count = 0;
			function onBrowserLoaded(id) {
				count++;
				assert.equal( count, id );
			}
			_browserApi.on('browserLoaded', onBrowserLoaded);
			_browserService.emit('browserLoaded', 1);
			_browserService.emit('browserLoaded', 2);
			assert.equal( 2, count );
			_browserApi.removeListener('browserLoaded', onBrowserLoaded);
			_browserService.emit('browserLoaded', 3);
			assert.equal( 2, count );
		});
	});

	describe('browserClosed', function() {
		it('should receive browserClosed events', function() {
			var count = 0;
			_browserApi.on('browserClosed', function (id) {
				count++;
				assert.equal( count, id );
			});
			_browserService.emit('browserClosed', 1);
			_browserService.emit('browserClosed', 2);
			assert.equal( 2, count );
		});

		it('should receive browserClosed event only once', function() {
			var count = 0;
			_browserApi.once('browserClosed', function (id) {
				count++;
				assert.equal( 1, id );
			});
			_browserService.emit('browserClosed', 1);
			_browserService.emit('browserClosed', 2);
			assert.equal( 1, count );
		});
	});

	describe('#isReady()', function() {
		it('should emit ready events when htmlshellremote connects/disconnects', function(done) {
			assert( !_browserApi.isReady() );
			_browserApi.on('ready', function(isReady) {
				if (isReady) {
					assert( _browserApi.isReady() );
					_browserService.setReady(false);
				} else {
					assert( !_browserApi.isReady() );
					done();
				}
			});
			_browserService.setReady(true);
		});
	});

	describe('dump', function() {
		it('should dump browser service state', function() {
			var dump = _browserApi.dump();
			assert.equal( dump.browsers.length, 1 );
			assert.equal( dump.browsers[0].id, 1 );
			assert.equal( dump.browsers[0].status, 'loaded' );
			assert.equal( dump.browsers[0].url, 'http://google.com' );
			assert.equal( dump.browsers[0].apis.length, 0 );
			assert.equal( Object.keys(dump.crashes).length, 1 );
			assert.equal( dump.crashes['test.dmp'], 'ABCDE' );
		});
	});
});
