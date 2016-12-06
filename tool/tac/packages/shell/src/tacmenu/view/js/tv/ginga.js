enyo.kind({
	name: 'GingaAppSelector',
	classes: 'ginga-app-selector',
	published: {
		model: null
	},
	events: {
		onRestoreSpotlightDummy: '',
	},
	handlers: {
		onSpotlightKeyDown: 'keyPressed',
	},
	bindings: [
		{ from: '^gingaModel.applications.length', to: '.$.scroller.showing', transform: function(length) {
			return length > 0;
		}},
		{ from: '^gingaModel.applications.length', to: '.$.info.showing', transform: function(length) {
			return length === 0;
		}}
	],
    components: [
		{ name: 'imageHeader', kind: enyo.Image, src: assets('ginga_ico.png'), classes: 'ginga-app-selector-img'},
        { name: 'title', content: 'APLICACIONES GINGA', classes: 'ginga-app-title ginga-font'},
        {
			name: 'scroller',
			kind: 'ShellScroller',
			memory: true,
			horizontal: 'hidden',
			classes: 'ginga-app-scroller',
			components: [
				{ kind: 'enyo.DataRepeater', name: 'repeater', components: [
					{
						events: { onRequestGingaRun: '' },
						components: [
							{
								name: 'appName',
								handlers: {
									onSpotlightFocus: 'onFocus'
								},
								spotlight: true,
								classes: 'ginga-font ginga-app',
								onSpotlightSelect: 'runApp',
								onFocus: function() {
									this.bubble('onRequestScrollIntoView');
								}
							}
						], bindings: [
							{ from: 'model.name', to: '$.appName.content' }
						],
					    runApp: function(oSender, oEvent) {
							this.bubble('onRequestScrollIntoView');
							this.doRequestGingaRun(oEvent.originator.parent.model.get('id'));
					    }
					}
				]},
        ]},
		{
			name: 'info',
			spotlight: false,
			classes: 'ginga-unavailable',
			components: [
				{ name: 'desc', classes: 'ginga-font', content: 'No hay aplicaciones disponibles' }
			]
		},
		{
			name: 'closeBtn',
			kind: Shell.SimpleButton,
			classes: 'ginga-app-selector-btn',
			content: 'CERRAR',
			onSpotlightSelect: 'hide'
		}
    ],
	create: enyo.inherit(function(sup) {
	   return function() {
		   sup.apply(this, arguments);
		   this.$.repeater.set('collection', gingaModel.get('applications'));
	   };
	}),
	hide: enyo.inherit(function(sup) {
	   return function() {
		   this.doRestoreSpotlightDummy();
		   sup.apply(this, arguments);
	   };
	}),
	show: enyo.inherit(function(sup) {
	   return function() {
	   	sup.apply(this, arguments);
	   	enyo.Spotlight.spot(this.$.repeater);
	   };
	}),
	keyPressed: function(oSender, oEvent) {
		if (oEvent.keyCode === tacKeyboardLayout.VK_EXIT) {
			this.hide();
			return true;
		}
	}
});

enyo.kind({
	name: 'DownloadGinga',
	classes: 'ginga-download',
	published: {
		percentage: 0
	},
	bindings: [
			{ from: '^gingaModel.download.percentage', to: '.percentage' },
	],
	components: [
		{
			content: 'DESCARGANDO APLICACIÓN GINGA',
			name: 'progressBar', kind: 'components.ProgressLabel',
			barClasses: 'ginga-bar-bg'
		}
	],
	show: enyo.inherit(function(sup) {
		return function() {
			if (this.get('percentage') === 100) {
				this.$.progressBar.stopAnim();
				this.$.progressBar.setProgress(0);
			}
			sup.apply(this, arguments);
		};
	}),
	percentageChanged: enyo.inherit(function(sup) {
		return function(was, is) {
			sup.apply(this, arguments);
			log.info('downloadGinga', 'percentageChanged', 'Value=%d', is);
			this.$.progressBar.animateProgressTo(is);
		};
	}),
	connectBinding: function(start) {
		if (start) {
			this.bindings[0].connect();
			this.$.progressBar.animateProgressTo(gingaModel.get('download').get('percentage'));
		} else {
			this.bindings[0].disconnect();
			this.$.progressBar.stopAnim();
			this.$.progressBar.setProgress(0);
		}
	}
});
