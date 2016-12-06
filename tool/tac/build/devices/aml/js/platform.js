'use strict';

var AudioPulse = require('audio_pulse');
var DevInput = require('input_linux');

function Factory() {
	var self = {};

	self.createAudio = function() {
		// Convert alsa volume value to pulse one, using Online Polynomial Regression http://www.xuru.org/rt/PR.asp
		//
		// Alsa	Pulse
		// 5	22
		// 50	85
		// 75	110
		// 100	130
		let realVol = function(vol) {
			return ""+Math.floor(-0.005127707476 * Math.pow(vol,2) + 1.672824315 * vol + 13.82957457)+"%";
		}

		return new AudioPulse(0,realVol);
	};

	self.createInput = function() {
		return new DevInput();
	};

	return self;
}

module.exports = Factory;

