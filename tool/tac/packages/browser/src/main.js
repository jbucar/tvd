'use strict';

var BrowserApi = require('./browserapi');
var BrowserService = require('./impl/browserservice');
var CrashObserver = require('./impl/crashobserver');
var HtmlShellRemote = require('htmlshellremote');
var path = require('path');
var _ = require('lodash');

// Return default configurations
function getDefaultCfg(adapter) {
	return {
		version: {major: 1, minor: 2},
		tmpDirectory: adapter.getTemporaryPath(),
		htmlshell: {
			workPath: adapter.getDataPath(),
			installPath: adapter.getInstallPath(),
			unsupportedCodecs: []
		}
	};
}

function addExtraParams( cfg, sys, pPath ) {
	if (sys.isDevelopmentMode()) {
		cfg.htmlshell.args.push('--remote-debugging-port=9876');
	}
	var info = sys.getInfo();
	cfg.htmlshell.args.push('--extra-ua=TAC/' + info.system.version + ' (' + info.platform.name + ')');
	cfg.htmlshell.args.push('--plugins=' + pPath);
}

function getLayout( plat ) {
	var layout = plat.getConfig('keyboardLayout');
	var oldLayout = {};
	Object.keys(layout).forEach(function (k) {
		oldLayout[k] = layout[k].virtual;
	});
	return oldLayout;
}

// Export module hooks
module.exports = function(adapter) {
	var self = this || {};
	var _browserSrv = null;
	var _cfg = {};
	var _remoteCfg = {};
	var _reg = null;
	var _pluginsPath = '';

	self.onStart = function( cb ) {
		_reg = adapter.registry();

		// Create plugins.json
		var pkgmgr = _reg.get('pkgmgr');

		var remote = new HtmlShellRemote();
		_browserSrv = new BrowserService(remote);
		_cfg = adapter.load('browser.json', getDefaultCfg(adapter));
		_remoteCfg = remote.defaultConfig();

		_pluginsPath = path.join(adapter.getDataPath(), 'plugins.json');
		adapter.save('plugins.json', pkgmgr.getAllComponentsInfo('browser_plugin').reduce(function(res, p) {
			return res.concat({
				'name': p.name,
				'out_of_process': p.out_of_process,
				'sandboxed': p.sandboxed,
				'path': path.join(pkgmgr.getInstallPath(p.id), p.main),
				'version': p.version,
				'mime_types': _.cloneDeep(p.mime_types)
			});
		}, []));

		var plat = _reg.get('platform');
		var sys = _reg.get('system');

		function mergeDefaults(dest, src, value, key) {
			if (dest[key] === undefined) {
				dest[key] = value;
			} else if (_.isArray(dest[key]) && _.isArray(src[key])) {
				_.merge(dest[key], src[key]);
			} else if (_.isPlainObject(dest[key]) && _.isPlainObject(src[key])) {
				_.forOwn(src[key], _.partial(mergeDefaults, dest[key], src[key]));
			}
		}

		// Config priority:
		//   1) browser.json
		//   2) getDefaultCfg()
		//   3) platfrom.getConfig('htmlshell')
		//   4) HtmlShellRemote.defaultConfig()
		var cfg = { htmlshell: plat.getConfig('htmlshell') };
		_.forOwn(cfg, _.partial(mergeDefaults, _cfg, cfg));
		_.forOwn(_remoteCfg, _.partial(mergeDefaults, _cfg, _remoteCfg));
		addExtraParams(_cfg, sys, _pluginsPath);

		if (_browserSrv.start(_cfg, new CrashObserver(sys), getLayout(plat))) {
			log.verbose('BrowserService', 'Started successfully');
			cb( undefined, Object.freeze(new BrowserApi(_browserSrv)) );
		} else {
			log.error('BrowserService', 'onStart failure!');
			cb( new Error( 'Cannot start browser service' ) );
		}
	};

	self.onStop = function( callback ) {
		if (_browserSrv) {
			_browserSrv.stop(callback);
		} else {
			callback();
		}
	};

	return Object.freeze(self);
};
