'use strict';

var os = require('os');
var path = require('path');
var util = require('util');
var EventEmitter = require('events').EventEmitter;
var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var Mocks = require('mocks');
var UsbUpdate = require('../src/main');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs-extra') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var tvdutil = require('tvdutil');

describe('USB update', function() {
	var reg = null;
	var adapter = null;
	var tmpPath = null;

	beforeEach(function(done) {
		mktemp.createDirAsync( path.join(os.tmpdir(),'usbupdate.XXXXXXXX') )
			.then(function(tmp) {
				tmpPath = tmp;
				reg = Mocks.init();
				adapter = Mocks.ServiceAdapter(reg);
				adapter.getDataPath = function() {
					var params = Array.prototype.slice.call(arguments, 0);
					params.unshift( path.join(tmpPath,'config','usbupdate') );
					return path.join.apply(path,params);
				};
				fs.ensureDirSync( path.join(tmpPath,'config','usbupdate') );

				reg.put( 'diskmgr', new Mocks.DiskManager() );
				reg.put( 'system', new Mocks.System() );
				done();
			});
	});

	afterEach(function() {
		Mocks.fin();
		adapter = null;
		reg = null;
		tvdutil.removeTreeSync( tmpPath );
	});

	describe('constructor', function() {
		it('should construct basic object', function() {
			new UsbUpdate(adapter);
		});

		it('should start/stop screen', function(done) {
			var s = new UsbUpdate(adapter);
			s.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				s.onStop(done);
			});
		});

		it('should start/stop/start/stop screen', function(done) {
			var s = new UsbUpdate(adapter);

			s.onStart(function(err,api) {
				assert.equal(err);
				s.onStop(function() {
					s.onStart(function(err1,api1) {
						assert.equal(err1);
						assert(api1);
						assert( api !== api1 );
						s.onStop(done);
					});
				});
			});
		});

		it('should start with a usb disk mounted and check for update', function(done) {
			var s = new UsbUpdate(adapter);
			var diskMgr = reg.get('diskmgr');
			var enumCalled=0;
			var getCalled=0;
			diskMgr.enumerateDevices = function(cb) {
				enumCalled++;
				cb( undefined, ['disk1'] );
			};
			diskMgr.get = function(id,cb) {
				getCalled++;
				assert.equal( id, 'disk1' );
				cb( undefined, diskMgr.diskInfo(id,'pepe','/mnt') );
			};

			s.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				assert.equal( enumCalled, 1 );
				assert.equal( getCalled, 1 );
				s.onStop(done);
			});
		});

		it('should start and fail when enumerateDevices fail', function(done) {
			var s = new UsbUpdate(adapter);
			var diskMgr = reg.get('diskmgr');
			var enumCalled=0;
			diskMgr.enumerateDevices = function(cb) {
				enumCalled++;
				cb( new Error('Alguno') );
			};
			s.onStart(function(err,api) {
				assert(err);
				assert.equal(err.message,'Alguno');
				assert.equal( enumCalled, 1 );
				done();
			});
		});
	});

	describe('diskmgr notifications', function() {
		var usb = null;

		beforeEach(function(done) {
			usb = new UsbUpdate(adapter);
			usb.onStart(function(err,impl) {
				assert.equal(err);
				assert.isObject(impl);
				done();
			});
		});

		afterEach(function(done) {
			usb.onStop(done);
		});

		it('Should update system', function(done) {
			var imagePath = path.join(tmpPath,'image.tac');
			var sys = reg.get('system');
			sys.updateFirmware = function( path, now ) {
				assert.equal( path, adapter.getDataPath('image.tac') );
				assert.equal( now, undefined );
				done();
			};

			//	Set image.tac
			fs.writeFileSync( imagePath, 'adafdsafd' );

			var diskMgr = reg.get('diskmgr');
			diskMgr.addDisk( diskMgr.diskInfo('disk1','pepe', tmpPath) );
		});

		it('Should check copyFile errors', function(done) {
			var imagePath = path.join(tmpPath,'image.tac');
			var sys = reg.get('system');
			var updateCalled = 0;

			sys.sendError = function( err ) {
				assert( err );
				assert( err.message.indexOf('Cannot copy update file to package data path') >= 0 );
				done();
			};

			sys.updateFirmware = function( path, now ) {
				updateCalled++;
			};

			//	Set image.tac
			fs.writeFileSync( imagePath, 'adafdsafd' );
			tvdutil.removeTreeSync( adapter.getDataPath() );

			var diskMgr = reg.get('diskmgr');
			diskMgr.addDisk( diskMgr.diskInfo('disk1','pepe', tmpPath) );
			assert.equal( updateCalled, 0 );
		});

		it('Should check for mount paths', function() {
			var imagePath = path.join(tmpPath,'image.tac');
			var sys = reg.get('system');
			var updateCalled = 0;
			sys.updateFirmware = function( path, now ) {
				updateCalled++;
			};

			//	Set image.tac
			fs.writeFileSync( imagePath, 'adafdsafd' );

			var diskMgr = reg.get('diskmgr');
			diskMgr.addDisk( diskMgr.diskInfo('disk1','pepe') );
			assert.equal( updateCalled, 0 );
		});

		it('Should not update system becouse image.tac is directory', function() {
			var imagePath = path.join(tmpPath,'image.tac');
			var sys = reg.get('system');
			var updateCalled = 0;
			sys.updateFirmware = function( path, now ) {
				updateCalled++;
			};

			//	Set image.tac
			fs.mkdirSync( imagePath );

			var diskMgr = reg.get('diskmgr');
			diskMgr.addDisk( diskMgr.diskInfo('disk1','pepe', tmpPath) );

			assert.equal( updateCalled, 0 );
		});

		it('Should not update system becouse image.tac not exists', function() {
			var imagePath = path.join(tmpPath,'image.tac');
			var sys = reg.get('system');
			var updateCalled = 0;
			sys.updateFirmware = function( path, now ) {
				updateCalled++;
			};

			var diskMgr = reg.get('diskmgr');
			diskMgr.addDisk( diskMgr.diskInfo('disk1','pepe', tmpPath) );

			assert.equal( updateCalled, 0 );
		});
	});
});
