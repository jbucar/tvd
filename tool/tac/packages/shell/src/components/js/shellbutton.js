/**
 * Shell.Button
 */
enyo.kind({
	name: 'Shell.Button',
	classes: 'shellbutton main-menu',
	spotlight: true,
	switchIcon: false,
	tooltipEnabled: false,
	tooltipMsg: '',
	tooltipStyle: '',
	published: {
		colorClass: ''
	},
	bindings: [
		{ from: '.label', to: '.$.label.content' },
		{ from: '.icon', to: '.$.iconUnselected.src', transform: assets},
		{ from: '.selectedIcon', to: '.$.iconSelected.src', transform: assets },
	],
	handlers: {
		onSpotlightFocused: 'onFocused',
		onSpotlightBlur: 'onBlur'
	},
	components: [
		{ name: 'iconSelected', kind: 'enyo.Image', classes: 'shellbutton-icon', showing: false },
		{ name: 'iconUnselected', kind: 'enyo.Image', classes: 'shellbutton-icon', showing: true },
		{ name: 'label', classes: 'shellbutton-label' }
	],
	create: enyo.inherit(function(sup) {
		return function() {
			sup.apply(this, arguments);
			this.addClass(this.colorClass);
		}
	}),
	switchIconTo: function(aBoolean) {
		this.set('switchIcon',aBoolean);
	},
	onFocused: function(oSender, oEvent) {
		this.switchIconTo(true);
		this.showTooltip();
	},
	showTooltip: function() {
		if (this.tooltipEnabled) {
			Tooltip.set('customStyle', this.tooltipStyle);
			Tooltip.set('text', this.tooltipMsg);
			Tooltip.showTooltip();
		}
	},
	tooltipMsgChanged: function(was, is) {
		if (is && enyo.Spotlight.getCurrent() === this) {
			this.showTooltip();
		}
	},
	onBlur: function() {
		this.switchIconTo(false);
		if (this.tooltipEnabled) {
			Tooltip.hideTooltip();
		}
	},
	switchIconChanged: function( isSelected ) {
		this.$.iconSelected.set('showing', !isSelected);
		this.$.iconUnselected.set('showing', isSelected);
	}
});

/**
 * Shell.TabButton
 */
enyo.kind({
	name: 'Shell.TabButton',
	kind: Shell.Button,
	spotlight: true,
	switchIcon: false,
	bindings: [
		{ from: '.label', to: '.$.label.content' },
		{ from: '.icon', to: '.$.icon.src', transform: function(src) { return assets(src); }},
	],
	handlers: {
		onSpotlightFocus: 'onFocus'
	},
	components: [
		{
			classes: 'label-wrapper',
			components: [
				{ name: 'icon', kind: 'enyo.Image', classes: 'shellbutton-icon tab' },
				{ name: 'label', classes: 'shellbutton-label' }
			]
		},
		{ name: 'bar', classes: 'bar' }
	],
	create: enyo.inherit(function(sup) {
		return function() {
			sup.apply(this, arguments);
			this.removeClass(this.getClasses());
			this.addClass('shellbutton tab');
			this.addClass(this.colorClass);
		}
	}),
	switchIconTo: function() {
		return true;
	},
	onFocus: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			this.bubble('onRequestScroll', {dir: oEvent.dir});
		}
	}
});

enyo.kind({
	name: 'Shell.SimpleButton',
	classes: 'simple-button',
	spotlight: true,
	fixed: false,
	handlers: {
		onSpotlightFocus: 'onSpotlightFocus',
		onSpotlightBlur: 'onSpotlightBlur'
	},
	create: enyo.inherit(function(sup) {
		return function() {
			sup.apply(this, arguments);
			if (this.fixed) {
				this.addClass('fixed');
			}
		}
	}),
	onSpotlightFocus: function(oSender, oEvent) {
		this.addClass( Context.current() );
		this.addClass( 'spotlight' ); // In case the button spotlight prop was redefined to "container"
	},
	onSpotlightBlur: function(oSender, oEvent) {
		this.removeClass( Context.current() );
		this.removeClass( 'spotlight' );
	}
});

enyo.kind({
	name: 'components.ProgressButton',
	kind: 'Shell.SimpleButton',
	style: 'margin: 0',
	fixed: true,
	bindings: [
		{ from: '.content', to: '.$.bar.content' },
		{ from: '.barClasses', to: '.$.bar.barClasses' }
	],
	components: [
		{ name: 'bar', kind: 'components.ProgressLabel' }
	],
	animateProgressTo: function() {
		return this.$.bar.animateProgressTo.apply(this.$.bar, arguments);
	}
});
