/**
 * Volume kind.
 */
enyo.kind({
	name: 'Volume',
	events: {
		onVanish: ''
	},
	published: {
		volume: undefined,
		mute: undefined,
	},
	components: [
		{
			name: 'volume-background',
			classes: 'volume-background',
			components: [
				{ name: 'levelNotFilled', classes: 'level-not-filled' },
				{ name: 'value', classes: 'volume-text'},
				{ name: 'level', classes: 'level' },
				{
					name: 'imageBox',
					classes: 'image-box',
					components: [
						{ name: 'unmuteImage', kind: enyo.Image, src: assets('vol.png'), showing: true },
						{ name: 'muteImage', kind: enyo.Image, src: assets('mute.png'), showing: false }
					]
				}
			]
		},
		{ kind: 'Animator', onStep: 'onAnimStep' }
	],
	TOP: 38,
	HEIGHT: 185,
	onAnimStep: function(oSender) {
		this.$.level.applyStyle('height', '{0}px'.format(oSender.value));
		this.$.level.applyStyle('top', '{0}px'.format(this.TOP + (this.HEIGHT - oSender.value)));
	},
	muteChanged: function(was, is) {
		this.$.level.addRemoveClass('mute', is);
		this.$.unmuteImage.setShowing(!is);
		this.$.muteImage.setShowing(is);
	},
	volumeChanged: function(was, is) {
		var newHeight = this.scaleToHeight(is);
		if (is >= 0 && is <= 100) {
			this.$.value.set('content', is);
			if (was !== undefined) {
				this.$.animator.play({
					startValue: this.scaleToHeight(was),
					endValue: newHeight,
					node: this.hasNode()
				});
			}
			else {
				this.$.level.applyStyle('height', '{0}px'.format(newHeight));
				this.$.level.applyStyle('top', '{0}px'.format(this.TOP + (this.HEIGHT - newHeight)));
			}

			this.setMute(false);
		}
		else {
			log.error('Volume', 'volumeChanged', 'volume is not in the valid range [0, 100]. volume=%s', is);
		}
	},
	scaleToHeight: function(value) {
		return Math.round((value * this.HEIGHT) / 100);
	},
	show: function() {
		// Shows volume widget by 3 seconds.
		this.inherited(arguments);
		this.startJob('hideVolumeJob', this.bindSafely('hideVolume'), 3000);
	},
	hideVolume: function() {
		// Hides volume widget.
		this.hide();
		this.doVanish();  // sends event onVanish.
	}
});
