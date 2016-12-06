
(function() {

	enyo.kind({
		name: 'Shell.PreloaderBar',
		classes: 'preloader',
		components: [
			{ tag: 'span', classes: 'bar animate' }
		]
	});

	enyo.kind({
		name: 'Shell.BallPreloader',
		classes: 'ballpreloader',
		components: [
			{ classes: 'ball b1' },
			{ classes: 'ball b2' },
			{ classes: 'ball b3' },
			{ classes: 'ball b4' },
			{ classes: 'ball b5' },
			{ classes: 'ball b6' },
			{ classes: 'ball b7' },
			{ classes: 'ball b8' }
		]
	});

	enyo.kind({
		name: 'components.Loading',
		classes: 'flex vertical center',
		style: 'height: 100%; width: 100%; align-items: center',
		components: [
			{ content: 'Cargando', classes: 'oswald-light', style: 'margin-bottom: 10px' },
			{ kind: 'Shell.BallPreloader' }
		]
	});

})();
