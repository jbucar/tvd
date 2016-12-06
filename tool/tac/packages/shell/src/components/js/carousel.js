enyo.kind({
	name: 'ArrowContainer',
	classes: 'arrows',
	published: {
		height: null,
		width: null,
		sideArrows: true,
	},
	bindings: [
		{ from: '.detailed', to: '.$.detailArrow.showing', transform: Boolean },
	],
	components: [
		{ classes: 'arrow center' },
		{
			name: 'rightleft',
			classes: 'right-left',
			components: [
				{ classes: 'arrow img left', kind:'enyo.Image', src: assets('arrow_up.png') },
				{ classes: 'arrow img right', kind:'enyo.Image', src: assets('arrow_up.png') },
			]
		},
		{
			name: 'detailArrow',
			classes: 'arrow center down',
			components: [
				{ kind: 'Shell.DetailArrow' },
			]
		},
	],
	heightChanged: function() {
		if (this.height) {
			this.$.rightleft.applyStyle('height', this.height + 'px');
		}
	},
	widthChanged: function() {
		if (this.width) {
			this.applyStyle('width', this.width + 'px');
		}
	},
	sideArrowsChanged: function(old) {
		this.$.rightleft.applyStyle('visibility', (this.sideArrows ? 'visible' : 'hidden') );
	}
});

enyo.kind({
	name: 'ViewMenu',
	kind: Shell.Menu,
	wrapperClasses: 'carousel-container',
	displace: 200,
	childWidth: 200, // 190 width + 10 margin-left
	calcViewPortWidth: enyo.inherit(function(sup) {
		return function(offset) {
			sup.call(this, offset !== undefined? offset : 100);
		};
	})
});

enyo.kind({
	name: 'Carousel',
	classes: 'carousel',
	spotlight: 'container',
	published: {
		detailed: true,
		sideArrows: true,
		height: 310,
		width: 340,
		heightMultiplier: 2,
		blocked: false,
		itemsPerPage: 5,
		minCounterStartShowing: 5
	},
	handlers: {
		onSpotlightContainerEnter: 'onEnter',
		onSpotlightContainerLeave: 'onLeave',
		onSpotlightUp: 'onUp',
		onSpotlightDown: 'onDown',
		onPretranslate: 'onPreTranslate',
		onCanceltranslate: 'onCancelTranslate',
		onRequestScroll: 'scrollRequested',
	},
	bindings: [
		{ from: '.detailed', to: '.$.arrows.detailed' },
		{ from: '.sideArrows', to: '.$.arrows.sideArrows' },
		{ from: '.height', to: '.$.arrows.height' },
		{ from: '.width', to: '.$.arrows.width' },
		{ from: '.$.container.length', to: '.length' },
		{ from: '.blocked', to: '.$.container.blocked' }
	],
	components: [
		{
			name: 'arrowsWrapper',
			classes: 'arrows-wrapper',
			components: [
				{ name: 'arrows', kind: 'ArrowContainer', style: 'opacity: 0' },
			]
		},
		{ name: 'posIndicator', classes: 'pos-indicator', showing: false},
		{
			name: 'wrapper',
			classes: 'container-wrapper',
			components: [
				{ name: 'container', kind: 'ViewMenu' }
			]
		},
		{ name: 'legend', kind: 'components.BackLegend' },
		{ kind: "Animator", duration: 500, onStep: 'animatorStep', onStop: 'animStop', onEnd: 'growEnded' },
		{ name: 'animator2', kind: "Animator", duration: 500, onStep: 'animatorStep', onStop: 'animStop'}
 	],
	create: function() {
		this.inherited(arguments);
		this.heightChanged();
	},
	heightChanged: function() {
		this.$.wrapper.applyStyle('height', this.height + 'px');
		this.$.arrowsWrapper.applyStyle('height', (this.height + 140) + 'px');
	},
	resetToFirst: function() {
		this.$.container.resetToFirst();
	},
	current: function() {
		return this.$.container.first();
	},
	hasChildren: function() {
		return this.$.container.hasChildren();
	},
	onUp: function(oSender, oEvent) {
		return this.$.container.isAnimating();
	},
	onDown: function(oSender, oEvent) {
		return this.$.container.isAnimating();
	},
	onEnter: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			log.trace('Carousel', 'onEnter');
			this.$.container.calcViewPortWidth(); // Grow item
			this.addClass('selected');
		}
	},
	onLeave: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			log.trace('Carousel', 'onLeave');
			this.$.container.calcViewPortWidth(0);
			this.removeClass('selected');
			this.shrinkCurrent();
			this.$.animator.stop();
			this._current = undefined;
		}
	},
	scrollRequested: function(oSender, oEvent) {
		log.trace('Carousel', 'scrollRequested', 'originator=%s', oEvent.originator.name);

		this.$.animator.stop();
		this.$.animator.play({
			startValue: 0,
			endValue: 50,
			node: oEvent.originator.hasNode(),
			targetItem: oEvent.originator,
		});

		this._current = oEvent.originator;
		this.updatePos();
	},
	scroll: function() {
		this.$.container.scroll.apply(this.$.container, arguments);
	},
	lengthChanged: function() {
		var l = this.length;
		if (this.$.posIndicator) {
			if (l > this.minCounterStartShowing) {
				this.$.posIndicator.setShowing(true);
				if (this._current) this.updatePos();
			} else {
				this.$.posIndicator.setShowing(false);
			}
		}

		this.addRemoveClass('center', l < this.itemsPerPage);
	},
	updatePos: function() {
		if (this.$.posIndicator) {
			this.$.posIndicator.setContent((this._current.idx + 1) + '/' + this.length);
		}
	},
	currentIdx: function() {
		return this._current.idx;
	},
	animatorStep: function(oSender) {
		if (oSender.targetItem && oSender.targetItem.$.wrapper) {
			// TODO: Dont break the encapsulation. Make a card kind.
			oSender.targetItem.applyStyle("min-width", (190 + oSender.value) + 'px');
			oSender.targetItem.applyStyle("width", (190 + oSender.value) + 'px');
			oSender.targetItem.applyStyle("height", (190 + oSender.value * this.heightMultiplier) + 'px');
		}
	},
	animStop: function(oSender) {
		if (oSender.targetItem && oSender.targetItem.$.wrapper) {
			// TODO: Dont break the encapsulation. Make a card kind.
			oSender.targetItem.applyStyle("min-width", '190px');
			oSender.targetItem.applyStyle("width", '190px');
			oSender.targetItem.applyStyle("height", '190px');
		}
	},
	growEnded: function(oSender) {
		log.trace('Carousel', 'growEnd');
		this.$.arrows.applyStyle('opacity', 1);
		oSender.targetItem.set('highlighted', true);
	},
	shrinkCurrent: function() {
		if (this._current && !this._current.destroyed) {
			this.$.animator2.play({
				startValue: 50,
				endValue: 0,
				node: this._current,
				targetItem: this._current
			});

			this._current.set('highlighted', false);
		}
	},
	scrollTo: function() {
		return this.$.container.scrollTo.apply(this.$.container, arguments);
	},
	onCancelTranslate: function(oSender, oEvent) {
		log.trace('Carousel', 'onCancelTranslate');
		this.$.animator.stop();
		this.$.animator2.stop();
	},
	onPreTranslate: function(oSender, oEvent) {
		log.trace('Carousel', 'onPreTranslate');
		this.$.arrows.applyStyle('opacity', 0);
		this.shrinkCurrent();
	}
});


enyo.kind({
	name: 'CarouselItem',
	spotlight: true,
	classes: 'carousel-item',
	model: undefined,
	handlers: {
		onSpotlightFocus: 'onFocus',
		onSpotlightBlur: 'onBlur'
	},
	published: {
		highlighted: false
	},
	iconBar: [],
	components: [
		{
			name: 'wrapper',
			classes: 'carousel-item-wrapper',
			components: [
				{ name: 'iconsBar', classes: 'icons-wrapper' },
				{ name: 'img', kind: enyo.Image, classes: 'carousel-item-image', src: assets('update-tac.png') },
				{ name: 'txt',
					classes: 'carousel-item-txt',
					showing: false,
					kind: 'moon.MarqueeText',
					mixins: ['moon.MarqueeSupport'],
					marqueeSpeed: 50,
					centered: true,
				}
			]
		}
	],
	create: enyo.inherit(function(sup) {
		return function() {
			sup.apply(this, arguments);
			if (this.iconBar.length > 0) {
				this.$.iconsBar.createComponents(this.iconBar, {owner: this});
				this.$.iconsBar.getClientControls().forEach(function(c) {
					if (c.showOnSelected) {
						c.addClass('show-on-selected');
					}
				});
			}
		};
	}),
	onFocus: function(oSender, oEvent) {
		this.bubble('onRequestScroll', {dir: oEvent.direction || oEvent.dir});
		this.$.txt.addClass( 'selected ' + Context.current() );
		this.$.txt.show();
		this.$.txt.startMarquee();
	},
	onBlur: function(oSender, oEvent) {
		this.$.txt.stopMarquee();
		this.$.txt.hide();
		this.$.txt.removeClass( 'selected ' + Context.current() );
	},
	highlightedChanged: function(was, is) {
		this.addRemoveClass('highlight', is);
		this.$.img.applyStyle('transition', is? 'transform .3s' : null);
		this.$.img.applyStyle('transform', is? 'scale(1.3)' : null);
	}
});
