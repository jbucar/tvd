'use strict';

var os = require('os');
var path = require('path');
var _ = require('lodash');
var tvdutil = require('tvdutil');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var mktemp = bPromise.promisifyAll( require('mktemp') );

var mockfs = require('mock-fs');
var chai = require("chai");
var assert = chai.assert;
chai.config.includeStack = true;
var Mocks = require('mocks');

var PackageManager = require('../src/packagemgr');


function mkdir(p) {
	try {
		fs.mkdirSync( p );
	} catch(err) {
		console.log( 'error en mkdir=%s', err.message );
	}
}

function rmDirTree(p) {
	if (fs.existsSync(p)) {
		fs.readdirSync(p).forEach(function(file,index) {
			var curPath = path.join(p,file);
			if(fs.lstatSync(curPath).isDirectory()) { // recurse
				rmDirTree(curPath);
			} else { // delete file
				fs.unlinkSync(curPath);
			}
		});
		fs.rmdirSync(p);
	}
}

describe('PackageManager', function() {

	function Component( id, cType ) {
		return {
			"id": id,
			"name": "name",
			"type": cType ? cType : "service",
			"version": "1.0.0",
			"main": "src/main.js"
		};
	}

	function CreateManifest( id, comps ) {
		return {
			"id": id,
			"version": "1.0.0",
			"manifest-version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "all",
			"components": comps
		};
	}

	function Package(name,man) {
		var self = {};
		self.name = name;

		if (man) {
			self.manifest = man;
		}
		else {
			self.manifest = new CreateManifest( name );
			self.manifest.components = [ new Component( name ) ];
		}

		self.install = {
			"manifest.json": JSON.stringify(self.manifest, null, 4)
		};

		self.config = {};

		self.update = function() {
			self.install = { "manifest.json": JSON.stringify(self.manifest, null, 4) };
		};

		return self;
	}

	function mkDeploy(root,cb) {
		var self = {};
		var _pkgs = {};

		self.getSystemPath = function() {
			return path.join(self.rw, 'config', 'ar.edu.unlp.info.lifia.tvd.system');
		};

		self.init = function() {
			tvdutil.ensureDir( path.join(self.rw, 'config') );
			tvdutil.ensureDir( self.getSystemPath() );
			tvdutil.ensureDir( path.join(self.rw, 'packages') );
		};

		self.fin = function() {
			rmDirTree( self.rootfs );
		};

		self.installPackagePath = function(name,ro) {
			var base = ro ? self.ro : self.rw;
			return path.join(base,'packages',name);
		};

		self.installPath = function(pkg,ro) {
			//console.log( 'install: base=%s, name=%s', base, pkg.name );
			return self.installPackagePath( pkg.name, ro );
		};

		self.getDatabaseFile = function() {
			return path.join( self.getSystemPath(), 'packages.json' );
		}

		self.writeConfig = function() {
			//console.log( 'writeConfig: db=%s, _pkgs=%j', self.getDatabaseFile(), _pkgs );
			fs.writeFileSync( self.getDatabaseFile(), JSON.stringify(_pkgs, null, 4) );
		};

		self.readConfig = function() {
			return fs.readFileSync( self.getDatabaseFile(), {encoding: 'utf8'} );
		};

		self.install = function( pkg, ro, noUpdateDB ) {
			fs.mkdirSync( self.installPath(pkg,ro) );

			Object.keys(pkg.install).forEach(function(file) {
				fs.writeFileSync( path.join(self.installPath(pkg,ro),file), pkg.install[file] );
				//console.log( 'file=%s, data=%s', file, path.join(self.installPath(pkg,ro),file), pkg.install[file] );
			});

			if (!noUpdateDB) {
				_pkgs[pkg.name] = self.installPath(pkg,ro);
				self.writeConfig();
				//console.log( '_pkgs=%j', _pkgs );
			}
		};

		if (!root) {
			root = os.tmpdir();
		}
		return mktemp.createDirAsync( path.join(root,'pkgmgr.XXXXXXXX') )
			.then(function(tmp) {
				self.rootfs = tmp;
			})
			.then(function() {
				self.ro = path.join(self.rootfs,'original');
				return fs.mkdirAsync( self.ro );
			})
			.then(function() {
				var p = path.join(self.ro,'packages');
				return fs.mkdirAsync( p );
			})
			.then(function() {
				self.rw = path.join(self.rootfs,'data');
				return fs.mkdirAsync( self.rw )
			})
			.then(function() {
				cb( self );
			});
	}

	function mkMockDeploy(cb) {
		var sfs = {
			'/mockFS': {}
		};
		mockfs(sfs);
		mkDeploy( '/mockFS', cb );
	}

	beforeEach(function() {
		Mocks.init('silly');
	});

	afterEach(function() {
		Mocks.fin();
	});

	describe('mock', function() {
		var deploy = null;

		beforeEach(function(done) {
			mkMockDeploy(function(d) {
				deploy = d;
				done();
			});
		});

		afterEach(function() {
			mockfs.restore();
		});

		describe( 'constructor', function() {
			it('should create a packages directory when it not exists', function() {
				var pkg = new PackageManager( deploy.ro, deploy.rw );
				assert.isTrue(fs.existsSync(path.join(deploy.rw,'packages')) );
				assert.isTrue(fs.existsSync(path.join(deploy.rw,'config')) );
				assert.isTrue(fs.existsSync(deploy.getDatabaseFile()));
			});

			it('should empty tmp directory when it begin', function() {
				deploy.init();
				var dir = path.join(deploy.getSystemPath(), 'tmp' );
				fs.mkdirSync( dir );
				fs.writeFileSync( path.join(dir,'pepe.conf'), "pepe" );
				var pkg = new PackageManager( deploy.ro, deploy.rw );
				var files = fs.readdirSync( dir );
				assert.equal( files.length, 0 );
			});

			it('should setup a global pkg manager', function() {
				var pkg = new PackageManager( deploy.ro, deploy.rw );
				assert.isObject( pkg );
			});

			it('should throw if try to create pkg manager without using new', function() {
				var fn = function() { return PackageManager(deploy.ro, deploy.rw); };
				assert.throw( fn, /undefined == true/ );
			});

			it('should throw when one argument is missing', function() {
				var fn = function() { return new PackageManager(deploy.ro); };
				assert.throw(fn, RecoveryError);
			});

			it('should throw ro is not a path', function() {
				var fn = function() { return new PackageManager('/badpath', deploy.rw); };
				assert.throw(fn, RecoveryError);
			});

			it('should throw rw is not a path', function() {
				var fn = function() { return new PackageManager(deploy.ro, '/badpath'); };
				assert.throw(fn, RecoveryError);
			});

			it('should get the config from rw when config exists', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, true );

				var pkg2 = new Package( 'pkg2' );
				deploy.install( pkg2, false );

				var pkg = new PackageManager(deploy.ro, deploy.rw);
				assert.isTrue( pkg.hasPackage( 'pkg1' ) );
				assert.isTrue( pkg.hasPackage( 'pkg2') );
			});

			it('should create a packages.json file when it not exists', function() {
				var pkg = new PackageManager(deploy.ro,deploy.rw);
				assert.isTrue(fs.existsSync(deploy.getDatabaseFile()), 'Should exists the packages.json');
			});

			it('should re-create a packages.json file when is first boot', function() {
				tac.firstBoot = true;

				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, true );

				var pkg2 = new Package( 'pkg2' );
				deploy.install( pkg2, true, true );

				var pkg = new PackageManager(deploy.ro, deploy.rw);
				assert.isTrue( pkg.hasPackage( 'pkg1' ) );
				assert.isTrue( pkg.hasPackage( 'pkg2') );
			});

			it('should write the list of installed packages in the packages.json file when it not exists', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, true );

				var pkg2 = new Package( 'pkg2' );
				deploy.install( pkg2, false );

				fs.unlinkSync( deploy.getDatabaseFile() );

				var pkg = new PackageManager(deploy.ro,deploy.rw);

				var result = {
					"pkg1": deploy.installPath(pkg1,true),
					"pkg2": deploy.installPath(pkg2,false)
				};
				assert.equal( deploy.readConfig(), JSON.stringify(result, null, 4) );
			});
		});

		it('api', function() {
			var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
			assert.isFunction( pkgMgr.getWebAPI );
			var exports = pkgMgr.getWebAPI();
			assert.isObject( exports );
			assert.isTrue( exports.name === 'pkgmgr' );
			assert.deepEqual( exports['public'], ['getAll','hasPackage','canUninstall','on'] );
			assert.deepEqual( exports['private'], ['install','uninstall'] );
			assert.isFunction( pkgMgr.getAll );
			assert.isFunction( pkgMgr.install );
			assert.isTrue( pkgMgr.install.isAsync );
			assert.isFunction( pkgMgr.uninstall );
			assert.isTrue( pkgMgr.uninstall.isAsync );
			assert.isFunction( pkgMgr.hasPackage );
			assert.isUndefined( pkgMgr.hasPackage.isAsync );
			assert.isFunction( pkgMgr.on );
			assert.isUndefined( pkgMgr.on.isAsync );
		});

		describe('hasPackage(id)', function() {
			it('should return false when there is no packages', function() {
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isFalse(pkgMgr.hasPackage('package.id'));
			});

			it('should return true when the package exists', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isTrue(pkgMgr.hasPackage('pkg1'));
			});
		});

		describe('getComponents(pkgID,type)', function() {
			it('should return the component from valid packageID', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );
				var comp1 = _.cloneDeep(pkg1.manifest.components[0]);
				comp1.pkgID = pkg1.name;

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isTrue(pkgMgr.hasPackage('pkg1'));

				var comps = pkgMgr.getComponents('pkg1');
				assert.isArray( comps );
				assert.isTrue( comps.length === 1 );
				assert.deepEqual( comps, [comp1] );
			});

			it('should return all components from valid packageID', function() {
				deploy.init();

				var man = CreateManifest('pkg1')
				man.components = [ new Component('comp1'), new Component('comp2') ];
				var pkg1 = new Package( 'pkg1', man );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isTrue(pkgMgr.hasPackage('pkg1'));

				var comps = pkgMgr.getComponents('pkg1');
				assert.isArray( comps );
				assert.isTrue( comps.length === 2 );

				var comp1 = _.cloneDeep(pkg1.manifest.components[0]);
				comp1.pkgID = pkg1.name;

				var comp2 = _.cloneDeep(pkg1.manifest.components[1]);
				comp2.pkgID = pkg1.name;

				assert.deepEqual( comps, [comp1,comp2] );
			});

			it('should return all apps components from valid packageID', function() {
				deploy.init();

				var man = CreateManifest('pkg1')
				man.components = [ new Component('comp1'), new Component('comp2', 'app') ];
				var pkg1 = new Package( 'pkg1', man );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isTrue(pkgMgr.hasPackage('pkg1'));

				var comps = pkgMgr.getComponents('pkg1', 'app');
				assert.isArray( comps );
				assert.isTrue( comps.length === 1 );

				var comp2 = _.cloneDeep(pkg1.manifest.components[1]);
				comp2.pkgID = pkg1.name;

				assert.deepEqual( comps, [comp2] );
			});

			it('should return all services components from valid packageID', function() {
				deploy.init();

				var man = CreateManifest('pkg1')
				man.components = [ new Component('comp1'), new Component('comp2', 'app') ];
				var pkg1 = new Package( 'pkg1', man );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isTrue(pkgMgr.hasPackage('pkg1'));

				var comps = pkgMgr.getComponents('pkg1', 'service');
				assert.isArray( comps );
				assert.isTrue( comps.length === 1 );

				var comp1 = _.cloneDeep(pkg1.manifest.components[0]);
				comp1.pkgID = pkg1.name;

				assert.deepEqual( comps, [comp1] );
			});

			it('should return empty if pkgID is invalid', function() {
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				var comps = pkgMgr.getComponents('package.pepe');
				assert.isArray( comps );
				assert.isTrue( comps.length === 0 );
			});
		});

		describe('hasComponent(id)', function() {
			it('should return false when there is no component with such id', function() {
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isFalse(pkgMgr.hasComponent('package.id.nocomp'));
			});

			it('should return true when there is a component with such id', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isTrue(pkgMgr.hasComponent('pkg1'));
			});

			it('should return false when there is no component with such id, but there is a package with that id', function() {
				deploy.init();

				var man = CreateManifest('pkg1')
				man.components = [ new Component('comp1'), new Component('comp2', 'app') ];
				var pkg1 = new Package( 'pkg1', man );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isTrue(pkgMgr.hasPackage('pkg1'));

				assert.isFalse(pkgMgr.hasComponent('pkg1'));
			});
		});

		describe('getDataPath(id, ...)', function() {
			it('should return only if id is a component', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				assert.equal(pkgMgr.getDataPath('comp1'), undefined);
			});

			it('should return the configuration path', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				assert.equal(pkgMgr.getDataPath('pkg1'), path.join(deploy.rw,'config/pkg1'));
			});

			it('should return the configuration path with the rest of the arguments appended', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				assert.equal(pkgMgr.getDataPath('pkg1', 'path', 'to'), path.join(deploy.rw,'config','pkg1','path','to'));
			});
		});

		describe('getInstallPath(id, ...)', function() {
			it('should return the install path when the component\'s package is installed in rw', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				assert.equal(pkgMgr.getInstallPath('pkg1'), path.join(deploy.rw,'packages','pkg1') );
			});

			it('should return the install path with the rest of the arguments appended when the component\'s package is installed in rw', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				assert.equal(pkgMgr.getInstallPath('pkg1', 'path', 'to'), path.join(deploy.rw,'packages','pkg1','path', 'to'));
			});

			it('should return the install path when the component\'s package is installed in ro', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, true );
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				assert.equal(pkgMgr.getInstallPath('pkg1'), path.join(deploy.ro,'packages', 'pkg1'));
			});

			it('should return undefined when the component is not installed', function() {
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isUndefined(pkgMgr.getInstallPath('not_exists'));
			});
		});

		describe('platform', function() {
			it('should validate empty platform', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.platform = null;
				pkg1.update();
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var fnc = function() { var pkg = pkgMgr.get('pkg1'); };
				assert.throw(fnc,RecoveryError);
			});

			it('should validate invalid type of platform field', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.platform = 10;
				pkg1.update();
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var fnc = function() { var pkg = pkgMgr.get('pkg1'); };
				assert.throw(fnc,RecoveryError);
			});

			it('should validate platform case 1', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.platform = 'all';
				pkg1.update();
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.isObject(pkg);
				assert.propertyVal(pkg, 'id', 'pkg1');
				assert.propertyVal(pkg, 'platform', 'all');
			});

			it('should validate platform case 2', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.platform = 'All';
				pkg1.update();
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.isObject(pkg);
				assert.propertyVal(pkg, 'id', 'pkg1');
				assert.propertyVal(pkg, 'platform', 'all');
			});

			it('should validate platform case3', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.platform = 'All';
				pkg1.update();
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.isObject(pkg);
				assert.propertyVal(pkg, 'id', 'pkg1');
				assert.propertyVal(pkg, 'platform', 'all');
			});

			it('should validate platform case3', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.platform = 'aml_isdbt_1';
				pkg1.update();
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.isObject(pkg);
				assert.propertyVal(pkg, 'id', 'pkg1');
				assert.propertyVal(pkg, 'platform', 'aml_isdbt_1');
			});

			it('should validate same platform', function() {
				deploy.init();

				tac.platform.name = 'AML_ISDBT_1';
				tac.platform.version = '1.0.296';
				tac.platform.commit = 'g989fdf2';

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.platform = 'aml_isdbt_1';
				pkg1.update();
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.isObject(pkg);
				assert.propertyVal(pkg, 'id', 'pkg1');
				assert.propertyVal(pkg, 'platform', 'aml_isdbt_1');
			});
		});

		describe('get(id)', function() {
			it('should return a package info when the package with the id exists (update must return false)', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.update = true;
				pkg1.update();
				deploy.install( pkg1, true );
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.isObject(pkg);
				assert.propertyVal(pkg, 'id', 'pkg1');
				assert.propertyVal(pkg, 'update', false);	//	Is on ro filesystem -> upadte=false
			});

			it('should return a package info when the package with the id exists (update must return true)', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.update = true;
				pkg1.update();
				deploy.install( pkg1, false );
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.isObject(pkg);
				assert.propertyVal(pkg, 'id', 'pkg1');
				assert.propertyVal(pkg, 'update', true);
			});

			it('should return a component info when the component exists and has the same name as the package', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, true );
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.isObject(pkg);
				assert.propertyVal(pkg, 'id', 'pkg1');
			});

			it('should return a package info from the cache if the package was previously query', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, true );
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.deepEqual(pkgMgr.get('pkg1'), pkg);
			});

			it('should throw recovery error if package is corrupted in ro fs', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1', {} );
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				var fnc = function() { var pkg = pkgMgr.get('pkg1'); }
				assert.throw(fnc,RecoveryError);
			});

			it('should return a valid package without components if package is corrupted in rw fs', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1', {} );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.deepEqual( pkg, {id:'pkg1', components: [], platform: 'all', update: true, version: '0.0.0'} );
			});

			it('should throw recovery error if cannot read the package manifest on ro fs', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1', {} );
				pkg1.install = {};
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var fnc = function() { var pkg = pkgMgr.get('pkg1'); };
				assert.throw(fnc,RecoveryError, 'PackageManager: cannot read package manifest. id=pkg');
			});

			it('should return a package without components if cannot read the package manifest from rw fs', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1', {} );
				pkg1.install = {};
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.deepEqual( pkg, {id:'pkg1', components: [], platform: 'all', update: true, version: '0.0.0'} );
			});

			it('should throw recovery error if the package has not version on ro fs', function() {
				deploy.init();

				var man = new CreateManifest( 'pkg1' );
				delete man.version;
				man.components = [ new Component( 'comp1' ) ];
				delete man.components[0].version;

				var pkg1 = new Package( 'pkg1', man );
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var fnc = function() { var pkg = pkgMgr.get('pkg1'); }
				assert.throw(fnc,RecoveryError);
			});

			it('should return a valid package without manifest if the package has not version on rw fs', function() {
				deploy.init();

				var man = new CreateManifest( 'pkg1' );
				delete man.version;
				man.components = [ new Component( 'comp1' ) ];
				delete man.components[0].version;

				var pkg1 = new Package( 'pkg1', man );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.deepEqual( pkg, {id:'pkg1', components: [], platform: 'all', update: true, version: '0.0.0'} );
			});

			it('should return a pkg with manifest-version when the package manifest define a manifest-version', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var pkg = pkgMgr.get('pkg1');
				assert.isObject(pkg);
				assert.propertyVal(pkg, 'id', 'pkg1');
				assert.propertyVal(pkg, 'manifest-version', '1.0.0');
			});

			it('should return undefined when the package with the id does not exist', function() {
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isUndefined(pkgMgr.get('noid'));
			});
		});

		describe('getComponentInfo(id)', function() {
			it('should return a component info when the component exists', function() {
				deploy.init();

				var man = CreateManifest('pkg1');
				man.components = [ new Component('comp1') ];
				var pkg1 = new Package( 'pkg1', man );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var component = pkgMgr.getComponentInfo('comp1');
				assert.isObject(component);
				assert.propertyVal(component, 'id', 'comp1');
				assert.propertyVal(component, 'pkgID', 'pkg1');
			});

			it('should return a component info when the component exists and has the same name as the package', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var component = pkgMgr.getComponentInfo('pkg1');
				assert.isObject(component);
				assert.propertyVal(component, 'id', 'pkg1');
				assert.propertyVal(component, 'pkgID', 'pkg1');
			});

			it('should return null if the component not exists', function() {
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isUndefined(pkgMgr.getComponentInfo('nocomp'));
			});
		});

		describe('getAllComponentsInfo', function() {
			it('should return all the component infos of all packages in the system', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );

				var comp1 = _.cloneDeep(pkg1.manifest.components[0]);
				comp1.pkgID = 'pkg1';

				var pkg2 = new Package( 'pkg2' );
				deploy.install( pkg2, false );

				var comp2 = _.cloneDeep(pkg2.manifest.components[0]);
				comp2.pkgID = 'pkg2';

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var comps = pkgMgr.getAllComponentsInfo();
				assert.equal( comps.length, 2 );
				assert.deepEqual( comps, [comp1,comp2] );
			});

			it('should return all components with type==app && type=service in the system', function() {
				deploy.init();

				var man1 = CreateManifest('pkg1');
				man1.components = [ new Component('comp1', 'service'), new Component('comp1', 'app') ];
				var pkg1 = new Package( 'pkg1', man1 );
				deploy.install( pkg1, false );

				var comp0 = _.cloneDeep(man1.components[0]);
				comp0.pkgID = 'pkg1';

				var comp1 = _.cloneDeep(man1.components[1]);
				comp1.pkgID = 'pkg1';

				var man2 = CreateManifest('pkg2');
				man2.components = [ new Component('comp1', 'service'), new Component('comp1', 'app') ];
				var pkg2 = new Package( 'pkg2', man2 );
				deploy.install( pkg2, false );

				var comp2 = _.cloneDeep(man2.components[0]);
				comp2.pkgID = 'pkg2';

				var comp3 = _.cloneDeep(man2.components[1]);
				comp3.pkgID = 'pkg2';

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var apps = pkgMgr.getAllComponentsInfo('app');
				assert.deepEqual( apps, [comp1,comp3] );

				var srvs = pkgMgr.getAllComponentsInfo('service');
				assert.deepEqual( srvs, [comp0,comp2] );
			});

			it('should return empty if type not found', function() {
				deploy.init();
				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var components = pkgMgr.getAllComponentsInfo('binary');
				assert.lengthOf( components, 0 );
			});

			it('should return an empty array when there is no packages in the system', function() {
				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.lengthOf(pkgMgr.getAllComponentsInfo(), 0);
			});
		});

		describe('getAll()', function() {
			it('should return all the packageinfo of all packages in the system', function() {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				var pkgs = pkgMgr.getAll();
				assert.isArray(pkgs);
				assert.lengthOf(pkgs, 1);
			});
		});

		describe( 'uninstall()', function() {
			it('should fail if package is installed in ro', function(done) {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				var uninstallPackageCalled=0;
				pkgMgr.on( 'uninstall', function() {
					uninstallPackageCalled++;
				});

				var uopts = {
					"id": 'pkg1'
				};
				assert.isFalse( pkgMgr.canUninstall(uopts.id) );
				pkgMgr.uninstall( uopts, function(err) {
					assert.isTrue( err !== undefined );
					assert.isTrue( err.message === 'Package is on read only filesystem' );
					assert.isTrue( uninstallPackageCalled === 0 );
					done();
				});
			});

			it('should fail if package is a system package', function(done) {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.system = true;
				pkg1.update();
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var uninstallPackageCalled=0;
				pkgMgr.on( 'uninstall', function() {
					uninstallPackageCalled++;
				});

				var uopts = {
					"id": 'pkg1'
				};
				assert.isFalse( pkgMgr.canUninstall(uopts.id) );
				pkgMgr.uninstall( uopts, function(err) {
					assert.isTrue( err !== undefined );
					assert.isTrue( err.message === 'Package is marked as system package' );
					assert.isTrue( uninstallPackageCalled === 0 );
					done();
				});
			});

			it('should fail if signal fail', function(done) {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );

				var uninstallPackageCalled=0;
				var fncUninstall = function(id,cb) {
					uninstallPackageCalled++;
					cb(new Error('pepe'));
				};
				fncUninstall.isAsync = true;
				pkgMgr.on( 'uninstall', fncUninstall );

				var uopts = {
					"id": 'pkg1',
					"update": true
				};
				assert.isTrue( pkgMgr.canUninstall(uopts.id) );
				pkgMgr.uninstall( uopts, function(err) {
					assert.isTrue( err !== undefined );
					assert.equal( err.message, 'pepe' );
					assert.isTrue( uninstallPackageCalled === 1 );
					done();
				});
			});
		});

		describe('update', function() {
			it('should fail if cant update package', function(done) {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				pkg1.manifest.update = false;
				pkg1.update();
				deploy.install( pkg1, false );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isTrue( pkgMgr.hasPackage('pkg1') );

				//	Begin update
				var uopts = {
					"id": 'pkg1',
					"url": path.join(__dirname,'test_app1_2.tpk')
				};
				pkgMgr.update( uopts, function(unErr) {
					assert.equal( unErr.message, 'Package cannot be updated: id=pkg1' );
					assert.isTrue( pkgMgr.hasPackage('pkg1') );
					done();
				});
			});

			it('should fail if package is on ro fs', function(done) {
				deploy.init();

				var pkg1 = new Package( 'pkg1' );
				deploy.install( pkg1, true );

				var pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				assert.isTrue( pkgMgr.hasPackage('pkg1') );

				//	Begin update
				var uopts = {
					"id": 'pkg1',
					"url": path.join(__dirname,'test_app1_2.tpk')
				};
				pkgMgr.update( uopts, function(unErr) {
					assert.equal( unErr.message, 'Package is on read only filesystem: id=pkg1' );
					assert.isTrue( pkgMgr.hasPackage('pkg1') );
					done();
				});
			});
		});
	});

	describe('no mocked methods', function() {
		var deploy = null;
		var pkgMgr = null;
		var installPackageCalled=0;
		var uninstallPackageCalled=0;

		function getTempFiles() {
			return fs.readdirSync(path.join(deploy.getSystemPath(), 'tmp' ));
		}

		function checkDB( name ) {
			var pkgs=deploy.readConfig();
			var result = {};
			result[name] = path.join(deploy.rw,'packages',name);
			assert.deepEqual( pkgs, JSON.stringify(result, null, 4) );
		}

		function checkPackage( name ) {
			assert.isFalse( tvdutil.isDirectory(path.join(deploy.rw,'packages',name)) );
			assert.isFalse( tvdutil.isDirectory(path.join(deploy.rw,'config',name)) );
		}

		beforeEach(function(done) {
			mkDeploy( null, function(d) {
				installPackageCalled = 0;
				uninstallPackageCalled = 0;
				deploy = d;

				pkgMgr = new PackageManager( deploy.ro, deploy.rw );
				pkgMgr.
					on( 'install', function() {
						installPackageCalled++;
					}).
					on( 'uninstall', function() {
						uninstallPackageCalled++;
					});

				done();
			});
		});

		afterEach(function() {
			deploy.fin();
		});

		describe('install(options,cb)', function() {
			it('should not install with invalid size', function(done) {
				var opts = {
					"url": 'http://172.16.0.206/tac_testcases/test_app1.tpk',
					"md5": 'be1303e467b240bae5883c060ce2f880'
				};
				pkgMgr.install(opts, function(err,pkgID) {
					assert.isDefined( err );
					assert.strictEqual( err.message,  'Invalid md5/size' );
					assert.isTrue( installPackageCalled === 0 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				});
			});

			it('should not install with invalid size', function(done) {
				var opts = {
					"url": 'http://172.16.0.206/tac_testcases/test_app1.tpk',
					"md5": 'be1303e467b240bae5883c060ce2f880'
				};
				pkgMgr.install(opts, function(err,pkgID) {
					assert.isDefined( err );
					assert.strictEqual( err.message,  'Invalid md5/size' );
					assert.isTrue( installPackageCalled === 0 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				});
			});

			it('should not check size if inDevelopmentMode', function(done) {
				tac.enableDevelopmentMode(true);
				var opts = {
					"url": 'http://172.16.0.206/tac_testcases/test_app1.tpk',
					"md5": 'be1303e467b240bae5883c060ce2f880'
				};
				pkgMgr.install(opts, function(err,pkgID) {
					assert.isUndefined( err );
					assert.strictEqual( pkgID, 'ar.edu.unlp.info.lifia.tvd.html5_test' );
					assert.strictEqual( installPackageCalled, 1 );
					checkDB( "ar.edu.unlp.info.lifia.tvd.html5_test" );
					tac.enableDevelopmentMode(false);
					done();
				});
			});

			it('should install with local file', function(done) {
				var fnc = function(err,pkgID) {
					assert.isTrue( err === undefined );
					assert.isTrue( pkgID === 'ar.edu.unlp.info.lifia.tvd.html5_test' );
					assert.isTrue( installPackageCalled === 1 );
					checkDB( "ar.edu.unlp.info.lifia.tvd.html5_test" );
					done();
				}
				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fnc );
			});

			it('should install remote package', function(done) {
				var fnc = function(err,pkgID) {
					assert.isTrue( err === undefined );
					assert.isTrue( pkgID === 'ar.edu.unlp.info.lifia.tvd.html5_test' );
					assert.isTrue( installPackageCalled === 1 );
					checkDB( "ar.edu.unlp.info.lifia.tvd.html5_test" );
					done();
				}
				var opts = {
					"url": 'http://172.16.0.206/tac_testcases/test_app1.tpk',
					"md5": 'be1303e467b240bae5883c060ce2f880',
					"size": 317
				};
				pkgMgr.install( opts, fnc );
			});

			it('should install with local via url', function(done) {
				var name = 'file://' + path.join(__dirname,'test_app1.tpk');
				var opts = {
					"url": name
				};
				pkgMgr.install( opts, function(err,pkgID) {
					assert.isTrue( err === undefined );
					assert.isTrue( pkgID === 'ar.edu.unlp.info.lifia.tvd.html5_test' );
					assert.isTrue( installPackageCalled === 1 );
					checkDB( "ar.edu.unlp.info.lifia.tvd.html5_test" );
					done();
				});
			});

			it('should fail if package already exists', function(done) {
				var fnc = function(err,pkgID) {
					assert.isTrue( err === undefined );
					assert.isTrue( pkgID === 'ar.edu.unlp.info.lifia.tvd.html5_test' );
					assert.isTrue( installPackageCalled === 1 );
					checkDB( "ar.edu.unlp.info.lifia.tvd.html5_test" );

					var opts2 = {
						"url": path.join(__dirname,'test_app1.tpk')
					};
					pkgMgr.install( opts2, function(err2) {
						assert.isTrue( err2 !== undefined );
						assert.isTrue( err2.message === 'Package already installed: id=ar.edu.unlp.info.lifia.tvd.html5_test' );
						assert.strictEqual( getTempFiles().length, 0 );
						checkDB( "ar.edu.unlp.info.lifia.tvd.html5_test" );
						done();
					});
				}
				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fnc );
			});

			it('should install package if package directory was dirty', function(done) {
				var fnc = function(err,pkgID) {
					assert.isTrue( err === undefined );
					assert.isTrue( pkgID === 'ar.edu.unlp.info.lifia.tvd.html5_test' );
					assert.isTrue( installPackageCalled === 1 );
					checkDB( "ar.edu.unlp.info.lifia.tvd.html5_test" );
					done();
				}

				var ip = path.join( deploy.rw, 'packages/ar.edu.unlp.info.lifia.tvd.html5_test' );
				fs.mkdirSync( ip );
				fs.writeFileSync( path.join(ip, 'manifest.json'), JSON.stringify('{pepe:"pepe"}', null, 4) );

				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fnc );
			});

			it('should not install with invalid url', function(done) {
				var fnc = function(err,pkgID) {
					assert.isFalse( err === undefined );
					assert.isTrue( err.message === 'Invalid URL: protocol=pepe:' );
					assert.isTrue( installPackageCalled === 0 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				}
				var opts = {
					"url": 'pepe://alguna_cosa'
				};
				pkgMgr.install( opts, fnc );
			});

			it('should check for invalid size', function(done) {
				var fnc = function(err,pkgID) {
					assert.isFalse( err === undefined );
					assert.isTrue( err.message === 'Downloaded file differ in size' );
					assert.isTrue( installPackageCalled === 0 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				}
				var opts = {
					"url": 'http://172.16.0.206/tac_testcases/test_app1.tpk',
					"md5": 'be1303e467b240bae5883c060ce2f880',
					"size": 0
				};
				pkgMgr.install( opts, fnc );
			});

			it('should check for invalid size 2', function(done) {
				var fnc = function(err,pkgID) {
					assert.isFalse( err === undefined );
					assert.isTrue( err.message === 'Downloaded file differ in size' );
					assert.isTrue( installPackageCalled === 0 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				}
				var opts = {
					"url": 'http://172.16.0.206/tac_testcases/test_app1.tpk',
					"md5": 'be1303e467b240bae5883c060ce2f880',
					"size": 20
				};
				pkgMgr.install( opts, fnc );
			});

			it('should check signature if present', function(done) {
				var fnc = function(err,pkgID) {
					assert.isTrue( err === undefined );
					assert.isTrue( installPackageCalled === 1 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				}
				var opts = {
					"url": 'http://172.16.0.206/tac_testcases/test_app1.tpk',
					"md5": 'be1303e467b240bae5883c060ce2f880',
					"size": 317
				};
				pkgMgr.install( opts, fnc );
			});

			it('should handle error if check signature fail', function(done) {
				var fnc = function(err,pkgID) {
					assert.isFalse( err === undefined );
					assert.isTrue( err.message === 'Signature error' );
					assert.isTrue( installPackageCalled === 0 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				}
				var opts = {
					"url": 'http://172.16.0.206/tac_testcases/test_app1.tpk',
					"md5": 'be1303e467b240bae5883c060ce2f881',
					"size": 317
				};
				pkgMgr.install( opts, fnc );
			});

			it('should handle error when test manifiest fail', function(done) {
				var fnc = function(err,pkgID) {
					assert.isFalse( err === undefined );
					assert.isTrue( err.message === 'Package manifest invalid' );
					assert.isTrue( installPackageCalled === 0 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				}
				var opts = {
					"url": path.join(__dirname,'test_app_invalid_manifest.tpk')
				};
				pkgMgr.install( opts, fnc );
			});

			it('should handle error when test manifiest fail2', function(done) {
				var fnc = function(err,pkgID) {
					assert.isFalse( err === undefined );
					assert.isTrue( err.name === 'ValidationError' );
					assert.isTrue( err.message.indexOf('Invalid type') != -1 );
					assert.isTrue( installPackageCalled === 0 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				}
				var opts = {
					"url": path.join(__dirname,'test_app_invalid_manifest1.tpk')
				};
				pkgMgr.install( opts, fnc );
			});

			it('should handle error when create temporary directory', function(done) {
				rmDirTree( path.join(deploy.rw,'/config/ar.edu.unlp.info.lifia.tvd.system/tmp') );

				var fnc = function(err,pkgID) {
					assert.isFalse( err === undefined );
					assert.isTrue( err.code === 'ENOENT' );
					assert.isTrue( err.isOperational );
					assert.isTrue( installPackageCalled === 0 );
					done();
				}

				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};

				pkgMgr.install( opts, fnc );
			});

			it('should handle error when untar invalid tpk file', function(done) {
				var fnc = function(err,pkgID) {
					assert.isFalse( err === undefined );
					assert.isTrue( err.message.indexOf( 'Error processing tool' ) >= 0 );
					assert.isTrue( installPackageCalled === 0 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				}

				var opts = {
					"url": path.join(__dirname,'test_invalid_app.tpk')
				};

				pkgMgr.install( opts, fnc );
			});

			it('should remove tpk file after untar', function(done) {
				var fnc = function(err,pkgID) {
					assert.isTrue( err === undefined );
					assert.isTrue( pkgID === 'ar.edu.unlp.info.lifia.tvd.html5_test' );
					assert.isTrue( installPackageCalled === 1 );
					checkDB( "ar.edu.unlp.info.lifia.tvd.html5_test" );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				}
				var opts = {
					"url": 'http://172.16.0.206/tac_testcases/test_app1.tpk',
					"md5": 'be1303e467b240bae5883c060ce2f880',
					"size": 317
				};
				pkgMgr.install( opts, fnc );
			});

			it('should handle error when manifiest in tpk not exists', function(done) {
				var fnc = function(err,pkgID) {
					assert.isFalse( err === undefined );
					assert.isTrue( err.code === 'ENOENT' );
					assert.isTrue( err.isOperational );
					assert.isTrue( installPackageCalled === 0 );
					assert.strictEqual( getTempFiles().length, 0 );
					done();
				}
				var opts = {
					"url": path.join(__dirname,'test_invalid_app2.tpk')
				};
				pkgMgr.install( opts, fnc );
			});
		});

		describe('uninstall(options,cb)', function() {
			it('should uninstall valid application', function(done) {
				var fnc = function(err,pkgID) {
					assert.isUndefined( err );
					assert.isTrue( pkgID === 'ar.edu.unlp.info.lifia.tvd.html5_test' );
					var uopts = {
						"id": pkgID
					};
					assert.isTrue( pkgMgr.canUninstall(pkgID) );
					pkgMgr.uninstall( uopts, function(unErr) {
						assert.isTrue( unErr === undefined );
						assert.isTrue( uninstallPackageCalled === 1 );
						checkPackage( 'ar.edu.unlp.info.lifia.tvd.html5_test' );
						done();
					});
				}
				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fnc );
			});

			it('should uninstall with a uninstall signal handler async', function(done) {
				var fnc = function(err,pkgID) {
					assert.isUndefined( err );
					assert.isTrue( pkgID === 'ar.edu.unlp.info.lifia.tvd.html5_test' );
					var listenerAsync = function(pkgID,cb) {
						setTimeout(function() {
							cb();
						}, 20);
					};
					listenerAsync.isAsync = true;

					pkgMgr.on('uninstall', listenerAsync );

					var uopts = {
						"id": pkgID
					};
					assert.isTrue( pkgMgr.canUninstall(pkgID) );
					pkgMgr.uninstall( uopts, function(unErr) {
						assert.isTrue( unErr === undefined );
						assert.isTrue( uninstallPackageCalled === 1 );
						checkPackage( 'ar.edu.unlp.info.lifia.tvd.html5_test' );
						done();
					});
				}
				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fnc );
			});

			it('should fail if package not exists', function(done) {
				var uopts = {
					"id": 'pepe'
				};
				assert.isFalse( pkgMgr.canUninstall(uopts.id) );
				pkgMgr.uninstall( uopts, function(err) {
					assert.isTrue( err !== undefined );
					assert.strictEqual( err.message, 'Package not found: id=pepe' );
					assert.isTrue( uninstallPackageCalled === 0 );
					done();
				});
			});
		});

		describe('update(options,cb)', function() {
			function backupPath() {
				return path.join( deploy.rw, 'packages/ar.edu.unlp.info.lifia.tvd.html5_test.old' );
			}

			it('should fail if package is not valid', function(done) {
				assert.isFalse( pkgMgr.hasPackage('pepe') );

				//	Begin update
				var uopts = {
					"id": 'pepe',
					"url": path.join(__dirname,'test_app1_2.tpk')
				};
				pkgMgr.update( uopts, function(unErr) {
					assert.equal( unErr.message, 'Package not found: id=pepe' );
					assert.isTrue( uninstallPackageCalled === 0 );
					assert.isTrue( installPackageCalled === 0 );
					done();
				});
			});

			it('should update valid application', function(done) {
				function fncOnInstall(err,pkgID) {
					//	Checks for install
					assert.equal(err);
					assert.isTrue( pkgMgr.hasPackage(pkgID) );
					var pkgInfo = pkgMgr.get( pkgID );

					//	Begin update
					var uopts = {
						"id": pkgID,
						"url": path.join(__dirname,'test_app1_2.tpk')
					};
					pkgMgr.update( uopts, function(unErr) {
						assert.isTrue( unErr === undefined );
						assert.isTrue( pkgMgr.hasPackage(pkgID) );
						assert.isTrue( uninstallPackageCalled === 0 );
						assert.isTrue( installPackageCalled === 1 );

						var newPkgInfo = pkgMgr.get( pkgID );
						assert.notEqual( pkgInfo, newPkgInfo );
						assert.isFalse( tvdutil.isDirectory(backupPath()) );

						done();
					});
				}

				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fncOnInstall );
			});

			it('should handle error when create temporary directory', function(done) {
				function fncOnInstall(err,pkgID) {
					//	Checks for install
					assert.equal(err);
					assert.isTrue( pkgMgr.hasPackage(pkgID) );
					var pkgInfo = pkgMgr.get( pkgID );

					rmDirTree( path.join(deploy.rw,'/config/ar.edu.unlp.info.lifia.tvd.system/tmp') );

					//	Begin update
					var uopts = {
						"id": pkgID,
						"url": path.join(__dirname,'test_app1_2.tpk')
					};
					pkgMgr.update( uopts, function(unErr) {
						assert.isObject( unErr );

						assert.isTrue( uninstallPackageCalled === 0 );
						assert.isTrue( installPackageCalled === 1 );

						var newPkgInfo = pkgMgr.get( pkgID );
						assert.equal( pkgInfo, newPkgInfo );
						assert.isFalse( tvdutil.isDirectory(backupPath()) );

						done();
					});
				}

				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fncOnInstall );
			});

			it('should handle error when size is invalid', function(done) {
				function fncOnInstall(err,pkgID) {
					//	Checks for install
					assert.equal(err);
					assert.isTrue( pkgMgr.hasPackage(pkgID) );
					var pkgInfo = pkgMgr.get( pkgID );

					//	Begin update
					var uopts = {
						"id": pkgID,
						"url": 'http://172.16.0.206/tac_testcases/test_app1.tpk',
						"md5": 'be1303e467b240bae5883c060ce2f880',
						"size": 0
					};
					pkgMgr.update( uopts, function(unErr) {
						assert.instanceOf( unErr, Error, 'Downloaded file differ in size' );

						assert.isTrue( uninstallPackageCalled === 0 );
						assert.isTrue( installPackageCalled === 1 );

						var newPkgInfo = pkgMgr.get( pkgID );
						assert.equal( pkgInfo, newPkgInfo );

						assert.strictEqual( getTempFiles().length, 0 );
						assert.isFalse( tvdutil.isDirectory(backupPath()) );

						done();
					});
				}

				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fncOnInstall );
			});

			it('should handle error on signal handler', function(done) {
				function fncOnInstall(err,pkgID) {
					//	Checks for install
					assert.equal(err);
					assert.isTrue( pkgMgr.hasPackage(pkgID) );
					var pkgInfo = pkgMgr.get( pkgID );

					var onUpdate = function(evt,cb) {
						cb( new Error('pepe') );
					};
					onUpdate.isAsync = true;
					pkgMgr.on('update', onUpdate );

					//	Begin update
					var uopts = {
						"id": pkgID,
						"url": path.join(__dirname,'test_app1_2.tpk')
					};
					pkgMgr.update( uopts, function(unErr) {
						assert.instanceOf( unErr, Error, 'pepe' );

						assert.isTrue( uninstallPackageCalled === 0 );
						assert.isTrue( installPackageCalled === 1 );

						var newPkgInfo = pkgMgr.get( pkgID );
						assert.equal( pkgInfo, newPkgInfo );

						assert.strictEqual( getTempFiles().length, 0 );
						assert.isFalse( tvdutil.isDirectory(backupPath()) );

						done();
					});
				}

				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fncOnInstall );
			});

			it('should handle error when backup old package', function(done) {
				function fncOnInstall(err,pkgID) {
					//	Checks for install
					assert.equal(err);
					assert.isTrue( pkgMgr.hasPackage(pkgID) );
					var pkgInfo = pkgMgr.get( pkgID );

					var old = fs.renameAsync;
					fs.renameAsync = function(oldPath,newPath) {
						if (newPath === backupPath()) {
							fs.renameAsync = old;
							return bPromise.reject(new Error('pepe'));
						}
						else {
							return old(oldPath,newPath);
						}
					};

					//	Begin update
					var uopts = {
						"id": pkgID,
						"url": path.join(__dirname,'test_app1_2.tpk')
					};
					pkgMgr.update( uopts, function(unErr) {
						assert.instanceOf( unErr, Error, 'pepe' );

						assert.isTrue( uninstallPackageCalled === 0 );
						assert.isTrue( installPackageCalled === 1 );

						var newPkgInfo = pkgMgr.get( pkgID );
						assert.equal( pkgInfo, newPkgInfo );

						assert.strictEqual( getTempFiles().length, 0 );
						assert.isFalse( tvdutil.isDirectory(backupPath()) );

						done();
					});
				}

				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fncOnInstall );
			});

			it('should handle error when install new package', function(done) {
				function fncOnInstall(err,pkgID) {
					//	Checks for install
					assert.equal(err);
					assert.isTrue( pkgMgr.hasPackage(pkgID) );
					var pkgInfo = pkgMgr.get( pkgID );

					var old = fs.renameAsync;
					fs.renameAsync = function(oldPath,newPath) {
						if (newPath === deploy.installPackagePath(pkgID)) {
							fs.renameAsync = old;
							return bPromise.reject(new Error('pepe'));
						}
						else {
							return old(oldPath,newPath);
						}
					};

					//	Begin update
					var uopts = {
						"id": pkgID,
						"url": path.join(__dirname,'test_app1_2.tpk')
					};
					pkgMgr.update( uopts, function(unErr) {
						assert.instanceOf( unErr, Error, 'pepe' );

						assert.isTrue( uninstallPackageCalled === 0 );
						assert.isTrue( installPackageCalled === 1 );

						var newPkgInfo = pkgMgr.get( pkgID );
						assert.equal( pkgInfo, newPkgInfo );

						assert.strictEqual( getTempFiles().length, 0 );
						assert.isTrue( tvdutil.isDirectory(deploy.installPackagePath(pkgID)) );
						assert.isFalse( tvdutil.isDirectory(backupPath()) );

						done();
					});
				}

				var opts = {
					"url": path.join(__dirname,'test_app1.tpk')
				};
				pkgMgr.install( opts, fncOnInstall );
			});
		});

	});
});
