"use strict";

var createService = require('./serviceimpl').createService;

function ImageService() {
	return createService( 'media.image', 'image.json' );
}

module.exports = ImageService;

