#!/usr/bin/env node

'use strict';

var assert = require('assert');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var path = require('path');
var run = bPromise.promisify( require('child_process').execFile );

if (process.argv.length != 4) {
	console.log('[ERROR] usage: ./%s <tools_dir> <crash_dump>', path.basename( process.argv[1] ));
	process.exit(1);
}

var toolsDir = path.resolve( process.argv[2] );
var crashDump = path.resolve( process.argv[3] );
var minidump = path.join( path.dirname(crashDump), path.basename(crashDump).split('.')[0] + '.minidump' );
var dmp2minidump = path.join( toolsDir, 'dmp2minidump.py' );
var stackWalker = path.join( toolsDir, 'minidump_stackwalk' );
var symbols = path.join( toolsDir, 'htmlshell.syms.d' );

bPromise.map( [crashDump, dmp2minidump, stackWalker], function(f) {
	return fs.statAsync(f).then( function(st) {
		assert( st.isFile() );
	});
})
.then( function() {
	fs.statAsync(symbols).then( function(stat) {
		assert( stat.isDirectory() );
	});
})
.then( function() {
	return run( dmp2minidump, [crashDump, minidump] );
})
.then( function() {
	run( stackWalker, [minidump, symbols] )
	.then( function(backtrace) {
		console.log( backtrace[0] );
	});
})
.catch( function(err) {
	console.log('[ERROR] Fail to generate backtrace: %s', err.message);
	process.exit(1);
});
