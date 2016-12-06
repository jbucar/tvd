enyo.kind({
	name: 'ProgressBar',
	classes: 'progressbar',
	spotlight: false,
	published: {
		value: 0,
		min: 0,
		max: 100,
		width: 240,
		showLabels: true,
		progressStyle: '',
		knobStyle: ''
	},
	bindings: [
		{ from: '.initLabel', to: '.$.initLabel.content', transform: function(value) {
			return value ? value : '';
		}},
		{ from: '.endLabel', to: '.$.endLabel.content', transform: function(value) {
			return value ? value : '';
		}},
	],
	components: [
		{ name: 'line', classes: 'progressbar-line' },
		{ name: 'progression', classes: 'progressbar-progress' },
		{ name: 'knob', classes: 'progressbar-knob' },
		{ name: 'labels', classes: 'progressbar-labels', components: [
			{ name: 'initLabel', classes: 'progressbar-label' },
			{ name: 'endLabel', classes: 'progressbar-label' }
		]}
	],
	create: function() {
		this.inherited(arguments);
		if (!this.showLabels) {
			this.$.labels.hide();
			this.$.initLabel.hide();
			this.$.endLabel.hide();
		}
		this.applyStyle( 'width', this.width + 'px' );
		this.$.line.applyStyle( 'width', this.width + 'px' );
	},
	valueChanged: function(was, is) {
		if (is >= 0 && is <= 100) {
			var knobWidth = 10;
			if (this.$.knob.node) {
				var style = window.getComputedStyle(this.$.knob.node);
				var widthStr = style.getPropertyValue('width')
				knobWidth = widthStr.substring( 0, widthStr.length - 2);
			}
			var newPos = is * (this.width - knobWidth/2)/ this.max;
			newPos = Math.round( newPos );
			this.$.knob.applyStyle( 'left', newPos + 'px' );
			this.$.progression.applyStyle( 'width', newPos + 'px' );
			this.show();
		}
		else {
			log.error('ProgressBar', 'valueChanged', 'Value invalid. value=%s', is);
		}
	},
	show: function (old, newV) {
		if (!Context.empty()) {
			var ctx = Context.current();
			this.set('progressStyle','{0}-progressbar-progress'.format(ctx));
			this.set('knobStyle','{0}-progressbar-knob'.format(ctx));
		}

		this.inherited(arguments);
	},
	progressStyleChanged: function(oldStyle, newStyle) {
		this.$.progression.removeClass(oldStyle);
		this.$.progression.addClass(newStyle);
	},
	knobStyleChanged: function(oldStyle, newStyle) {
		this.$.knob.removeClass(oldStyle);
		this.$.knob.addClass(newStyle);
	}
});
