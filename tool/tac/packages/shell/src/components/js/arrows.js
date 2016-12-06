enyo.kind({
	name: 'Shell.BackArrow',
	classes: 'arrow-box up',
	components: [
		{ kind: enyo.Image, classes: 'image', src: assets('arrow_up.png') },
		{ classes: 'text', content: 'VOLVER' }
	]
});

enyo.kind({
	name: 'Shell.DetailArrow',
	classes: 'arrow-box down',
	components: [
		{ classes: 'text', content: 'DETALLES' },
		{ kind: enyo.Image, classes: 'image', src: assets('arrow_up.png') }
	]
});

enyo.kind({
	name: 'Shell.LeftArrow',
	classes: 'arrow-box left',
	components: [
		{ kind: enyo.Image, classes: 'image', src: assets('arrow_up.png') }
	]
});

enyo.kind({
	name: 'Shell.RightArrow',
	classes: 'arrow-box right',
	components: [
		{ kind: enyo.Image, classes: 'image', src: assets('arrow_up.png') }
	]
});
