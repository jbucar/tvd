'use strict';

var AudioPulse = require('audio_pulse');
var DevInput = require('input_linux');

function Factory() {
	var self = {};

	self.createAudio = function() {
		let realVol = function(vol) {	//	TODO:
			return ""+Math.floor(-0.005127707476 * Math.pow(vol,2) + 1.672824315 * vol + 13.82957457)+"%";
		}

		return new AudioPulse(undefined,realVol);
	};

	self.createInput = function() {
		return new DevInput();
	};

	return self;
}

module.exports = Factory;

