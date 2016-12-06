enyo.kind({
	name: 'Shell.Collapser',
	events: {
		onEnd: ''
	},
	published: {
		state: ''
	},
	components: [
		{ name: 'animator', kind: 'enyo.StyleAnimator', onComplete: 'onComplete' }
	],
	collapse: function(target, from, to, skipAnim) {
		if (!skipAnim) {
			this.set('state', 'animating');
			this.$.animator.newAnimation({
				name: 'collapse',
				duration: 300,
				timingFunction: 'linear',
				keyframes: {
					0: [
						{
							control: target,
							properties: {
								'height': from
							}
						},
					],
					30: [],
					100: [
						{
							control: target,
							properties: {
								'height': to
							}
						},
					]
				}
			});

			this.$.animator.play('collapse');
		}
		else {
			this.set('state', 'collapse');
			target.applyStyle('transition', null);
			target.applyStyle('height', to);
		}
	},
	expand: function(target, from, to) {
		if (!this.isExpanded()) {
			this.set('state', 'animating');
			this.$.animator.newAnimation({
				name: 'expand',
				duration: 300,
				timingFunction: 'linear',
				keyframes: {
					0: [
						{
							control: target,
							properties: {
								'height': from
							}
						},
					],
					30: [],
					100: [
						{
							control: target,
							properties: {
								'height': to
							}
						},
					]
				}
			});

			this.$.animator.play('expand');
		}
	},
	isAnimating: function() {
		return this.state === 'animating';
	},
	isExpanded: function() {
		return this.state === 'expand';
	},
	onComplete: function(oSender, oEvent) {
		this.set('state', oEvent.animation.name);
		this.doEnd(oEvent);
	}
});
