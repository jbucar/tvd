#!/usr/bin/env node
"use strict";

var mock = require('htmlshellmock');
var crypto = require('crypto');
var path = require('path');
var dso = path.resolve('..', '..', '..', 'libdtvhtmlshellremote.so');
var timer = null;
var mustStart = true;
var stress = false;
var keepAliveTimeout = 1000;

function randomIntInc (low, high) {
    return Math.floor(Math.random() * (high - low + 1) + low);
}

function onKeepAlive() {
	if (stress) {
		mock.EmitErrorEvent(0,12345,'pepe_code', 'pepe_error');
		mock.EmitKeyEvent(0,11,true);
		mock.EmitKeyEvent(0,11,false);

		var msg = crypto.randomBytes( randomIntInc(10,30000) ).toString('base64');
		mock.EmitApiCall('RemoteExtension', 0, 5, 'ApiCall', msg );
	}
}

function onSystemExit() {
	console.log( '[mock] on system exit' );
	stress = false;
	clearInterval( timer );
	mock.Stop();
	mock.Fin();
}

function onJsResponse( browserID, queryID, isSignal, error, params ) {
	if (!stress) {
		mock.EmitApiCall('RemoteExtension', browserID, queryID, error, params );
	}
}

console.log( '[mock] start' );

if (!mock.Init( onSystemExit, onJsResponse )) {
	console.log( 'init failed!' );
	mock.Stop();
	mock.Fin();
}

//	Process parameters
process.argv.forEach(function(val) {
	if (val.indexOf("test_do_stop") > 0) {
		setTimeout( function() {
			console.log( '[mock] stop lib' );
			mock.Stop();
		}, 50 );
	}

	if (val.indexOf("test_no_start") > 0) {
		mustStart = false;
	}

	if (val.indexOf("test_keys") > 0) {
		setTimeout( function() {
			console.log( '[mock] send keys' );
			mock.EmitKeyEvent(0,11,true);
		}, 50 );
	}

	if (val.indexOf("test_browser_error") > 0) {
		setTimeout( function() {
			console.log( '[mock] send error' );
			mock.EmitErrorEvent(0,12345,'pepe_code', 'pepe_error');
		}, 50 );
	}

	if (val.indexOf("test_api") > 0) {
		setTimeout( function() {
			console.log( '[mock] send api' );
			mock.EmitApiCall('RemoteExtension', 0, 5, 'ApiCall', 'params_pepe');
		}, 50 );
	}

	if (val.indexOf("test_browser_loaded") > 0) {
		setTimeout( function() {
			console.log( '[mock] send loaded' );
			mock.EmitLoadedEvent(0);
		}, 50 );
	}

	if (val.indexOf("test_stress") > 0) {
		stress=true;
		keepAliveTimeout = 10;
	}

	if (val.indexOf("tvd-remote-server") > 0) {
		var url = val.substring( val.indexOf('=')+1 );
		if (!mock.SetSwitchCmd("tvd-remote-server-url", url )) {
			process.exit(-1);
		}
	}
});

if (mustStart && !mock.Start(dso)) {
	console.log( 'start failed!' );
	process.exit(-1);
}

//	Wait
timer = setInterval( onKeepAlive, keepAliveTimeout );

