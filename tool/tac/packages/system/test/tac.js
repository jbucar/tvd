'use strict';

var tvdutil = require('tvdutil');
var path = require('path');
var _ = require('lodash');
var bPromise = require('bluebird');
var fs = require('fs');
var mockfs = require('mock-fs');
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;

var Tac = require('../src/tac').Tac;
var TacMain = require('../src/tac').main;

describe('Tac', function() {
	function createManifest( id, comps ) {
		return {
			"id": id,
			"version": "1.0.0.g1123954i35143",
			"manifest-version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "all",
			"components": comps
		};
	}

	function systemID() {
		return 'ar.edu.unlp.info.lifia.tvd.system';
	}

	function getSystemManifest(ro) {
		return path.join(ro,'packages', systemID(), 'manifest.json');
	}

	function writeFile(name,data) {
		fs.writeFileSync( name, data );
	}

	function writeJsonFile(name,data) {
		writeFile( name, JSON.stringify(data, null, 4) );
	}

	var tac;
	var ro;
	var rw;

	function setup(pRO,pRW) {
		ro = pRO ? pRO : '/system/original';
		rw = pRW ? pRW : '/system/data';
		mockfs({});
		fs.mkdirSync('/etc');
		fs.mkdirSync(path.dirname(ro));
		if (path.dirname(ro) !== path.dirname(rw)) {
			fs.mkdirSync(path.dirname(rw));
		}
		fs.mkdirSync(ro);
		fs.mkdirSync(rw);
		fs.mkdirSync(path.join(ro,'packages'));
		fs.mkdirSync(path.join(ro,'packages',systemID()));
		writeJsonFile( getSystemManifest(ro), createManifest(systemID(),[]) );
	}

	function start(needSetup) {
		if (needSetup) {
			setup();
		}
		tac = new Tac(ro,rw);
		assert(tac);
		tac.start();
	}

	function setupExit(cb,exitCode) {
		var oldExit = process.exit;
		process.exit = function(code) {
			console.log( 'Exit dummy called: code=%d, exitCode=%d', code, exitCode );
			assert.equal(code,exitCode);
			process.exit = oldExit;
			cb();
		};
	}

	function stop(cb,exitCode) {
		assert(tac);
		setupExit(function() {
			tac = null;
			cb();
		},exitCode);
		tac.powerOff();
	}

	beforeEach(function() {
		tac = null;
		ro = null;;
		rw = null;
	});

	afterEach(function() {
		assert(tac === null);
		delete global.tac;
		mockfs.restore();
	});

	describe('constructor', function() {
		it('should construct a Tac', function() {
			setup();
			tac = new Tac(ro,rw);
			assert(tac);
			tac = null;
		});

		it('should construct and start', function(done) {
			setup();
			tac = new Tac(ro,rw);
			assert(tac);
			tac.start();
			assert.equal( tac.getInstallRootPath(), ro );
			assert.equal( tac.getDataRootPath(), rw );
			stop(done,102);
		});

		it('should call to tac.main', function(done) {
			setup();
			TacMain(ro,rw);
			assert(global.tac);
			tac = global.tac;
			stop(done,102);
		});

		it('should construct and start with others root path', function(done) {
			setup( '/pepe/original', '/pepe1/data' );
			tac = new Tac(ro,rw);
			assert(tac);
			tac.start();
			assert.equal( tac.getDataRootPath(), rw );
			assert.equal( tac.getInstallRootPath(), ro );
			stop(done,102);
		});

		it('should construct and start fail (system manifest invalid)', function(done) {
			setupExit(function() {
				done();
				throw new Error('Exit from node');
			},100);

			setup();
			writeFile( getSystemManifest(ro), 'pepe' );

			var tac = new Tac(ro,rw);
			assert(tac);
			var fnc = function() { tac.start(); };
			assert.throw(fnc,'Exit from node');
		});

		it('should start and catch pkgmgr/srvmgr errors', function(done) {
			setupExit(function() {
				tac = null;
				done();
			},100);
			setup();
			writeJsonFile( getSystemManifest(ro), { version: '1.0.0.gasdagsdgas'});

			tac = new Tac(ro,rw);
			assert(tac);
			tac.start();
		});

		it('should start and change Error to define toJSON', function(done) {
			setup();
			tac = new Tac(ro,rw);
			assert(tac);
			tac.start();

			let err = new Error('pepe');
			let strErr = JSON.stringify(err);
			let err2 = JSON.parse(strErr);
			assert.equal( err.message, err2.message );
			assert.equal( err.code, err2.code );

			stop(done,102);
		});

		it('should construct and start cannot parse system manifest', function(done) {
			setupExit(function() {
				done();
				throw new Error('Exit from node');
			},100);

			var ro = '/system/original';
			var rw = '/system/data';
			var sfs = {
				ro: {},
				rw: {},
			};

			mockfs(sfs);
			var tac = new Tac(ro,rw);
			assert(tac);
			var fnc = function() { tac.start(); };
			assert.throw(fnc,'Exit from node');
		});
	});

	describe('events', function() {
		var oldEvent;
		var signals;

		beforeEach(function() {
			signals = {};
			oldEvent = process.on;
			process.on = function(sig,cb) {
				signals[sig] = cb;
			};
		});

		afterEach(function() {
			process.on = oldEvent;
			tac = null;
		});

		it('should check alive', function(done) {
			setup();
			tac = new Tac(ro,rw);
			assert(tac);
			tac.start(100);
			setTimeout(function() {
				stop(done,102);
			},200);
		});

		it('should catch SIGINT signals', function(done) {
			setupExit(done,0);
			start(true);
			signals['SIGINT']( new Error('pepe') );
		});

		it('should catch uncaughtException normal error', function(done) {
			setupExit(done,103);
			start(true);
			signals['uncaughtException']( new Error('pepe') );
		});

		it('should catch uncaughtException normal error and sendError', function(done) {
			function SystemMock() {
				var self = {};
				self.sendError = function(err) {
					assert.equal( err.message, 'pepe' );
				};
				return self;
			}

			setupExit(done,103);
			start(true);
			tac.registry().put('system', new SystemMock() );
			signals['uncaughtException']( new Error('pepe') );
		});

		it('should catch uncaughtException normal error', function(done) {
			setupExit(done,100);
			start(true);
			signals['uncaughtException']( new RecoveryError('pepe') );
		});
	});

	describe('exit', function() {
		beforeEach(function() {
			setup();
			tac = new Tac(ro,rw);
			assert(tac);
			tac.start();
		});

		afterEach(function() {
			tac = null;
		});

		it('should powerOff', function(done) {
			setupExit(done,102);
			tac.powerOff();
		});

		it('should factoryReset', function(done) {
			setupExit(done,100);
			tac.factoryReset();
		});

		it('should reboot', function(done) {
			setupExit(done,103);
			tac.reboot();
		});

		it('should exit by timeout', function(done) {
			this.timeout(7000);

			setupExit(done,102);

			function SrvMgr() {
				var self = {};
				self.fin = function() {
					return new bPromise(function() {});
				};
				return self;
			};
			tac.registry().swap('srvmgr', new SrvMgr());

			tac.powerOff();
		});
	});

	describe('license/first boot', function() {
		afterEach(function(done) {
			if (tac) {
				stop(done,102);
			}
			else {
				done();
			}
		});

		it('should set first boot in true when no version in data', function() {
			start(true);
			assert.isTrue( tac.isFirstBoot() );
		});

		it('should set first boot in true when cannot parse configuration file', function() {
			setup();
			writeFile( path.join(rw,'system_version.json'), '{}' );
			start();
			assert.isTrue( tac.isFirstBoot() );
		});

		it('should set first boot in false when the same version', function() {
			setup();
			writeFile( path.join(rw,'system_version.json'), '{"version": "1.0.0"}' );
			start();
			assert.isFalse( tac.isFirstBoot() );
		});

		it('should accept license and not be in first boot inmediatly', function() {
			start(true);
			assert.isTrue( tac.isFirstBoot() );
			tac.licenseAccepted();
			assert.isFalse( tac.isFirstBoot() );
		});

		it('should accept license and not be in first boot after reboot', function(done) {
			start(true);
			assert.isTrue( tac.isFirstBoot() );
			tac.licenseAccepted();
			stop(function() {
				start();
				assert.isFalse( tac.isFirstBoot() );
				done();
			},102);
		});

		it('should no change data version if no first boot', function() {
			setup();
			writeFile( path.join(rw,'system_version.json'), '{"version": "1.0.0"}' );
			start();
			assert.isFalse( tac.isFirstBoot() );
			var old = fs.writeFile;
			fs.writeFile = function(name,data) {
				assert(false);
			};
			tac.licenseAccepted();
			fs.writeFile = old;
		});

		it('should reboot if error in accept license', function(done) {
			setupExit(function() {
				tac = null;
				done();
			},103);
			start(true);
			assert.isTrue( tac.isFirstBoot() );
			var old = fs.writeFile;
			fs.writeFile = function(name,data,cb) {
				fs.writeFile = old;
				cb( new Error('pepe') );
			};
			tac.licenseAccepted();
		});
	});

	describe('registry', function() {
		beforeEach(function() {
			start(true);
		});

		afterEach(function(done) {
			stop(done,102);
		});

		it('should get registry with srvmgr and pkgmgr', function() {
			var reg = tac.registry();
			assert(reg);
			assert(reg.get('srvmgr'));
			assert(reg.get('pkgmgr'));
		});

		it('should return registry from instance', function() {
			var reg = tac.registry();
			assert(reg);
			stop(function() {
				start();
				var reg1 = tac.registry();
				assert(reg1);

				assert(reg != reg1);
			},102);
		});
	});

	describe('develompent mode', function() {
		beforeEach(function() {
			start(true);
		});

		afterEach(function(done) {
			stop(done,102);
		});

		it('should start always with inDevelopmentMode in false', function() {
			assert.isFalse( tac.inDevelopmentMode() );
		});

		it('should change and get successfully', function() {
			assert.isFalse( tac.inDevelopmentMode() );
			tac.enableDevelopmentMode(true);
			assert.isTrue( tac.inDevelopmentMode() );
			tac.enableDevelopmentMode(false);
			assert.isFalse( tac.inDevelopmentMode() );
		});
	});

	describe('getPlatform', function() {
		afterEach(function(done) {
			stop(done,102);
		});

		it('should get default platform if /etc/platform.version file not exists', function() {
			start(true);

			var info = tac.getPlatform();
			assert.isObject( info );
			assert.equal( info.name, 'UNKNOWN' );
			assert.equal( info.version, '0.0.0' );
			assert.equal( info.commit, '0' );
		});

		it('should get platform from CR file /etc/platform.version', function() {
			setup();
			writeFile( '/etc/platform.version', 'AML_ISDBT_1-1-0-296-g989fdf2\n' );
			start();

			var info = tac.getPlatform();
			assert.isObject( info );
			assert.equal( info.name, 'AML_ISDBT_1' );
			assert.equal( info.version, '1.0.296' );
			assert.equal( info.commit, 'g989fdf2' );
		});

		it('should get platform from file /etc/platform.version', function() {
			setup();
			writeFile( '/etc/platform.version', 'AML_ISDBT_1-1-0-296-g989fdf2' );
			start();

			var info = tac.getPlatform();
			assert.isObject( info );
			assert.equal( info.name, 'AML_ISDBT_1' );
			assert.equal( info.version, '1.0.296' );
			assert.equal( info.commit, 'g989fdf2' );
		});
	});

	describe('getSystemVersion', function() {
		afterEach(function(done) {
			stop(done,102);
		});

		it('should get system version', function() {
			start(true);
			var info = tac.getSystemVersion();
			assert(info);
			assert.equal(info.api,1);
			assert.equal(info.version,"1.0.0");
			assert.equal(info.commit,"g1123954i35143");
		});
	});

	describe('getSerial', function() {
		afterEach(function(done) {
			stop(done,102);
		});

		it('should get default platform if /etc/platform.version file not exists', function() {
			process.env.TAC_SERIAL = 'p:e:p:e';
			start(true);
			assert.equal( tac.getSerial(), 'pepe' );
			delete process.env.TAC_SERIAL;
		});
	});

	describe('updateFirmware', function() {
		beforeEach(function() {
			start(true);
		});

		afterEach(function() {
			tac = null;
		});

		it('should reboot if fail to write update url to tmp file', function(done) {
			setupExit(done,100);
			tac.factoryReset();
		});
	});

	describe('updateFirmware', function() {
		beforeEach(function() {
			start(true);
		});

		afterEach(function() {
			tac = null;
		});

		it('should reboot if fail to write update url to tmp file', function(done) {
			setupExit(done,103);
			tac.updateFirmware({},true);
		});

		it('should update on poweroff', function(done) {
			setupExit(done,101);
			tac.updateFirmware('http://alguna',true);
		});

		it('should update on poweroff', function(done) {
			setupExit(done,101);
			tac.updateFirmware('http://alguna',false);
			tac.powerOff();
		});
	});
});

