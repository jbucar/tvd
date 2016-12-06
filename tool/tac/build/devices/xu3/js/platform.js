'use strict';

var AudioPulse = require('audio_pulse');
var DevInput = require('input_linux');

function Factory() {
	var self = {};

	self.createAudio = function() {
		// Convert alsa volume value to pulse one, using Online Polynomial Regression http://www.xuru.org/rt/PR.asp
		//
		// Alsa Pulse(XU3)
		// 25    4% (2621,-83.88 dB)
		// 50    7% (4587,-69.30 dB)
		// 75    8% (5242,-65.82 dB)
		// 100  14% (9175,-51.23 dB)
		let realVol = function(vol) {
			return ""+Math.floor(0.04894933333 * Math.pow(vol,3) - 8.3912 * Math.pow(vol,2) + 493.8266667 * vol - 5245)+"%";
		}
		return new AudioPulse(0,realVol);
	};

	self.createInput = function() {
		return new DevInput();
	};

	return self;
}

module.exports = Factory;

