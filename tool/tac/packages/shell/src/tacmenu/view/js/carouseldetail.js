(function() {
	'use strict';

	enyo.kind({
		name: 'CarouselDetail',
		style: 'height: 100%; width: 100%',
		published: {
			collection: undefined,
			componentView: undefined,
		},
		bindings: [
			{ from: '.carouselItemView', to: '.$.carousel.componentView' },
			{ from: '.collection', to: '.$.carousel.collection' },
			{ from: '.sortedBy', to: '.$.carousel.sortedBy' },
			{ from: '.detailItemView', to: '.$.detail.innerKind' },
			{ from: '.collection', to: '.$.detail.collection' },
			{ from: '.sortedBy', to: '.$.detail.sortedBy' },
		],
		handlers: {
			onSpotlightKeyDown: 'onKey',
			onIdxChanged: '_detailNavigated',
			onCollectionChanged: '_collectionChanged'
		},
		components: [
			{
				style: 'height: 100%; width: 100%',
				classes: 'flex horizontal center',
				components: [
					{
						name: 'carousel',
						kind: 'Shell.CollectionAwareCarousel',
						onSpotlightDown: 'onCarouselDown',
						onSpotlightContainerEnter: 'onCarouselEnter',
						style: 'height: 100%;'
					}
				]
			},
			{
				name: 'detail',
				kind: 'DetailPanels',
				onSpotlightContainerLeave: 'onDetailLeave',
				onSpotlightContainerEnter: 'onDetailEnter',
				showing: false,
				onSpotLost: '_detailSpotLosted'
			},
			{
				name: 'anim', kind: "Animator",
				easingFunction: enyo.easing.linear, duration: 500,
				onStep: 'animStep', onStop: 'animStop', onEnd: 'animStop'
			}
		],
		canEnter: function() {
			return this.get('collection').length !== 0;
		},
		reset: function() {
			this.$.anim.stop();
			// this.applyStyle('transition', null);
			this.applyStyle('transform', null);
			this.$.detail.hide();
		},
		onKey: function(sender, evt) {
			var back = (evt.keyCode === tacKeyboardLayout.VK_EXIT || evt.keyCode === tacKeyboardLayout.VK_BACK);
			if (sender === this.$.detail && back) {
				enyo.Spotlight.spot(this.$.carousel);
				return true;
			}
		},
		onCarouselEnter: function(/*sender*/ /*, evt */) {
			this._setHelp('carousel');
		},
		showDetail: function() {
			this.onCarouselDown();
		},
		onCarouselDown: function(/* sender */ /*, evt*/) {
			// If animating dont allow to down.
			if (this.$.anim.isAnimating()) {
				return true;
			}

			var detail = this.$.detail;

			var idx = this.$.carousel.currentIdx();
			detail.show();
			enyo.Spotlight.spot(detail);
			detail.setIdx(idx);

			this.$.anim.play({
				startValue: 0,
				endValue: 100,
				node: this.hasNode()
			});

			return true;
		},
		onDetailLeave: function(sender, evt) {
			if (evt.originator === this.$.detail) {
				this.$.detail.reset();
				this.$.anim.play({
					startValue: 100,
					endValue: 0,
					node: this.hasNode()
				});
			}
		},
		onDetailEnter: function(/*sender*/ /*, evt */) {
			this._setHelp('detail');
		},
		_setHelp: function(help) {
			if (this.help && (help in this.help)) {
				Context.setHelp(this.help[help]);
			}
		},
		animStep: function(sender /*, evt */) {
			this.applyStyle('transform', 'translateY(-{0}%)'.format(sender.value));
		},
		animStop: function(sender /*, evt */) {
			if (sender.endValue === 0 && this.$.detail) {
				this.$.detail.hide();
			}
		},
		_collectionChanged: function(sender, evt) {
			var cur = enyo.Spotlight.getCurrent();
			if (evt.empty && cur.destroyed) {
				this.bubble('onSpotLost', {originator: this});
				return true;
			}
		},
		_detailNavigated: function(sender, evt) {
			log.trace('CarouselDetail', 'detailNavigated', 'was=%s', evt.was);
			var cur = enyo.Spotlight.getCurrent();
			if(evt.was !== undefined) {
				this.$.carousel.scroll(1, evt.direction);
			}
			return true;
		},
		_detailSpotLosted: function(sender, evt) {
			if (!this.destroyed && this && this.$ && this.$.carousel && enyo.Spotlight.isSpottable(this.$.carousel)) {
				return enyo.Spotlight.spot(this.$.carousel);
			}
			return false;
		}
	});

})();
