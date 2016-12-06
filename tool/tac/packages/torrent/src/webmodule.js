"use strict";

var assert = require('assert');
var path = require('path');
var fs = require('fs');

//	The.Choice.2016.PROPER.720p.BluRay.x264-DRONES[rarbg]
//	4576.0 / 5605.6 MB Rate:   0.0 /   0.0 KB Uploaded:    41.8 MB [81%] --d --:-- [T  R: 0.01]

let colors = [ "cadetblue", "beige" ];
let style = fs.readFileSync(path.join(__dirname,'style.css'));

function WebModule( torrentClient ) {
	assert(torrentClient,'Invalid torrent client');
	
	//	API
	var self = {};

	//	API
	self.name = 'torrent';
	self.useUTF8 = true;

	self.start = function() {
		log.verbose( 'torrent::web', 'Start' );
		
		self.addHandler( 'GET', '/torrent', function(req,res) {
			torrentClient.getAll().then(function(torrents) {
				let html = '<!DOCTYPE html>';
				html += '<html>';
				html += '<head>' + style + '</head>';
				html += '<body>';
				html += '<section id="TorrentsSection">';

				for (let i=0; i<torrents.length; i++) {
					let torrent = torrents[i];
					let color = colors[(i % colors.length)];
					
					html += '<section style="background-color:' + color + '";>';
					html += '	<label>' + torrent.name + '</label>';
					html += '	<section id="TorrentDetails">';
					html += '		<label> Bytes: ' + torrent.size + ' / ' + torrent.completed + '</label>';
					html += '		<label> Rate: ' + torrent.up_rate + ' / ' + torrent.down_rate + '</label>';
					html += '	</section>';
					html += '</section>';						
				}

				html += '</section>';
				html += '</body>';
				html += '</html>';

				self.sendResult( res, 200, 'text/html', html );
			}, function(err) {
				let html = '<!DOCTYPE html>';
				html += '<html>';
				html += '<body>';
				html += 'Error: ' + err.message;
				html += '</body>';
				html += '</html>';				
				self.sendResult( res, 200, 'text/html', html );				
			});
		});
	};

	self.stop = function() {
		log.verbose( 'torrent::web', 'Stop' );
	};

	return self;
}

module.exports = WebModule;
