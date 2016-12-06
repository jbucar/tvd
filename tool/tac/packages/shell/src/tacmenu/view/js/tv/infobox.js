/*
* InfoBox
*/
enyo.kind({
	name: 'InfoBox',
	classes: 'infobox',
	spotlight: true,
	published: {
		model: null,
		details: false,
		zapping: false
	},
	handlers: {
		onSpotlightKeyDown: 'onKey',
		onSpotlightLeft: 'scrollToLeft',
		onSpotlightRight: 'scrollToRight',
	},
	events: {
		onOpenGingaSelector: ''
	},
	components: [
		{ name: 'iconBar', kind: 'IconBar' },
		{
			name: 'infoboxContainer', classes: 'infobox-details-container',
			components: [
				{
					classes: 'infobox-logo-container',
					components: [
						{ name: 'logo', classes: 'infobox-logo', kind: 'enyo.Image' }
					]
				},
				{
					classes: 'infobox-channel-container',
					components: [
						{
							classes: 'infobox-channel-group',
							components: [
								{ name: 'chName', classes: 'infobox-channel-txt' },
								{ name: 'chNumber', classes: 'infobox-channel-txt' },
							]
						},
						{
							classes: 'infobox-fields-container',
							components: [
								{ name: 'showName', kind: 'InfoboxField', classes: 'program', title: 'PROGRAMA' },
								{ name: 'showStart', kind: 'InfoboxField', classes: 'field small', title: 'INICIA' },
								{ name: 'showEnd', kind: 'InfoboxField', classes: 'field small', title: 'TERMINA' },
								{ name: 'showLang', kind: 'InfoboxField', classes: 'field', title: 'IDIOMA' },
								{ name: 'showAge', kind: 'InfoboxField', classes: 'field', title: 'EDAD' },
							]
						},
						{ name: 'progressBar', kind: 'ProgressBarInfobox', showing: false },
						{ name: 'descTitle', classes: 'infobox-field-title', content: 'DESCRIPCIÓN', showing: false }
					]
				}
			]
		},
		{
			name: 'showDescription',
			kind: 'TextPaginator',
			classes: 'infobox-show-desc',
			showing: false, spotlight: false
		},
		{ name: 'actionBar', kind: 'ActionBar', showing: false },
		{ kind: 'enyo.Signals', onlocalechange: 'updateProgressBar' }
	],
	bindings: [
		{ from: '.model.logo', to: '.$.logo.src', transform: 'updateLogo' },
		{ from: '.model.name', to: '.$.chName.content' },
		{ from: '.model.channel', to: '.$.chNumber.content' },
		{ from: '.model.isProtected', to: '.$.iconBar.isProtected' },

		{ from: '.model.isBlocked', to: '.$.iconBar.isBlocked' },
		{ from: '.model.isFavorite', to: '.$.iconBar.isFavorite' },

		{ from: '.model.currentShow', to: '.currentShow' },
		{ from: '.currentShow.name', to: '.$.showName.data' },
		{
			from: '.currentShow.description', to: '.$.showDescription.text',
			transform: function(value) {
				return value? value : 'Descripción no disponible.';
			}
		},
		{ from: '.currentShow.startTime', to: '.$.showStart.data', transform: 'split' },
		{ from: '.currentShow.endTime', to: '.$.showEnd.data', transform: 'split' },
		{ from: '.currentShow.lang', to: '.$.showLang.data' }, /* confirmar */
		{ from: '.currentShow.parentalAge', to: '.$.showAge.data' },
		{ from: '.currentShow.category', to: '.$.iconBar.category' },
		{ from: '.currentShow.subtitleCount', to: '.$.actionBar.subtitleCount' },
		{ from: 'showProgress', to: '.$.progressBar.showing' },
		{ from: '.details', to: '.$.actionBar.showing' },
		{ from: '.details', to: '.$.showDescription.showing' },
		{ from: '.details', to: '.$.descTitle.showing' },
		{ from: '.zapping', to: '.$.iconBar.zapping'}
	],
	computed: [
		{ method: "showProgress", path: ["details", "currentShow.startTime", "currentShow.endTime"] }
	],
	/* private */
	_channelCounter: 0,
	_isVirtual: false,
	onKey: function(oSender, oEvent) {
		if (
			(oEvent.keyCode === tacKeyboardLayout.VK_EXIT ||
			oEvent.keyCode === tacKeyboardLayout.VK_INFO) &&
			this.details
		){
			this.hide();
			return true;
		}
	},
	hide: enyo.inherit(function(sup) {
	   return function() {
		   sup.apply(this, arguments);
		   this.stopJob('refreshTimer');
		   if (this.get('zapping')) {
			   this.set('zapping', false);
		   }
	   };
	}),
	populate: function( chId ) {
		this.set('model', channels.getModelFromID(chId));
		this.model.fetchShow();
	},
	detailsChanged: function() {
		this.addRemoveClass('full', this.details);
		this.$.infoboxContainer.addRemoveClass('full', this.details);
		if (this.details) {
			this.updateSubs();
		} else {
			this.stopJob('refreshCC');
		}
	},
	scrollToLeft: function() {
		this.$.showDescription.scrollToLeft();
	},
	scrollToRight: function() {
		this.$.showDescription.scrollToRight();
	},
	updateSubs: function() {
		this.get('currentShow').updateSubtitles();
		this.startJob('refreshCC', this.bindSafely('updateSubs'), 5000);
	},
	split: function(value) {
		return value? util.dateToString(value) + ' Hs' : '';
	},
	updateLogo: function(value) {
		return value ? value : assets('default_infobox.png');
	},
	showDetailsInfo: function() {
		this.updateProgressBar();
		this.stopJob('autoHide');
		this._isVirtual = false;
		this.set('details', true );
		this.set('zapping', false);
		this.showChInfo( false );
	},
	// Shows or/and update the InfoBox
	showInfo: function( isVirtual ) {
		this.stopJob('autoHide');
		this.set('details', false);

		this.$.chNumber.removeClass('big');
		this._isVirtual = isVirtual;

		// Disable zapping mode.
		if (this.get('zapping') && !isVirtual) {
			this.exitZappingMode();
		}

		this.showChInfo( isVirtual );
		this.startJob('autoHide', this.bindSafely('hide'), 5000);
	},
	showChInfo: function( isVirtual ) {
		var current = tvdadapter.current(isVirtual);
		if (current !== -1) {
			this.populate( current );
		}

		this.show();
	},
	showingChanged: function(oldValue, newValue) {
		this.inherited(arguments);
		if (!this.showing) {
			if (this._isVirtual) {
				this._isVirtual = false;
				tvdadapter.resetVirtualChannel();
			}
		}
	},
	changeToZappingMode: function() {
		if (!this.get('zapping')) {
			// Disable the info box.
			this.stopJob('autoHide');
			// Shows the change chanel box (zapping mode).
			this.set('zapping', true);
			this.set('details', false);
			this.set('model', null);
			this.$.chNumber.set('content','');
			this.$.chNumber.addClass('big');
			this.show();
		}
	},
	changeToChannel: function(keyCode) {
		this.changeToZappingMode();

		// if we already are seeing a channel we need to erase the channel number field
		// in order to show the new numbers
		if (this.$.chNumber.get('content').length === 5) {
			this.$.chNumber.set('content','');
		}

		this.changeChNumber(keyCode);
		this._channelCounter++;
		if (this._channelCounter < 4) {
			this.startJob('zappingJob', this.bindSafely('zappingTimeout'), 4000);
		}
		else {
			this.exitZappingMode();
			this.changeChannel();
		}
	},
	zappingTimeout: function() {
		this.exitZappingMode();
		this.showInfo();
	},
	exitZappingMode: function() {
		this.stopJob('zappingJob');
		this.set('zapping', false);
		this._channelCounter = 0;
	},
	changeChannel: function() {
		var chNumber = this.$.chNumber.get('content');
		var ch = channels.getModelFromNumber( chNumber );

		if (ch && ch.get('id') !== tvdadapter.current()) {
			tvdadapter.change( ch.get('id') );
		}
		else {
			log.info('InfoBox', 'changeChannel', 'invalid channel. channel=%s', chNumber);
			this.showInfo();
		}
	},
	onEnter: function() {
		if (this._isVirtual) {
			this.exitZappingMode();
			this.stopJob('autoHide');
			// TODO: review this, change to current ??
			tvdadapter.change( tvdadapter.current( true ) );
		}
	},
	changeChNumber: function( n ) {
		var chNumber = this.$.chNumber;
		var current = chNumber.get('content') ? chNumber.get('content') : '' ;
		if (current.length == 2) {
			current = current + '.';
		}
		chNumber.set('content', current + n.toString());
	},
	updateProgressBar: function() {
		var show = this.get('currentShow');
		if (show.get('startTime') && show.get('endTime')) {
			var current = new Date().getTime();
			var init = new Date( show.get('startTime') ).getTime();
			var end = new Date( show.get('endTime') ).getTime();
			this.$.progressBar.set('value', (current - init) * 100 / (end - init));
		}
		this.startJob("refreshTimer", this.bindSafely("updateProgressBar"), 60000);
	},
	showProgress: function() {
		// Show the event progress only if it has the startTime and endTime data from EPG.
		var show = this.get('currentShow');
		var showProgress = this.get("details") && show && show.get('startTime') && show.get('endTime');
		if (showProgress) {
			this.updateProgressBar();
		}
		return showProgress;
	},
	currentShowChanged: function() {
		this.$.progressBar.set('value', 0);
	}
});

/*
* IconBar
*/
enyo.kind({
	name: 'IconBar',
	classes: 'infobox-iconbar',
	published: {
		isProtected: false,
		isBlocked: false,
		isFavorite: false,
	},
	components: [
		{ classes: 'infobox-transparent' },
		{
			classes: 'infobox-iconbar-container-b',
			components: [
				{ name: 'ginga', kind: 'enyo.Image', classes: 'infobox-icon', src: assets('infobox/ginga.png'), showing: false },
				{ name: 'blocked', kind: 'enyo.Image', classes: 'infobox-icon bloq', src: assets('infobox/block.png'), showing: false },
				{ name: 'fav', kind: 'enyo.Image', classes: 'infobox-icon', src: assets('infobox/fav.png'), showing: false },
				{ name: 'category', kind: 'enyo.Image', classes: 'infobox-icon' },
			]
		},
		{ name: 'separator', classes: 'infobox-iconbar-separator' },
		{
			classes: 'infobox-iconbar-container-a',
			components: [
				{ name: 'calendar', kind: 'InfoboxTime', classes: 'infobox-calendar', src: assets('infobox-calendario.png') },
				{ name: 'time', kind: 'InfoboxTime', classes: 'infobox-time', src: assets('infobox-reloj.png') },
			]
		},
		{ kind: 'enyo.Signals', onlocalechange: 'updateDate' }
	],
	bindings: [
		{ from: 'showGinga', to: '.$.ginga.showing', transform: Boolean },
		{ from: '^gingaModel.hasInteractivity', to: '.interactivity' },
		{ from: '.isFavorite', to: '.$.fav.showing', transform: Boolean },
		{ from: 'showBlock', to: '.$.blocked.showing' },
		{ from: '.category', to: '.$.category.src', transform: function(cat) { return assets('infobox/categories/{0}.png'.format(cat ? cat : '0'));} },
	],
	computed: [
		{ method: "showBlock", path: ["isProtected", "isBlocked"] },
		{ method: "showGinga", path: ["interactivity", "zapping"] }
	],
	handlers: {
		onShowingChanged: 'onShowingChangedHandler'
	},
	onShowingChangedHandler: function() {
		if (this.showing) {
			this.refreshJob();
		}
	},
	_formatNumber: function(inValue) {
		return (inValue) ? (String(inValue).length < 2) ? "0"+inValue : inValue : "00";
	},
	showBlock: function() {
		return this.get('isProtected') || this.get('isBlocked');
	},
	showGinga: function(){
		return gingaModel.get('hasInteractivity') && !this.zapping;
	},
	updateDate: function() {
		var d = new Date(Date.now());
		this.$.calendar.set('data', this._formatNumber(d.getDate()) + '/' + this._formatNumber((d.getMonth()+1)) + '/' + d.getFullYear() );
		this.$.time.set('data', this._formatNumber(d.getHours()) + ':' + this._formatNumber(d.getMinutes()) + ' Hs');
	},
	refreshJob: function() {
		this.updateDate();
		this.startJob("refresh", this.bindSafely("refreshJob"), 60000);
	},
});

/*
* ActionBar
*/
enyo.kind({
	name: 'ActionBar',
	classes: 'infobox-actionbar',
	published: {
		subtitleCount: 0
	},
	bindings: [
		{ from: '^gingaModel.hasInteractivity', to: '.$.ginga.showing', transform: Boolean },
		{ from: '.subtitleCount', to: '.$.cc.showing', transform: function(count) {
			return count > 0;
		}},
	],
	components: [
		{ classes: 'infobox-iconbar-container-a', components: [
			{ name: 'exit', classes: 'infobox-actionbar-txt', components: [
				{ content: '[SALIR o INFO]: ', classes: 'infobox-actionbar-txt option' },
				{ content: 'CERRAR', classes: 'infobox-actionbar-txt action' },
			]},

			{ name: 'cc', classes: 'infobox-actionbar-txt', showing: false, components: [
				{ name: 'option', content: '[SUBT]: ', classes: 'infobox-actionbar-txt option' },
				{ content: 'ACTIVAR CLOSED CAPTION', classes: 'infobox-actionbar-txt action' },
			]},

			{ name: 'ginga', classes: 'infobox-actionbar-txt', showing: false, components: [
				{ content: '[ROJO]: ', classes: 'infobox-actionbar-txt option' },
				{ content: 'INTERACTIVIDAD', classes: 'infobox-actionbar-txt action' },
			]}
		]},
		{ classes: 'infobox-actionbar-separator' },
		{ classes: 'infobox-transparent' },
	]
});

/*
* InfoBoxField
*/
enyo.kind({
	name: 'InfoboxField',
	classes: 'infobox-field',
	components: [
		{ name: 'title', classes: 'infobox-field-title' },
		{ name: 'data', classes: 'infobox-field-data' }
	],
	bindings: [
		{ from: '.title', to: '.$.title.content' },
		{ from: '.data', to: '.$.data.content', transform: function (value) { return value ? value : '--';} }
	]
});

/*
* InfoboxTime
*/
enyo.kind({
	name: 'InfoboxTime',
	bindings: [
		{ from: '.data', to: '.$.data.content'},
		{ from: '.src', to: '.$.icon.src'}
	],
	components: [
		{ name: 'icon', kind: 'enyo.Image', classes: 'infobox-icon transparent' },
		{ name: 'data', content: '' },
	]
});

/*
* ProgressBarInfobox
*/

enyo.kind({
	name: 'ProgressBarInfobox',
	kind: 'ProgressBar',
	classes: 'infobox-progressbar',
	width: 153,
	showLabels: false,
	create: function() {
		this.inherited(arguments);
		this.$.line.addClass('infobox-progressbar-line');
	}
});
