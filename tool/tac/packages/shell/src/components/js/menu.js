var menu_helper = {
	add: function(ph, c) {
		var d = enyo.clone(c);
		d.placeholder = true;
		d.spotlight = false;
		d.addBefore = undefined;
		d.spotlightDisable = true;
		var comp = ph.createComponent(d, {owner: ph});
		comp.applyStyle('display', 'none');
		comp.render();
	},
	remove: function(ph, pos) {
		ph.getComponents()[pos].destroy();
	}
};

enyo.kind({
	name: 'Shell.Menu',
	spotlight: 'container',
	classes: 'shell-menu',
	published: {
		displace: 0,
		wrapperClasses: '',
		circular: true,
		length: 0,
		blocked: false
	},
	events: {
		onPretranslate: '',
		onPosttranslate: '',
		onCanceltranslate: '',
		onResetMenu: ''
	},
	handlers: {
		onRequestScroll: 'scrollRequested',
		onSpotlightContainerLeave: 'onLeave',
		onSpotlightLeft: 'onLeft',
		onSpotlightRight: 'onRight'
	},
	components: [
		{
			name: 'view-port', classes: 'view-port', components: [
				{ name: 'client', classes: 'client' }
			],
		},
		{ kind: "Animator", duration: 500, onStep: 'animationStep', onEnd: "animationCompleted", onStop: 'animationStopped' },
	],
	placeHolder: { classes: 'placeholder', spotlight: false },
	originals: [],
	memory: true,
	onLeft: function(oSender, oEvent) {
		// Block left navigation when menu is blocked.
		return this.originals.length === 1 || this.blocked;
	},
	onRight: function(oSender, oEvent) {
		// Block right navigation when menu is blocked.
		return this.originals.length === 1 || this.blocked;
	},
	scrollRequested: function(oSender, oEvent) {
		log.trace('Menu', 'scrollRequested', 'originator=%s', oEvent.originator.name);
		if (this.originals.length === 1 || this.blocked) {
			return;
		}

		this._current = oEvent.originator;

		if (oEvent.dir === 'RIGHT') {
			this.scrollRight(true);
		}
		else if (oEvent.dir === 'LEFT') {
			this.scrollLeft(true);
		}
		else if (oEvent.originator !== this.originals[0]) {
			this.resetToFirst();
			for(var i=0; i < oEvent.originator.idx; i++) {
				this.scrollRight(false);
			}
		}
	},
	create: function() {
		this.inherited(arguments);
		this.$.client.addClass(this.wrapperClasses);

		this.ph0 = this.createComponent(enyo.clone(this.placeHolder), {owner: this, isChrome: true});
		this.ph0.applyStyle('width', this.childWidth + 'px');
		this.ph0.applyStyle('min-width', this.childWidth + 'px');
		this.ph0.applyStyle('order', '-100');
		this.ph0.hide();

		this.ph1 = this.createComponent(enyo.clone(this.placeHolder), {owner: this, isChrome: true});
		this.ph1.applyStyle('width', this.childWidth + 'px');
		this.ph1.applyStyle('min-width', this.childWidth + 'px');
		this.ph1.applyStyle('order', '1000');
		this.ph1.hide();
	},
	_resetPosition: function() {
		this.originals.forEach(function(c, idx) {
			c.order = 0;
			c.applyStyle('order', 0);
		});

		this.sort();
	},
	_resetSpot: function() {
		var originals = this.originals;
		this.originals.forEach( function(c, idx) {
			var l = idx > 0 ? originals[idx - 1] : originals[originals.length -1];
			var r = idx === originals.length-1 ? originals[0] : originals[idx+1];
			c.defaultSpotlightLeft = l.name;
			c.defaultSpotlightRight = r.name;
			c.defaultSpotlightDisappear = r.name;
			c.idx = idx;
			c.order = 0;
		});
	},
	stabilize: function() {
		if (this._current && !this._current.destroyed) {
			while(this.first() !== this._current) {
				this.scrollRight();
			}
			util.forceSetLasFocusedChild(this, this._current);
			return true;
		}

		return false;
	},
	resetToFirst: function() {
		this._resetPosition();
		this._resetSpot();
		util.forceSetLasFocusedChild(this, this.originals[0]);
	},
	createComponents: function(comps, ext) {
		if (this.circular && comps && comps.length && !ext.isChrome && this.originals) {
			this._resetPosition();
		}

		var res = this.inherited(arguments);

		if (this.circular && comps && comps.length && !ext.isChrome) {
			this.originals = this.getClientControls();
			this._resetSpot();

			this.calcViewPortWidth();
			this.set('length', this.originals.length);

			comps.forEach(menu_helper.add.bind(undefined, this.ph0));
			this.selectPlaceHolder(this.ph0, this.originals.length-1);

			comps.forEach(menu_helper.add.bind(undefined, this.ph1));
			this.selectPlaceHolder(this.ph1, 0);

			util.forceSetLasFocusedChild(this, this.originals[0]);

			this.doResetMenu();
		}

		return res;
	},
	destroyComponent: function(c) {
		var idx;
		if (this.circular) {
			this._resetPosition();
			idx = c.idx;
		}
		c.destroy();
		if (this.circular) {
			this.originals = this.getClientControls();

			this._resetSpot();
			this.calcViewPortWidth();
			this.set('length', this.originals.length);

			menu_helper.remove(this.ph0, idx);
			menu_helper.remove(this.ph1, idx);

			this.selectPlaceHolder(this.ph0, this.originals.length-1);
			this.selectPlaceHolder(this.ph1, 0);

			util.forceSetLasFocusedChild(this, this.originals[0]);
		}

	},
	createComponent: function(comp, ext) {
		if (this.circular && comp && !ext.isChrome && this.originals) {
			this._resetPosition();
		}

		var c = this.inherited(arguments);
		if (this.circular && c && !ext.isChrome) {
			this.originals = this.getClientControls();
			this._resetSpot();

			this.calcViewPortWidth();
			this.set('length', this.originals.length);

			menu_helper.add(this.ph0, comp);
			menu_helper.add(this.ph1, comp);

			this.selectPlaceHolder(this.ph0, this.originals.length-1);
			this.selectPlaceHolder(this.ph1, 0);

			util.forceSetLasFocusedChild(this, this.originals[0]);
		}

		return c;
	},
	calcViewPortWidth: function(offset) {
		this.$['view-port'].applyStyle('width', ((offset || 0) + (this.childWidth * this.originals.length)) + 'px');
	},
	selectPlaceHolder: function(ph, id) {
		ph.getComponents().forEach(function(c, i) {
			c.applyStyle('display', (i === id) ? 'flex' : 'none');
		});
	},
	onLeave: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			if (this.$.animator.isAnimating()) {
				this.$.animator.stop();
			}
			this.ph0.hide();
			this.ph1.hide();
		}
	},
	isAnimating: function() {
		return this.$.animator.isAnimating();
	},
	scrollRight: function(animate) {
		var preElement = this.originals[0];
		this.$.animator.stop();
		this.reorder();
		this.originals[0].applyStyle('order', '2');
		this.originals[0].order = 2;
		this.sort();

		this.selectPlaceHolder(this.ph0, this.originals[this.originals.length - 1].idx);
		this.ph1.hide();
		this.ph0.show();

		if (animate) {
			this.doPretranslate({originator: preElement});
			this.$.animator.play({
				startValue: 0,
				endValue: this.displace * -1,
				node: this.$.client.hasNode()
			});
		}
		else {
			this.reset();
		}
	},
	scrollLeft: function(animate) {
		var preElement = this.originals[0];
		this.$.animator.stop();
		this.reorder();
		this.originals[this.originals.length - 1].applyStyle('order', '-1');
		this.originals[this.originals.length - 1].order = -1;
		this.sort();

		this.selectPlaceHolder(this.ph1, this.originals[0].idx);
		this.ph1.show();

		this.$.client.applyStyle("transform", 'translate(' + (this.displace * -1) + 'px, 0)');

		if (animate) {
			this.doPretranslate({originator: preElement});
			this.$.animator.play({
				startValue: this.displace * -1,
				endValue: 0,
				node: this.$.client.hasNode()
			});
		}
		else {
			this.reset();
		}
	},
	scrollToControl: function(ctrol) {
		if (ctrol) {
			enyo.Spotlight.spot(ctrol, 'RIGHT');
			return true;
		}
		return false;
	},
	scrollTo: function(finder) {
		var ctrol = this.originals.find(finder);
		return this.scrollToControl(ctrol);
	},
	animationStep: function(oSender) {
		this.$.client.applyStyle("transform", 'translate(' + oSender.value + 'px, 0)');
	},
	animationCompleted: function(oSender, oEvent) {
		this.reset();
		this.doPosttranslate({originator: this.originals[0]});
	},
	animationStopped: function() {
		this.reset();
		this.doCanceltranslate({originator: this.originals[0]});
	},
	sort: function() {
		this.originals.sort(function(a, b) {
			return (a.order !== b.order) ? a.order - b.order : a.idx - b.idx;
		});
	},
	first: function() {
		return this.originals[0];
	},
	hasChildren: function() {
		return (this.originals !== undefined) && (this.getClientControls().length > 0);
	},
	reset: function() {
		this.ph0.hide();
		if (this.$.client) {
			this.$.client.applyStyle("transform", 'translate(0px, 0px)');
		}
	},
	unscroll: function() {
		this.originals.sort(function(a, b) {
			return a.idx - b.idx;
		});

		this.originals.forEach(function(c) {
			c.applyStyle('order', '0');
			c.order = 0;
		});

		util.forceSetLasFocusedChild(this, this.originals[0]);
		this.reset();
	},
	scroll: function(offset, direction) {
		log.trace('Menu', 'scroll', 'offset=%s, direction=%s current=%s', offset, direction, this.originals[0].idx);
		var method = direction === 'RIGHT' ? 'scrollRight' : 'scrollLeft';
		for(var i = 0; i < offset; i++ ) {
			this[method](false); // Don't animate.
		}
		log.debug('Menu', 'scroll', 'setLastFocusedChild=%s', this.originals[0].model.get('name'));
		util.forceSetLasFocusedChild(this, this.originals[0]);
	},
	reorder: function() {
		if (this.originals[0].idx === 0) {
			this.originals.forEach(function(c) {
				c.applyStyle('order', '0');
				c.order = 0;
			});
		}
	},
	destroyClientControls: function() {
		this.ph0.destroyComponents();
		this.ph1.destroyComponents();
		this.set('length', 0);
		return this.inherited(arguments);
	}
});
