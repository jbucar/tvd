/* exported PopupBox */

function PopupBox(res) {

	let prot = {
		template: res.tpl('popupbox'),
		css: res.style('popupbox'),
	};

	prot.onconstruct = function(opts) {
		log.verbose( 'PopupBox', 'onconstruct: opts=%j', opts);
		if (opts.data.visible === undefined) {
			opts.data.visible = false;
		}
		if (opts.data.orientation === undefined) {
			opts.data.orientation = 'ltr';
		}
		if (opts.data.delay === undefined) {
			opts.data.delay = 1000;
		}
		if (opts.data.boxBgColor === undefined) {
			opts.data.boxBgColor = 'transparent';
		}
		if (opts.data.boxInitialPos === undefined) {
			opts.data.boxInitialPos = "0";
		}
	};

	return {
		proto: prot
	};
}
