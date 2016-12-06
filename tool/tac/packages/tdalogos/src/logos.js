'use strict';

var bPromise = require('bluebird');
var path = require('path');
var fs = bPromise.promisifyAll( require('fs') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var tvdutil = require('tvdutil');


function dbName( dbBase ) {
	return path.join(dbBase,'channels.json');
}

function imageName( dbBase, imgName ) {
	return path.join(dbBase,'images',imgName);
}

function fetch( opts, ret ) {
	return tvdutil.fetchURLAsync( opts )
		.then(function() {
			return ret;
		});
}

function fetchDB( params ) {
	log.silly( 'tdalogos', 'Fetch DB: URL=%s', params.url );
	params.dbFile = path.join(params.tempPath,'db.json');
	var options = {
		'url': params.url,
		'fileOutput': params.dbFile,
		'user': params.user,
		'password': params.password
	};
	return fetch( options, params );
}

function fetchImage( params, imgName, imgURL ) {
	var options = {
		'url': imgURL,
		'fileOutput': imageName(params.tempPath,imgName),
		'user': params.user,
		'password': params.password
	};
	log.silly( 'tdalogos', 'Fetch image: out=%s, URL=%s', options.fileOutput, imgURL );
	return fetch( options, params );
}

function parse( params ) {
	log.silly( 'tdalogos', 'Parse DB: db=%s', params.dbFile );
	return fs.readFileAsync(params.dbFile)
		.then(JSON.parse)
		.then(function(jsonValues) {
			if (jsonValues.ultima_fecha_modificacion == params.lastChanged) {
				throw new Error( 'No changes in DB' );
			}

			if (jsonValues.canales.length === 0) {
				throw new Error( 'Empty DB' );
			}

			//	Update change time
			params.lastChanged = jsonValues.ultima_fecha_modificacion;

			var fetchImages = [];
			var logos = [];

			//	For each channel
			jsonValues.canales.forEach(function(data) {
				//	Add promise to fetch image
				var imgName = path.basename(data.nombre_imagen);
				fetchImages.push( fetchImage( params, imgName, data.nombre_imagen ) );

				//	For each virtual channel
				data.canales_virtuales.forEach(function(ch) {
					//	Fix channelID
					var tmpChannel = '0000' + String(Number(ch.numero));
					var first = tmpChannel.slice( tmpChannel.length-4, tmpChannel.length-2 );
					var last = tmpChannel.slice( tmpChannel.length-2 );
					var channel = first + '.' + last;

					//	Fix frequency
					var freq = data.frequency;
					if (freq) {
						freq = parseInt(parseInt(freq)/1000).toString();
					}

					var info = {
						"name": data.nombre,
						"frequency": freq,
						"nitID": ch.on_id,
						"tsID": ch.ts_id,
						"srvID": ch.service_id,
						"channelID": channel,
						"logo": imgName
					};

					logos.push( info );
				});
			});

			//	Fetch all images
			return bPromise.all(fetchImages)
				.then(function() {
					params.logos = logos;
					return params;
				});
		});
}

function fetchLogos( params ) {
	log.verbose( 'tdalogos', 'Fetch logos from webservice' );
	//	Create temporary directory for new DB
	var dbBase = path.join(params.basePath,'logos-XXXXXX');
	return mktemp.createDirAsync(dbBase)
		.then(function(tmpDir) {
			log.silly( 'tdalogos', 'Temporary directory created: tmpDir=%s', tmpDir );
			//	Fetch DB
			params.tempPath = tmpDir;
			return fetchDB( params );
		})
		.then(function(params) {
			log.silly( 'tdalogos', 'Create images directory' );
			//	Create images directory
			var imagesPath = path.join(params.tempPath,'images');
			return fs.mkdirAsync( imagesPath )
				.then(function() {
					log.silly( 'tdalogos', 'Images directory created' );
					return params;
				});
		})
		.then(parse)
		.then(function(params) {
			//	Write new DB
			log.silly( 'tdalogos', 'Logos parsed, save new db: count=%d', params.logos.length );
			params.logosDB = dbName(params.tempPath);
			return fs.writeFileAsync( params.logosDB, JSON.stringify(params.logos, null, 4) )
				.then(function() {
					return {
						'logos': params.logos,
						'path': params.tempPath,
						'lastChanged': params.lastChanged
					};
				});
		},function(err) {
			return tvdutil.removeTreeAsync( params.tempPath ).
				then(function() {
					throw err;
				});
		});
}

module.exports.fetchLogos = fetchLogos;
module.exports.dbName = dbName;
module.exports.imageName = imageName;

