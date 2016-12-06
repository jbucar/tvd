/* exported Carrousel */

function Carrousel(res) {

	function onKeydown(event) {
		log.verbose( 'Carrousel', 'onKeydown[%s]: code=%s', this.get('title'), event.original.which);
		let count = this.get('items').length;

		let selected = this.get('selected');
		switch (event.original.which) {
			case tacKeyboardLayout.VK_RIGHT:
				if (selected < count-1) {
					selected++;
				} else {
					return true;
				}
				break;
			case tacKeyboardLayout.VK_LEFT:
				if (selected > 0) {
					selected--;
				} else {
					return true;
				}
				break;
			case tacKeyboardLayout.VK_ENTER:
				let item = this.get('items')[selected];
				if (item.onSelected) {
					item.onSelected( item );
				}
				let onSelected = this.get('onSelected');
				if (onSelected) {
					onSelected( item );
				}
				break;
			default:
				return true;
		}

		this.set('selected', selected);
		return false;
	}

	function onFocusChanged(event, focus) {
		log.verbose( 'Carrousel', 'onFocusChanged[%s] focus=%s', this.get('title'), focus);
		this.set('focused', focus);
	}

	function onSelectedChange(value, oldValue, keyPath) {
		log.verbose( 'Carrousel', 'onSelectedChange[%s]: %s change from "%s" to "%s"', this.get('title'), keyPath, oldValue, value );
		let px = 0;
		if (value >= 2) {
			let upperLimit = this.get('items').length - 2;
			px = getTotalItemWidth(this) * ((value >= upperLimit) ? upperLimit-3 : value-2);
		}
		if (this.nodes.carrousel_view) {
			this.nodes.carrousel_view.style.transform = 'translate(-' + px + 'px)';
		}
		this.fire( 'itemFocused', this.get('items')[value] );
	}

	function getTotalItemWidth(self) {
		if (!self.totalItemWidth) {
			let item = self.nodes.carrousel_view.firstChild;
			let css = window.getComputedStyle(item, null);
			let w = item.clientWidth +
				parseInt(css['margin-left']) +
				parseInt(css['margin-right']) +
				parseInt(css['padding-left']) +
				parseInt(css['padding-right']) +
				parseInt(css['border-width']);
			self.totalItemWidth = w;
		}
		return self.totalItemWidth;
	}

	let prot = {
		template: res.tpl('carrousel'),
		css: res.style('carrousel'),
	};

	prot.focus = function(idx) {
		let el = this.nodes.carrousel;
		let focused = false;
		if (el) {
			log.verbose( 'Carrousel', 'focus[%s]', this.get('title') );
			let selected = this.get('selected');
			let count = this.get('items').length;
			if (count>0) {
				if (idx !== undefined) {
					let validIdx = (idx < count) ? idx : count - 1;
					selected = (validIdx < 0) ? 0 : validIdx;
				}
				this.set({
					visible: true,
					selected: selected,
				});
				el.focus();
				focused = true;
			}
		} else {
			log.warn( 'Carrousel', 'Fail to focus carrousel[%s]', this.get('title') );
		}
		return focused;
	};

	prot.setVisible = function(visible) {
		this.set({ visible: visible });
	};

	prot.isEmpty = function() {
		return this.get('items').length === 0;
	};

	prot.getComputedHeight = function() {
		if (!this.computedHeight) {
			let item = this.nodes.carrousel;
			let css = window.getComputedStyle(item, null);
			this.computedHeight = item.clientHeight;
			this.computedHeight += parseInt(css['margin-top']);
			this.computedHeight += parseInt(css['margin-bottom']);
			this.computedHeight += parseInt(css['padding-top']);
			this.computedHeight += parseInt(css['padding-bottom']);
			this.computedHeight += parseInt(css['border-width']);
		}
		return this.computedHeight;
	};

	prot.getSelectedItem = function() {
		return this.get('items')[this.get('selected')];
	};

	prot.onconstruct = function(opts) {
		log.verbose( 'Carrousel', 'onconstruct' );

		if (!opts.data.title) {
			opts.data.title = 'UNTITLED';
		}
		if (!opts.data.items) {
			opts.data.items = [];
		}
		opts.data.title_color = 'white';
		opts.data.title_font = 'Oswald-Light';
		opts.data.title_size = '1.583rem';
		opts.data.transform = 0;
		opts.data.selected = 0;
		opts.data.focused = false;
		opts.data.visible = true;
		opts.data.getSelected = function(idx) {
			return ((this.get('selected') == idx) && this.get('focused')) ? 'selected' : '';
		};
	};

	prot.oninit = function() {
		log.info( 'Carrousel', 'oninit[%s]', this.get('title') );

		this.on({
			keyhandler: onKeydown,
			focusHandler: onFocusChanged,
		});
		this.observe('selected', onSelectedChange);
		this.observe('items', () => {
			if (this.get('selected') >= this.get('items').length) {
				this.subtract('selected');
			}
			this.fire('itemsChanged');
		});
	};

	prot.oncomplete = function() {
		log.info( 'Carrousel', 'oncomplete[%s]', this.get('title') );
		this.fire( 'itemFocused', this.getSelectedItem() );
	};

	prot.onrender = function() {
		this.resetPartial('item', this.get('tpl'));
	};

	prot.onunrender = function() {
		this.resetPartial('item', '');
	};

	return {proto: prot};
}
