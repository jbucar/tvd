"use strict";

var createService = require('./serviceimpl').createService;

function VideoService() {
	return createService( 'media.video', 'video.json' );
}

module.exports = VideoService;

