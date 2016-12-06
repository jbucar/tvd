"use strict";

var tvdutil = require('tvdutil');
var qs = require('querystring');

function WebModule() {
	//	API
	var self = {};

	function addYoutube( url, videoID ) {
		log.silly( 'youtube::web', 'addVideo: video=%id', videoID );

		var data = {
			'serviceID': 'ar.edu.unlp.info.lifia.tvd.media.main',
			'apiName': 'media.movie',
			'method': 'add',
			'params': [{
				src: {
					type: 'youtube',
					uri: videoID
				},
				download: true
			}]
		};

		var reqOpts = {
			method: 'POST',
			url: url + '/api',
			data: data
		};

		return tvdutil.doRequestAsync( reqOpts );
	}

	//	API
	self.name = 'youtube';
	self.useUTF8 = true;

	self.start = function() {
		log.verbose( 'youtube::web', 'Start' );

		self.addHandler( 'GET', '/youtube', function(req,res) {
			let html = '<!DOCTYPE html>';
			html += '<html>';
			html += '<body>';
			html += '	<form action="/youtube" method="POST">';
			html += '		URI:<br>';
			html += '		<input type="text" name="uri" value="">';
			html += '		<br><br>';
			html += '		<input type="submit" value="Submit">';
			html += '	</form>';
			html += '</body>';
			html += '</html>';

			self.sendResult( res, 200, 'text/html', html );
		});

		self.addHandler( 'POST', '/youtube', function(req,res) {
			let formData = qs.parse(req.data);

			let header = '<!doctype html><html><head><title>response</title></head><body>';
			let footer = '</body></html>';

			addYoutube( self.getBaseUrl(req), formData.uri ).then(function() {
				let html = header;
				html += 'Sucess!!';
				html += footer;

				self.sendResult( res, 200, 'text/html', html );
			}, function(err) {
				let html = header;
				html += 'Error: ' + err.message;
				html += footer;
				self.sendResult( res, 200, 'text/html', html );
			});
		});
	};

	self.stop = function() {
		log.verbose( 'youtube::web', 'Stop' );
	};

	return self;
}

module.exports = WebModule;
