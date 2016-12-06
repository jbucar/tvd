'use strict';

var os = require('os');
var path = require('path');
var fs = require('fs-extra');
var util = require('util');
var tvdutil = require('tvdutil');
var _ = require('lodash');
var bPromise = require('bluebird');
var mktemp = bPromise.promisifyAll( require('mktemp') );

var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var mockfs = require('mock-fs');
var Mocks = require('mocks');

var DiskProvider = require('../src/diskprovider');

describe('DiskProvider', function() {
	var reg = null;
	var adapter = null;
	var rootDir = null;

	beforeEach(function() {
		reg = Mocks.init('silly');
		var dest = path.join( os.tmpDir(), 'diskprovider.XXXXXXXX');
		rootDir = mktemp.createDirSync( dest );

		Mocks.rwPath( rootDir );
		adapter = new Mocks.ServiceAdapter(reg,'localmedia');
		reg.put( 'ar.edu.unlp.info.lifia.tvd.media.image', new Mocks.Provider() );
		reg.put( 'ar.edu.unlp.info.lifia.tvd.media.video', new Mocks.Provider() );
		reg.put( 'diskmgr', new Mocks.DiskManager() );
	});

	afterEach(function() {
		Mocks.fin();
		fs.remove( rootDir );
		rootDir = null;
		adapter = null;
	});

	describe('constructor', function() {
		it('should construct basic DiskProvider', function() {
			new DiskProvider(adapter);
		});
	});

	describe('onStart/onStop', function() {
		var image = null;

		beforeEach(function() {
			image = new DiskProvider(adapter);
		});

		afterEach(function() {
			image = null;
		});

		it('should check api from image', function(done) {
			image.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				image.onStop(done);
			});
		});

		it('should start/stop/start', function(done) {
			image.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				image.onStop(function() {
					image.onStart(function(err1,api1) {
						assert.equal(err1);
						assert.isObject(api1);
						assert(api1 != api);
						image.onStop(done);
					});
				});
			});
		});
	});

	describe('start', function() {
		var prov = null;

		function startService(cb) {
			prov = new DiskProvider(adapter);
			prov.onStart(cb);
		}

		beforeEach(function() {
			prov = null;
		});

		afterEach(function(done) {
			prov.onStop(function() {
				prov = null;
				done();
			});
		});

		it('should add disk provider for disk1', function(done) {
			var disk = reg.get('diskmgr');
			disk.enumerateDevices = function(cb) {
				cb( undefined, ['disk1'] );
			};
			disk.get = function(id,cb) {
				assert.equal( id, 'disk1' );
				cb( undefined, disk.diskInfo(id,'pepe','/mnt') );
			};

			startService(function() {
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 1 );
				assert.equal( image.providers['disk1'].id, 'disk1' );
				assert.equal( image.providers['disk1'].name, 'pepe' );
				done();
			});
		});

		it('should add disk provider with name from drive info', function(done) {
			var disk = reg.get('diskmgr');
			disk.enumerateDevices = function(cb) {
				cb( undefined, ['disk1'] );
			};
			disk.get = function(id,cb) {
				assert.equal( id, 'disk1' );
				cb( undefined, disk.diskInfo(id,undefined,'/mnt') );
			};

			startService(function() {
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 1 );
				assert.equal( image.providers['disk1'].id, 'disk1' );
				assert.equal( image.providers['disk1'].name, 'vendor' );
				done();
			});
		});

		it('should add disk provider with name from model info', function(done) {
			var disk = reg.get('diskmgr');
			disk.enumerateDevices = function(cb) {
				cb( undefined, ['disk1'] );
			};
			disk.get = function(id,cb) {
				assert.equal( id, 'disk1' );
				var i = disk.diskInfo(id,undefined,'/mnt');
				i.drive.vendor = undefined;
				cb( undefined, i );
			};

			startService(function() {
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 1 );
				assert.equal( image.providers['disk1'].id, 'disk1' );
				assert.equal( image.providers['disk1'].name, 'pepe' );
				done();
			});
		});

		it('should add disk provider with name from revision info', function(done) {
			var disk = reg.get('diskmgr');
			disk.enumerateDevices = function(cb) {
				cb( undefined, ['disk1'] );
			};
			disk.get = function(id,cb) {
				assert.equal( id, 'disk1' );
				var i = disk.diskInfo(id,undefined,'/mnt');
				i.drive.vendor = undefined;
				i.drive.model = undefined;
				cb( undefined, i );
			};

			startService(function() {
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 1 );
				assert.equal( image.providers['disk1'].id, 'disk1' );
				assert.equal( image.providers['disk1'].name, 'pepe1' );
				done();
			});
		});

		it('should add disk provider with name from serial info', function(done) {
			var disk = reg.get('diskmgr');
			disk.enumerateDevices = function(cb) {
				cb( undefined, ['disk1'] );
			};
			disk.get = function(id,cb) {
				assert.equal( id, 'disk1' );
				var i = disk.diskInfo(id,undefined,'/mnt');
				i.drive.vendor = undefined;
				i.drive.model = undefined;
				i.drive.revision = undefined;
				cb( undefined, i );
			};

			startService(function() {
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 1 );
				assert.equal( image.providers['disk1'].id, 'disk1' );
				assert.equal( image.providers['disk1'].name, 'pepe2' );
				done();
			});
		});

		it('should add disk provider with name from path', function(done) {
			var disk = reg.get('diskmgr');
			disk.enumerateDevices = function(cb) {
				cb( undefined, ['disk1'] );
			};
			disk.get = function(id,cb) {
				assert.equal( id, 'disk1' );
				var i = disk.diskInfo(id,undefined,'/mnt/pepe1');
				i.drive.vendor = undefined;
				i.drive.model = undefined;
				i.drive.revision = undefined;
				i.drive.serial = undefined;
				cb( undefined, i );
			};

			startService(function() {
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 1 );
				assert.equal( image.providers['disk1'].id, 'disk1' );
				assert.equal( image.providers['disk1'].name, 'pepe1' );
				done();
			});
		});

		it('should check error when add disk provider', function(done) {
			var disk = reg.get('diskmgr');
			disk.enumerateDevices = function(cb) {
				cb( undefined, ['disk1'] );
			};
			disk.get = function(id,cb) {
				assert.equal( id, 'disk1' );
				cb( new Error('pepe'), undefined );
			};

			startService(function() {
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 0 );
				done();
			});
		});

		it('should check disk is mounted', function(done) {
			var disk = reg.get('diskmgr');
			disk.enumerateDevices = function(cb) {
				cb( undefined, ['disk1'] );
			};
			disk.get = function(id,cb) {
				assert.equal( id, 'disk1' );
				cb( undefined, { id: id, mountPaths: [] } );
			};

			startService(function() {
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 0 );
				done();
			});
		});

		it('should check error on disk enumeration', function(done) {
			var disk = reg.get('diskmgr');
			disk.enumerateDevices = function(cb) {
				cb( new Error('pepe'), [] );
			};
			startService(function(err) {
				assert.equal( err.message, 'pepe' );
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 0 );
				done();
			});
		});

		it('should add providers when insert a new disk', function(done) {
			var disk = reg.get('diskmgr');
			startService(function(err) {
				assert.equal( err );
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 0 );
				disk.addDisk( disk.diskInfo('disk1','pepe', '/mnt') );
				assert.equal( Object.keys(image.providers).length, 1 );
				done();
			});
		});

		it('should add/remove providers when insert a new disk', function(done) {
			var disk = reg.get('diskmgr');
			startService(function(err) {
				assert.equal( err );
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 0 );
				disk.addDisk( disk.diskInfo('disk1', 'pepe', '/mnt') );
				assert.equal( Object.keys(image.providers).length, 1 );
				disk.rmDisk( 'disk1' );
				assert.equal( Object.keys(image.providers).length, 0 );
				done();
			});
		});

		it('should test add/mount/remove disk', function(done) {
			var disk = reg.get('diskmgr');
			var disks = [];
			disk.enumerateDevices = function(cb) {
				cb( undefined, Object.keys(disks) );
			};
			disk.get = function(id,cb) {
				cb( undefined, disks[id] );
			};
			startService(function(err) {
				assert.equal( err );
				var image = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
				assert.equal( Object.keys(image.providers).length, 0 );

				//	Add without mount
				disks['disk1'] = disk.diskInfo('disk1', 'pepe');
				disk.emit( 'added', 'disk1' );
				assert.equal( Object.keys(image.providers).length, 0 );

				//	Mount
				disks['disk1'] = disk.diskInfo('disk1', 'pepe', '/mnt');
				disk.emit( 'changed', 'disk1' );
				assert.equal( Object.keys(image.providers).length, 1 );

				//	Change other properties
				disk.emit( 'changed', 'disk1' );
				assert.equal( Object.keys(image.providers).length, 1 );

				//	Umount
				disks['disk1'] = disk.diskInfo('disk1', 'pepe');
				disk.emit( 'changed', 'disk1' );
				assert.equal( Object.keys(image.providers).length, 0 );

				//	Remove
				disk.emit( 'removed', 'disk1' );
				assert.equal( Object.keys(image.providers).length, 0 );
				done();
			});
		});
	});
});
