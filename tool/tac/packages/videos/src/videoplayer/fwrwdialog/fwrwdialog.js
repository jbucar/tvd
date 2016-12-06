/* exported FFRWDialog */
function FFRWDialog(res) {

	function onKeyDown(event) {
		log.verbose( 'FFRWDialog', 'onKeyDown: code=%d', event.original.which );

		let handled = true;
		switch (event.original.which) {
			case tacKeyboardLayout.VK_PAUSE:
				this.parent.fire('infoBtnHandler', 'pause');
				break;
			case tacKeyboardLayout.VK_ENTER:
			case tacKeyboardLayout.VK_PLAY:
				this.parent.fire('infoBtnHandler', 'play');
				break;
			default:
				handled = false;
		}
		return !handled;
	}

	let prot = {
		template: res.tpl('fwrwdialog'),
		css: res.style('fwrwdialog'),
		isolated: true,
	};

	prot.onconfig = function() {
		log.info( 'FFRWDialog', 'onconfig' );

		ui.setDefaults(this, {
			show: false
		});
	};

	prot.oninit = function() {
		log.verbose( 'FFRWDialog', 'oninit' );

		this.on('keyHandler', onKeyDown );
	};

	prot.show = function(speed, focus) {
		log.verbose( 'FFRWDialog', 'show: speed=%d focus=%s', speed, focus );

		this.set({
			show: true,
			image: res.img( speed > 0 ? 'ff' : 'rw' ),
			label: Math.abs(speed) + 'x',
		});
		if (focus) {
			this.nodes.FFRWDialog.focus();
		}
	};

	prot.hide = function() {
		log.verbose( 'FFRWDialog', 'hide' );
		this.set( 'show', false );
	};

	return {
		proto: prot,
	};
}
