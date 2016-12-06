#!/usr/bin/env node

'use strict';

var assert = require('assert');
var path = require('path');
var fs = require('fs');

var cfg = process.argv[2];
var depot = process.env.DEPOT;
var tacPath = path.join(depot,'tool','tac');

console.log( 'Check dependencies using config: %s, TAC directory=%s', cfg, tacPath );

function parseJson(f) {
	//	Parse config
	try {
		var data = JSON.parse(fs.readFileSync( f ));
		return data;
	} catch(err) {
		console.log( 'Config not found or with errors: err=%s, file%s', err.message, f );
		process.exit(1);
	}
}

var packages = {};

//	Parse data
var cfgParsed = parseJson(cfg);
assert(cfgParsed.packages);
cfgParsed.packages.forEach(function(pkg) {
	var manifest = parseJson( path.join(tacPath,'packages',pkg,'manifest.json') );
	assert(manifest.components);
	manifest.components.forEach(function(c) {
		packages[c.id] = c.dependencies ? c.dependencies : [];
	});
});

function checkDeps( id, visited, root ) {
	if (root === 0) {
		console.log( 'Component %s ->', id );
	}
	packages[id].forEach(function(d) {
		if (root === 0) {
			console.log( '\t%s', d );
		}

	 	//console.log( '%s', d );
		if (visited.indexOf(d) >= 0) {
			console.error( 'Error in dependencies!: component=%s, path=%j', d, visited );
			process.exit( 0 );
		}
		if (packages[d]) {
			visited.push(d);
			checkDeps( d, visited, root+1 );
			visited.pop();
		}
	});
}

//	Check dependencies
Object.keys(packages).forEach(function(id) {
	checkDeps( id, [id], 0 );
});

