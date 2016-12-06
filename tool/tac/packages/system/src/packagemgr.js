'use strict';

var util = require('util');
var assert = require('assert');
var mURL = require('url');
var path = require('path');
var EventEmitter = require('events').EventEmitter;
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var _ = require('lodash');
var tvdutil = require('tvdutil');
var validateManifest = bPromise.promisify( require('./jsonvalidator').isValid );

var manifest_file_name = 'manifest.json';

function PackageManager(ro,rw) {
	var _cache = [];
	var _packages = null;
	var _lockDatabaseCondition = null;
	init();

	//	Private functions
	function getPackagesPath(base) {
		return path.join(base, 'packages');
	}

	function getInstallPathForNewPackage( id ) {
		return path.join( getPackagesPath( rw ), id );
	}

	function getPackageInstallPath( pkgID ) {
		return _packages[pkgID];
	}

	function getConfigPath() {
		return path.join( rw, 'config' );
	}

	function getPackageConfigPath( id ) {
		var p = path.join( getConfigPath(), id );
		if (!tvdutil.isDirectory( p )) {
			fs.mkdirSync( p );
		}
		return p;
	}

	function getSystemConfigPath() {
		return getPackageConfigPath( 'ar.edu.unlp.info.lifia.tvd.system' );
	}

	function getTemporaryPath() {
		return path.join( getSystemConfigPath(), 'tmp' );
	}

	function joinPath(p, args) {
		var paths = Array.prototype.slice.call(args, 1);
		paths.unshift(p);
		return path.join.apply(null, paths);
	}

	function createPackage( manifestData ) {
		//	Assign packageID to all components
		for (var p in manifestData.components) {
			manifestData.components[p].pkgID = manifestData.id;
		}

		//	Check if enable update for the package
		if (isReadOnly(getPackageInstallPath(manifestData.id))) {
			manifestData.update = false;
		}
		else if (manifestData.update !== undefined) {
			manifestData.update = manifestData.update;
		}
		else {
			manifestData.update = true;
		}

		manifestData.platform = (manifestData.platform !== undefined) ? manifestData.platform.toLowerCase() : "all";
		return manifestData;
	}

	function isReadOnly( installPath ) {
		return installPath.indexOf( ro ) >= 0 ? true : false;
	}

	//	Components
	function get(mgr, id, filter) {
		var allPackages = mgr.getAllInfo();
		var findComp = function(c) {
			return c.id == id;
		};
		for (var i in allPackages) {
			var pkg = allPackages[i];
			var comp = _.find(pkg.components, findComp);
			if (comp) {
				return filter(pkg, comp);
			}
		}
		return undefined;
	}

	function getComponent(mgr, id) {
		return get(mgr, id, function(pkg, component) { return component; });
	}

	function getPackage(mgr, id) {
		return get(mgr, id, function(pkg) { return pkg; });
	}

	function checkPlatform( plat ) {
		if (!plat) {
			log.warn( 'PackageManager', 'Field platform missing' );
			return false;
		}

		if (typeof plat !== 'string') {
			log.warn( 'PackageManager', 'Field platform must be string' );
			return false;
		}

		var pkgPlat = plat.toUpperCase();
		if (pkgPlat === 'ALL') {
			return true;
		}

		var sysPlat = tac.getPlatform().name.toUpperCase();
		if (sysPlat === 'UNKNOWN') {
			//	Ingore dev platform
			return true;
		}

		return sysPlat === pkgPlat;
	}

	function checkManifest( info ) {
		return info &&
			info.id &&
			info.version &&
			info.vendor &&
			info['vendor-url'] &&
			info['api-level'] &&
			checkPlatform(info.platform);
	}

	//	Database
	function getDatabaseFile() {
		return path.join( getSystemConfigPath(), 'packages.json' );
	}

	function createPackageDB(filename) {
		var res = {};

		log.info( 'PackageManager', 'Create package database' );

		//	From ro
		var pkgPath = getPackagesPath(ro);
		fs.readdirSync(pkgPath).forEach(function(file) {
			res[file] = path.join(pkgPath, file);
		});

		//	From rw
		pkgPath = getPackagesPath(rw);
		fs.readdirSync(pkgPath).forEach(function(file) {
			res[file] = path.join(pkgPath, file);
		});

		fs.writeFileSync(filename, JSON.stringify(res, null, 4));
		return res;
	}

	function updateDB() {
		//	Write to packages.json.new
		var filename = getDatabaseFile();
		var newFile = filename + '.new';
		var oldFile = filename + '.old';
		return fs.writeFileAsync( newFile, JSON.stringify(_packages, null, 4) )
			//	Move packages.json to packages.json.old
			.then(function() {
				return fs.renameAsync( filename, oldFile );
			})
			//	Move packages.json.new to packages.json
			.then(function() {
				fs.renameAsync( newFile, filename );
			});
	}

	//	Install aux
	function iDownloadPkg( param ) {
		log.verbose( 'PackageManager', 'Download package: url=%s', param.url );
		var oURL = mURL.parse(param.url);
		if (!oURL || (oURL.protocol &&
					  oURL.protocol != 'http:' &&
					  oURL.protocol != 'https:' &&
					  oURL.protocol != 'file:')) {
			return bPromise.reject( tvdutil.warnErr( 'PackageManager', 'Invalid URL: protocol=%s', oURL.protocol) );
		}
		else if (oURL.protocol == 'http:' || oURL.protocol == 'https:') {
			var check = tac.inDevelopmentMode() ? false : true;
			if (check && (param.md5 === undefined || param.size === undefined)) {
				return bPromise.reject( tvdutil.warnErr( 'PackageManager', 'Invalid md5/size') );
			}
			return mktemp.createFileAsync( path.join( getTemporaryPath(), 'image_XXXXXXXX.tpk' ) )
				.then(function(tmpFile) {
					log.silly( 'PackageManager', 'Fetch tpk file from url' );
					var options = {
						'url': param.url,
						'fileOutput': tmpFile,
						'md5': param.md5,
						'size': param.size
					};
					return tvdutil.fetchURLAsync( options )
						.then(function() {
							param.tpk = tmpFile;
							return param;
						});
				});
		}
		else {
			//	Fill tpk file
			param.tpk = oURL.pathname;
			return bPromise.resolve( param );
		}
	}

	function iUntarPkg( param ) {
		log.verbose( 'PackageManager', 'Untar package: url=%s', param.url );
		return tvdutil.untarFile( getTemporaryPath(), param.tpk )
			.then(function(outDir) {
				param.tempPkgPath = outDir;
				return param;
			});
	}

	function iRemoveTemporaryTpk( param ) {
		//	If tpk in temporary directory
		log.verbose( 'PackageManager', 'Remove temporary tpk: url=%s, tpk=%s', param.url, param.tpk );
		if (param.tpk && param.tpk.indexOf( getTemporaryPath() ) >= 0) {
			return fs.unlinkAsync( param.tpk ).then(function() {
				param.tpk = null;
				return param;
			});
		}
		else {
			return bPromise.resolve(param);
		}
	}

	function iCheckPkg( param ) {
		log.verbose( 'PackageManager', 'Check package manifest: url=%s', param.url );
		var manifest = path.join(param.tempPkgPath,manifest_file_name);
		return fs.readFileAsync( manifest ).then(JSON.parse).then(function(data) {
			if (!checkManifest( data )) {
				throw tvdutil.warnErr( 'PackageManager', 'Package manifest invalid' );
			}
			else {
				return validateManifest( data )
					.then(function() {
						param.id = data.id;
						return param;
					}, function(err) {
						throw err;
					});
			}
		});
	}

	function iCheckAlreadyInstalled( param ) {
		log.verbose( 'PackageManager', 'Check if package already installed: id=%s', param.id );
		return new bPromise(function(resolve,reject) {
			if (param.pkgMgr.hasPackage( param.id )) {
				reject( tvdutil.warnErr( 'PackageManager', 'Package already installed: id=%s', param.id ) );
			}
			else {
				resolve(param);
			}
		});
	}

	function iInstallPackage( param ) {
		log.verbose( 'PackageManager', 'Install package from temporary directory to install directory: id=%s', param.id );
		var newPath = getInstallPathForNewPackage( param.id );
		var moveFnc = function() {
			return fs.renameAsync( param.tempPkgPath, newPath )
				.then(function() {
					param.tempPkgPath = null;
					param.installPath = newPath;
					return param;
				});
		};
		return tvdutil.removeTreeAsync( newPath )
			.then(moveFnc)
			.catch(function() {
				return moveFnc();
			});
	}

	function uBackupPackage( param ) {
		log.verbose( 'PackageManager', 'Backup package to temporary directory: id=%s', param.id );
		var oldPackagePath = getInstallPathForNewPackage( param.id );
		var backupPackagePath = oldPackagePath + '.old';
		return fs.renameAsync( oldPackagePath, backupPackagePath )
			.then(function() {
				param.backupPackage = backupPackagePath;
				return param;
			});
	}

	function uCleanBackup( param ) {
		if (param.backupPackage) {
			var prom;

			if (param.installPath) {
				log.verbose( 'PackageManager', 'Remove backup package: id=%s', param.id );
				prom = tvdutil.removeTreeAsync( param.backupPackage );
			}
			else {
				log.verbose( 'PackageManager', 'Restore package from backup: id=%s', param.id );
				var newPath = getInstallPathForNewPackage( param.id );
				prom = fs.renameAsync( param.backupPackage, newPath );
			}

			return prom.then(function() {
				param.backupPackage = null;
				return param;
			});
		}
		else {
			return bPromise.resolve(param);
		}
	}

	function uRemoveFromCache( param ) {
		log.verbose( 'PackageManager', 'Remove package from cache: id=%s', param.id );
		delete _cache[param.id];
		return param;
	}

	function uSignalUpdate( state, param ) {
		log.verbose( 'PackageManager', 'Signal update: id=%s, state=%d', param.id, state );
		if (state) {
			//	Force service/appmgr to stop service/application
			return tvdutil.emitThen( self, 'update', param.id )
				.return( param );
		}
		else {
			self.emit( 'updated', param.id );
			return param;
		}
	}

	function iAddPackage( param ) {
		log.verbose( 'PackageManager', 'Add package to database: id=%s', param.id );
		//	Add to array
		_packages[param.id] = param.installPath;
		return updateDB().then(function() {
			return param;
		});
	}

	function iRemoveTemporaryDirectory( param ) {
		//	Remove temporary directory if exists
		log.verbose( 'PackageManager', 'Remove temporary directory: id=%s, dir=%s', param.id, param.tempPkgPath );
		if (param.tempPkgPath) {
			return tvdutil.removeTreeAsync(param.tempPkgPath).then(function() {
				param.tempPkgPath = null;
				return param;
			});
		}
		else {
			return bPromise.resolve(param);
		}
	}

	function iCleanup( param ) {
		return iRemoveTemporaryTpk(param).then(iRemoveTemporaryDirectory);
	}

	function lockDatabase( param ) {
		//	Wait for unlock
		return new bPromise(function(resolve) {
			log.silly( 'PackageManager', 'Wait for database: id=%s', param.id );
			_lockDatabaseCondition.lock(function() {
				log.verbose( 'PackageManager', 'Lock database: id=%s', param.id );
				resolve(param);
			});
		});
	}

	function unlockDatabase( param ) {
		log.verbose( 'PackageManager', 'Unlock database' );
		_lockDatabaseCondition.dec();
		return bPromise.resolve(param);
	}

	function uRemovePackageFromDB( param ) {
		log.verbose( 'PackageManager', 'Remove package from database: id=%s', param.pkg.id );
		//	Remove from array
		delete _packages[param.pkg.id];
		delete _cache[param.pkg.id];
		return updateDB().then(function() {
			return param;
		});
	}

	function uRemovePackageStorage( param ) {
		log.verbose( 'PackageManager', 'Remove package storage: id=%s', param.pkg.id );
		//	Remove install directory
		return tvdutil.removeTreeAsync( param.installPath ).then(function() {
			//	Remove config directory (only if no update)
			return tvdutil.removeTreeAsync( param.configPath ).then(function() {
				return param;
			});
		});
	}

	function init() {
		log.info( "PackageManager", "Init: firstBoot=%s", tac.isFirstBoot() );

		//	Check for ro path
		if (!tvdutil.isDirectory(ro)) {
			throw new RecoveryError(util.format("%s is not a directory", ro));
		}

		//	Check for rw path
		if (!tvdutil.isDirectory(rw)) {
			throw new RecoveryError(util.format("%s is not a directory", rw));
		}

		//	Create config directory
		tvdutil.ensureDir( getConfigPath() );
		//	Create rw packages directory
		tvdutil.ensureDir( getPackagesPath(rw) );
		//	Clean temporary directory
		tvdutil.cleanDir( getTemporaryPath() );

		//	Build package list
		var location = getDatabaseFile();
		var buildDB = tac.isFirstBoot() ? true : false;
		if (!buildDB) {
			try {
				_packages = JSON.parse(fs.readFileSync(location));
			} catch(error) {
				log.warn("PackageManager", "Error reading db: err=%s", error.message);
				buildDB = true;
			}
		}

		if (buildDB) {
			_packages = createPackageDB(location);
		}

		//	Setup lock database
		_lockDatabaseCondition = new tvdutil.BasicWaitCondition();
	}

	//	Public API
	assert(this);
	var self = this;
	EventEmitter.call(self);

	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	self.hasPackage = function(id) {
		return id in _packages;
	};

	self.get = function(id) {
		function throwError( installPath, msg ) {
			log.warn( 'PackageManager', msg );

			if (isReadOnly(installPath)) {
				throw new RecoveryError( msg );
			}
			else {
				//	Returns a package without components, and try update ...
				return {
					id: id,
					version: '0.0.0',
					components: []
				};
			}
		}

		if (self.hasPackage(id)) {
			if (!(id in _cache)) {
				var data = null;
				var installPath = getPackageInstallPath(id);

				try {
					//	Read manifest from disk
					data = JSON.parse(fs.readFileSync(path.join(installPath,manifest_file_name)));
				} catch(error) {
					data = throwError( installPath, util.format('PackageManager: cannot read package manifest. id=%s', id) );
				}

				//	Basic manifest checks
				if (!checkManifest( data )) {
					data = throwError( installPath, util.format('PackageManager: package manifest bad formated: id=%s', id) );
				}

				_cache[id] = createPackage(data);
			}
			return _cache[id];
		} else {
			log.warn('PackageManager', 'No Package found. id=%s', id);
			return undefined;
		}
	};

	self.getAll = function() {
		return _.keys(_packages);
	};

	self.getAllInfo = function() {
		return self.getAll().map(function(id) {
			return self.get(id);
		});
	};

	self.getComponents = function( pkgID, type ) {
		var pkgInfo = self.get(pkgID);
		if (pkgInfo) {
			if (type) {
				return pkgInfo.components.filter(function(comp) {
					return comp.type == type;
				});
			}
			else {
				return pkgInfo.components;
			}
		}
		return [];
	};

	self.install = function(options,cb) {
		assert(options.url);

		log.info('PackageManager', 'Install package: url=%s', options.url);

		//	Fill object with data to each steps
		var installParams = {
			'pkgMgr': self,
			'url': options.url,
			'md5': options.md5,
			'size': options.size
		};
		iDownloadPkg(installParams)
			.then(iUntarPkg)
			.then(iRemoveTemporaryTpk)
			.then(iCheckPkg)
			.then(lockDatabase)
			.then(iCheckAlreadyInstalled)
			.then(iInstallPackage)
			.then(iAddPackage)
			.then(unlockDatabase)
			.then(function(param) {
				self.emit( 'install', param.id );
				cb(undefined,param.id);
			}, function(err) {
				log.error( 'PackageManager', 'Instalation failed: err=%s', err.message );
				return unlockDatabase(installParams)
					.then(iCleanup)
					.finally(function() {
						cb(err);
					});
			});
	};
	self.install.isAsync = true;

	self.canUninstall = function(pkgID) {
		//	Get package
		var pkg = self.get(pkgID);
		if (!pkg) {
			log.silly( 'PackageManager', 'Package not found: id=%s', pkgID );
			return false;
		}

		//	Check if package
		if (isReadOnly( getPackageInstallPath(pkgID) )) {
			log.silly( 'PackageManager', 'Package is on read only filesystem: id=%s', pkgID );
			return false;
		}

		return pkg.system ? false : true;
	};

	self.uninstall = function(options,cb) {
		log.info('PackageManager', 'Uninstall package: id=%s', options.id );

		//	Get package
		var pkg = self.get(options.id);
		if (!pkg) {
			cb( tvdutil.warnErr( 'PackageManager', 'Package not found: id=%s', options.id ) );
			return;
		}

		//	Check if package is on ro fs and save install path
		var installPath = getPackageInstallPath(pkg.id);
		if (isReadOnly(installPath)) {
			cb( tvdutil.warnErr( 'PackageManager', 'Package is on read only filesystem') );
			return;
		}

		//	Check if package is a marked as system
		if (pkg.system) {
			cb( tvdutil.warnErr( 'PackageManager', 'Package is marked as system package') );
			return;
		}

		var uninstallParam = {
			'id': options.id,
			'pkg': pkg,
			'installPath': installPath,
			'configPath' : getPackageConfigPath(pkg.id)
		};

		//	Notify to depends
		tvdutil.emitThen( this, 'uninstall', pkg.id )
			.then(function() {
				log.verbose( 'PackageManager', 'Signal uninstall handled' );
				return uninstallParam;
			})
			.then(lockDatabase)
			.then(uRemovePackageFromDB)
			.then(uRemovePackageStorage)
			.then(unlockDatabase)
			.then(function() {
				cb();
			},function(err){
				unlockDatabase().finally(function() {
					cb(err);
				});
			});
	};
	self.uninstall.isAsync = true;

	self.update = function(options,cb) {
		log.info('PackageManager', 'Update package: id=%s, url=%s', options.id, options.url );
		assert(options.id);
		assert(options.url);

		//	Get package
		var pkg = self.get(options.id);
		if (!pkg) {
			cb( tvdutil.warnErr( 'PackageManager', 'Package not found: id=%s', options.id ) );
			return;
		}

		//	Check if package is on ro fs and save install path
		var installPath = getPackageInstallPath(pkg.id);
		if (isReadOnly(installPath)) {
			cb( tvdutil.warnErr( 'PackageManager', 'Package is on read only filesystem: id=%s', options.id ) );
			return;
		}

		//	Check update flag in manfiest
		if (!pkg.update) {
			cb( tvdutil.warnErr( 'PackageManager', 'Package cannot be updated: id=%s', options.id ) );
			return;
		}

		//	Fill object with data to each steps
		var updateOperation = {
			pkgMgr: self,
			id: options.id,
			url: options.url,
			md5: options.md5,
			size: options.size
		};
		iDownloadPkg(updateOperation)
			.then(iUntarPkg)
			.then(iRemoveTemporaryTpk)
			.then(iCheckPkg)
			.then(uSignalUpdate.bind(undefined,true))
			.then(uBackupPackage)
			.then(iInstallPackage)
			.then(uRemoveFromCache)
			.then(uCleanBackup)
			.then(uSignalUpdate.bind(undefined,false))
			.then(function(param) {
				cb(undefined,param.id);
			}, function(err) {
				log.error( 'PackageManager', 'Update failed: err=%s', err.message );
				return uCleanBackup(updateOperation)
					.then(iCleanup)
					.finally(function() {
						cb(err);
					});
			});
	};
	self.update.isAsync = true;

	//	Components API
	self.getInstallPath = function( compID ) {
		var pkg = getPackage(self, compID);
		if (pkg) {
			return joinPath( getPackageInstallPath( pkg.id ), arguments);
		}
		log.warn('PackageManager', 'Component not installed. id=%s', compID);
		return undefined;
	};

	self.getDataPath = function( compID ) {
		var pkg = getPackage(self, compID);
		if (pkg) {
			return joinPath( getPackageConfigPath( pkg.id ), arguments);
		}
		return undefined;
	};

	self.hasComponent = function( compID ) {
		var comp = getComponent( self, compID );
		return comp ? true : false;
	};

	self.getComponentInfo = function( compID ) {
		var comp = getComponent( self, compID );
		if (comp) {
			return _.cloneDeep(comp);
		} else {
			log.warn('PackageManager', 'No Component found. compID=%s', compID);
			return undefined;
		}
	};

	self.getAllComponentsInfo = function(type) {
		var comps = [];
		self.getAllInfo().forEach(function(pkgInfo) {
			pkgInfo.components.forEach(function(comp) {
				if (!type || comp.type == type) {
					comps.push( comp );
				}
			});
		});
		return _.cloneDeep(_.flatten(comps));
	};

	self.getWebAPI = function() {
		return {
			'name': 'pkgmgr',
			'public': ['getAll','hasPackage','canUninstall','on'],
			'private': ['install','uninstall']
		};
	};

	return self;
}

util.inherits(PackageManager, EventEmitter);

// Exports
module.exports = PackageManager;

