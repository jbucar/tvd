enyo.kind({
	name: 'components.StateImage',
	published: {
		enabled: true,
		enabledSrc: undefined,
		disabledSrc: undefined,
		imageClasses: undefined,
	},
	bindings: [
		{ from: '.enabled', to: '.$.enabled.showing' },
		{ from: '.enabled', to: '.$.disabled.showing', transform: util.negate },
		{ from: '.enabledSrc', to: '.$.enabled.src' },
		{ from: '.disabledSrc', to: '.$.disabled.src' },
		{ from: '.imageClasses', to: '.$.enabled.classes' },
		{ from: '.imageClasses', to: '.$.disabled.classes' }
	],
	components: [
		{ name: 'enabled', kind: enyo.Image },
		{ name: 'disabled', kind: enyo.Image }
	]
});