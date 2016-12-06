'use strict';

var path = require('path');
var fs = require('fs');
var tvdutil = require('tvdutil');

function removePlatform( base, plat ) {
	var p = path.join(base,plat);
	if (tvdutil.isDirectory( p ))  {
		console.log( "\tRemove tree %s", p );
	}
}

var PLATFORM = process.argv[2];
var depot =process.env['DEPOT'];
var tacPath = path.join(depot,'tool','tac');

console.log( 'Remove platform: %s using TAC directory=%s', PLATFORM, tacPath );

//	Remove build/devices/{platform}
removePlatform( path.join(tacPath,'build','devices'), PLATFORM );

//	For e/package in packages -> remove packages/{pkg}/impl/{platform}
var basePkgs = path.join(tacPath,'packages');
fs.readdirSync( basePkgs ).forEach(function(entry) {
	removePlatform( path.join(basePkgs,entry,'impl'), PLATFORM );
});
