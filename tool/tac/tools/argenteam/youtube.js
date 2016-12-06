'use strict';

var path = require('path');
var assert = require('assert');
var bPromise = require('bluebird');
var minimist = require("minimist");
var tvdutil = require('tvdutil');

function addYoutube( oper ) {
	log.silly( 'youtube', 'addVideo: video=%id', oper.videoID );

	var data = {
		'serviceID': 'ar.edu.unlp.info.lifia.tvd.media.main',
		'apiName': 'media.movie',
		'method': 'add',
		'params': [{
			src: {
				type: 'youtube',
				uri: oper.videoID
			},
			download: true
		}]
	};

	let tac_ip = oper.host;
	let tac_port = oper.port;
	var reqOpts = {
		method: 'POST',
		url: 'http://'+tac_ip+':'+tac_port+'/api',
		data: data
	};

	return tvdutil.doRequestAsync( reqOpts );
}

function main() {
	let argv = minimist(process.argv.slice(2));
	tvdutil.setupLog( argv.logLevel || 'silly' );

	let oper = {
		host: argv.host || '127.0.0.1',
		port: argv.port || 2000,
		videoID: argv.id
	};

	addYoutube( oper );
}

main();

