(function() {

	enyo.kind({
		name: 'ChInfo',
		wrapView: true,
		style: 'height: 100%',
		bindings: [
			{ from: '.model', to: '.$.chWrapper.model' },
			{ from: '.model.currentShow', to: '.$.chWrapper.showModel' },
		],
		components: [
			{ name: 'chWrapper', kind: 'ChInfoWithShow', chView: true },
			{ kind: 'Signals', TVModeActived: 'actived' }
		],
		actived: function(sender, evt) {
			if (enyo.Spotlight.getCurrent().isDescendantOf(this.$.chWrapper) && evt.mode === 'preview' &&
				evt.prevMode === 'fullscreen') {
				tvdadapter.change(this.model.get('id'));
			}
		},
	});

	var previewDelay = 2000;
	var previewBounds = { left: 102, top: 288, width: 240, height: 180 };

	enyo.kind({
		name: 'ChInfoWithShow',
		kind: 'components.ViewContent',
		classes: 'chinfo',
		published: {
			model: null
		},
		handlers: {
			onSpotlightRight: 'onRight',
			onSpotlightLeft: 'onLeft'
		},
		bindings: [
			{ from: '.model', to: '.$.infoOptions.model' },
			{ from: '.model', to: '.$.infoDetails.model' },
			{ from: '.showModel', to: '.$.infoOptions.showModel' },
			{ from: '.showModel', to: '.$.infoDetails.showModel' },
			{ from: '.chView', to: '.$.infoOptions.chView' }
		],
		components: [
			{
				classes: 'details-actions',
				components: [
					{ name: 'infoOptions', kind: 'ChInfoActions' }
				]
			},
			{
				classes: 'details-desc',
				components: [
					{ name: 'infoDetails', kind: 'ShowInfoDetails' }
				]
			}
		],
		onRight: function(sender, evt) {
			if (evt.originator === this) {
				this.bubble('onRequestScroll', {originator: this, dir: 'RIGHT'});
			}
		},
		onLeft: function(sender, evt) {
			if (evt.originator === this) {
				this.bubble('onRequestScroll', {originator: this, dir: 'LEFT'});
			}
		},
		onContentEnter: enyo.inherit(function(sup) {
			return function(sender, evt) {
				sup.apply(this, arguments);
				if (evt.originator === this) {
					this.startJob('previewTV', this.bindSafely('_previewTV'), previewDelay);
				}
			}
		}),
		onContentLeave: enyo.inherit(function(sup) {
			return function(sender, evt) {
				sup.apply(this, arguments);
				if (evt.originator === this) {
					this.stopJob('previewTV');
					tvdadapter.shutdown();
					enyo.Signals.send('onTVMode', {mode: 'off'});
				}
			}
		}),
		destroy: enyo.inherit(function(sup) {
			return function() {
				{
					// TODO [nbaglivo]: Remove this when video tag works properly
					var ch = this.get('model').get('id');
					if (tvdadapter.current() === ch) {
						app._prevMode = 'off';
					}
				}

				var cur = enyo.Spotlight.getCurrent();
				if (cur && cur.isDescendantOf(this)) {
					{
						// TODO [nbaglivo]: Remove this when video tag works properly
						tvdadapter.shutdown();
						enyo.Signals.send('onTVMode', {mode: 'off'});
					}

					this.bubble('onSpotLost', {});
				}

				sup.apply(this, arguments);
			}
		}),
		_previewTV: function() {
			if (Context.current() !== 'tvlayer') {
				var ch = this.get('model').get('id');
				log.trace('ChContainer', 'previewTV', 'channel=%s', ch);
				tvdadapter.change(ch);
				enyo.Signals.send('onTVMode', {mode: 'preview', bounds: previewBounds});
			}
		}
	});


	/*
	 * kind ChInfoActions
	 */
	enyo.kind({
		name: 'ChInfoActions',
		kind: 'components.flexLayout.Vertical',
		center: false,
		published: {
			model: null,
		},
		bindings: [
			{ from: '.showModel', to: '.$.preview.showModel' },
			{ from: '.chView', to: '.$.favoriteBtn.showing' },
			{ from: '.chView', to: '.$.block.showing' },
			{ from: '.chView', to: '.$.details.showing' },
			{
				from: '.model.isFavorite', to: '.$.favoriteBtn.content',
				transform: function(isFav) {
					return isFav ? 'QUITAR FAVORITO' : 'AGREGAR FAVORITO';
				}
			},
			{
				from: '.model.isBlocked', to: '.$.block.content',
				transform: function(isBlocked) {
					return isBlocked ? 'DESBLOQUEAR CANAL' : 'BLOQUEAR CANAL';
				}
			},
		],
		components: [
			{ name: 'preview', kind: 'ChDetailVideoPreview', classes: 'ch-infoaction-view' },
			{
				kind: 'components.VerticalMenu',
				style: 'min-width: 100%',
				components: [
					{
						name: 'watch',
						kind: Shell.SimpleButton,
						content: 'VER CANAL',
						onSpotlightSelect: 'watchCh',
					},
					{
						name: 'favoriteBtn',
						kind: Shell.SimpleButton,
						onSpotlightSelect: 'toggleFav',
					},
					{
						name: 'block',
						kind: Shell.SimpleButton,
						content: 'BLOQUEAR CANAL',
						onSpotlightSelect: 'requestBlock',
					},
					{
						name: 'details',
						kind: Shell.SimpleButton,
						content: 'DETALLES TÉCNICOS',
						onSpotlightSelect: 'showDetails',
					},
				]
			},
			{
				name: 'detailsPopup',
				kind: 'PopupChannelInfo',
				context: 'ch',
				title: 'DETALLES TÉCNICOS'
			}
		],
		watchCh: function() {
			tvdadapter.change(this.model.get('id'));
			enyo.Signals.send('onTVMode', {
				mode: 'fullscreen',
				prev: 'preview',
				prevBounds: previewBounds
			});
		},
		toggleFav: function() {
			this.model.toggleFavorite();
		},
		requestBlock: function() {
			enyo.Spotlight.States.push('focus', this.$.block);
			security.openCreateSession({
				owner: Context.owner(),
				title: 'USTED NO HA CREADO UNA SESIÓN',
				desc: 'Para bloquear necesita una contraseña.\n¿Desea crearla?',
				icon: 'dialog_warn.png'
			})
			.then(function(open) {
				if (open) {
					this.model.toggleBlocked();
				}
			}.bind(this)).catch(log.error.bind(log, 'ChInfoActions', 'requestBlock'));

			return true;
		},
		showDetails: function() {
			enyo.Spotlight.States.push('focus');
			this.$.detailsPopup.show();
		}
	});

	enyo.kind({
		name: 'ChDetailVideoPreview',
		classes: 'ch-detail-options',

		bindings: [
			{ from: 'progressVisible', to: '.$.unavailableProgress.showing', transform: util.negate },
			{ from: '.showModel.startTime', to : '.startTime'},
			{ from: '.showModel.endTime', to : '.endTime'},
			{
				from: '.showModel.startTime', to: '.$.progressBar.initLabel',
				transform: function( start ) {
					this.showModelChanged();
					return start ? util.dateToString(start) : '';
				}
			},
			{
				from: '.showModel.endTime', to: '.$.progressBar.endLabel',
				transform: function( end ) {
					this.showModelChanged();
					return end ? util.dateToString(end) : '';
				}
			},
		],
		computed: [
			{ method: "progressVisible", path: ["startTime", "endTime"] }
		],
		components: [
			{ name: 'video', style: 'width: 240px; min-height: 180px; max-height: 180px; background: black' },
			{
				name: 'progressBar',
				kind: 'ProgressBar',
				classes: 'ch-detail-progress',
				width: 240
			},
			{ name: 'unavailableProgress', content: 'No disponible', classes: 'unavailable-progress'},
			{ kind: 'Signals', TVModeActived: 'actived' },
		],
		progressVisible: function() {
			return this.startTime !== undefined && this.endTime !== undefined;
		},
		actived: function(oSender, oEvent) {
			this.$.video.applyStyle('background', oEvent.mode === 'preview'? 'transparent' : 'black');
		},
		showModelChanged: function() {
			var diff = 0;
			var init = this.showModel.get('startTime');
			var end = this.showModel.get('endTime');
			if( init && end ) {
				var current = new Date().getTime();
				init = init.getTime();
				end = end.getTime();
				diff = (current - init) * 100 / (end - init);
			}
			this.$.progressBar.set( 'value', diff );
		}
	});

	enyo.kind({
		name: 'ShowInfoDetails',
		classes: 'apps-info-details',
		bindings: [
			{
				from: 'model.name',
				to: '.$.title.content',
				transform: function(name) {
					return name ? name.toUpperCase() : 'SHOW DESCONOCIDO';
				},
			},
			{
				from: '.showModel.name',
				to: '.$.show.info',
				transform: function(name) {
					return name ? name : 'Programa desconocido';
				}
			},
			{
				from: '.showModel.description',
				to: '.$.desc.content',
				transform: function(desc) {
					return desc ? desc : '';
				}
			},
			{
				from: '.showModel.parentalAge',
				to: '.$.parental.info',
				transform: function(age) {
					return age ? age : 'Contenido no disponible';
				}
			},
			{
				from: '.showModel.length',
				to: '.$.length.info',
				transform: function(length) {
					return length ? length + 'm.' : 'Desconocida';
				}
			},
			{
				from: '.showModel.audio',
				to: '.$.audio.info',
				transform: function(audio) {
					return audio === 'Unknown' ? 'Idioma no disponible' : util.capitalize(i18(audio));
				}
			},
			{
				from: '.showModel.startTime',
				to: '.$.time.info',
				transform: 'timeCalc'
			},
			{
				from: '.showModel.endTime',
				to: '.$.time.info',
				transform: 'timeCalc'
			},
			{ from: 'model.logo', to: '.$.titleIcon.src' }
		],
		components: [
			{
				name: 'chContainer', classes: 'ch-title-container',
				components: [
					{
						name: 'iconWrapper', classes: 'ch-icon-wrapper',
						components: [
							{ name: 'titleIcon', classes: 'ch-title-icon', kind: enyo.Image }
						]
					},
					{ name: 'title', classes: 'info-details-title ch-title-text' }
				]
			},
			{ name: 'show', kind: 'components.Detail', title: 'PROGRAMA:' },
			{ name: 'time', kind: 'components.Detail', title: 'HORARIO:', info: 'Horario desconocido' },
			{ name: 'length', kind: 'components.Detail', title: 'DURACIÓN:' },
			{ name: 'audio', kind: 'components.Detail', title: 'AUDIO:' },
			{ name: 'parental', kind: 'components.Detail', title: 'CLASIFICACIÓN:' },
			{ name: 'desc', classes: 'info-details-description' },
		],
		timeCalc: function(time) {
			var content = 'Horario desconocido';
			if (this.model) {
				var from = this.showModel.get('startTime');
				var to = this.showModel.get('endTime');
				if ( from && to ) {
					content = util.dateToString(from) + ' a ' + util.dateToString(to);
				}
			}
			return content;
		}
	});

	/*
	* PopupChannelInfo
	*/
	enyo.kind({
		name: 'PopupChannelInfo',
		kind: 'Dialog',
		bindings: [
			{ from: '.title', to: '.$.title.content' },
			{ from: 'model.audioPID', to: '.$.audioContent.content', transform: 'formatValue' },
			{ from: 'model.audioChannels', to: '.$.audioChContent.content', transform: 'formatValue' },
			{ from: 'model.audioRate', to: '.$.audioRateContent.content', transform: 'formatValue' },
			{ from: 'model.videoPID', to: '.$.videoPIDContent.content', transform: 'formatValue' },
			{ from: 'model.videoRes', to: '.$.videoResContent.content', transform: 'formatValue' },
			{ from: 'model.videoFR', to: '.$.videoFRContent.content', transform: 'formatValue' },
			{ from: 'chModel.info.frecuency', to: '.$.signalFreq.content', transform: 'formatValue' },
			// TODO: Use a computed property, since the value depends on several chModel properties.
			{ from: 'chModel.info.srvID', to: '.$.serviceID.content', transform: function(srv) {
				var nitID = util.zeroFill(this.chModel.get('info').nitID.toString(16), 4);
				var tsID =  util.zeroFill(this.chModel.get('info').tsID.toString(16), 4);
				var srvID = util.zeroFill(srv.toString(16), 4);

				return  nitID + '.' + tsID + '.' + srvID;
			}},
			{ from: 'chModel.channel', to: '.$.chNumber.content', transform: 'formatValue' },
			{
				from: 'model.quality', to: '.$.signalQuality.content',
				transform: function(quality) {
					return quality + '%';
				}
			},
			{
				from: 'model.level', to: '.$.signalLevel.content',
				transform: function(slvl) {
					return slvl + '%';
				}
			}
		],
		components: [
			//AUDIO
			{ content: 'Audio', classes: 'popup-group-container popup-text popup-tiny-text popup-details-title' },
			{ classes: 'popup-group-container', components: [
				{ classes: 'popup-text popup-tiny-text', content: 'PID:' },
				{ name: 'audioContent', classes: 'popup-text popup-tiny-text' }
			]},
			{ classes: 'popup-group-container', components: [
				{  classes: 'popup-group-container popup-text popup-tiny-text', content: 'Número de canales:' },
				{ name: 'audioChContent', classes: 'popup-text popup-tiny-text' }
			]},
			{ classes: 'popup-group-container', components: [
				{  classes: 'popup-group-container popup-text popup-tiny-text', content: 'Rate:' },
				{ name: 'audioRateContent', classes: 'popup-text popup-tiny-text' }
			]},
			//VIDEO
			{ content: 'Video', classes: 'popup-group-container popup-text popup-tiny-text popup-details-title' },
			{ classes: 'popup-group-container', components: [
				{ classes: 'popup-text popup-tiny-text', content: 'PID:' },
				{ name: 'videoPIDContent', classes: 'popup-text popup-tiny-text' }
			]},
			{ classes: 'popup-group-container', components: [
				{ classes: 'popup-group-container popup-text popup-tiny-text', content: 'Resolución:' },
				{ name: 'videoResContent', classes: 'popup-text popup-tiny-text' }
			]},
			{ classes: 'popup-group-container', components: [
				{ classes: 'popup-group-container popup-text popup-tiny-text', content: 'FrameRate:' },
				{ name: 'videoFRContent', classes: 'popup-text popup-tiny-text' }
			]},
			//SIGNAL
			{ content: 'Señal', classes: 'popup-group-container popup-text popup-tiny-text popup-details-title' },
			{ classes: 'popup-group-container', components: [
				{ classes: 'popup-text popup-tiny-text', content: 'Nivel de señal:' },
				{ name: 'signalLevel', classes: 'popup-text popup-tiny-text' }
			]},
			{ classes: 'popup-group-container', components: [
				{ classes: 'popup-text popup-tiny-text', content: 'Calidad de señal:' },
				{ name: 'signalQuality', classes: 'popup-text popup-tiny-text' }
			]},
			{ classes: 'popup-group-container', components: [
				{ classes: 'popup-text popup-tiny-text', content: 'Frecuencia:' },
				{ name: 'signalFreq', classes: 'popup-text popup-tiny-text' }
			]},
			//CHANNEL
			{ content: 'Canal', classes: 'popup-group-container popup-text popup-tiny-text popup-details-title' },
			{ classes: 'popup-group-container', components: [
				{ classes: 'popup-text popup-tiny-text', content: 'Service ID:' },
				{ name: 'serviceID', classes: 'popup-text popup-tiny-text' }
			]},
			{ classes: 'popup-group-container', components: [
				{ classes: 'popup-group-container popup-text popup-tiny-text', content: 'Número de Canal:' },
				{ name: 'chNumber', classes: 'popup-text popup-tiny-text' }
			]}
		],
		create: function() {
			this.inherited(arguments);
			this.$.descriptionContainer.destroy();
			this.$.icon.destroy();
			this.$.wrapper.addClass('popup-details');
			this.$.client.addClass('popup-details-components');
			this.set('acceptLabel', 'CERRAR');
			this.set('model', details);
		},
		show: function() {
			this.inherited(arguments);
			var current = tvdadapter.current(false);
			this.set('chModel', channels.getModelFromID(current));
			this.refresh();
		},
		hide: function() {
			this.inherited(arguments);
			this.stopJob('refreshDetailsTimer');
			this.model.reset();
		},
		formatLevel: function(l) {
			return Math.round(l*4/100);
		},
		refresh: function() {
			this.model.updateSignal();
			if (this.model.get('videoPID') === -1 || this.model.get('audioPID') === -1) {
				this.model.updateAV();
			}
			this.startJob('refreshDetailsTimer', this.bindSafely('refresh'), 2000);
		},
		formatValue: function( param ) {
			return param === -1 ? 'No disponible' : param;
		}
	});

})();
