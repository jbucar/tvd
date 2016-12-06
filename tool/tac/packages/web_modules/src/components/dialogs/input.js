/* globals Dialog */
/* exported InputDialog */
function InputDialog(res) {

	function onInput(idx, input, data) {
		if (data.status == 'accepted') {
			input.value = data.input;
		}
		this.nodes[this.get('inputs.' + idx + '.name')].focus();
	}

	function onKeyDown(event, idx) {
		switch (event.original.which) {
			case tacKeyboardLayout.VK_UP:
			case tacKeyboardLayout.VK_DOWN:
				let i = idx + (event.original.which == tacKeyboardLayout.VK_UP ? -1 : 1);
				if (i<0 || i>=this.get('inputs').length) {
					this.focusButton();
				} else {
					this.nodes[this.get('inputs.' + i + '.name')].focus();
				}
				break;
			case tacKeyboardLayout.VK_ENTER:
				let input = this.get('inputs.' + idx);
				apiCallAsync( appmgr.showVirtualKeyboard, {
					title: input.label,
					placeholder: input.placeholder,
					type: input.type || 'text',
					bgColor: this.get('theme.bgColor'),
					bgImage: this.get('ThemeVirtualKeyboardBgImage'),
				} ).then( onInput.bind(this, idx, input) );
				break;
			case tacKeyboardLayout.VK_BACK:
			case tacKeyboardLayout.VK_EXIT:
				this.onResult( false );
				break;
			default:
				return true;
		}
		return false;
	}

	let prot = {
		css: res.style('input'),
	};

	prot.onconstruct = function(opts) {
		log.verbose( 'InputDialog', 'onconstruct: opts=%j', opts);
		this._super(opts);
		opts.partials.content = res.tpl('input');
	};

	prot.oninit = function() {
		log.info( 'InputDialog', 'oncomplete');
		this._super();
		this.on('inputKeyHandler', onKeyDown);
	};

	prot.oncomplete = function() {
		log.info( 'InputDialog', 'oncomplete');
		this._super();
		let inputs = this.get('inputs');
		inputs.forEach(input => input.value = '');
		this.nodes[inputs[0].name].focus();
	};

	// override
	prot.onResult = function(result) {
		log.info( 'InputDialog', 'onResult: result=%s', result);
		let tmp = result;
		let waitingValidation = false;
		if (result) {
			let inputs = this.get('inputs');
			tmp = {};
			for (let i in inputs) {
				tmp[inputs[i].name] = inputs[i].value;
			}

			let validator = this.get('validator');
			if (validator) {
				waitingValidation = true;
				validator(tmp, err => {
					this.set('error', err);
					if (err !== undefined) {
						this.focusContent(true);
					} else {
						this._super( tmp );
					}
				});
			}
		}

		if (!waitingValidation) {
			this._super( tmp );
		}
	};

	// override
	prot.focusContent = function(top) {
		let inputs = this.get('inputs');
		this.nodes[inputs[top ? 0 : inputs.length - 1].name].focus();
	};

	return {
		extends: Dialog,
		proto: prot
	};
}
