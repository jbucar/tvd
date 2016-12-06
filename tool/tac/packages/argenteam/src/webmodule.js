"use strict";

var qs = require('querystring');

function WebModule(impl) {
	var self = {};

	//	API
	self.name = 'argenteam';
	self.useUTF8 = true;

	self.start = function() {
		log.verbose( 'web::argenteam', 'Start' );

		self.addHandler( 'GET', '/argenteam', function(req,res) {
			let html = '<!DOCTYPE html>';
			html += '<html>';
			html += '<body>';
			html += '	<form action="./argenteam/checkFeeds" method="POST">';
			html += '		<button type="submit">Check feeds</button>';
			html += '	</form>';
			html += '   <form action="./argenteam/addID" method="POST">';
			html += '		<select name="type">';
			html += '			<option value="serie">Serie</option>';
			html += '			<option value="movie">Movie</option>';
			html += '		</select></br>';
			html += '		ID<input type="text" name="id"></br>';
			html += '		<button type="submit">Add</button>';
			html += '	</form>';
			html += '</body>';
			html += '</html>';

			self.sendResult( res, 200, 'text/html', html );
		});

		self.addHandler( 'POST', '/argenteam/checkFeeds', function(req,res) {
			impl.checkFeeds();
			let html = '<!doctype html><html><head><title>response</title></head><body>';
			html += 'Sucess!!';
			html += '</body></html>';
			self.sendResult( res, 200, 'text/html', html );
		});

		self.addHandler( 'POST', '/argenteam/addID', function(req,res) {
			function showResult(err) {
				let html = '<!doctype html><html><head><title>response</title></head><body>';
				html += err ? 'Error: ' + err.message : 'Sucess!!';
				html += '</body></html>';
				self.sendResult( res, 200, 'text/html', html );
			}

			let formData = qs.parse(req.data);
			impl.addID( formData.type, formData.id, 'pepe' )
				.then( () => showResult(), showResult );
		});
	};

	self.stop = function() {
		log.verbose( 'web::argenteam', 'Stop' );
	};

	return self;
}

module.exports = WebModule;
