(function(exports) {
	'use strict';

	exports.FadeSupport = {
		name: 'FadeSupport',
		constructor: enyo.inherit(function (sup) {
			return function () {
				sup.apply(this, arguments);
				this._fadeOriginalShowChanging = this.showingChanged;
				this.extend({
					showingChanged: enyo.inherit(function(sup) {
						return function(was, is) {
							if (this.$._fadeAnimator.isAnimating()) {
								this.$._fadeAnimator.stop();
							}

							if (this.fadeDisabled) {
								return sup.apply(this, arguments);
							}

							if (is) {
								sup.apply(this, arguments);
							}

							this.$._fadeAnimator.play({
								startValue: is ? 0 : 1,
								endValue: !is ? 0 : 1,
								node: this.hasNode(),
								was: was
							});
						}
					})
				}, this);

				this.createComponent({
					name: '_fadeAnimator',
					kind: "Animator",
					duration: this.fadeDuration || 600, easingFunction: enyo.easing.linear,
					onStep: '_fadeAnimatorStep', onEnd: '_fadeAnimationCompleted'
				});
			};
		}),
		_fadeAnimationCompleted: function(sender, evt) {
			if (evt.originator.endValue === 0) {
				this._fadeOriginalShowChanging.call(this, sender.was, !sender.was);
			}
		},
		_fadeAnimatorStep: function(sender) {
			this.applyStyle('opacity', sender.value);
		}
	};

})(components);
