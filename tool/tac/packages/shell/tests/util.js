var testutil = (function(exports) {
	exports = exports || {};

	function sendKeyDown(k) {
		return sendKeyEvt(k, 'down');
	}

	function sendKeyUp(k) {
		return sendKeyEvt(k, 'up');
	}

	function sendKeyEvt(k, dir) {
		var oEvent = document.createEvent('KeyboardEvent');

		// Chromium Hack
		Object.defineProperty(oEvent, 'keyCode', {
			get : function() {
				return this.keyCodeVal;
			}
		});

		Object.defineProperty(oEvent, 'which', {
			get : function() {
				return this.keyCodeVal;
			}
		});

		oEvent.initKeyboardEvent("key"+dir, true, true, window, true, true, true, true, k, k);

		oEvent.keyCodeVal = k;

		if (oEvent.keyCode !== k) {
			throw new Error("sendKeyEvt Error: keyCode mismatch " + oEvent.keyCode + "(" + oEvent.which + ")");
		}

		document.dispatchEvent(oEvent);

		return oEvent;
	}

	exports.sendkey = function(k) {
		sendKeyDown(k);
		sendKeyUp(k);
	}

	exports.noop = function () { return; }
	exports.true = function () { return true; }

	return exports;
}());

// Vars
var keys = tacKeyboardLayout;
var spotlight = enyo.Spotlight;
var should;
var app;

var navigate = (function(exports, util) {
	exports = exports || {};

	exports.right = testutil.sendkey.bind(undefined, keys.VK_RIGHT);
	exports.left = testutil.sendkey.bind(undefined, keys.VK_LEFT);
	exports.down = testutil.sendkey.bind(undefined, keys.VK_DOWN);
	exports.up = testutil.sendkey.bind(undefined, keys.VK_UP);
	exports.enter = testutil.sendkey.bind(undefined, keys.VK_ENTER);
	exports.back = testutil.sendkey.bind(undefined, keys.VK_BACK);

	return exports;
}(undefined, util));

// mocha.setup({ui:'bdd', reporter:WebConsole});
mocha.setup('bdd');

enyo.kind({
	name: 'AppTestContainer',
	style: 'height: 300px',
	components: [],
});

var test = {
	init: function(nodeName) {
		should = chai.should();

		// enyo.setLogLevel(-1);
		spotlight.disablePointerMode();
		spotlight.Accelerator.frequency = [1, 1, 1, 1, 1, 1, 1];
		app = new AppTestContainer().renderInto(nodeName);
	}
}
