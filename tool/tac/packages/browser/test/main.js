'use strict';

var BrowserModule = require('../src/main');
var bUtils = require('../src/util');
var BrowserMocks = require('./mocks');
var assert = require('chai').assert;
var fs = require('fs');
var path = require('path');
var _ = require('lodash');
var Mocks = require('mocks');

var testPackage = {
	'id': 'ar.edu.unlp.info.lifia.test',
	'version': '1.0.0',
	'vendor': 'LIFIA',
	'platform': 'all',
	'system': true,
	'components': [{
		'id': 'ar.edu.unlp.info.lifia.test_plugin',
		'name': 'Test PlugIn',
		'type': 'browser_plugin',
		'out_of_process': true,
		'sandboxed': false,
		'main': 'plugins/libtest.so',
		'version': '2.2',
		'mime_types': [{
			'type': 'application/test',
			'extensions': 'xyz',
			'description': 'Test application (XYZ)'
		}]
	}]
};

describe('BrowserModule', function() {
	var _module = null;
	var _reg = null;
	var _adapter = null;
	var _cfg = null;
	var _workDir = '';

	beforeEach( function() {
		_reg = Mocks.init('verbose');
		_reg.put( 'platform', new Mocks.Factory() );
		_reg.put( 'system', new Mocks.System() );
		_reg.put( 'pkgmgr', new Mocks.PackageManager([testPackage]) );

		_workDir = path.join('/tmp', 'browser');
		fs.mkdirSync(_workDir);
		_cfg = {
			tmpDirectory: '/tmp',
			htmlshell: {
				workPath: _workDir,
				installPath: path.resolve(__dirname, 'resources'),
				cmd: 'dummy.js',
				args: [],
			}
		};
		_adapter = new BrowserMocks.ServiceAdapter(_cfg, _reg);
		_module = BrowserModule(_adapter);
		assert( _module );
	});

	afterEach( function (done) {
		_module.onStop(done);
		_module = null;
		_adapter = null;
		_cfg = null;
		_reg = null;
		bUtils.cleanDirectory(_workDir, true);
	});

	it('should return browser api on start', function(done) {
		_module.onStart(function(err,api) {
			assert.isUndefined( err );
			assert.isObject( api );
			assert( _.isFunction(api.on) );
			assert( _.isFunction(api.once) );
			assert( _.isFunction(api.removeListener) );
			assert( _.isFunction(api.launchBrowser) );
			assert( _.isFunction(api.closeBrowser) );
			assert( _.isFunction(api.showBrowser) );
			assert( _.isFunction(api.dump) );
			assert( _.isFunction(api.isReady) );
			done();
		});
	});

	it('should start successfully if sys.isDevelopmentMode() is disabled', function(done) {
		_reg.get('system').devModeEnabled = false;
		_module.onStart(function(err,api) {
			assert.isUndefined( err );
			assert.isObject( api );
			assert( _.isFunction(api.on) );
			assert( _.isFunction(api.once) );
			assert( _.isFunction(api.removeListener) );
			assert( _.isFunction(api.launchBrowser) );
			assert( _.isFunction(api.closeBrowser) );
			assert( _.isFunction(api.showBrowser) );
			assert( _.isFunction(api.dump) );
			assert( _.isFunction(api.isReady) );
			done();
		});
	});

	it('should fail to start if htmlshell cmd is corrupted', function() {
		_cfg.protoFile = 'bad';
		_module.onStart(function(err,api) {
			assert.isUndefined( api );
			assert.instanceOf(err, Error, 'Cannot start browser service' );
		});
	});

	it('should save plugins.json file', function() {
		_module.onStart(function(err,api) {
			assert.isUndefined( err );
			assert.isObject( api );
			var pluginsJSON = _adapter.savedFiles['plugins.json'];
			assert( _.isArray(pluginsJSON) );
			assert.equal( 1, pluginsJSON.length );
			var p = pluginsJSON[0];
			assert.equal( 'Test PlugIn', p.name );
			assert.equal( true, p.out_of_process );
			assert.equal( false, p.sandboxed );
			assert.equal( '/system/original/packages/ar.edu.unlp.info.lifia.test_plugin/plugins/libtest.so', p.path );
			assert.equal( '2.2', p.version );
			assert( _.isArray(p.mime_types) );
			assert.equal( 1, p.mime_types.length );
			var m = p.mime_types[0];
			assert.equal( 'application/test', m.type );
			assert.equal( 'xyz', m.extensions );
			assert.equal( 'Test application (XYZ)', m.description );
		});
	});

	it('should not fail if onStop is called multiples times', function(done) {
		var stopped = 0;
		function onStopped(err) {
			assert( _.isUndefined(err) );
			stopped++;
			if (stopped === 2) {
				done();
			}
		}
		_module.onStop(onStopped);
		_module.onStop(onStopped);
	});
});
