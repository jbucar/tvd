'use strict';

var assert = require('assert');
var path = require('path');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var tvdutil = require('tvdutil');

var _updateName = 'image.tac';

//	Service entry point
function UpdateService(adapter) {
	var self = {};
	var _diskMgr = null;
	var getDiskInfo = null;

	function copyFile(source, target) {
		return new bPromise(function(resolve, reject) {
			var rd = fs.createReadStream(source);
			rd.on('error', reject);
			var wr = fs.createWriteStream(target);
			wr.on('error', reject);
			wr.on('finish', resolve);
			rd.pipe(wr);
		});
	}

	function system() {
		return adapter.registry().get('system');
	}

	function update( path ) {
		var newPath = adapter.getDataPath(_updateName);
		copyFile( path, newPath )
			.then(function() {
				system().updateFirmware( newPath );
			}, function(err) {
				system().sendError( tvdutil.warnErr( 'usb_update', 'Cannot copy update file to package data path: err=%s', err.message ) );
			});
	}

	function checkUpdate( diskID ) {
		log.verbose( 'usb_update', 'Disk added/changed: diskID=%s', diskID );
		getDiskInfo( diskID )
			.then(function(diskInfo) {
				assert(diskID === diskInfo.id);
				if (diskInfo.mountPaths.length > 0) {
					var mountPath = diskInfo.mountPaths[0];
					log.verbose('usb_update', 'Check for update on USB disk: diskID=%s, mnt=%s, diskInfo=%j', diskID, mountPath, diskInfo );
					var imagePath = path.join(mountPath,_updateName);
					return fs.statAsync(imagePath)
                        .then(function(stat) {
                            if (stat.isFile()) {
                                return update( imagePath );
                            }
                        }, function() {} );	//      Ignore error
				}
			});
	}

	self.onStart = function(cb) {
		_diskMgr = adapter.registry().get('diskmgr');
		assert(_diskMgr);
		getDiskInfo = bPromise.promisify(_diskMgr.get,_diskMgr);

		_diskMgr.on( 'added', checkUpdate );
		_diskMgr.on( 'changed', checkUpdate );
		_diskMgr.enumerateDevices(function(err,aDevices) {
			if (err) {
				cb( err );
				return;
			}

			aDevices.forEach(function(devID) {
				checkUpdate( devID );
			});

			cb( undefined, {} );
		});
	};

	self.onStop = function(cb) {
		assert(cb);
		_diskMgr.removeListener( 'added', checkUpdate );
		_diskMgr.removeListener( 'changed', checkUpdate );
		_diskMgr = null;
		getDiskInfo = null;
		cb();
	};

	return Object.freeze(self);
}

module.exports = UpdateService;

