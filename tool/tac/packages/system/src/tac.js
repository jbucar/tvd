'use strict';

var assert = require('assert');
var util = require('util');
var path = require('path');
var fs = require('fs');
var bPromise = require('bluebird');
var tvdutil = require('tvdutil');
var Registry = require('registry');
var PackageManager = require('./packagemgr');
var ServiceManager = require('./servicemgr');

var ALIVE_TIMEOUT = 10000;

var EXIT_CODE_RECOVERY = 100;
var EXIT_CODE_UPDATE = 101;
var EXIT_CODE_SHUTDOWN = 102;
var EXIT_CODE_REBOOT = 103;

function Tac( ro, rw ) {
	var self = {};
	var _info = null;
	var _reg = null;
	var _inDevelopmentMode = false;
	var _updateOnShutdown = false;
	var _checkInterval = null;

	function checkAlive() {
		log.silly( 'tac', 'Check alive!' );
	}

	function checkSignal() {
		log.error( 'tac', 'Got SIGINT ... exit');
		process.exit(0);
	}

	function parse(name) {
		var data;
		try {
			data = JSON.parse(fs.readFileSync( name ));
		} catch(err) {
			if (err.code !== 'ENOENT') {
				log.warn( 'tac', 'Cannot parse configuration file: file=%s, err=%s', name, err.message );
			}
		}
		return data;
	}

	function panic( err ) {
		log.error("tac", err.stack );
		if (err instanceof RecoveryError) {
			self.factoryReset();
		}
		else {
			self.reboot();
		}
	}

	function catchAnyError( err ) {
		log.error( 'tac', 'UncaughtException: %s', err.message );

		//	Try get system service
		var sys = _reg.get('system');
		if (sys) {
			sys.sendError( err );
		}

		panic( err );
	}

	function doExit( code ) {
		log.info( 'tac', 'Exit node: code=%d', code );
		process.exit(code);
		delete global.tac;
	}

	function getSerial() {
		var serial = process.env.TAC_SERIAL;
		if (serial) {
			return serial.replace(/:/g,"");
		}
		return '1';
	}

	function getPlatform() {
		var data;
		try {
			data = fs.readFileSync( '/etc/platform.version' ).toString().trim();
		} catch(err) {
			data = 'UNKNOWN-0-0-0-0';
		}
		var comps = data.split("-");
		return {
			name: comps[0],
			version: comps[1] + '.' + comps[2] + '.' + comps[3],
			commit: comps[4],
		};
	}

	function getInfoFromVersion( version ) {
		var comps = version.split(".");
		assert(comps.length == 4);
		return {
			"api": parseInt(comps[0]),
			"version": comps[0] + '.' + comps[1] + '.' + comps[2],
			"commit": comps[3]
		};
	}

	function persistedDataVersion() {
		return path.join(rw,'system_version.json');
	}

	function getDataVersion() {
		var data = parse( persistedDataVersion() );
		return data ? data.version : data;
	}

	function getSystemVersion() {
		var data = parse( path.join(ro,'packages','ar.edu.unlp.info.lifia.tvd.system', 'manifest.json') );
		if (!data || !data.version) {
			doExit(EXIT_CODE_RECOVERY);
		}
		return data.version;
	}

	function loadInformation() {
		var dataVersion = getDataVersion();
		var systemVersion = getSystemVersion();

		//	Setup information
		_info = {};
		_info.platform = getPlatform();
		_info.system = getInfoFromVersion( systemVersion );
		_info.id = getSerial();
		_info.isFirstBoot = (_info.system.version !== dataVersion);

		log.info( 'tac', 'System information: %j', _info );
		global.tac = self;
	}

	function init(aliveTimer) {
		assert(!_reg);

		//	Register RecoveryError
		global.RecoveryError = function (message) {
			Error.call(this);
			Error.captureStackTrace(this, this.constructor);
			this.name = this.constructor.name;
			this.message = message;
		};
		util.inherits(global.RecoveryError, Error);

		//	Define toJSON in error
		if (!('toJSON' in Error.prototype)) {
			Object.defineProperty(Error.prototype, 'toJSON', {
				value: function () {
					var alt = {
						message: this.message,
						code: this.code
					};
					return alt;
				},
				configurable: true,
				writable: true
			});
		}

		//	Create registry
		_reg = new Registry();

		try {
			//	Initialize and put package manager into registry
			_reg.put(
				'pkgmgr',
				new PackageManager( ro, rw )
			);

			{	//	Create and initialize service manager
				var srvMgr = new ServiceManager();
				srvMgr.init(_reg);
			}

		} catch( err ) {
			log.error( 'tac', 'Cannot initialize tac: err=%s', err.message );
			doExit( EXIT_CODE_RECOVERY );
		}

		//	Register Ctrl-C
		process.on('SIGINT', checkSignal );

		//	Register unhandled errors
		process.on('uncaughtException', catchAnyError );

		//	Register unhandled rejections
		bPromise.onPossiblyUnhandledRejection( catchAnyError );

		//	Register check alive timer
		_checkInterval = setInterval( checkAlive, aliveTimer );
	}

	function fin() {
		assert(_reg);
		var srvMgr = _reg.get('srvmgr');
		assert(srvMgr);
		//	Stop service manager
		return srvMgr.fin().then( function() {
			log.silly( 'tac', 'Fin success' );
			//	Cleanup system handlers
			process.removeListener('SIGINT', checkSignal );
			process.removeListener('uncaughtException', catchAnyError);
			bPromise.onPossiblyUnhandledRejection( undefined );
			clearInterval( _checkInterval );
			_reg = null;
		});
	}

	function exitTac( code ) {
		assert(code !== undefined);

		if (_updateOnShutdown) {
			code = EXIT_CODE_UPDATE;
		}

		log.info( 'tac', 'Exit: code=%d', code );

		//	Finalize
		fin()
			.timeout(5000, 'Fin timeout')
			.then(function() {
				doExit(code);
			},function(err) {
				log.error( 'tac', 'Error in shutdown: %s', err.message );
				doExit(code);
			});
	}

	//	API
	self.registry = function() {
		return _reg;
	};

	self.getDataRootPath = function() {
		return rw;
	};

	self.getInstallRootPath = function() {
		return ro;
	};

	self.getPlatform = function() {
		return _info.platform;
	};

	self.getSystemVersion = function() {
		return _info.system;
	};

	self.getSerial = function() {
		return _info.id;
	};

	self.isFirstBoot = function() {
		return _info.isFirstBoot;
	};

	self.licenseAccepted = function() {
		if (self.isFirstBoot()) {
			log.info('tac', 'License accepted' );
			_info.isFirstBoot = false;
			var res = { version: _info.system.version };
			fs.writeFile( persistedDataVersion(), JSON.stringify(res, null, 4), function(err) {
				if (err) {
					log.error( 'tac', 'Cannot persist data version file: err=%s', err.message );
					self.reboot();
				}
			});
		}
	};

	self.inDevelopmentMode = function() {
		return _inDevelopmentMode;
	};

	self.enableDevelopmentMode = function(st) {
		_inDevelopmentMode = st;
	};

	self.updateFirmware = function(url,now) {
		try {
			assert(typeof url === 'string');
			assert(typeof now === 'boolean');

			//	Fix local url
			var relativePath = url.replace(rw,'');
			var fileURL = 'file://' + relativePath;

			fs.writeFileSync( '/tmp/tac_update_url', fileURL );
			if (now) {
				exitTac( EXIT_CODE_UPDATE );
			}
			else {
				//	Save and process in shutdown
				_updateOnShutdown = true;
			}
		} catch(err) {
			log.error('tac', 'Cannot persist firmware update url: err=%s', err.message );
			self.reboot();
		}
	};

	self.factoryReset = function() {
		exitTac( EXIT_CODE_RECOVERY );
	};

	self.reboot = function() {
		exitTac( EXIT_CODE_REBOOT );
	};

	self.powerOff = function() {
		exitTac( EXIT_CODE_SHUTDOWN );
	};

	self.start = function(aliveTimer) {
		if (!aliveTimer) {
			aliveTimer = ALIVE_TIMEOUT;
		}

		//	Register global log object and load log configuration from "/system/data/log.cfg"
		tvdutil.setupLog('info',path.join(rw,'log.cfg'));

		//	Load version information
		loadInformation();

		//	Init
		return init(aliveTimer);
	};

	return Object.freeze(self);
}

function main( ro, rw ) {
	var tac = new Tac( ro, rw );
	tac.start( ALIVE_TIMEOUT );
}

module.exports.Tac = Tac;
module.exports.main = main;

