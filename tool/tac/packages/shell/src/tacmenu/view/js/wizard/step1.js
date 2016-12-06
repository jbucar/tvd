
enyo.kind({
	name: 'Step1',
	kind: 'WizardPanel',
	prev: false,
	next: 'ACEPTO',
	stepIndex: 0,
	handlers: {
		onEndScroll: 'onDown',
	},
	components: [
		{ name: 'licence', kind: 'Licence' },
		{	// Help
			classes: 'licenceHelp',
			components: [
				{ name: 'labelPageDown', classes: 'label', content: '[PAG+]:' },
				{ name: 'pageDown', classes: 'action', content: 'AVANZAR' },
				{ name: 'labelPageUp', classes: 'label', content: '[PAG-]:' },
				{ name: 'pageUp', classes: 'action', content: 'RETROCEDER' },
			]
		}
	],
	create: function() {
		this.inherited(arguments);
		this.$.rightBtn.addClass('disabled');
	},
	onDown: function () {
		enyo.Spotlight.spot(this.$.rightBtn);
	},
	onEnter: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			enyo.Spotlight.spot(this.$.licence);
			this.$.licence.init();
		}
	},
	rightBtnAction: enyo.inherit(function(sup) {
		return function() {
			system.licenseAccepted(util.catchError(function() {
				sup.apply(this, arguments);
			}.bind(this)));
		}
	})
});

/*
 * Licence
 */
enyo.kind({
	name: 'Licence',
	classes: 'licence',
	handlers: {
		onSpotlightKeyDown: 'onSpotlightKeyDown'
	},
	published: {
		// Scroll step setted to show a new line.
		scrollStep: 44,
		// Scroll step setted to show a new page.
		scrollPage: 440,
	},
	events: {
		onEndScroll: '',
	},
	components: [
		{
			name: 'scroll',
			kind: 'ShellScroller',
			vertical: 'scroll',
			horizontal: 'hidden',
			components: [
				{
					name: 'text',
					allowHtml: true,
					spotlight: true,
					content: licence_text,
					classes: 'licence-box'
				},
			]
		}
	],
	init: function() {
		log.trace('Licence', 'init');
		// In case the creation was while the scroll was hidden
		this.$.scroll.resize();
		this.scrollTo(0);
	},
	onSpotlightKeyDown: function(oSender, oEvent) {
		switch(oEvent.keyCode) {
			case tacKeyboardLayout.VK_UP:
				this.scrollLineUp();
				return true;
			case tacKeyboardLayout.VK_DOWN:
				this.scrollLineDown();
				return true;
			case tacKeyboardLayout.VK_PAGE_UP:
				this.scrollPageUp();
				return true;
			case tacKeyboardLayout.VK_PAGE_DOWN:
				this.scrollPageDown();
				return true;
		}
	},
	scrollTo: function(verticalPos) {
		// Scrolls text to the position given.
		this.$.scroll.scrollTo(0, verticalPos, false);
		// Moves scroll indicator.
		this.$.scroll.setScrollTop(verticalPos);
	},
	scrollLineUp: function() {
		var verticalPos = this.$.scroll.getScrollBounds().top - this.scrollStep;
		this.scrollTo(verticalPos);
	},
	scrollLineDown: function() {
		var verticalPos = this.$.scroll.getScrollBounds().top + this.scrollStep;
		this.scrollTo(verticalPos);
		if (this.$.scroll.getScrollBounds().top < this.$.scroll.getScrollBounds().maxTop) {
			return true;
		} else {
			this.doEndScroll();
		}
	},
	scrollPageUp: function() {
		var verticalPos = this.$.scroll.getScrollBounds().top - this.scrollPage;
		this.scrollTo(verticalPos);
	},
	scrollPageDown: function() {
		var verticalPos = this.$.scroll.getScrollBounds().top + this.scrollPage;
		this.scrollTo(verticalPos);
		if (this.$.scroll.getScrollBounds().top < this.$.scroll.getScrollBounds().maxTop) {
			return true;
		} else {
			this.doEndScroll();
		}
	},
});
