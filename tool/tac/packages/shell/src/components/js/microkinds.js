enyo.kind({
	name: 'IconsBar',
	classes: 'icons-wrapper',
});

/**
 * NoIndicatorScrollStrategy
 */
enyo.kind({
	name: 'NoIndicatorScrollStrategy',
	kind: 'moon.ScrollStrategy',
	showHideScrollColumns: util.true
});

enyo.kind({
	name: 'ShellScroller',
	kind: 'moon.Scroller',
	classes: 'shell-scroller',
	strategyKind: 'NoIndicatorScrollStrategy',
});

enyo.kind({
	name: 'components.Detail',
	published: {
		title: '',
		info: ''
	},
	classes: 'detail-component',
	components: [
		{ name: 'detailTitle', classes: 'detail-title oswald-regular' },
		{ name: 'detailInfo', classes: 'detail-info' }
	],
	bindings: [
		{ from: 'title', to: '.$.detailTitle.content', transform: function(title) {
			return title + ' ';
		}},
		{ from: 'info', to: '.$.detailInfo.content' }
	],
});

enyo.kind({
	name: 'components.BackLegend',
	kind: 'components.Detail',
	classes: 'back-legend',
	title: '[VOLVER]:',
	info: ' PANTALLA ANTERIOR'
});
