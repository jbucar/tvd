"use strict";

var assert = require('assert');
var path = require('path');
var bPromise = require('bluebird');
var Busboy = require('busboy');
var fs = require('fs');

function WebModule( adapter ) {
	var self = {};

	function reg(srv) {
		return adapter.registry().get(srv);
	}

	function tmpDir() {
		return adapter.getDataPath('tmp');
	}

	function parsePost(req,fields,workDir) {
		log.silly( 'web::System', 'parsePost: req=%j', req );
		return new bPromise(function(resolve,reject) {
			let files = [];
			let busboy = new Busboy({ headers: req.headers });
			busboy.on('file', function(fieldName, file, fileName) {
				log.silly( 'web::system', 'On file: fieldName=%s, fileName=%s, fileds=%j', fieldName, fileName, fields );
				if (fields.find( (f) => f === fieldName)) {
					let tmpName = path.join( workDir, path.basename(fileName) );
					log.verbose( 'web::system', 'Getting file: name=%s, dst=%s', fileName, tmpName );

					//	Setup stream
					let stream = fs.createWriteStream( tmpName );
					stream.on('error', reject );
					file.on( 'error', reject );
					file.pipe(stream);

					files.push({
						fieldName: fieldName,
						fileName: tmpName
					});
				}
			});

			busboy.on('finish', () => resolve(files) );
			busboy.on('error', reject );
			req.pipe(busboy);
		});
	}

	//	API
	self.name = 'system';
	self.useUTF8 = false;

	self.start = function() {
		log.verbose( 'web::system', 'Start' );

		self.addHandler( 'GET', '/system', function(req,res) {
			let html = '<!DOCTYPE html>';
			html += '<html>';
			html += '<body>';
			html += '<form action="./system/updateFirmware" method="POST" enctype="multipart/form-data">';
			html += '	Actualizar firmware: <input type="file" id="firmware" name="firmware"/></br>';
			html += '	<button type="submit">Actualizar</button>';
			html += '</form>';
			html += '<form action="./system/installPkt" method="POST" enctype="multipart/form-data">';
			html += '	Instalar paquete: <input type="file" id="firmware" name="pktFile"/></br>';
			html += '	<button type="submit">Instalar pkt</button>';
			html += '</form>';
			html += '</body>';
			html += '</html>';

			self.sendResult( res, 200, 'text/html', html );
		});

		let upHandler = self.addHandler( 'POST', '/system/updateFirmware', function(req,res) {
			parsePost( req, ['firmware'], tmpDir() )
				.then(function(files) {
					assert(files.length === 1 && files[0].fieldName === 'firmware', 'Invalid file' );

					let html = '<!doctype html><html><head><title>response</title></head><body>';
					html += 'Sucess!!';
					html += '</body></html>';
					self.sendResult( res, 200, 'text/html', html );

					reg('system').updateFirmware( files[0].fileName, true );
				})
				.catch(function(err) {
					log.warn( 'web::system', 'Error in updateFirmeware handler: err=%s', err.message );
					self.sendResult( res, 500 );
				});
		});
		upHandler.handleData = true;

		let ipHandler = self.addHandler( 'POST', '/system/installPkt', function(req,res) {
			parsePost( req, ['pktFile'], tmpDir() )
				.then(function(files) {
					assert(files.length === 1 && files[0].fieldName === 'pktFile', 'Invalid file' );

					reg('pkgmgr').install({url: files[0].fileName}, function(err) {
						if (err) {
							log.warn( 'web::system', 'Error installing pkt: err=%s', err.message );
							self.sendResult( res, 500 );
						}
						else {
							let html = '<!doctype html><html><head><title>response</title></head><body>';
							html += 'Sucess!!';
							html += '</body></html>';
							self.sendResult( res, 200, 'text/html', html );
						}
					});
				})
				.catch(function(err) {
					log.warn( 'web::system', 'Error in installPkt handler: err=%s', err.message );
					self.sendResult( res, 500 );
				});
		});
		ipHandler.handleData = true;
	};

	self.stop = function() {
		log.verbose( 'web::system', 'Stop' );
	};

	return self;
}

module.exports = WebModule;

