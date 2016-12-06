'use strict';

var AudioPulse = require('audio_pulse');
var DevInput = require('input_linux');

function Factory() {
	var self = {};

	self.createAudio = function() {
		return new AudioPulse();
	};

	self.createInput = function() {
		return new DevInput();
	};

	return self;
}

module.exports = Factory;

