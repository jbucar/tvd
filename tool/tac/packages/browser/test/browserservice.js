'use strict';

var BrowserService = require('../src/impl/browserservice.js');
var bUtils = require('../src/util');
var BrowserMocks = require('./mocks');
var Mocks = require('mocks');
var assert = require('assert');
var fs = require('fs');
var path = require('path');

describe('BrowserService', function() {
	var _htmlshell = null;
	var _browserService = null;
	var _crashObserver = null;
	var _cfg = {
		htmlshell: {
			workPath: '/tmp/browser'
		}
	};

	beforeEach( function() {
		Mocks.init('verbose');
		fs.mkdirSync(_cfg.htmlshell.workPath);
		_htmlshell = new BrowserMocks.HtmlShellRemote();
		_browserService = new BrowserService(_htmlshell);
		_crashObserver = BrowserMocks.CrashObserver();
	});

	afterEach( function() {
		_htmlshell = null;
		_browserService = null;
		_crashObserver = null;
		bUtils.cleanDirectory(_cfg.htmlshell.workPath, true);
		Mocks.fin();
	});

	describe('#onStart()', function() {
		it('should fail if htmlshellremote fail to start', function() {
			_htmlshell.startOk = false;
			assert( !_browserService.start(_cfg, _crashObserver, {'VK_ENTER': 13}) );
		});

		it('should succeed if htmlshellremote start successfully', function() {
			assert( _browserService.start(_cfg, _crashObserver, {'VK_ENTER': 13}) );
		});

		it('should succeed and create htmlshell directory in work path', function() {
			var htmlshellDir = path.join(_cfg.htmlshell.workPath, 'htmlshell');
			assert( !fs.existsSync(htmlshellDir) );
			assert( _browserService.start(_cfg, _crashObserver, {'VK_ENTER': 13}) );
			assert( fs.existsSync(htmlshellDir) );
		});

		it('should succeed if htmlshell directory already exists in work path', function() {
			var htmlshellDir = path.join(_cfg.htmlshell.workPath, 'htmlshell');
			fs.mkdirSync(htmlshellDir);
			assert( fs.existsSync(htmlshellDir) );
			assert( _browserService.start(_cfg, _crashObserver, {'VK_ENTER': 13}) );
			assert( fs.existsSync(htmlshellDir) );
		});
	});

	describe('#onStop()', function() {
		beforeEach( function(done) {
			_browserService.once('ready', function (isReady) {
				assert( isReady );
				done();
			});
			assert( _browserService.start(_cfg, _crashObserver, {'VK_ENTER': 13}) );
		});

		it('should succeed if htmlshellremote stop successfully', function (done) {
			_browserService.stop(done);
		});

		it('should fail if htmlshellremote fail to stop', function (done) {
			_htmlshell.stopOk = false;
			_browserService.stop(function (err) {
				assert.equal( err.message, 'Fail to stop!!!' );
				done();
			});
		});

		it('should check for minidumps when browser service is stopped', function (done) {
			_browserService.stop(function() {
				assert.equal( 1, _crashObserver.reports );
				done();
			});
		});

		it('should succeed if stoped with open browsers', function (done) {
			var id = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( id, event.browserID );
						_browserService.stop(done);
						break;
				}
			});
		});
	});

	describe('#isReady()', function() {
		beforeEach( function() {
			assert( _browserService.start(_cfg, _crashObserver, {'VK_ENTER': 13}) );
		});

		it('should emit ready events when htmlshellremote connects/disconnects', function(done) {
			assert( !_browserService.isReady() );
			var notReadyCalled = false;
			_browserService.on('ready', function(isReady) {
				if (isReady) {
					assert( _browserService.isReady() );
					_browserService.stop(function() {
						assert( !_browserService.isReady() );
						assert( notReadyCalled );
						done();
					});
				} else {
					notReadyCalled = true;
				}
			});
		});
	});

	describe('Browser methods and events', function () {
		beforeEach( function (done) {
			_browserService.once('ready', function (isReady) {
				if (isReady) {
					done();
				}
			});
			_cfg.htmlshell.unsupportedCodecs = ['webm'];
			assert( _browserService.start(_cfg, _crashObserver, {'VK_ENTER': 13}) );
		});

		afterEach( function (done) {
			_browserService.stop(done);
		});

		it('should throw a recovery error when receives an error from htmlshellremote', function (done) {
			Mocks.CheckRecoveryError('[BrowserService] Received remote error: TEST_REMOTE_ERROR', done);
			process.nextTick(function () {
				_htmlshell.emit('error', new Error('TEST_REMOTE_ERROR'));
			});
		});

		it('should close browser when remote disconnects and fail to send more messages', function (done) {
			var id = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( id, event.browserID );
						_htmlshell.emit('disconnected');
						break;
					case 'onClose':
						assert.equal( id, event.browserID );
						assert.equal( _browserService.launchBrowser('http://google.com.ar', {}), -1 );
						assert( !_browserService.closeBrowser(0) );
						assert( !_browserService.showBrowser(0) );
						done();
						break;
				}
			});
			assert.equal( id, 0 );
		});

		it('should succed to launch a browser when remote re-connects affter disconnection', function (done) {
			_htmlshell.emit('disconnected');
			assert.equal( _browserService.launchBrowser('http://google.com.ar', {}), -1 );
			_htmlshell.emit('connected');
			var id = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( id, event.browserID );
						done();
						break;
				}
			});
			assert.equal( id, 0 );
		});

		it('should launch a browser successfully from http url', function (done) {
			var id = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( id, event.browserID );
						done();
						break;
				}
			});
			assert.equal( id, 0 );
		});

		it('should launch a browser successfully from https url', function (done) {
			var id = _browserService.launchBrowser('https://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( id, event.browserID );
						done();
						break;
				}
			});
			assert.equal( id, 0 );
		});

		it('should launch a browser successfully from file url', function( done ) {
			var id = _browserService.launchBrowser('file://' + __dirname + '/resources/bin/basic.html', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( id, event.browserID );
						done();
						break;
				}
			});
			assert.equal( id, 0 );
		});

		it('should launch a browser successfully from file url with params', function( done ) {
			var id = _browserService.launchBrowser('file://' + __dirname + '/resources/bin/basic.html?param=value', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( id, event.browserID );
						done();
						break;
				}
			});
			assert.equal( id, 0 );
		});

		it('should launch a browser successfully with default options', function( done ) {
			let expectedInitJs = 'TacOnExit=c=>window.onbeforeunload=e=>{c(TacExit);e.returnValue="HTMLSHELL_ASYNC_EXIT";};' +
				'TacExit=()=>{window.onbeforeunload=null;HtmlShellCall("htmlshell","async_exit_complete");};' +
				'window.tacKeyboardLayout={"VK_ENTER":13};';

			_htmlshell.sendMessage = function (msg) {
				assert( msg.launchBrowser );
				var launchMsg = msg.launchBrowser;
				assert.equal( 0, launchMsg.options.bounds.x );
				assert.equal( 0, launchMsg.options.bounds.y );
				assert.equal( 0, launchMsg.options.bounds.w );
				assert.equal( 0, launchMsg.options.bounds.h );
				assert.equal( false, launchMsg.options.bgTransparent );
				assert.equal( true, launchMsg.options.focus );
				assert.equal( expectedInitJs, launchMsg.options.js );
				assert.equal( 0, launchMsg.options.plugins.length );
			};
			var id = _browserService.launchBrowser('https://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( id, event.browserID );
						done();
						break;
				}
			});
			assert.equal( id, 0 );
		});

		it('should launch a browser successfully with valid options', function( done ) {
			var expectedInitJs = 'TacOnExit=c=>window.onbeforeunload=e=>{c(TacExit);e.returnValue="HTMLSHELL_ASYNC_EXIT";};' +
				'TacExit=()=>{window.onbeforeunload=null;HtmlShellCall("htmlshell","async_exit_complete");};' +
				'function MakeApi(n,t,o){' +
					'var e=window;' +
					't.split(".").forEach(n=>{' +
						'e[n]=e[n]||{},' +
						'e=e[n]' +
					'}),' +
					'o.forEach(u=>{' +
						'e[u]=(...e)=>{' +
							'var o,i=e.length;' +
							'return i>0&&"function"==typeof e[--i]&&(o=e[i],e=e.slice(0,i)),HtmlShellCall("RemoteExtension",{id:n,name:t,method:u,params:e},o)' +
						'}' +
					'})' +
				'}' +
				'MakeApi("service.id","TestService",["method1","method2"]);' +
				'window.tacKeyboardLayout={' +
					'"VK_ENTER":13' +
				'};';

			_htmlshell.sendMessage = function (msg) {
				assert( msg.launchBrowser );
				var launchMsg = msg.launchBrowser;
				assert.equal( expectedInitJs, launchMsg.options.js );
				assert.equal( 10, launchMsg.options.bounds.x );
				assert.equal( 20, launchMsg.options.bounds.y );
				assert.equal( 200, launchMsg.options.bounds.w );
				assert.equal( 100, launchMsg.options.bounds.h );
				assert.equal( true, launchMsg.options.bgTransparent );
				assert.equal( false, launchMsg.options.focus );
				assert.equal( 2, launchMsg.options.plugins.length );
				assert.equal( 'PlugIn 1', launchMsg.options.plugins[0] );
				assert.equal( 'PlugIn 2', launchMsg.options.plugins[1] );
			};

			var id = _browserService.launchBrowser('https://google.com.ar', {
					apis:[{id:'service.id', name: 'TestService', exports: ['method1', 'method2']}],
					bounds: { x:10, y:20, w:200, h:100 },
					focus: false,
					bgTransparent: true,
					plugins: ['PlugIn 1', 'PlugIn 2']
				},
				function (event) {
					switch (event.name) {
						case 'onLaunched':
							assert.equal( id, event.browserID );
							done();
							break;
					}
				});
			assert.equal( id, 0 );
		});

		it('should launch browser with keyboard layout allways', function( done ) {
			var expectedInitJs = 'TacOnExit=c=>window.onbeforeunload=e=>{c(TacExit);e.returnValue="HTMLSHELL_ASYNC_EXIT";};' +
				'TacExit=()=>{window.onbeforeunload=null;HtmlShellCall("htmlshell","async_exit_complete");};' +
				'window.tacKeyboardLayout={"VK_ENTER":13};';
			_htmlshell.sendMessage = function (msg) {
				assert( msg.launchBrowser );
				assert.equal( expectedInitJs, msg.launchBrowser.options.js );
			};

			var id = _browserService.launchBrowser('https://google.com.ar', {},
				function (event) {
					switch (event.name) {
						case 'onLaunched':
							assert.equal( id, event.browserID );
							done();
							break;
					}
				});
			assert.equal( id, 0 );
		});

		it('should launch browser with disabled codecs', function( done ) {
			_htmlshell.sendMessage = function (msg) {
				assert( msg.launchBrowser );
				assert.equal(
					msg.launchBrowser.options.js,
					'TacOnExit=c=>window.onbeforeunload=e=>{c(TacExit);e.returnValue="HTMLSHELL_ASYNC_EXIT";};' +
					'TacExit=()=>{window.onbeforeunload=null;HtmlShellCall("htmlshell","async_exit_complete");};' +
					'window.tacKeyboardLayout={"VK_ENTER":13};' +
					'function DisableCodecs(e){function i(i,n){return void 0===n||e.some(function(e){return-1!=n.indexOf(e)})?"":i(n)}var n=document.createElement("video");n.__proto__.canPlayType=i.bind(this,n.canPlayType.bind(n));var o=window.MediaSource;void 0!==o&&(o.isTypeSupported=i.bind(this,o.isTypeSupported.bind(o)))}' +
					'DisableCodecs(["webm"]);'
				);
			};

			var id = _browserService.launchBrowser('https://google.com.ar', {hwCodecsOnly: true},
				function (event) {
					switch (event.name) {
						case 'onLaunched':
							assert.equal( id, event.browserID );
							done();
							break;
					}
				});
			assert.equal( id, 0 );
		});

		it('should emit error if a browser is launched for an inexistent id', function (done) {
			_browserService.on('BrowserServiceError', function (err) {
				assert.equal( 'Browser launched for inexisten id=5', err.message );
				done();
			});
			_htmlshell.evtCallback({
				type: 'onLaunched',
				browserID: 5
			});
		});

		it('should emit error if a browser is done loading when after browser was closed', function (done) {
			_browserService.on('BrowserServiceError', function (err) {
				assert.equal( 'Received browser loaded event for inexisten id=5', err.message );
				done();
			});
			_htmlshell.evtCallback({
				type: 'onLoaded',
				browserID: 5
			});
		});

		it('should fail to launch a browser from invalid urls', function() {
			_browserService.on('browserLaunched', function (/*id*/) {
				assert( false && 'Browser should fail to launch' );
			});
			assert.equal( -1, _browserService.launchBrowser('ftp://google.com.ar') );
			assert.equal( -1, _browserService.launchBrowser('file://' + __dirname + '/resources/bin/unexistent.html') );
		});

		it('should fail to launch a browser with invalid bounds parameter', function() {
			_browserService.on('browserLaunched', function (/*id*/) {
				assert( false && 'Browser should fail to launch' );
			});
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', {bounds:'invalid param'}) );
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', {bounds:{}}) );
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', {bounds:{x:0,y:0,w:-1,h:0}}) );
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', {bounds:{x:0,y:0}}) );
		});

		it('should fail to launch a browser with invalid plugins parameter', function() {
			_browserService.on('browserLaunched', function (/*id*/) {
				assert( false && 'Browser should fail to launch' );
			});
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', {plugins:'invalid param'}) );
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', {plugins:{}}) );
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', {plugins:[1]}) );
		});

		it('should fail to launch a browser with invalid jsapis parameter', function() {
			_browserService.on('browserLaunched', function (/*id*/) {
				assert( false && 'Browser should fail to launch' );
			});

			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', null, 'invalid param') );
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', null, {}) );
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', null, ['']) );
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', null, [{id: '', name: '', exports: []}]) );
			assert.equal( -1, _browserService.launchBrowser('http://google.com.ar', null, [{id: 'id', name: 'name', exports: ''}]) );
		});

		it('should launch two browsers successfully', function( done ) {
			var launched = 0;
			_browserService.on('browserLaunched', function (id) {
				launched++;
				switch (launched) {
					case 1:
						assert.equal( id1, id );
						break;
					case 2:
						assert.equal( id2, id );
						done();
						break;
				}
			});

			var id1 = _browserService.launchBrowser('http://google.com.ar');
			var id2 = _browserService.launchBrowser('file://' + __dirname + '/resources/bin/basic.html');
			assert.equal( id1, 0 );
			assert.equal( id2, 1 );
		});

		it('should show/hide a browser successfully', function (done) {
			var id = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( id, event.browserID );
						assert( _browserService.showBrowser(id, true) );
						assert( _browserService.showBrowser(id, false) );
						done();
						break;
				}
			});
			assert.equal( id, 0 );
		});

		it('should close a browser successfully after it was successfully launched if id is valid', function (done) {
			var id = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( id, event.browserID );
						assert( _browserService.closeBrowser(id) );
						break;
					case 'onClose':
						assert.equal( id, event.browserID );
						done();
						break;
				}
			});
		});

		it('should close a browser successfully inmediatly after it was launched if id is valid', function (done) {
			var id = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						done(new Error('Shouldn\'t call launched callback when browser is closed'));
						break;
					case 'onClose':
						assert.equal( id, event.browserID );
						done();
						break;
				}
			});
			assert( _browserService.closeBrowser(id) );
		});

		it('should close multiples browsers successfully', function (done) {
			var b2Closed = false;
			_browserService.on('browserClosed', function (id) {
				if (id === id1) {
					assert( b2Closed );
					done();
				}
			});

			var id1 = _browserService.launchBrowser('http://google.com.ar');
			var id2 = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert( id1 != id2 );
						assert.equal( id2, event.browserID );
						assert( _browserService.closeBrowser(id2) );
						assert( _browserService.closeBrowser(id1) );
						break;
					case 'onClose':
						assert.equal( id2, event.browserID );
						b2Closed = true;
						break;
				}
			});
		});

		it('should fail to close a browser from an invalid id', function () {
			_browserService.on('browserClosed', function (/*id*/) {
				assert( false && 'Should not call onBrowserClosed if closeBrowser fail' );
			});
			assert( !_browserService.closeBrowser(2) );
		});

		it('should fail to close a browser twice', function () {
			var closed = 0;
			var id = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onClose':
						closed++;
						assert.equal( closed, 1 );
						assert.equal( id, event.browserID );
						break;
				}
			});
			assert( _browserService.closeBrowser(id) );
			assert( !_browserService.closeBrowser(id) );
		});

		it('should emit browserLaunched event when a browser is launched', function (done) {
			_browserService.once('browserLaunched', function (bId) {
				assert.equal( id, bId );
				assert( _browserService.closeBrowser(bId) );
				done();
			});
			var id = _browserService.launchBrowser('http://google.com.ar');
			assert.equal( id, 0 );
		});

		it('should emit browserLaunched event once for each browser launched', function (done) {
			var launched = 0;

			_browserService.on('browserLaunched', function (id) {
				launched++;
				if (id !== id1 && id !== id2) {
					done(new Error('browserLaunched emited with unexpected browser id'));
				}
				if (launched == 2) {
					assert( _browserService.closeBrowser(id1) );
					assert( _browserService.closeBrowser(id2) );
					done();
				}
			});

			var id1 = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched': assert.equal( id1, event.browserID ); break;
				}
			});
			var id2 = _browserService.launchBrowser('http://html5test.com', {}, function (event) {
				switch (event.name) {
					case 'onLaunched': assert.equal( id2, event.browserID ); break;
				}
			});
		});

		it('should emit browserClosed event when a browser is closed', function (done) {
			_browserService.once('browserClosed', function (id) {
				assert.equal( id, bId );
				done();
			});

			var bId = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( bId, event.browserID );
						assert( _browserService.closeBrowser(bId) );
						break;
				}
			});
		});

		it('should emit browserClosed event for each browser closed', function (done) {
			var closed = 0;

			_browserService.on('browserClosed', function (id) {
				closed++;
				if (id !== bId1 && id !== bId2) done(new Error('Got bad id'));
				if (closed == 2) done();
			});

			var bId1 = _browserService.launchBrowser('http://google.com.ar', {}, function (event) {
				switch (event.name) {
					case 'onLaunched': assert.equal( bId1, event.browserID ); break;
				}
			});

			var bId2 = _browserService.launchBrowser('http://html5test.com', {}, function (event) {
				switch (event.name) {
					case 'onLaunched':
						assert.equal( bId2, event.browserID );
						assert( _browserService.closeBrowser(bId1) );
						assert( _browserService.closeBrowser(bId2) );
						break;
				}
			});
		});

		it('should emit browser closed for loaded browsers when remote disconnects', function (done) {
			_browserService.on('browserLaunched', function (id) {
				assert.equal( id, originalId );
				_htmlshell.emit('disconnected');
			});
			_browserService.on('browserClosed', function (id, error) {
				assert.equal( 'Htmlshell disconnected', error.message );
				assert.equal( id, originalId );
				done();
			});
			_htmlshell.shouldSendResponse = true;
			var originalId = _browserService.launchBrowser('http://google.com');
			assert.equal( originalId, 0 );
		});

		it('should emit browser closed for loading browsers when remote disconnects', function (done) {
			_browserService.on('browserClosed', function (id, error) {
				assert.equal( 'Htmlshell disconnected', error.message );
				assert.equal( id, originalId );
				done();
			});
			_htmlshell.shouldSendResponse = false;
			var originalId = _browserService.launchBrowser('http://google.com');
			assert.equal( originalId, 0 );
				_htmlshell.emit('disconnected');
		});


		it('should emit browser closed for closing browsers when remote disconnects', function (done) {
			_browserService.on('browserLaunched', function (id) {
				assert.equal( id, originalId );
				_htmlshell.shouldSendResponse = false;
				_browserService.closeBrowser(originalId);
				_htmlshell.shouldSendResponse = true;
				_htmlshell.emit('disconnected');
			});
			_browserService.on('browserClosed', function (id, error) {
				assert.equal( 'Htmlshell disconnected', error.message );
				assert.equal( id, originalId );
				done();
			});
			_htmlshell.shouldSendResponse = true;
			var originalId = _browserService.launchBrowser('http://google.com');
			assert.equal( originalId, 0 );
		});

		it('should not emit browser closed for closed browsers when remote disconnects', function () {
			_browserService.on('browserLaunched', function (id) {
				assert.equal( id, originalId );
				_browserService.closeBrowser(originalId);
			});
			_browserService.on('browserClosed', function (id) {
				assert.equal( id, originalId );
				_htmlshell.emit('disconnected');
			});

			var originalId = _browserService.launchBrowser('http://google.com');
			assert.equal( originalId, 0 );
		});

		it('apicall should return error if browser do not exist', function (done) {
			_htmlshell.sendMessage = function( msg ) {
				if (msg.jsResponse) {
					var evt = msg.jsResponse;
					assert.equal( 1, evt.queryID );
					assert.equal( 2, evt.browserID );
					assert.equal( 'Api call arrived from unexistent browser with id=2', evt.error );
					assert( !evt.response );
					assert( !evt.isSignal );
					done();
				}
			};

			_htmlshell.evtCallback({
				type: 'onAPI',
				browserID: '2',
				queryID: '1',
				params: '{"id":"ar.edu.unlp.info.lifia.tvd.FakeService","method":"test","params":["test_params"]}',
			});
		});

		it('apicall should return error if browser do not handle api call', function (done) {
			_htmlshell.sendMessage = function( msg ) {
				if (msg.jsResponse) {
					var evt = msg.jsResponse;
					assert.equal( bId, evt.browserID );
					assert.equal( 2, evt.queryID );
					assert.equal( 'Api call not handled by browser with id=' + bId, evt.error );
					assert( !evt.response );
					assert( !evt.isSignal );
					_browserService.closeBrowser(bId);
				}
			};

			_browserService.on('browserClosed', function (id) {
				assert.equal( bId, id );
				done();
			});

			_browserService.on('browserLaunched', function (id) {
				assert.equal( bId, id );
				_htmlshell.evtCallback({
					type: 'onAPI',
					browserID: bId,
					queryID: '2',
					params: '{"id":"ar.edu.unlp.info.lifia.tvd.FakeService","method":"test","params":["test_params"]}'
				});
			});
			var bId = _browserService.launchBrowser('http://google.com');
		});

		it('apicall should not remove callback if apicall is a signal', function (done) {
			var resultCB = null;
			var called = 0;

			_htmlshell.sendMessage = function( msg ) {
				if (msg.jsResponse) {
					var evt = msg.jsResponse;
					assert.equal( 2, evt.queryID );
					assert.equal( 0, evt.browserID );
					assert( !evt.error );
					assert.equal( 'Test signal', JSON.parse(evt.params) );
					assert( evt.isSignal );
					called++;
				}
			};

			var bId = _browserService.launchBrowser('http://google.com', {}, function (evt) {
				switch (evt.name) {
					case 'onLaunched':
						assert.equal( bId, evt.browserID );
						_htmlshell.evtCallback({
							type: 'onAPI',
							browserID: bId,
							queryID: 2,
							params: '{"id":"ar.edu.unlp.info.lifia.tvd.FakeService","method":"on","params":[]}'
						});
						break;
					case 'onClose':
						assert.equal( bId, evt.browserID );
						assert( !evt.error );
						done();
						break;
					case 'onAPI':
						assert.equal( bId, evt.browserID );
						assert.equal( 'ar.edu.unlp.info.lifia.tvd.FakeService', evt.serviceID );
						assert.equal( 'on', evt.method );
						assert.equal( evt.params, undefined );
						resultCB = evt.resultCB;
				}
			});
			process.nextTick(function() {
				assert( resultCB );
				resultCB({data:['Test signal'],isSignal:true});
				resultCB({data:['Test signal'],isSignal:true});
				assert.equal( 2, called );
				_browserService.closeBrowser(bId);
			});
		});

		it('apicall should call api handler correctly', function (done) {
			_htmlshell.sendMessage = function( msg ) {
				if (msg.jsResponse) {
					var evt = msg.jsResponse;
					assert.equal( 2, evt.queryID );
					assert.equal( bId, evt.browserID );
					assert.equal( 'test_response', JSON.parse(evt.params) );
					assert( !evt.error );
					assert( !evt.isSignal );
					_browserService.closeBrowser(bId);
				}
			};

			var bId = _browserService.launchBrowser('http://google.com', {}, function (evt) {
				switch (evt.name) {
					case 'onLaunched':
						assert.equal( bId, evt.browserID );
						_htmlshell.evtCallback({
							type: 'onAPI',
							browserID: bId,
							queryID: 2,
							params: '{"id":"ar.edu.unlp.info.lifia.tvd.FakeService","method":"test","params":["test_params"]}',
						});
						break;
					case 'onClose':
						assert.equal( bId, evt.browserID );
						assert( !evt.error );
						done();
						break;
					case 'onAPI':
						assert.equal( bId, evt.browserID );
						assert.equal( 'ar.edu.unlp.info.lifia.tvd.FakeService', evt.serviceID );
						assert.equal( 'test', evt.method );
						assert.equal( 'test_params', evt.params );
						evt.resultCB({data:['test_response'],isSignal:false});
				}
			});
		});

		it('key event should call onKey handler correctly', function (done) {
			var bId = _browserService.launchBrowser('http://google.com', {}, function (evt) {
				switch (evt.name) {
					case 'onLaunched':
						assert.equal( bId, evt.browserID );
						_htmlshell.evtCallback({
							type: 'onKey',
							browserID: bId,
							keyCode: 13,
							isUp: true
						});
						break;
					case 'onClose':
						assert.equal( bId, evt.browserID );
						assert( !evt.error );
						done();
						break;
					case 'onKey':
						assert.equal( bId, evt.browserID );
						assert.equal( "VK_ENTER", evt.code );
						assert.equal( true, evt.isUp );
						_browserService.closeBrowser(bId);
						break;
				}
			});
		});

		it('should emit error if a key event arrive for an inexsitent browser id', function (done) {
			_browserService.on('BrowserServiceError', function (err) {
				assert.equal( 'Received key event from invalid browser with id=5', err.message );
				done();
			});
			_htmlshell.evtCallback({
				type: 'onKey',
				browserID: 5,
				keyCode: 10,
				isUp: true
			});
		});

		it('error event should call onWebLog handler correctly', function (done) {
			var bId = _browserService.launchBrowser('http://google.com', {}, function (evt) {
				switch (evt.name) {
					case 'onLaunched':
						assert.equal( bId, evt.browserID );
						_htmlshell.evtCallback({
							type: 'onWebLog',
							browserID: bId,
							logData: {
								source: 'www.google.com.ar',
								line: 43,
								level: 2,
								message: 'Error description',
							}
						});
						break;
					case 'onClose':
						assert.equal( bId, evt.browserID );
						assert( !evt.error );
						done();
						break;
					case 'onWebLog':
						assert.equal( bId, evt.browserID );
						assert.equal( 'Error description', evt.logData.message );
						assert.equal( 'www.google.com.ar', evt.logData.source );
						assert.equal( 43, evt.logData.line );
						assert.equal( 2, evt.logData.level );
						_browserService.closeBrowser(bId);
						break;
				}
			});
		});

		it('should emit error if a webLog event arrive for an inexsitent browser id', function (done) {
			_browserService.on('BrowserServiceError', function (err) {
				assert.equal( 'Received weblog event from invalid browser with id=5', err.message );
				done();
			});
			_htmlshell.evtCallback({
				type: 'onWebLog',
				browserID: 5,
				logData: {
					message: 'Error description',
					source: 'www.google.com.ar',
					line: 43,
					level: 2
				}
			});
		});

		it('not handled message should emit error event', function (done) {
			_browserService.once('BrowserServiceError', function (err) {
				assert.equal( 'Received invalid message from htmlshell', err.message );
				done();
			});
			_htmlshell.evtCallback({
				type: 'badMsg'
			});
		});

		it('should check for minidumps when a browser is closed with error', function (done) {
			var bId = _browserService.launchBrowser('http://google.com', {}, function (evt) {
				switch (evt.name) {
					case 'onLaunched':
						assert.equal( bId, evt.browserID );
						_htmlshell.evtCallback({
							type: 'onLoaded',
							browserID: bId,
						});
						break;
					case 'onLoaded':
						_htmlshell.evtCallback({
							type: 'onClosed',
							browserID: bId,
							error: 'Renderer process crashed'
						});
						break;
					case 'onClose':
						assert.equal( bId, evt.browserID );
						assert.equal( evt.error.message, 'Renderer process crashed' );
						assert.equal( 1, _crashObserver.reports );
						done();
						break;
				}
			});
		});

		describe('dump', function() {
			it('should dump browser service state', function() {
				assert.equal( _browserService.launchBrowser('http://google.com'), 0 );
				var dump = _browserService.dumpState();
				assert.equal( dump.browsers.length, 1 );
				assert.equal( dump.browsers[0].id, 0 );
				assert.equal( dump.browsers[0].status, 'loading' );
				assert.equal( dump.browsers[0].url, 'http://google.com' );
				assert.equal( dump.browsers[0].apis.length, 0 );
				assert.equal( Object.keys(dump.crashes).length, 1 );
				assert.equal( dump.crashes['test.dmp'], 'ABCDE' );
			});
		});
	});
});
