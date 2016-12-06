/* global Carrousel */
/* exported CarrouselList */

function CarrouselList(res) {

	function focusCarrousel(self, idx, item) {
		let px = 0;
		if (idx > 0) {
			let totalHeight = 0;
			self._carrousels.forEach( function(cur, i) {
				if (i<idx && !cur.isEmpty()) {
					totalHeight -= cur.getComputedHeight();
				}
				cur.setVisible( i>=idx );
			});
			px = totalHeight;
		}
		self.set( {current: idx, transform: px} );
		return self._carrousels[idx].focus( item );
	}

	function focusPrevCarrousel(self) {
		for (let i=self.get('current')-1; i>=0; i--) {
			if (!self._carrousels[i].isEmpty()) {
				return focusCarrousel(self, i);
			}
		}
		return false;
	}

	function focusNextCarrousel(self) {
		let len = self._carrousels.length;
		for (let i=self.get('current')+1; i<len; i++) {
			if (!self._carrousels[i].isEmpty()) {
				return focusCarrousel(self, i);
			}
		}
		return false;
	}

	function onKeyDown(event) {
		log.verbose( 'CarrouselList', 'OnKeyDown: code=%d', event.original.which );

		switch (event.original.which) {
			case tacKeyboardLayout.VK_UP:
				focusPrevCarrousel(this);
				break;
			case tacKeyboardLayout.VK_DOWN:
				focusNextCarrousel(this);
				break;
			default:
				return true;
		}
		return false;
	}

	function updateCarrousels(item) {
		log.verbose( 'CarrouselList', 'updateCarrousels' );

		let cur = this.get('current');
		return this._carrousels[cur].isEmpty() ? focusNextCarrousel(this) : focusCarrousel(this, cur, item);
	}

	let prot = {
		template: res.tpl('carrousellist'),
		css: res.style('carrousellist'),
		components: ui.getComponents( Carrousel ),
	};

	prot.onconstruct = function(opts) {
		log.verbose( 'CarrouselList', 'onconstruct' );
		if (!opts.data.carrousels) {
			opts.data.carrousels = [];
		}
		opts.data.current = 0;
		opts.data.transform = 0;
		opts.data.animated = '';

		this.on('keyhandler', onKeyDown);
	};

	prot.oninit = function() {
		log.info( 'CarrouselList', 'oninit' );

		this.pendingFocus = undefined;
		this.rendered = false;
	};

	prot.oncomplete = function() {
		log.info( 'CarrouselList', 'oncomplete' );

		this._carrousels = this.findAllComponents('Carrousel', {live: true});
		assert(this._carrousels);
		this.rendered = true;
		if (this.pendingFocus !== undefined) {
			this.focus(this.pendingFocus);
			this.pendingFocus = undefined;
		}
		this.on('refreshCarrousels', updateCarrousels);
	};

	prot.onunrender = function() {
		log.info( 'CarrouselList', 'onunrender' );

		this.pendingFocus = undefined;
		this.rendered = false;
	};

	prot.focus = function(which) {
		log.verbose( 'CarrouselList', 'focus: carrousel=%d item=%d', which.carrousel, which.item );

		if (!this.rendered) {
			this.pendingFocus = which;
			let carrousels = this.get('carrousels');
			let canFocus = which.carrousel >= 0 && which.carrousel < carrousels.length;
			canFocus &= carrousels.some(csl => csl.items.length > 0);
			return canFocus;
		}

		if (which.carrousel >= 0 && which.carrousel < this._carrousels.length) {
			this.set('current', which.carrousel);
		}

		let focused = false;
		if (this._carrousels.length > 0) {
			focused = updateCarrousels.call(this, which.item);
		} else {
			log.warn( 'CarrouselList', 'No carrousel available' );
		}

		this.set('animated', 'animated');
		return focused;
	};

	prot.getCurrentPosition = function() {
		let current = this.get('current');
		return {
			carrousel: current,
			item: this._carrousels ? this._carrousels[current].get('selected') : -1,
		};
	};

	prot.getSelectedItem = function() {
		return this._carrousels ? this._carrousels[this.get('current')].getSelectedItem() : undefined;
	};

	return {
		proto: prot
	};
}
