/* exported Dialog */

function Dialog(res) {

	function onKeyPress(event, focusIdx, accepted) {
		switch (event.original.which) {
			case tacKeyboardLayout.VK_UP:
			case tacKeyboardLayout.VK_DOWN:
				this.focusContent( event.original.which == tacKeyboardLayout.VK_DOWN );
				break;
			case tacKeyboardLayout.VK_LEFT:
			case tacKeyboardLayout.VK_RIGHT:
				this.lastBtnFocused = focusIdx;
				this.focusButton();
				break;
			case tacKeyboardLayout.VK_EXIT:
			case tacKeyboardLayout.VK_BACK:
			case tacKeyboardLayout.VK_ENTER:
				this.onResult( event.original.which == tacKeyboardLayout.VK_ENTER ? accepted : false );
				break;
			default:
				return true;
		}

		return false;
	}

	let prot = {
		template: res.tpl('dialog'),
		css: res.style('dialog')
	};

	prot.onconfig = function() {
		log.info( 'Dialog', 'onconfig');
		ui.setDefaults( this, {
			dialogTitleColor: 'white',
			hasButtons: true,
			positive: res.txt('accept'),
			negative: res.txt('cancel')
		});
	};

	prot.oninit = function() {
		log.info( 'Dialog', 'oninit');
		this.on('btnKeyHandler', onKeyPress);
	};

	prot.oncomplete = function() {
		log.info( 'Dialog', 'oncomplete');
		this.lastBtnFocused = 0;
		this.focusButton();
	};

	// The following methods are called and/or overriden by subclasses

	prot.onResult = function(result) {
		this.fire('result', result);
	};

	prot.focusContent = function(top) {
		log.verbose( 'Dialog', 'focusContent: top=%s', top );
	};

	prot.focusButton = function() {
		log.verbose( 'Dialog', 'focusButton' );
		if (this.get('hasButtons')) {
			let btns = this.nodes.buttons.children;
			btns[btns.length > 1 ? this.lastBtnFocused : 0].focus();
		}
	};

	return {
		proto: prot
	};
}
