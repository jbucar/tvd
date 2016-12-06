var fs = require('fs');
var path = require('path');
var linuxinput = require('./linuxinput');

function listenPowerOff( reg ) {
	var inputs = [];
	var input_dir = '/dev/input';
	var files = fs.readdirSync( input_dir );

	function onInputEvt(evt) {
		log.silly( 'aml_power', 'evt=(type=%d,code=%d,value=%d)', evt.type, evt.code, evt.value );
		if (evt.type === 1 && evt.code === 116 && evt.value === 1) {
			//	EV_KEY && KEY_POWER && KEY_PRESS
			reg.get('system').powerOff();
		}
	}

	for (var x=0; x<files.length; x++) {
		var f = path.join(input_dir,files[x]);
		if (f.search( 'event') == -1) {
			continue;
		}

		//	Add input
		inputs.push(new linuxinput(f, onInputEvt));
	}

	return inputs;
}

function removePowerOff( inputs ) {
	for (var x=0; x<inputs.length; x++) {
		inputs[x].close();
	}
	inputs = undefined;
}

//	Service entry point
function aml_power() {
	var _adapter = null;
	var _inputs = {};

	//	Hooks
	var self = {};

	self.onLoad = function(adapter) {
		_adapter = adapter;
		return true;
	};

	self.onStart = function() {
		_inputs = listenPowerOff(_adapter.registry());
		return {};
	};

	self.onStop = function(cb) {
		removePowerOff( _inputs );
		cb();	//	TODO: Fix
	};

	return Object.freeze(self);
}

module.exports = aml_power;
