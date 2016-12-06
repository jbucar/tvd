(function(scope) {
	/**
	* Guide global vars
	*/
	enyo.kind({
		name: 'GuideModel',
		kind: 'enyo.Model',
		attributes: {
			currentShow: new ShowModel(),
			isFavorite: undefined,
			isBlocked: undefined,
			channelName: undefined,
			currentCell: undefined
		},
		reset: function() {
			this.set({
				currentShow: new ShowModel(),
				isFavorite: undefined,
				isBlocked: undefined,
				channelName: undefined,
			});
		},
		currentShowChanged: function() {
			var ch = this.get('currentShow').get('channel');
			if (ch) {
				this.set({
					'channelName': ch.get('name'),
					'isFavorite': ch.get('isFavorite'),
					'isBlocked': ch.get('isBlocked')
				});
			}
		}
	});

	scope.guideModel = new GuideModel();
	var cellWidth = 227;
	var halfHourMs = 1800000;

	/**
	* gridCell
	*/
	enyo.kind({
		name: 'gridCell',
		spotlight: true,
		classes: 'guide-cell',
	});

	/**
	* timeCell
	*/
	enyo.kind({
		name: 'timeCell',
		kind: 'gridCell',
		spotlight: false,
		published: {
			date: undefined
		},
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.dateChanged();
			};
		}),
		dateChanged: function() {
			this.setContent(this.date.toTimeString().slice(0, 5));
		}
	});

	/**
	* channelGridCell
	*/
	enyo.kind({
		name: 'channelGridCell',
		classes: 'guide-channel-cell',
		spotlight: true,
		mixins: ['moon.MarqueeSupport'],
		marqueeOnSpotlight: true,
		handlers: {
			onSpotlightFocused: 'onFocus',
		},
		bindings: [
			{ from: '.model.name', to: '.$.cell.content', transform: 'updateName' },
			{ from: '.model.logo', to: '.$.icon.src' },
			{ from: '.defaultSpotlightRight', to: '.$.cell.defaultSpotlightRight' }
		],
		components: [
			{ name: 'icon', classes: 'guide-title-icon', kind: enyo.Image },
			{
				name: 'cell',
				classes: 'guide-channel-name',
				kind: 'moon.MarqueeText',
				marqueeSpeed: 40,
			}
		],
		updateName: function(value) {
			return value + ' ' + this.model.get('channel');
		},
		onFocus: function(oSender, oEvent) {
			if (oEvent.originator === this) {
				guideModel.set('currentShow', this.model.get('currentShow'));
				this.model.fetchShow();
				this.bubble('onRequestScrollIntoView');
			}
		}
	});

	/**
	* showGridCell
	*/
	enyo.kind({
		name: 'showGridCell',
		kind: 'gridCell',
		mixins: ['moon.MarqueeSupport'],
		marqueeOnSpotlight: true,
		style: 'display: flex; justify-content: center',
		bindings: [
			{ from: '.model', to: '.show' },
			{ from: '.model.name', to: '.$.title.content', transform: function(v) { return v.toUpperCase(); } }
		],
		handlers: {
			onSpotlightFocused: 'onFocus'
		},
		events: {
			onShowSelected: ''
		},
		components: [
			{
				name: 'title',
				classes: 'gridcell-show-title',
				kind: 'moon.MarqueeText',
				marqueeSpeed: 40,
				centered: true,
			}
		],
		onFocus: function(sender, e) {
			log.trace( 'chRow', 'onFocus', 'Focus row: ' + this.model.get('name'));
			if (e.originator === this) {
				guideModel.set('currentCell', this.model);
				this.doShowSelected({start: this.model.get('startTime'), end: this.model.get('endTime')});
				this.bubble('onRequestScroll');
			}
		},
		update: function() {
			this.applyStyle('order', this.get('position'));
		},
		showChanged: function() {
			assert(this.show, 'Model is undefined in grid cell');
			this.resize( this.model.get('endTime').getTime() - this.model.get('startTime').getTime() );
		},
		resize: function(secs) {
			var cellMargin = 3;
			var diff = (secs * cellWidth / halfHourMs) - cellMargin;
			this.applyStyle( 'width', diff + 'px' );
			this.applyStyle( 'min-width', diff + 'px' );
		},
		dummyChanged: function() {
			this.addRemoveClass('dummy', this.dummy);
			this.set('spotlight', !this.dummy);
		}
	});

	/**
	* chRow
	*/
	enyo.kind({
		name: 'chRow',
		classes: 'guide-row ch',
		published: {
			from: undefined,
			to: undefined
		},
		handlers: {
			onSpotlightSelect: 'onSelect',
			onSpotlightFocused: 'onFocused'
		},
		events: {
			onShowEPGDetails: ''
		},
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.resetListener = this.bindSafely('reset');
				this.modelChanged();
				this.init();
			};
		}),
		init: function() {
			log.debug('chRow', 'init', 'Init Row');
			this.createCell(new ShowModel({
				startTime: this.from,
				endTime: this.to,
				name: ''
			}), 1, false, undefined, true );
		},
		modelChanged: function(was, is) {
			if (was) {
				was.get('shows').off('reset', this.resetListener);
			}

			if (this.model) {
				this.model.get('shows').on('reset', this.resetListener);
			}
		},
		reset: function() {
			if (this.getAbsoluteShowing(true)) {
				if (enyo.Spotlight.getCurrent().isDescendantOf(this)) {
					enyo.Spotlight.spot(this.owner);
				}
				this.destroyClientControls();
				this.init();
			}
		},
		onSelect: function(oSender, oEvent) {
			this.doShowEPGDetails();
		},
		onFocused: function(oSender, oEvent) {
			oSender.model.updateInfo();
			guideModel.set('currentShow', oSender.model);
		},
		fetchShows: function() {
			this.model.removeInvalidShows();
			this.update(this.model.get('shows'));
		},
		fetchNewShows: function() {
			this.model.getShowsBetween( this.from, this.to, this.bindSafely('update') );
		},
		update: function(shows) {
			var slots = [];
			var self = this;

			shows.forEach(function(show) {
				var clientControls = self.getClientControls();
				var position = self.findNewPos(show);
				var currentCell = clientControls[position];

				var currentStartTime = currentCell.model.get('startTime');
				var currentEndTime = currentCell.model.get('endTime');
				var showStartTime = show.get('startTime');
				var showEndTime = show.get('endTime');

				assert(currentCell.get('dummy'), '[guide] Error inserting show: ' + show.get('name') +
					' and startTime: ' + showStartTime);

				//The dummy cell has the same size to the new show
				if (currentStartTime.getTime() >= showStartTime.getTime() &&
					currentEndTime.getTime() <= showEndTime.getTime()) {
					log.debug('chRow', 'update', 'Shows match' );
					//Modify current cell
					show = self.fixTiming( show );
					currentCell.set('model', show);
					currentCell.set('dummy', false);
				}
				//Dummy cell starts at the same time that the new show
				//Resize dummy and put it in the back of the hole
				else if (currentStartTime.getTime() >= showStartTime.getTime()) {
					log.debug('chRow', 'update', 'New show start match');
					self.createCell(show, currentCell.get('position'), true, currentCell);
					currentCell.model.set('startTime', showEndTime);
					currentCell.showChanged();
				} else if(currentStartTime < showStartTime) {
					log.debug('chRow', 'update', 'New show starts after dummy');
					var before = position == clientControls.length - 1 ? undefined : clientControls[position+1];
					if(currentEndTime > showEndTime) {
						var dummyModel = new ShowModel({
							startTime: showEndTime,
							endTime: currentEndTime,
							name: ''
						});
						var dummy = self.createCell(dummyModel, currentCell.get('position') + 2, true, before, true );
						before = dummy;
					}
					self.createCell(show, currentCell.get('position') + 1, true, before);
					currentCell.model.set('endTime', showStartTime);
					currentCell.showChanged();
				}
			});
			self.reOrder();
		},
		findNewPos: function(show) {
			var clientControls = this.getClientControls();
			var i = 0;
			log.debug('chRow', 'findNewPos', 'Client controls =%s', clientControls.length);
			while (clientControls[i].model.get('startTime') < show.get('startTime') &&
				(clientControls[i].model.get('endTime') < show.get('endTime')) && i < clientControls.length -1 ) {
				i++;
			}
			return i;
		},
		createCell: function(show, position, reOrder, before, dummy) {
			log.trace('chRow', 'createCell', 'New cell for show=%s channel=%s', show.get('name'), this.model.get('name'));
			log.trace('chRow', 'createCell', 'startTime=%s', show.get('startTime'));
			var component = {
				kind: 'showGridCell',
				model: this.fixTiming(show),
				triggerReOrder: reOrder,
				position: position
			};
			if (before) {
				component.addBefore = before;
			}
			var cell = this.createComponent( component, {owner: this.getInstanceOwner()} );
			cell.set('dummy', dummy === undefined ? false : true);
			cell.render();
			return cell;
		},
		reOrder: function() {
			var foundTrigger = false;
			var clientControls = this.getClientControls();
			clientControls.forEach( function(cell, i) {
				if (foundTrigger) {
					cell.set('position', clientControls[i-1].get('position') + 1);
					cell.set('triggerReOrder', false);
				}
				cell.update();
				foundTrigger = foundTrigger || cell.get('triggerReOrder');
			});
		},
		fixTiming: function(show) {
			show.set('startTime', this.getShowInit(show));
			show.set('endTime', this.getShowEnd(show));
			return show;
		},
		getShowInit: function(show) {
			return (show.get('startTime') < this.from && show.get('endTime') > this.from) ? this.from : show.get('startTime');
		},
		getShowEnd: function(show) {
			return (show.get('endTime') > this.to && show.get('startTime') < this.to) ? this.to : show.get('endTime');
		}
	});

	/**
	* guideChannelDetail
	*/
	enyo.kind({
		name: 'GuideChannelDetail',
		classes: 'guide-ch-detail',
		mixins: ['moon.MarqueeSupport'],
		marqueeOnRender: true,
		bindings: [
			{ from: '^guideModel.channelName', to: '.channelName' },
			{ from: 'show.name', to: '.showName' },
			{ from: '.showNameLabel', to: '.$.chShow.info' },
			{ from: '^guideModel.isFavorite', to: '.$.favIcon.showing' },
			{ from: '^guideModel.isBlocked', to: '.$.blockIcon.showing', transform: 'checkSession' },
			{ from: '^guideModel.currentShow', to: 'show' },
			{ from: 'show.startTime', to: '.$.showTime.info', transform: 'getHour' },
			{ from: 'show.description', to: '.$.text.content', transform: 'getShowDescription' },
		],
		computed: [
			{ method: 'showNameLabel', path: ['showName', 'channelName'] }
		],
		components: [
			{ kind: 'TVideo', name: 'video', style: 'min-width: 200px; max-width: 200px; height: 150px;' },
			{
				name: 'info', classes: 'guide-info',
				components: [
					{
						name: 'header', classes: 'guide-info-header',
						components: [
							{
								name: 'infoHeader', classes: 'guide-info-header-text',
								components: [
									{ name: 'chShow', kind: 'components.Detail', title: 'PROGRAMA:' },
									{ name: 'showTime', kind: 'components.Detail', title: 'HORARIO:' }
								]
							},
							{
								name: 'iconHeader',
								components: [
									{
										name: 'favIcon',
										classes: 'guide-popup-icon',
										kind: enyo.Image,
										src: assets('epg_fav.png'),
										showing: false
									},
									{
										name: 'blockIcon',
										classes: 'guide-popup-icon',
										kind: enyo.Image,
										src: assets('epg_bloq.png'),
										showing: false
									}
								]
							},
						]
					},
					{ name: 'text', classes: 'guide-info-text' }
				]
			},
		],
		getHour: function(value) {
			return (value? util.dateToString(value) : 'Horario no disponible');
		},
		showNameLabel: function() {
			var show = this.get('showName');
			return show? (show + ' - ' + this.get('channelName')).toUpperCase(): 'Programa no disponible.';
		},
		getShowDescription: function(value) {
			return value? value: 'Descripción no disponible.';
		},
		showVideo: function() {
			log.info('GuideChannelDetail', 'showVideo');
			this.$.video.play();
		},
		hideVideo: function() {
			log.info('GuideChannelDetail', 'hideVideo');
			this.$.video.pause();
		},
		checkSession: function(value) {
			var isOpen = session.get('isOpen');
			return isOpen ? value : isOpen;
		}
	});

	/**
	* guideHeader
	*/
	enyo.kind({
		name: 'guideHeader',
		kind: 'moon.Scroller',
		classes: 'guide-header',
		vertical: 'hidden',
		horizontal: 'hidden',
		spotlight: false,
		spotlightDisable: true,
		spotlightPagingControls: false,
		published: {
			from: undefined,
			to: undefined,
		},
		components: [
			{ name: 'timeElapse', classes: 'guide-header-row' },
			{ name: 'progressBar', kind: 'ProgressBar', showLabels: false, value: 0, width: 912 }
		],
		/* private */
		_currentOffset: 0,
		_maxColumns: 4,
		_refreshInterval: 60000,
		fromChanged: function(was, is) {
			log.trace('GuideHeader', 'fromChanged', 'Create guide header');
			if (this.from) {
				var components = [];
				var current = this.from.getTime();
				var res = current % halfHourMs; //Saco el resto de la divisón de una hora, para obtener la última válida
				this.from = new Date(current - res);

				var tempDate = this.from;
				for( var i=1; i<4; i++) {
					var tempFrom = tempDate;
					while (tempFrom.getDay() === tempDate.getDay() ) {
						components.push( { date: tempDate, kind: 'timeCell'} );
						tempDate = new Date( tempDate.getTime() + halfHourMs );
					}
				}

				// Remove invalid cells
				if (was) {
					this.$.timeElapse.getClientControls().filter(function(cell) {
						return cell.date < this.from;
					}.bind(this)).forEach(function(cell) {
						cell.destroy();
					});
				}

				this.to = tempDate;
				this.$.timeElapse.createComponents(components, { owner: this.$.timeElapse.getInstanceOwner() }).forEach(function(cell) {
					cell.render();
				});
			}
		},
		updateProgressBar: function() {
			var current = new Date();
			if (this._currentOffset <= this._maxColumns && current >= this.from && current <= this.to) {

				var timeRange = halfHourMs*4; //Milisegundos en 2 horas
				var end  = new Date( this.from.getTime() + timeRange );

				this.$.progressBar.set('value', ((current.getTime() - this.from.getTime())* 100) / timeRange);
				this.$.progressBar.show();
				this.startJob('refresh', this.bindSafely('updateProgressBar'), this._refreshInterval);
			}
		},
		updateHeader: function(x, y) {
			this.scrollTo(x, y, false);
		},
		startRefreshTask: function(start) {
			if (start) {
				this.refreshJob();
			} else {
				this.stopJob('refresh');
			}
		},
		refreshJob: function() {
			this.updateProgressBar();
			this.startJob('refresh', this.bindSafely('refreshJob'), this._refreshInterval);
		}
	});

	// Aux function to set the before element and keep de grid ordered
	function setBefore(defs, comps) {
		defs.forEach(function(def) {
			def.addBefore = comps.getClientControls().find(function(c) {
				return c.get('model').get('id') > def.model.get('id');
			}, this);
		});
	}

	enyo.kind({
		name: 'ScrollWrapper',
		spotlight: 'container',
		handlers: {
			onSpotlightFocused : 'focused',
		},
		focused: function(sender, oevt) {
			var evt = enyo.Spotlight.getLast5WayEvent();
			var dir = evt? evt.type.replace('onSpotlight', '') : '';

			if (enyo.Spotlight.getCurrent() === this && (dir === 'Up' || dir === 'Down')) {
				var y = dir === 'Down'? this.getAbsoluteBounds().top : this.getAbsoluteBounds().bottom;
				var x = evt.originator.getAbsoluteBounds().left;
				var ctrol = enyo.Spotlight.NearestNeighbor.getNearestPointerNeighbor(this, dir.toUpperCase(), x, Math.abs(y));

				enyo.Spotlight.spot(ctrol);
				return true;
			}

			return false;
		}
	});

	enyo.kind({
		name: 'gridWrapper',
		classes: 'guide-vertical-scroller',
		kind: 'moon.Scroller',
		width: 1111,
		height: 186,
		maxHeight: 186,
		strategyKind: 'NoIndicatorScrollStrategy',
		horizontal: 'hidden',
		vertical: 'scroll',
		spotlight: 'container',
		handlers: {
			onSpotlightContainerLeave: 'onLeave',
			onRequestScroll : 'requestScroll',
			onSpotlightFocused : 'focused',
		},
		components: [
			{
				classes: 'guide-gridwrapper',
				components: [
					{
						name: 'chNames',
						spotlight: 'container',
						mixins: [CircularNavSupport],
						classes: 'guide-chnames',
						onSpotlightFocused: 'onFocusCh',
						onSpotlightSelect: 'onSelectCh'
					},
					{
						name: 'chs',
						kind: 'moon.Scroller',
						vertical: 'hidden',
						strategyKind: 'NoIndicatorScrollStrategy',
						classes: 'scroller',
						components: [
							{
								name: 'scrollwrapper',
								kind: 'ScrollWrapper',
								onSpotlightFocused: 'onFocusCell'
							}
						]
					}
				]
			}
		],
		focused: function(sender, oevt) {
			var evt = enyo.Spotlight.getLast5WayEvent();
			var dir = evt? evt.type.replace('onSpotlight', '') : '';
			if (enyo.Spotlight.getCurrent() === this.$.scrollwrapper && dir === 'Left') {
				var y = evt.originator.getAbsoluteBounds().top;
				var x = evt.originator.getAbsoluteBounds().left;
				var ctrol = enyo.Spotlight.NearestNeighbor.getNearestPointerNeighbor(this, dir.toUpperCase(), x, Math.abs(y));

				enyo.Spotlight.spot(ctrol);
				return true;
			}

			return false;
		},
		onLeave: function() {
			this.stopJob('changeCh');
			this.lastID = -1;
			this.changeCh();
		},
		requestScroll: function(oSender, oEvent) {
			this.$.chs.$.strategy.requestScrollIntoView(oSender, oEvent);
			var elementBounds = oEvent.originator.getBounds();
			var scrollBounds = this.getScrollBounds();
			var elementBottom = elementBounds.top+elementBounds.height;
			var scrollerBottom = scrollBounds.clientHeight + this.getScrollTop();
			if (elementBottom > scrollerBottom) {
				var diff = elementBottom - scrollerBottom;
				this.setScrollTop(this.getScrollTop() + diff);
			} else if(elementBounds.top < this.getScrollTop()) {
				this.setScrollTop(elementBounds.top);
			}

			return true;
		},
		onSelectCh: function(oSender, oEvent) {
			this.stopJob('changeCh');
			this.changeCh().then(
				enyo.Signals.send.bind(enyo.Signals, 'onTVMode', {mode: 'fullscreen'})
			);
		},
		onFocusCh: function(oSender, oEvent) {
			this.onFocusRow(oEvent.originator.model);
		},
		onFocusCell: function(oSender, oEvent) {
			this.onFocusRow(oEvent.originator.parent.model);
		},
		onFocusRow: function(model) {
			this.lastID = model? model.get('id') : -1;
			this.startJob('changeCh', this.bindSafely('changeCh'), 2000);
		},
		changeCh: function() {
			return tvdadapter.change( this.lastID );
		},
		populate: function(chNames, chShows) {
			var current = enyo.Spotlight.getCurrent();
			var spot = current ? current.isDescendantOf(this) : undefined;

			setBefore(chNames, this.$.chNames);
			this.$.chNames.createComponents(chNames, {owner: this}).forEach(function(c) {
				c.render();
			});

			setBefore(chShows, this.$.scrollwrapper);
			this.$.scrollwrapper.createComponents(chShows, {owner: this}).forEach(function(c) {
				c.render();
			});

			if (spot) {
				this.scrollToControl(current);
			}

		},
		traverseRows: function(fnc) {
			this.$.scrollwrapper.getClientControls().forEach(fnc);
		},
		destroyShows: function() {
			this.traverseRows(function(row){
				row.reset();
			});
			this.resetScroll();
		},
		reset: function(from, to) {
			this.traverseRows(function(row){
				row.from = from;
				row.to = to;
			});
			this.destroyShows();
		},
		resetScroll: function() {
			this.$.chs.setScrollLeft(0);
		},
		selectCh: function(chId) {
			var chComponent = this.$.chNames.getClientControls().find(function(ch) {
				return ch.model.get('id') === chId;
			});

			if (chComponent !== undefined) {
				enyo.Spotlight.spot(chComponent);
			}

		},
		destroyChannels: function( chModels ) {
			function finder(chView) {
				return chModels.indexOf(chView.model) > -1;
			}

			var current = enyo.Spotlight.getCurrent();
			var spot = current.isDescendantOf(this);

			var row;
			if (spot) {
				enyo.Spotlight.freeze();
				if (finder(current)) {
					row = enyo.Spotlight.NearestNeighbor.getNearestNeighbor('UP', current);
					if (!row) {
						row = enyo.Spotlight.NearestNeighbor.getNearestNeighbor('DOWN', current);
					}
				} else {
					row = current;
				}
			}

			this.$.scrollwrapper.getClientControls().filter(finder)
			.concat(this.$.chNames.getClientControls().filter(finder))
			.forEach(function(c) {
				c.destroy();
			});

			if (spot) {
				enyo.Spotlight.unfreeze();
				this.resize();
				if (!enyo.Spotlight.spot(row)) {
					enyo.Spotlight.spot(this.owner);
				}
				this.stabilize();
			}
		},
		clean: function() {
			this.$.scrollwrapper.destroyClientControls();
			this.$.chNames.destroyClientControls();
		}
	});

	/**
	* Grid
	*/
	enyo.kind({
		name: 'Grid',
		classes: 'guide-channels',
		kind: 'components.ViewContent',
		mixins: [DISupport],
		handlers: {
			onScroll: 'onScrollStop',
		},
		bindings: [
			{ from: '^guideModel.currentCell', to: '.$.dateLabel.content', transform: function(model) {
				var date = model ? model.get('startTime') : this.date;
				return date ? util.formatDate(date) : ''; }
			},
			{ from: '.date', to: '.$.guideHeader.from' }
		],
		components: [
			{
				name: 'guideBox', classes: 'guide-box',
				components: [
					{ name: 'channelDetail', kind: 'GuideChannelDetail' },
					{
						name: 'guideWrapper',
						components: [
							{
								name: 'timeLine',
								classes: 'header',
								components: [
									{ name: 'dateLabel', classes: 'guide-cell all' },
									{ name: 'guideHeader', kind: 'guideHeader', classes: 'time-header' }
								]
							},
							{ name: 'gridWrapperComponent', kind: 'gridWrapper' }
						]
					},

				],
			},
			{ kind: 'Signals', onTVMode: 'onTvMode' },
			{ kind: 'Signals', TVModeActived: 'onTVModeActived' },
		],
		_actived: false,
		_poolInterval: 5000,
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.collection = this.getAPI('chModel');

				this.collection.on('add', this.bindSafely('onModelsAdded'));
				this.collection.on('reset', this.bindSafely('onCollectionReset'));
				this.collection.on('remove', this.bindSafely('onCollectionRemove'));

				var date = new Date();
				this.set('date', date);
				this.$.guideHeader.set('from', date);

				var models = this.collection.models.slice();
				if (models.length) this.onModelsAdded({}, {}, {models: models});
			};
		}),
		onContentEnter: enyo.inherit(function(sup) {
			return function(oSender, oEvent) {
				sup.apply(this, arguments);
				if (oEvent.originator === this) {
					log.info('Grid', 'onEnter');
					this.set('date', new Date());
					this.$.gridWrapperComponent.reset(this.$.guideHeader.get('from'), this.$.guideHeader.get('to'));
					this.$.gridWrapperComponent.traverseRows(function(row) {
						row.fetchShows();
					});
					this.startJob('updateShows', this.bindSafely('updateShows'), this._poolInterval);
					this._actived = true;
					this.$.channelDetail.showVideo();
					this.$.guideHeader.startRefreshTask(true);
				}
			}
		}),
		onTvMode: function(oSender, oEvent) {
			if (this._actived) {
				if (oEvent.mode === 'previous') {
					this.$.channelDetail.showVideo();
					this.$.guideHeader.startRefreshTask(true);
				}
				else if (oEvent.mode === 'fullscreen') {
					this.$.channelDetail.hideVideo();
					this.$.guideHeader.startRefreshTask(false);
				}
			}
		},
		onTVModeActived: function(oSender, oEvent) {
			if (this._actived) {
				if (oEvent.mode === 'off') {
					this.$.gridWrapperComponent.selectCh(tvdadapter.current(false));
				}
			}
		},
		onContentLeave: enyo.inherit(function(sup) {
			return function(oSender, oEvent) {
				sup.apply(this, arguments);
				if (oEvent.originator === this || oEvent.originator === this.$.gridWrapperComponent) {
					log.info('Grid', 'onLeave');
					this.stopJob('updateShows');
					this.$.gridWrapperComponent.destroyShows();
					guideModel.reset();
					this._actived = false;
					this.$.channelDetail.hideVideo();
					this.$.guideHeader.startRefreshTask(false);
				}
			}
		}),
		onScrollStop: function(oSender, oEvent) {
			log.trace('Grid', 'onScrollStop');
			if (oEvent.scrollBounds && oSender.isDescendantOf(this.$.gridWrapperComponent)) {
				this.$.guideHeader.updateHeader(oEvent.scrollBounds.left, 0);
				return true;
			}
		},
		onModelsAdded: function(oSender, oEvent, oAdded) {
			log.debug('Grid', 'Added channels');
			var channels = [];
			var chRows = [];
			oAdded.models.forEach(function(ch, i) {
				var rowName = 'chrow' + ch.get('name');
				channels.push({
					kind: 'channelGridCell',
					defaultSpotlightRight: rowName,
					model: ch
				});
				chRows.push({
					name: rowName,
					kind: 'chRow',
					model: ch,
					from: this.$.guideHeader.get('from'),
					to: this.$.guideHeader.get('to')
				});
			}.bind(this));

			this.$.gridWrapperComponent.populate(channels, chRows);
		},
		onCollectionReset: function() {
			this.$.gridWrapperComponent.clean();
		},
		onCollectionRemove: function(oSender, oEvent, oRemoved) {
			this.$.gridWrapperComponent.destroyChannels(oRemoved.models);
		},
		updateShows: function() {
			log.trace('Grid', 'updateShows');
			this.$.gridWrapperComponent.traverseRows(function(row) {
				row.fetchNewShows();
			});
			this.startJob('updateShows', this.bindSafely('updateShows'), this._poolInterval);
		}
	});

	/**
	* Guide
	*/
	enyo.kind({
		name: 'Guide',
		spotlight: 'container',
		help: {txt: msg.help.guide.ch, img: 'carousel-tda.png'},
		handlers: {
			onShowEPGDetails: 'onShowDetails',
			onSpotlightKeyDown: 'onKey',
			onSpotlightContainerEnter: 'onEnter'
		},
		bindings: [
			{ from: 'showGuide', to: '.$.guideContainer.showing' },
			{ from: '^availableChannels.length', to: '.channels' },
			{ from: '^scanModel.isScanning', to: '.isScanning' }
		],
		computed: [
			{ method: 'showGuide', path: ['channels', 'isScanning'] }
		],
		components: [
			{
				name: 'guideContainer',
				classes: 'ch-content',
				components: [
					{
						name: 'grid',
						kind: 'Grid',
						dependencies: {'chModel': availableChannels}
					}
				]
			},
			{ name: 'popup', kind: 'GuideShowPopup' },
		],
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.popup.on('accepted', function() {
					// Get the chance to recover the focus before send fullscreen request
					enyo.asyncMethod(this, function() {
						enyo.Signals.send('onTVMode', {mode: 'fullscreen'});
					});
				});
			};
		}),
		showGuide: function() {
			return this.get('channels') > 0 && !this.get('isScanning');
		},
		onShowDetails: function(oSender, oEvent) {
			enyo.Spotlight.States.push('focus');
			this.$.popup.show();
		},
		onKey: function(oSender, oEvent) {
			// Filter EPG shortcut
			return oEvent.keyCode === tacKeyboardLayout.VK_GUIDE;
		},
		onEnter: function() {
			Context.setHelp(this.help);
		}
	});

	/**
	* GuideShowPopup
	*/
	enyo.kind({
		name: 'GuideShowPopup',
		kind: 'ConfirmDialog',
		acceptLabel: 'VER PROGRAMA',
		denyLabel: 'CERRAR',
		showing: false,
		mixins: ['moon.MarqueeSupport'],
		components: [
			{
				namme: 'titleWrapper', classes: 'guide-popup-title-wrapper',
				components: [
					{ name: 'showName', classes: 'guide-popup-title', kind: 'moon.MarqueeText' },
					{ name: 'favIcon', classes: 'guide-popup-icon', kind: enyo.Image, src: assets('fav_epg.png'), showing: false },
					{ name: 'blockIcon', classes: 'guide-popup-icon', kind: enyo.Image, src: assets('bloq_epg.png'), showing: false }
				]
			},
			{ name: 'hour', classes: 'guide-popup-details', content: 'HORARIO: ' },
			{ name: 'duration', classes: 'guide-popup-details', content: 'DURACIÓN: ' },
			{ name: 'audio', classes: 'guide-popup-details', content: 'AUDIO: ' },
			{ name: 'clasification', classes: 'guide-popup-details', content: 'CLASIFICACIÓN: ' },
			{
				name: 'scroll',
				kind: 'ShellScroller',
				horizontal: 'hidden',
				classes: 'guide-scroller-desc guide-popup-description',
				components: [
					{ name: 'description', 	allowHtml: true, spotlight: true },
				]
			},

			{ name: 'btnContainer', classes: 'popup-align-center popup-btn-container' }
		],
		bindings: [
			{ from: '^guideModel.isFavorite', to: '.$.favIcon.showing' },
			{ from: '^guideModel.isBlocked', to: '.$.blockIcon.showing', transform: 'checkSession' },
			{ from: '^guideModel.currentShow', to: 'currentShow' },
			{ from: '^guideModel.channelName', to: 'channelName' },
			{ from: 'currentShow.name', to: 'showName' },
			{ from: 'getTitle', to: '.$.showName.content' },
			{ from: 'currentShow.startTime', to: '.$.hour.content', transform: 'setHour' },
			{ from: 'currentShow.length', to: '.$.duration.content', transform: 'setDuration' },
			{ from: 'currentShow.audio', to: '.$.audio.content', transform: 'setAudio' },
			{ from: 'currentShow.parentalAge', to: '.$.clasification.content', transform: 'setClasification' },
			{ from: 'currentShow.description', to: '.$.description.content' },
			{ from: 'currentShow.description', to: '.description' },
			{ from: '^session.isOpen', to: '.sessionOpen' }
		],
		computed: [
			{ method: 'getTitle', path: ['showName', 'channelName']}
		],
		descriptionChanged: function() {
			this.$.scroll.resize();
			this.scrollTo(0);
		},
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.title.destroy();
				this.$.icon.destroy();
				this.$.descriptionContainer.destroy();
				this.$.wrapper.addClass('guide-popup');
				this.$.actions.addClass('guide-popup-btn-container');
			};
		}),
		keyPressed: enyo.inherit(function(sup) {
			return function(sender, evt) {
				switch(evt.keyCode) {
					case tacKeyboardLayout.VK_UP:
						var verticalPos = this.$.scroll.getScrollBounds().top - 10;
						this.scrollTo(verticalPos);
						return true;
					case tacKeyboardLayout.VK_DOWN:
						var verticalPos = this.$.scroll.getScrollBounds().top + 10;
						this.scrollTo(verticalPos);
						return true;
				}
				return sup.apply(this, arguments);
			}
		}),
		showingChanged: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				var fnc = this.showing? 'startMarquee' : 'stopMarquee';
				this[fnc]();
				this.$.scroll.resize();
				this.scrollTo(0);
			}
		}),
		scrollTo: function(verticalPos) {
			// Scrolls text to the position given.
			this.$.scroll.scrollTo(0, verticalPos, false);
			this.$.scroll.setScrollTop(verticalPos);
		},
		getTitle: function() {
			var name = this.get('showName');
			return name ? (name + ' - ' + this.get('channelName')).toUpperCase() : 'Nombre no disponible';
		},
		setHour: function(value) {
			return 'HORARIO:  '+ (value ? util.dateToString(value)+' hs.' : 'Horario no disponible');
		},
		setDuration: function(value) {
			return 'DURACIÓN: '+ (value ? value + 'm.' : 'Duración no disponible');
		},
		setAudio: function (audio) {
			return 'AUDIO: '+ (audio === 'Unknown' ? 'Idioma no disponible' : util.capitalize(i18(audio)));
		},
		setClasification: function(value) {
			return 'CLASIFICACIÓN: '+ (value ? value : 'Clasificación no disponible');
		},
		checkSession: function(value) {
			var isOpen = session.get('isOpen');
			return  isOpen ? value : isOpen;
		},
		sessionOpenChanged: function(oldValue, newValue) {
			if (!newValue && this.showing) {
				this.doAction(this._exitAction);
			}
		}
	});
})(this);
