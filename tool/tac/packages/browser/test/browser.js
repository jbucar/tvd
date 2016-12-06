'use strict';

var mocks = require('./mocks');
var Browser = require('../src/impl/browser');
var Mocks = require('mocks');
var assert = require('assert');

describe('BrowserConstructor', function() {
	it('should construct browser using new successfully', function() {
		assert( new Browser(new mocks.BrowserService()) );
	});

	it('should construct browser not using new successfully', function() {
		assert( Browser(new mocks.BrowserService()) );
	});
});

describe('Browser', function() {
	var _browserServiceMock;
	var _browser;

	beforeEach( function() {
		Mocks.init('silly');
		_browserServiceMock = new mocks.BrowserService();
		_browser = new Browser(_browserServiceMock);
		assert.equal( 'closed', _browser.state.name );
	});

	afterEach( function() {
		_browser = undefined;
		_browserServiceMock = undefined;
		Mocks.fin();
	});

	describe('#load()', function() {
		it('should fail if no url provided', function() {
			assert( !_browser.load() );
			assert.equal( 0, _browserServiceMock.loadedBrowsers );
		});

		it('should fail if options params is not an object', function() {
			assert( !_browser.load('http://google.com.ar', []) );
			assert.equal( 0, _browserServiceMock.loadedBrowsers );

			assert( !_browser.load('http://google.com.ar', 'bad') );
			assert.equal( 0, _browserServiceMock.loadedBrowsers );
		});

		it('should fail if bounds param is not an object with valid x,y,w,h members', function() {
			assert( !_browser.load('http://google.com.ar', {bounds:{x:0,y:0,w:100,h:'bad'}}) );
			assert.equal( 0, _browserServiceMock.loadedBrowsers );

			assert( !_browser.load('http://google.com.ar', {bounds:{x:0,y:0,w:-1,h:100}}) );
			assert.equal( 0, _browserServiceMock.loadedBrowsers );

			assert( !_browser.load('http://google.com.ar', {bounds:'{x:0,y:0,w:100,h:100}'}) );
			assert.equal( 0, _browserServiceMock.loadedBrowsers );
		});

		it('should fail if jsapis param is not a function', function() {
			assert( !_browser.load('http://google.com.ar', {apis:'bad jsapis param'}) );
			assert.equal( 0, _browserServiceMock.loadedBrowsers );
		});

		it('should fail if onevent param is not a function', function() {
			assert( !_browser.load('http://google.com.ar', {}, 'bad onevent param') );
			assert.equal( 0, _browserServiceMock.loadedBrowsers );
		});

		it('should fail if url is not valid', function() {
			assert( !_browser.load('ftp://bad/url') );
			assert.equal( 0, _browserServiceMock.loadedBrowsers );
		});

		it('should load successfully a http url', function() {
			assert( _browser.load('http://google.com.ar') );
			assert.equal( 0, _browser.id );
			assert.equal( 'http://google.com.ar', _browser.url );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
		});

		it('should load successfully a https url', function() {
			assert.equal( -1, _browser.id );
			assert( _browser.load('https://google.com.ar') );
			assert.equal( 0, _browser.id );
			assert.equal( 'https://google.com.ar', _browser.url );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
		});

		it('should load successfully a file url', function() {
			assert.equal( -1, _browser.id );
			var url = 'file://' + __dirname + '/resources/bin/basic.html';
			assert( _browser.load(url) );
			assert.equal( 0, _browser.id );
			assert.equal( url, _browser.url );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
		});

		it('should create new browser with different id', function() {
			assert.equal( -1, _browser.id );
			assert( _browser.load('http://google.com.ar') );
			var newBrowser = new Browser(_browserServiceMock);
			assert( newBrowser.load('http://html5test.com') );
			assert.equal( 1, newBrowser.id );
			assert.equal( 2, _browserServiceMock.loadedBrowsers );
		});

		it('should load successfully with valid params', function() {
			let expectedInitJs = 'TacOnExit=c=>window.onbeforeunload=e=>{c(TacExit);e.returnValue="HTMLSHELL_ASYNC_EXIT";};' +
				'TacExit=()=>{window.onbeforeunload=null;HtmlShellCall("htmlshell","async_exit_complete");};' +
				'window.tacKeyboardLayout={"VK_ENTER":13};';

			assert( _browser.load('http://google.com.ar', {
					bounds:{x:20,y:30,w:200,h:400},
					apis:[],
					bgTransparent: true,
					visible: true,
					zIndex: 2,
					enableMouse: true,
					enableGeolocation: true,
					extraUA: 'Test (1.1)'
				}, function(){}) );
			assert.equal( 0, _browser.id );
			assert.equal( 'http://google.com.ar', _browser.url );
			assert.equal( 20, _browser.options.bounds.x );
			assert.equal( 30, _browser.options.bounds.y );
			assert.equal( 200, _browser.options.bounds.w );
			assert.equal( 400, _browser.options.bounds.h );
			assert.equal( expectedInitJs, _browser.options.js );
			assert.equal( true, _browser.options.bgTransparent );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
			assert( _browser.options.enableMouse );
			assert( _browser.options.enableGeolocation );
		});

		it('should fail if it is already in loading state', function() {
			assert( _browser.load('http://google.com.ar') );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 'http://google.com.ar', _browser.url );
			assert( !_browser.load('http://html5test.com') );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 'http://google.com.ar', _browser.url );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
		});

		it('should fail if it is in loaded state', function() {
			assert( _browser.load('http://google.com.ar') );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 'http://google.com.ar', _browser.url );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserLoaded );
			assert( !_browser.load('http://html5test.com') );
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 'http://google.com.ar', _browser.url );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
		});

		it('should fail if it is in closing state', function() {
			assert( _browser.load('http://google.com.ar') );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 'http://google.com.ar', _browser.url );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );
			assert( !_browser.load('http://html5test.com') );
			assert.equal( 'closing', _browser.state.name );
			assert.equal( 'http://google.com.ar', _browser.url );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
			assert.equal( 1, _browserServiceMock.closedBrowsers );
		});
	});

	describe('#onLaunched()', function() {
		it('should change from loading to loaded state', function() {
			var count = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( _browser.id, event.browserID );
						count++;
						break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserLoaded );
			assert.equal( 1, count );
		});

		it('shouldnt do anything if it is already in loaded state', function() {
			var count = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( _browser.id, event.browserID );
						count++;
						break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserLoaded );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserLoaded );
			assert.equal( 1, count );
			assert.equal( 0, _browserServiceMock.closedBrowsers );
		});

		it('should stay in closing state until onClose arrive', function() {
			var count = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( _browser.id, event.browserID );
						count++;
						break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'closing', _browser.state.name );
			_browser.onClose();
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
			assert.equal( 0, count );
		});

		it('shouldnt do anything if it is in closed state', function() {
			var count = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( _browser.id, event.browserID );
						count++;
						break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onClose();
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
			_browser.onLaunched();
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 0, _browserServiceMock.onBrowserLoaded );
			assert.equal( 0, count );
		});

		it('shouldnt emit onLoad event if it recive onLoad after being closed', function(done) {
			var fail = false;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( _browser.id, event.browserID );
						assert.equal( 'loaded', _browser.state.name );
						_browser.close();
						_browser.onLoad();
						break;
					case 'onLoaded':
						done(new Error('Shouldn\'t emit onLoaded event'));
						fail = true;
						break;
				}
			});
			_browser.onLaunched();
			if (!fail) {
				done();
			}
		});
	});

	describe('#onClose()', function() {
		it('should change from loading to closed state', function() {
			var closeCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onClose': closeCount++; break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onClose();
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
			assert.equal( 1, closeCount );
		});

		it('should change from loading to closed state with error', function() {
			var closeError = null;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onClose': closeError = event.error; break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
			_browser.onClose('Error description');
			assert.equal( 'closed', _browser.state.name );
			assert( closeError );
			assert.equal( 'Error description', closeError.message );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
		});

		it('should change from loaded to closed state', function() {
			var closeCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onClose': closeCount++; break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			_browser.onClose();
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
			assert.equal( 1, closeCount );
		});

		it('should change from loaded to closed state with error', function() {
			var closeError = null;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onClose': closeError = event.error; break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserLoaded );
			_browser.onClose('Error description');
			assert.equal( 'closed', _browser.state.name );
			assert( closeError );
			assert.equal( 'Error description', closeError.message );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
		});

		it('should change from closing to closed state', function() {
			var closeCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onClose': closeCount++; break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );
			_browser.onClose();
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
			assert.equal( 1, closeCount );
		});

		it('should change from closing to closed state with error', function() {
			var closeError = null;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onClose': closeError = event.error; break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserLoaded );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );
			_browser.onClose('Error description');
			assert.equal( 'closed', _browser.state.name );
			assert( closeError );
			assert.equal( 'Error description', closeError.message );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
		});

		it('shouldnt do anything if it is already in closed state', function() {
			var closeCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onClose': closeCount++; break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onClose();
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
			assert.equal( 1, closeCount );
			_browser.onClose();
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
			assert.equal( 1, closeCount );
		});
	});

	describe('#close()', function() {
		it('should change from loading to closing state', function() {
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );
			assert.equal( 1, _browserServiceMock.closedBrowsers );
		});

		it('should change from loaded to closing state', function() {
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserLoaded );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );
			assert.equal( 1, _browserServiceMock.closedBrowsers );
		});

		it('shouldnt do anything if it is already in closing state', function() {
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );
			assert.equal( 1, _browserServiceMock.closedBrowsers );
			assert( !_browser.close() );
			assert.equal( 'closing', _browser.state.name );
			assert.equal( 1, _browserServiceMock.closedBrowsers );
		});

		it('shouldnt do anything if it is in closed state', function() {
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.loadedBrowsers );
			_browser.onClose();
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 1, _browserServiceMock.onBrowserClosed );
			assert( !_browser.close() );
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 0, _browserServiceMock.closedBrowsers );
		});
	});

	describe('#onApiCall()', function() {
		it('shouldnt do anything if it is in loading state', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onAPI':
						assert.equal( _browser.id, event.browserID );
						apiCallCount++;
						break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			assert( !_browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":["Hello"]}'}) );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 0, apiCallCount );
			assert.equal( 0, _browserServiceMock.apiCallsHandled );
			assert.equal( 1, _browserServiceMock.apiCallErrors );
			assert.equal( 0, _browserServiceMock.signalsEmited );
		});

		it('should not remove callback if apicall is a signal', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onAPI':
						assert.equal( _browser.id, event.browserID );
						assert.equal( 'FakeService', event.serviceID );
						assert.equal( 'on', event.method );
						assert.equal( event.params, undefined );
						apiCallCount++;
						event.resultCB({data:[{name:'Test event'}], isSignal: true});
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( _browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"on","params":[]}'}) );
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, apiCallCount );
			assert.equal( 1, _browserServiceMock.apiCallsHandled );
			assert.equal( 0, _browserServiceMock.apiCallErrors );
			assert.equal( 1, _browserServiceMock.signalsEmited );
		});

		it('should call onApiCall callback if it is in loaded state', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onAPI':
						assert.equal( _browser.id, event.browserID );
						assert.equal( 'FakeService', event.serviceID );
						assert.equal( 'test', event.method );
						assert.equal( 1, event.params.length );
						assert.equal( 'Hello', event.params[0] );
						apiCallCount++;
						event.resultCB({data:['result'], isSignal: false});
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( _browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":["Hello"]}'}) );
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, apiCallCount );
			assert.equal( 1, _browserServiceMock.apiCallsHandled );
			assert.equal( 0, _browserServiceMock.apiCallErrors );
			assert.equal( 0, _browserServiceMock.signalsEmited );
		});

		it('should call onApiCall callback with empty result if it is in loaded state', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onAPI':
						assert.equal( _browser.id, event.browserID );
						assert.equal( 'FakeService', event.serviceID );
						assert.equal( 'test', event.method );
						assert.equal( undefined, event.params );
						apiCallCount++;
						event.resultCB({data:['result'], isSignal: false});
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( _browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":[]}'}) );
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, apiCallCount );
			assert.equal( 1, _browserServiceMock.apiCallsHandled );
			assert.equal( 0, _browserServiceMock.apiCallErrors );
			assert.equal( 0, _browserServiceMock.signalsEmited );
		});

		it('should not call onApiCall callback if response isn\'t an array', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onAPI':
						assert.equal( _browser.id, event.browserID );
						assert.equal( 'FakeService', event.serviceID );
						assert.equal( 'test', event.method );
						assert.equal( 1, event.params.length );
						assert.equal( 'Hello', event.params[0] );
						apiCallCount++;
						event.resultCB({data:'result', isSignal: false});
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( _browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":["Hello"]}'}) );
			assert.equal( 1, apiCallCount );
			assert.equal( 0, _browserServiceMock.apiCallsHandled );
			assert.equal( 1, _browserServiceMock.apiCallErrors );
			assert.equal( 0, _browserServiceMock.signalsEmited );
		});

		it('shouldnt send apicall_response if response arrive in other than loaded or closing state', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onAPI':
						if (apiCallCount === 0) {
							_browser.close();
						} else {
							_browser.onClose();
						}
						assert.equal( _browser.id, event.browserID );
						assert.equal( 'FakeService', event.serviceID );
						assert.equal( 'test', event.method );
						assert.equal( 1, event.params.length );
						assert.equal( 'Hello', event.params[0] );
						apiCallCount++;
						event.resultCB({data:['result'], isSignal: false});
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( _browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":["Hello"]}'}) );
			assert.equal( 'closing', _browser.state.name );
			assert( _browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":["Hello"]}'}) );
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 2, apiCallCount );
			assert.equal( 1, _browserServiceMock.apiCallsHandled );
			assert.equal( 0, _browserServiceMock.apiCallErrors );
			assert.equal( 0, _browserServiceMock.signalsEmited );
		});

		it('should send apicall_response with error if api call handler return error', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onAPI':
						assert.equal( _browser.id, event.browserID );
						assert.equal( 'FakeService', event.serviceID );
						assert.equal( 'test', event.method );
						assert.equal( 1, event.params.length );
						assert.equal( 'Hello', event.params[0] );
						apiCallCount++;
						event.resultCB({error: new Error('An apicall error'), isSignal: false});
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( _browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":["Hello"]}'}) );
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, apiCallCount );
			assert.equal( 0, _browserServiceMock.apiCallsHandled );
			assert.equal( 1, _browserServiceMock.apiCallErrors );
			assert.equal( 0, _browserServiceMock.signalsEmited );
		});

		it('should return false if api call is not handled', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( !_browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":["Hello"]}'}) );
			assert.equal( 0, apiCallCount );
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 0, _browserServiceMock.apiCallsHandled );
			assert.equal( 1, _browserServiceMock.apiCallErrors );
			assert.equal( 0, _browserServiceMock.signalsEmited );
		});

		it('should return false if api call params aren\'t an array', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( !_browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":"Hello"}'}) );
			assert.equal( 0, apiCallCount );
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 0, _browserServiceMock.apiCallsHandled );
			assert.equal( 1, _browserServiceMock.apiCallErrors );
			assert.equal( 0, _browserServiceMock.signalsEmited );
		});

		it('should return false if api call params are malformed json', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( !_browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":[{"Hello"]}'}) );
			assert.equal( 0, apiCallCount );
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 0, _browserServiceMock.apiCallsHandled );
			assert.equal( 1, _browserServiceMock.apiCallErrors );
			assert.equal( 0, _browserServiceMock.signalsEmited );
		});

		it('should call onApiCall callback if it is in closing state', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onAPI':
						assert.equal( _browser.id, event.browserID );
						assert.equal( 'FakeService', event.serviceID );
						assert.equal( 'test', event.method );
						assert.equal( 1, event.params.length );
						assert.equal( 'Hello', event.params[0] );
						apiCallCount++;
						event.resultCB({data:['result'], isSignal: false});
				}
			});
			assert.equal( 'loading', _browser.state.name );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );
			assert( _browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":["Hello"]}'}) );
			assert.equal( 'closing', _browser.state.name );
			assert.equal( 1, apiCallCount );
			assert.equal( 1, _browserServiceMock.apiCallsHandled );
			assert.equal( 0, _browserServiceMock.apiCallErrors );
			assert.equal( 0, _browserServiceMock.signalsEmited );
		});

		it('shouldnt do anything if it is in closed state', function() {
			var apiCallCount = 0;
			_browser.load('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onAPI': apiCallCount++; break;
				}
			});
			assert.equal( 'loading', _browser.state.name );
			_browser.onClose();
			assert.equal( 'closed', _browser.state.name );
			assert( !_browser.onApiCall({browserID:1,queryID:1,params:'{"id":"FakeService","method":"test","params":["Hello"]}'}) );
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 0, apiCallCount );
		});
	});

	describe('#onKey()', function() {
		it('shouldnt do anything if it is in closed state', function() {
			assert.equal( 'closed', _browser.state.name );
			_browser.onEvent = function( evt ) {
				switch (evt.name) {
					case 'onKey':
						assert( false && 'shouldnt call onKey event');
						break;
				}

			};
			_browser.onKey(10, true);
		});

		it('shouldnt do anything if it is in loading state', function() {
			assert.equal( 'closed', _browser.state.name );
			assert( _browser.load('http://google.com.ar', {}, function( evt ) {
				switch (evt.name) {
					case 'onKey':
						assert( false && 'shouldnt call onKey event');
						break;
				}
			}) );
			assert.equal( 'loading', _browser.state.name );
			_browser.onKey(10, true);
		});

		it('shouldnt do anything if it is in closing state', function() {
			assert.equal( 'closed', _browser.state.name );
			assert( _browser.load('http://google.com.ar', {}, function( evt ) {
				switch (evt.name) {
					case 'onKey':
						assert( false && 'shouldnt call onKey event');
						break;
				}

			}) );
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );
			_browser.onKey(10, true);
		});

		it('should emit onKey event if it is in loaded state', function (done) {
			assert.equal( 'closed', _browser.state.name );
			assert( _browser.load('http://google.com.ar', {}, function( evt ) {
				switch (evt.name) {
					case 'onKey':
						done();
				}
			}) );
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			_browser.onKey(10, true);
		});
	});

	describe('#onWebLog()', function() {
		it('shouldnt do anything if it is in closed state', function() {
			_browser.onEvent = function( /*evt*/ ) {
				assert( false && 'shouldnt emit any event');
			};
			assert.equal( 'closed', _browser.state.name );
			_browser.onWebLog({level: 2, line: 1, message: 'Error description', source: 'www.google.com.ar'});
		});

		it('should emit onError if it is in closing state', function(done) {
			assert.equal( 'closed', _browser.state.name );
			assert( _browser.load('http://google.com.ar', {}, function (evt) {
				if (evt.name === 'onWebLog') {
					assert.equal( evt.logData.message, 'Error description' );
					assert.equal( evt.logData.source, 'www.google.com.ar' );
					assert.equal( evt.logData.line, 1 );
					assert.equal( evt.logData.level, 2 );
					done();
				}
			}) );
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );
			_browser.onWebLog({level: 2, line: 1, message: 'Error description', source: 'www.google.com.ar'});
		});

		it('shouldnt do anything if it is in loading state', function() {
			assert.equal( 'closed', _browser.state.name );
			assert( _browser.load('http://google.com.ar', {}, function (evt) {
				if (evt.name === 'onWebLog') {
					assert( false && 'shouldnt emit onError event');
				}
			}) );
			assert.equal( 'loading', _browser.state.name );
			_browser.onWebLog({level: 2, line: 1, message: 'Error description', source: 'www.google.com.ar'});
		});

		it('should emit onError event if it is in loaded state', function(done) {
			assert.equal( 'closed', _browser.state.name );
			assert( _browser.load('http://google.com.ar', {}, function (evt) {
				if (evt.name === 'onWebLog') {
					assert.equal( evt.logData.message, 'Error description' );
					assert.equal( evt.logData.source, 'www.google.com.ar' );
					assert.equal( evt.logData.line, 1 );
					assert.equal( evt.logData.level, 2 );
					done();
				}
			}) );
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );
			_browser.onWebLog({level: 2, line: 1, message: 'Error description', source: 'www.google.com.ar'});
		});
	});

	describe('#show()', function() {
		it('should hide/show browser if it is in loading state', function() {
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			assert( _browser.show(1, false) );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 1, _browserServiceMock.visibilityModified );
			assert( _browser.show(1, true) );
			assert.equal( 'loading', _browser.state.name );
			assert.equal( 2, _browserServiceMock.visibilityModified );
		});

		it('should hide/show browser if it is in loading state', function() {
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			_browser.onLaunched();
			assert.equal( 'loaded', _browser.state.name );

			assert( _browser.show(1, false) );
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 1, _browserServiceMock.visibilityModified );
			assert( _browser.show(1, true) );
			assert.equal( 'loaded', _browser.state.name );
			assert.equal( 2, _browserServiceMock.visibilityModified );
		});

		it('shouldnt do anything if it is in closing state', function() {
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			assert( _browser.close() );
			assert.equal( 'closing', _browser.state.name );

			assert( !_browser.show(1, false) );
			assert.equal( 'closing', _browser.state.name );
			assert.equal( 0, _browserServiceMock.visibilityModified );
			assert( !_browser.show(1, true) );
			assert.equal( 'closing', _browser.state.name );
			assert.equal( 0, _browserServiceMock.visibilityModified );
		});

		it('shouldnt do anything if it is in closing state', function() {
			_browser.load('http://google.com.ar');
			assert.equal( 'loading', _browser.state.name );
			_browser.onClose();
			assert.equal( 'closed', _browser.state.name );

			assert( !_browser.show(1, false) );
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 0, _browserServiceMock.visibilityModified );
			assert( !_browser.show(1, true) );
			assert.equal( 'closed', _browser.state.name );
			assert.equal( 0, _browserServiceMock.visibilityModified );
		});
	});

	describe('#dumpState', function() {
		it('should dump state successfully in initial state', function() {
			var dump = _browser.dumpState();
			assert.equal(-1, dump.id);
			assert.equal('closed', dump.status);
			assert.equal('', dump.url);
			assert.equal(0, Object.keys(dump.apis).length);
		});

		it('should dump state successfully in loading state', function() {
			assert( _browser.load('http://google.com.ar', {bounds:{x:20,y:30,w:200,h:400}, apis:[], bgTransparent: true}, function(){}) );
			var dump = _browser.dumpState();
			assert.equal(0, dump.id);
			assert.equal('loading', dump.status);
			assert.equal('http://google.com.ar', dump.url);
			assert.equal( 0, dump.apis.length );
		});

		it('should dump state successfully in loaded state', function() {
			assert( _browser.load('http://google.com.ar', {bounds:{x:20,y:30,w:200,h:400}, apis:[], bgTransparent: true}, function(){}) );
			_browser.onLaunched();
			var dump = _browser.dumpState();
			assert.equal(0, dump.id);
			assert.equal('loaded', dump.status);
			assert.equal('http://google.com.ar', dump.url);
			assert.equal( 0, dump.apis.length );
		});

		it('should dump state successfully in closing state', function() {
			assert( _browser.load('http://google.com.ar', {bounds:{x:20,y:30,w:200,h:400}, apis:[{name:'test', id:'ar.edu.unlp.info.lifia.test', exports:['hi']}], bgTransparent: true}, function(){}) );
			_browser.onLaunched();
			assert( _browser.close() );
			var dump = _browser.dumpState();
			assert.equal(0, dump.id);
			assert.equal('closing', dump.status);
			assert.equal('http://google.com.ar', dump.url);
			assert.equal( 1, dump.apis.length );
		});

		it('should dump state successfully in closed state', function() {
			assert( _browser.load('http://google.com.ar', {bounds:{x:20,y:30,w:200,h:400}, apis:[], bgTransparent: true}, function(){}) );
			_browser.onLaunched();
			assert( _browser.close() );
			_browser.onClose();
			var dump = _browser.dumpState();
			assert.equal(0, dump.id);
			assert.equal('closed', dump.status);
			assert.equal('http://google.com.ar', dump.url);
			assert.equal( 0, dump.apis.length );
		});
	});
});
