#!/usr/bin/env node
/*jshint esversion: 6 */

'use strict';

var assert = require('assert');
var path = require('path');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs-extra') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var _ = require('lodash');
var child_process = require('child_process');
var spawn = child_process.spawn;
var exec = child_process.execSync;

var clean_after_exit = true;

Array.prototype.diff = function(a) {
	return this.filter( (i) => {
		return a.indexOf(i) < 0;
	});
};

function list2dict(list, defaults) {
	return _.reduce(list, function(dict, value) {
			dict[value] = defaults;
			return dict;
	}, {});
}

function scanPackages(pkgsPath) {
	return fs.readdirAsync(pkgsPath)
	.then(function(pkgs) {
		return bPromise.settle(pkgs.map(function(pkgName) {
			var absPath = path.join(pkgsPath,pkgName,'package.json');
			return fs.statAsync(absPath)
			.then(function(stats) {
					return pkgName;
			})
			.error( function(error) {
				throw new Error('Cannot get stats from file:',absPath, error.message);
			});
		}))
		.filter(function(prom) {
			return prom.isFulfilled();
		})
		.map(function(prom) {
			return prom.value();
		});
	});
}

function getPackageInfo( data, base, pkg ) {
	if (!data.packages[pkg]) {
		var filePath = path.join(base,pkg,'package.json');
		var pkgInfo = fs.readJsonSync( filePath );
		if (pkgInfo) {
			data.packages[pkg] = pkgInfo;
			data.packages[pkg].path = path.dirname(filePath);
			updateDep( data, data.deps, 'dep', pkgInfo.dependencies );
			updateDep( data, data.deps, 'devDep', pkgInfo.devDependencies );
		}
	}
}

function updateDep( data, deps, depsType, pkgDeps ) {
	if (!pkgDeps) {
		return;
	}
	Object.keys( pkgDeps ).forEach(function(dep) {
		var stats = deps.find( (d) => d.name === dep );
		if (stats) {
			stats.count[depsType]++;
		}
		else {
			var info = pkgDeps[dep];
			//	Is not internal module?
			if (info.indexOf('file:') < 0) {
				stats = {
					name: dep,
					info: info,
					count: {
						dep: 0,
						devDep: 0
					}
				};
				stats.count[depsType]++;
				deps.push(stats);
			}
			else {
				getPackageInfo( data, path.join(tacPath,'packages','node_modules'), path.basename(info) );
			}
		}
	});
}

function getPackagesInfo( pkgs, base ) {
	base = base || '';
	var data = {
		packages: {},
		deps: [],
		devDeps: []
	};
	pkgs.forEach( (pkg) => getPackageInfo(data,base,pkg) );
	return data;
}

function filterDeps( deps, condition ) {
	var result = {};
	deps.filter( condition )
		.forEach(function (stat) {
			result[stat.name] = "latest";
		});
	return result;
}

function filterExternalDeps( deps ) {
	return Object.keys(deps).filter( (x) => {
		return deps[x].indexOf('file:') < 0;
	});
}

function verboseSpawn( cmd, args, opts ) {
	return new bPromise( function(resolve, reject) {
		opts.stdio = 'inherit';
		var s = spawn(cmd, args, opts);

		s.once('error', (error) => {
			reject( 'Error ' + error.message );
		});
		s.once('close', (code) => {
			if(code === 0) {
				resolve();
			} else {
				reject( new Error('Error '+code+' after spawning: '+cmd+' '+args.join(' ')));
			}
		});
	});
}

function installPackages( pkgs, dest_path, node_path, extraCmds ) {
	console.log( "Installing packages: %s",  pkgs.join(" ") );
	extraCmds = extraCmds || [];
	return verboseSpawn('npm', ['install', '--no-bin-links', '--no-optional'].concat(pkgs).concat(extraCmds), { 'cwd': dest_path, 'env': { 'PATH': process.env.PATH, 'NODE_PATH': node_path } });
}

function processCommonModules( info, depsPath, devDepsPath ) {

	// Create temporary directory
	var deps = Object.keys(info.dependencies);
	var devDeps = Object.keys(info.devDependencies);

	// Create destination direcotries
	fs.emptyDirSync(depsPath);
	fs.emptyDirSync(devDepsPath);

	// Write package.json
	fs.writeJsonSync(path.join(depsPath, 'package.json'), info);
	fs.writeJsonSync(path.join(devDepsPath, 'package.json'), info);

	// Installing deps modules
	return installPackages( [], depsPath, '', ['--only=prod'] )
	.then( () => {
		// Installing dev deps modules
		return installPackages( [], devDepsPath, path.join(depsPath, 'node_modules'), ['--only=dev']);
	});

}

function processPrivateModules( data, info, nodePathDep, nodePathDevDep ) {
	var packages = Object.keys(data.packages);
	var dependencies = Object.keys(info.dependencies);
	var devDependencies = Object.keys(info.devDependencies);

	var proms = [];
	packages.forEach( (p) => {
		var pkg = data.packages[p];
		var packDeps=pkg.dependencies;
		var packDevDeps=pkg.devDependencies;
		var destPath;
		if(packDeps) {
			var privPackDeps = filterExternalDeps(packDeps).diff(dependencies);
			if(privPackDeps.length > 0) {
				console.log("Installing %s private modules: %s", p, privPackDeps.join(', '));
				destPath = path.join(pkg.path, 'src');
				fs.emptyDirSync(path.join(destPath, 'node_modules'));
				proms.push(installPackages( privPackDeps, destPath, nodePathDep+':'+nodePathDevDep));
			}
		}
		if(packDevDeps) {
			var privPackDevDeps = filterExternalDeps(packDevDeps).diff(devDependencies).diff(dependencies);
			if(privPackDevDeps.length > 0) {
				console.log("Installing %s private dev modules: %s", p, privPackDevDeps.join(', '));
				destPath = path.join(pkg.path, 'test');
				fs.emptyDirSync(path.join(destPath, 'node_modules'));
				proms.push(installPackages( privPackDevDeps, destPath, nodePathDep+':'+nodePathDevDep));
			}
		}
	});
	return bPromise.all(proms);
}

function fullInstall(pkgsPath) {
	scanPackages(pkgsPath)
	.then(function(pkgs) {
		//	Parse all package.json
		console.log( "Procesing packages dependencies...");
		var data = getPackagesInfo( pkgs, path.join(tacPath,'packages') );

		var info = {
			name: 'tac',
			version: '1.0.0',
			description: 'TAC tv',
			author: 'LIFIA',
			license: 'ISC',
			dependencies: filterDeps( data.deps, (stat) => (stat.count.dep > 1) || (stat.count.dep && stat.count.devDep) ),
			devDependencies: filterDeps( data.deps, (stat) => (stat.count.devDep > 1) && (stat.count.dep === 0) )
		};
		console.log( JSON.stringify(info, null, 4) );

		mktemp.createDirAsync('/tmp/get_modules_XXXXXXXX').then( (tmpPath) => {
			var depsPath = path.join(tmpPath, 'deps');
			var devDepsPath = path.join(tmpPath, 'devDeps');
			processCommonModules( info, depsPath, devDepsPath )
			.then ( () => {
				// Process internal dependencies

				var nodePathDep = path.join(tmpPath, 'deps', 'node_modules');
				var nodePathDevDep = path.join(tmpPath, 'devDeps', 'node_modules');
				return processPrivateModules( data, info, nodePathDep, nodePathDevDep);
			})
			.then ( () => {
				// Move modules to its final destination
				var outPath = path.join(pkgsPath, 'node');
				[
					{ from: "deps", to: "node_modules" },
					{ from: "devDeps", to: "node_modules_dev" }
				].forEach( (m) => {
					var nodePathDepSrc = path.join(tmpPath, m.from, 'node_modules');
					var nodePathDepDst = path.join(outPath, m.to);
					fs.removeSync(nodePathDepDst);
					fs.copySync(nodePathDepSrc, nodePathDepDst);
				});
			})
			.catch( (error) => {
				console.log(error);

			})
			.finally( () => {
				if(clean_after_exit && tmpPath) {
					console.log("Cleaning directory "+tmpPath);
					fs.removeSync(tmpPath);
				}
				console.log("Done!");
			});
		});
	});

}

function specificInstall(pkgsPaths) {
	scanPackages(pkgsPath)
	.then(function(pkgs) {
		//	Parse all package.json
		var nodePathDep = path.join(pkgsPath, 'node', 'node_modules');
		var nodePathDevDep = path.join(pkgsPath, 'node', 'node_modules_dev');

		console.log( "Procesing packages dependencies...");
		var data = getPackagesInfo( pkgsPaths );
		var specificPkgs = 	{};
		pkgsPaths.forEach( (p) => { specificPkgs[p] = data.packages[p]; });
		data.packages = specificPkgs;

		var info = {
			name: 'tac',
			version: '1.0.0',
			description: 'TAC tv',
			author: 'LIFIA',
			license: 'ISC',
			dependencies: list2dict(fs.readdirSync(nodePathDep), "latest"),
			devDependencies: list2dict(fs.readdirSync(nodePathDevDep), "latest")
		};
		console.log( JSON.stringify(info, null, 4) );

		return processPrivateModules( data, info, nodePathDep, nodePathDevDep)
		.catch( (error) => {
			console.log(error);
		})
		.finally( () => {
			console.log("Done!");
		});
	});
}

//---main ----------------------------------------------------------------------

var depot = process.env.DEPOT || path.dirname(path.dirname(path.dirname(path.dirname(__dirname))));
var tacPath = path.join(depot,'tool','tac');
var pkgsPath = path.join(tacPath, 'packages');

var specificPkgs = process.argv.slice(2);

if( specificPkgs.length ) {
	specificInstall(specificPkgs);
} else {
	fullInstall(pkgsPath);
}
