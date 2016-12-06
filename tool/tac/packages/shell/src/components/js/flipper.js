
(function(scope) {
	'use strict';

	//TODO [nbaglivo]: Impl flipper component using FlipperSupport
	enyo.kind({
		name: 'components.Flipper',
		classes: 'flip-container',
		published: {
			flipped: false,
		},
		components: [
			{ name: 'client', classes: 'flipper' },
			{ kind: 'Animator', duration: 600, onStep: 'animStep' }
		],
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				var ctrls = this.getClientControls();

				// Setup items
				ctrls[0].addClass('front');
				ctrls[0].defaultSpotlightDisappear = ctrls[1].name;
				ctrls[1].addClass('back');
				ctrls[1].defaultSpotlightDisappear = ctrls[0].name;
				ctrls[1].set('disabled', true);
			}
		}),
		animStep: function(sender) {
			this.$.client.applyStyle('transform', 'rotateX(' + sender.value + 'deg)');
		},
		setOrigin: function(origin) {
			this.$.client.applyStyle('transform-origin', '100% ' + origin + 'px');
		},
		flippedChanged: function(was, is) {
			this.$.animator.stop();

			this.addRemoveClass('flipped', is);

			// Spotlight managment
			var ctrls = this.getClientControls();
			ctrls[is? 1 : 0].set('disabled', false);
			ctrls[is? 0 : 1].set('disabled', true);

			var height = this.height? this.height : parseInt(ctrls[is? 1 : 0].getComputedStyleValue('height'), 10);
			this.$.client.applyStyle('transform-origin', '100% ' + height/2 + 'px');

			this.$.animator.play({
				startValue: is ? 0 : -180,
				endValue: is ? -180 : -0,
				node: this.hasNode()
			});
		},
		toggle: function() {
			this.setFlipped(!this.get('flipped'));
		}
	});

	scope.components.FlipperSupport = {
		name: 'FlipperSupport',
		constructed: enyo.inherit(function (sup) {
			return function () {
				this.published = enyo.mixin(this.published || {}, { flipped: false });
				sup.apply(this, arguments);
				this.createComponent({ kind: 'Animator', duration: 600, onStep: 'animStep' });
				this.addClass('flipper');
				this.setupItems();
			};
		}),
		animStep: function(sender) {
			this.applyStyle('transform', 'rotateX(' + sender.value + 'deg)');
		},
		setupItems: function() {
			var ctrls = this.getClientControls();

			ctrls[0].addClass('front');
			ctrls[0].defaultSpotlightDisappear = ctrls[0].defaultSpotlightDisappear || ctrls[1].name;
			ctrls[1].addClass('back');
			ctrls[1].defaultSpotlightDisappear = ctrls[1].defaultSpotlightDisappear || ctrls[0].name;
			ctrls[1].set('disabled', true);
		},
		flippedChanged: function(was, is) {
			this.$.animator.stop();

			this.addRemoveClass('flipped', is);

			// Spotlight managment
			var ctrls = this.getClientControls();
			ctrls[is? 1 : 0].set('disabled', false);
			ctrls[is? 0 : 1].set('disabled', true);

			if (this.height) {
				this.applyStyle('transform-origin', '100% ' + (this.height? this.height/2 : 0) + 'px');
			}

			this.$.animator.play({
				startValue: is ? 0 : -180,
				endValue: is ? -180 : -0,
				node: this.hasNode()
			});
		},
	}

})(window);
