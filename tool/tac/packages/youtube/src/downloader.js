'use strict';

var assert = require('assert');
var path = require('path');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var fse = bPromise.promisifyAll( require('fs-extra') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var ytdl = require('ytdl-core');

function YoutubeModule() {
	var _workDir;
	var _onComplete;

	function download( fetchID, videoID ) {
		mktemp.createDirAsync( path.join(_workDir,'youtube_XXXXXXXX') )
			.then(function(tmpPath) {
				return mktemp.createFileAsync( path.join(tmpPath,'youtube_XXXXXXXX.mp4') )
					.then(function(tmpFile) {
						log.silly( 'YoutubeModule', 'Download video: id=%s, tmpFile=%s', fetchID, tmpFile );

						let result = {
							id: fetchID,
							meta: {},
							path: tmpPath
						};

						return new bPromise(function(resolve,reject) {
							//	Setup stream
							var stream = fs.createWriteStream( tmpFile );
							stream.on('finish', function() {
								resolve(result);
							});
							stream.on('error', reject );

							let yStream = ytdl( videoID, { filter: function(format) { return format.container === 'mp4'; } } );
							yStream.pipe(stream);
							yStream.on('info', function(info) {
								result.meta.title = info.title;
								result.meta.writer = [info.author];
								result.meta.poster = info.iurl;
							});
							yStream.on('error', reject);
						});
					})
					.catch(function(err) {
						return fse.removeAsync( tmpPath ).then( () => {throw err;} );
					});
			})
			.then(function(result) {
				_onComplete( result );
			},function(err) {
				log.warn( 'YoutubeModule', 'Error processing video: fetchID=%s, videoID=%s, err=%s', fetchID, videoID, err.message );
				_onComplete( { id: fetchID, err: err } );
			});
	}

	//--------------------- -----------------------------------------------------------------------
	//    API
	//--------------------- -----------------------------------------------------------------------
	var self = {};

	self.name = 'youtube';
	self.type = 'youtube';

	self.fetch = bPromise.method(function( opts ) {
		assert(opts, 'YoutubeModule.fetch: Invalid opts');
		assert(opts.uri, 'YoutubeModule.fetch: Invalid opts.uri');
		assert(opts.type === self.type, 'YoutubeModule.fetch: Invalid opts.type');

		log.silly( 'YoutubeModule', 'Download youtube video: uri=%s', opts.uri );
		download( opts.id, opts.uri );
	});

	self.start = bPromise.method(function( opts ) {
		assert(opts.workDir,'YoutubeModule.start: Invalid workDir');
		assert(opts.onComplete,'YoutubeModule.start: Invalid onComplete');

		_workDir = opts.workDir;
		_onComplete = opts.onComplete;
	});

	self.stop = bPromise.method(function() {
	});

	return self;
}

module.exports = YoutubeModule;

