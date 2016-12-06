'use strict';

var assert = require('assert');

function FocusManager() {
	var _stack = [];

	function printStack() {
		return _stack.reduce(function(apps, app) {
			apps[app.info.id] = app.hasFocus();
			return apps;
		}, {});
	}

	function getTopApp() {
		if (_stack.length > 0) {
			return _stack[_stack.length-1];
		}
	}

	function stack(app) {
		log.silly( 'FocusManager', 'Stack app=%s, stack=%j', app.info.id, printStack() );

		var active = getTopApp();
		if (active) {
			active.onBlur();
		}

		// App should not be on stack
		assert( _stack.indexOf(app) === -1 );
		_stack.push(app);
		app.onFocus();
	}

	function unStack(app) {
		log.silly( 'FocusManager', 'Unstack app=%s stack=%j', app.info.id, printStack() );
		var idx = _stack.indexOf(app);
		if (idx >= 0) {
			_stack.splice(idx, 1);
		}
		// If it was the active one => give focus to the one that remains on top of the stack
		if (app.hasFocus()) {
			app.onBlur();
			var tmp = getTopApp();
			if (tmp) {
				tmp.onFocus();
			}
		}
	}

	this.onAppChanged = function(app, state) {
		switch (state) {
			case 'loaded': stack(app); break;
			case 'launching':
			case 'stopped':
				unStack(app);
				break;
		}
	};
}

module.exports = FocusManager;
