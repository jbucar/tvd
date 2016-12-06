#!/usr/bin/env node

"use strict";

var fs = require('fs');
var path = require('path');
var tvdutil = require('tvdutil');

var ORIGINAL_DIR = process.argv[2];
var DATA_DIR = process.argv[3];

function isDirectory( dir ) {
	try {
		return fs.statSync(dir).isDirectory();
	} catch( error ) {
		return false;
	}
}

function getPackage( id ) {
	// look for package in packages db
	try {
		var location = path.join(DATA_DIR, 'config', 'ar.edu.unlp.info.lifia.tvd.system', 'packages.json');
		var packages = JSON.parse(fs.readFileSync(location));
		if (id in packages) {
			return packages[id];
		}
	} catch(error) {
		// Do Nothing
	}

	// look for package in ORIGINAL_DIR
	var original = path.join(ORIGINAL_DIR , 'packages', id);
	if (isDirectory( original )) {
		return original;
	} else {
		throw new Error('Package ' + id + ' not installed');
	}
}

function loadModule( id, js ) {
	var pkg = getPackage( id );
	try {
		var mod = path.join( pkg, js );
		log.info("Bootstrap", "Loading module: module=%s", mod );
		return require( mod );
	} catch( error ) {
		throw new Error('Module ' + path.join(pkg,js) + ' not available');
	}
}

//	-------------------------------------------------------------------------------------------------
//	main
//	-------------------------------------------------------------------------------------------------

//	Setup log
tvdutil.setupLog( 'verbose' );
log.info("Bootstrap", 'Loading node: ORIGINAL_DIR=' + ORIGINAL_DIR + ', DATA_DIR=' + DATA_DIR );

try {
	//	Load system
	var main = loadModule( 'ar.edu.unlp.info.lifia.tvd.system', 'main.js' );

	//	Init system
	main( ORIGINAL_DIR, DATA_DIR );

} catch( error ) {
	log.error("Bootstrap", error.stack );
	//	Recovery
	process.exit(147);
}


