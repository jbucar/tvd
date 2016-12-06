/* exported VirtualKeyboard */
function VirtualKeyboard(res) {

	function onKeyPress(event) {
		let propagate = true;
		switch (event.original.which) {
			case tacKeyboardLayout.VK_ENTER:
			case tacKeyboardLayout.VK_EXIT:
				let resp = {
					status: (event.original.which == tacKeyboardLayout.VK_ENTER) ? 'accepted' : 'cancelled',
					input: this.nodes.user_input_text.value,
				};
				this.nodes.user_input_text.value = '';
				log.verbose( 'VirtualKeyboard', 'resp=%j', resp );
				this.set('show', false).then( () => this.parent.showLayer(false, false) );
				notification.emit('VirtualKeyboardInput', resp);
				propagate = false;
				break;
		}

		return propagate;
	}

	function showVirtualKeyboard(evt, opts) {
		log.verbose( 'VirtualKeyboard', 'on VirtualKeyboard: evt=%j opts=%j', evt, opts );
		this.set({
			show: true,
			title: opts.title || res.txt('default_title'),
			type: opts.type || 'text',
			bgColor: opts.bgColor || 'transparent',
			bgImage: opts.bgImage || ''
		}).then(() => {
			let el = this.nodes.user_input_text;
			el.placeholder = opts.placeholder;
			el.focus();
			this.parent.showLayer( true, true );
		});
	}

	let prot = {
		template: res.tpl('virtualkeyboard'),
		css: res.style('virtualkeyboard'),
		isolated: true,
	};

	prot.onconstruct = function(opts) {
		log.verbose( 'VirtualKeyboard', 'onconstruct: opts=%j', opts );
		opts.data.show = false;
		opts.data.title = res.txt('default_title');
		opts.data.bgImage = '';
	};

	prot.oninit = function() {
		log.info( 'VirtualKeyboard', 'oninit' );

		this.on('keyHandler', onKeyPress);
		registerNotification( 'VirtualKeyboard', showVirtualKeyboard.bind(this) );
	};

	return {
		proto: prot,
	};
}
