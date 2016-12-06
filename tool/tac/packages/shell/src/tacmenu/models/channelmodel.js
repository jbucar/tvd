/*
* ShowModel
*/
enyo.kind({
	name: 'ShowModel',
	kind: 'enyo.Model',
	attributes: {
		name: '',
		startTime: undefined,
		endTime: undefined,
		length: 0,
		description: '',
		audio: 'Unknown',
		parentalContent: '',
		parentalAge: '',
		channel: undefined
	},
	computed: [
		{ method: 'length', path: ['startTime', 'endTime'] }
	],
	length: function() {
		var start = this.get('startTime');
		var end = this.get('endTime');
		return (start && end)? Math.round((end.getTime() - start.getTime()) / 60000) : 0;
	},
	updateInfo: function() {
		tvd.player.audioInfo(util.catchError(function setAudio(res) {
			this.set('audio', res.lang);
		}.bind(this)));

		this.updateSubtitles();
	},
	updateSubtitles: function() {
		tvd.player.subtitleInfo(util.catchError(function getSubInfo(res) {
			this.set('subtitleCount', res.count);
		}.bind(this)));
	}
});

enyo.kind({
	name: 'ShowsCollection',
	kind: 'enyo.Collection',
	options: {
		merge:true,
		sort: function( a, b ) {
			return a.get('startTime').getTime() - b.get('startTime').getTime();
		}
	},
	model: 'ShowModel',
});

enyo.kind({
	name: 'ChModel',
	mixins:[DISupport],
	kind: 'enyo.Model',
	attributes: {
		name: '',
		channel: undefined,
		id: undefined,
		logo: undefined,
		currentShow: undefined,
		shows: undefined,
		isFavorite: undefined,
		isBlocked: undefined
	},
	bindings: [
		{ from: '^session.hasPassword', to:'.isBlocked', transform: 'setBlocked' }
	],
	initialize: function() {
		var shows = new ShowsCollection();
		this.set('shows', shows);

		var cur = new ShowModel();
		cur.set('channel', this);
		this.set('currentShow', cur);

		session.on('onSessionChanged', function(sender, evt, info) {
			if (!info.open) {
				shows.empty();
			}
		});
	},
	fetchShow: function( cbk ) {
		var show = this.get('currentShow');
		this.getAPI('tvd').channel.getShow( this.get('id'), util.catchError(function fetchCurrentShow( current ) {
			current.startTime = current.startTime ? new Date(current.startTime) : undefined;
			current.endTime = current.endTime ? new Date(current.endTime) : undefined;

			show.set(current);
			show.updateInfo();

			if (cbk) {
				cbk(show);
			}
		}));
	},
	toggleFavorite: function() {
		var self = this;
		this.getAPI('tvd').channel.toggleFavorite(this.get('id'), util.catchError(function setFav() {
			var toggled = !self.get('isFavorite');
			self.set('isFavorite', toggled);
			notification.emit('favoriteChannel', toggled);
		}));
	},
	toggleBlocked: function(cbk) {
		var self = this;
		this.getAPI('tvd').channel.toggleBlocked( this.get('id'), util.catchError(function() {
			self.set('isBlocked', !self.attributes.isBlocked);
			notification.emit('blockedChannel', {isBlocked: self.attributes.isBlocked});
			if (cbk) {
				cbk( self.attributes.isBlocked );
			}
		}));
	},
	setBlocked: function(value) {
		return enyo.isTrue(value) ? this.get('isBlocked') : false;
	},
	getShowsBetween: function(from, to, cbk) {
		var self = this;
		var chID = this.get('id');
		log.trace('channelmodel', 'getShowsBetween', 'Get shows for channel: ' + this.get('name') + ' between: ' + from + ' to: ' + to);
		this.getAPI('tvd').channel.getShowsBetween( chID, util.parseDate(from), util.parseDate(to), util.catchError(function(chShows) {
			chShows.forEach( function(show) {
				show.startTime = new Date( show.startTime );
				show.endTime = new Date( show.endTime );
				show.id = show.startTime.getTime()+chID;
				show.channel = self;
				log.trace('channelmodel', 'getShowsBetween', 'channel: ' + self.get('name') + ' name: ' + show.name + ' startTime: ' + show.startTime);
			});

			log.trace( 'channelmodel', 'getShowsBetween', 'length: ' + self.get('shows').length);
			var added = self.get('shows').add(chShows, {merge:true});

			log.trace('channelmodel', 'getShowsBetween', 'added ' + added.length + ' channels');
			if (added.length > 0) {
				cbk(added);
			}
		}));
	},
	removeInvalidShows: function() {
		var shows = this.get('shows');
		var current = new Date();
		var res = current % 1800000;
		var today = new Date( current - res );
		var model = shows.find( function(show) {
			return show.get('startTime') >= today || show.get('endTime') > today;
		});
		var index = shows.indexOf(model);
		if (index > 0) {
			var invalid = shows.models.slice(0, index);
			shows.remove(invalid);
		}
	}
});

/*
* DetailsModel
*/
enyo.kind({
	name: 'DetailsModel',
	kind: 'enyo.Model',
	attributes: {
		audioPID: -1,
		audioChannels: -1,
		audioRate: -1,
		videoPID: -1,
		videoRes: -1,
		videoFR: -1,
		quality: -1,
		level: -1
	},
	updateAV: function() {
		var self = this;
		tvd.player.audioInfo( util.catchError(function( res ) {
			self.set( 'audioPID', res.pid );
			self.set( 'audioChannels', res.channels );
			self.set( 'audioRate', res.rate );
		}));
		tvd.player.videoInfo( util.catchError(function( res ) {
			self.set( 'videoPID', res.pid );
			if (res.height !== -1 && res.width !== -1) {
				self.set( 'videoRes', res.width + ' * ' + res.height );
			}
			self.set( 'videoFR', res.fps );
		}));
	},
	updateSignal: function() {
		var self = this;
		tvd.player.status( util.catchError(function( res ) {
			self.set( 'quality', res.quality );
			self.set( 'level', res.signal );
		}));
	},
	reset: function() {
		this.set('audioPID', -1);
		this.set('audioChannels', -1);
		this.set('audioRate', -1);
		this.set('videoPID', -1);
		this.set('videoRes', -1);
		this.set('videoFR', -1);
		this.set('quality', -1);
		this.set('level', -1);
	}
});

function setLogo(ch) {
	ch.logo = ch.logo? ch.logo : assets('default_selected.png');
}

/*
* ChsCollection
*/
enyo.kind({
	name: 'ChsCollection',
	mixins:[DISupport],
	model: 'ChModel',
	kind: 'enyo.Collection',
	initialize: function() {
		var self = this;
		this.tvd = this.getAPI('tvd');
		this.tvdAdapter = this.getAPI('tvdAdapter');
		this.tvd.on( 'currentShowChanged', function() {
			var chID = self.tvdAdapter.current(false);
			if ( chID !== -1) {
				self.getModelFromID(chID).fetchShow();
			}
		});

		this.getAPI('scan').addObserver('isScanning', this.set.bind(this, 'batching'));

		this.tvd.on( 'channelAdded', this.addChannel.bind(this));

		this.tvd.on( 'playerProtectedChanged', function(isProtected) {
			if (isProtected) {
				self.emit('onBlockedCh', {
					reason: self.isChBlocked(self.tvdAdapter.current(false)) ? 'ch' : 'content'
				});
			}
		});

		this.tvd.on('logosChanged', function invalidateLogos() {
			self.forEach(function(ch) {
				self.tvd.channel.get(ch.get('id'), util.catchError(function getCh( info ) {
					if (info.logo) {
						ch.set('logo', info.logo);
					}
				}));
			});
		});

		return this;
	},
	prepareModel: enyo.inherit(function(sup) {
		return function() {
			var mdl = sup.apply(this, arguments);
			mdl.addAPIs({'tvd': this.tvd});
			mdl.initialize();

			return mdl;
		};
	}),
	addChannel: function(chID) {
		this.tvd.channel.get(chID, util.catchError(function(ch) {
			setLogo(ch);
			this.add(ch);
		}.bind(this)));
	},
	populate: function() {
		if (this.length > 0) {
			this.empty();
		}

		var get = util.promisify(this.tvd.channel.get);

		this.tvd.db.getAll(util.catchError(function(add) {
			Promise.all(add.map(function(id) {
				return get(id);
			})).then(function(channels) {
				channels.forEach(setLogo);

				this.add(channels);
			}.bind(this));

		}.bind(this)));
	},
	isChBlocked: function( chID ) {
		var model = this.getModelFromID( chID );
		return model ? model.get('isBlocked') : true;
	},
	getModelFromNumber: function( chNumber ) {
		return this.find(function( ch ) {
			return ch.get('channel') === chNumber;
		});
	},
	getModelFromID: function( chID ) {
		return this.find(function( ch ) {
			return ch.get('id') === chID;
		});
	}
});

/*
* FilterCollection
*/
enyo.kind({
	name: 'FilterCollection',
	kind: 'enyo.Collection',
	published: {
		collection: '',
	},
	bindings: [
		{ from: '.collection.batching', to: '.batching' }
	],
	options: {
		merge: true,
		sort: function(a, b) {
			return a.get('id') - b.get('id');
		}
	},
	collectionChanged: function(was, is) {
		if (is) {
			var handler = this.bindSafely(function(sender, evtName, evt) {
				switch (evtName) {
					case 'reset':
						this.empty();
						break;
					case 'change':
						this._evaluateModel(evt.model);
						break;
					case 'add':
						this._ensureAdd(evt.models);
						break;
					case 'remove':
						this.remove(evt.models);
						break;
				}
			});

			['add', 'change', 'reset', 'remove'].forEach(function(evt) {
				is.on(evt, handler);
			});

			var mdls = is.models.slice(0);
			if (mdls.length) {
				this._ensureAdd(mdls);
			}
		}
	},
	_ensureAdd: function( models ) {
		var toAdd = models.filter(this.isFiltered, this);
		this.add(toAdd);
	},
	_evaluateModel: function( model ) {
		var isFiltered = this.isFiltered(model);
		if (isFiltered !== undefined) {
			if (isFiltered) {
				if (!this.has(model)) {
					this.add(model);
				}
			}
			else {
				this.remove(model);
			}
		}
	}
});

/*
* ChCategoriesCollection
*/
enyo.kind({
	name: 'ChCategoriesCollection',
	kind: 'enyo.Collection',
	published: {
		collection: '',
	},
	collectionChanged: function() {
		this.collection.on('add', this.bindSafely('collectionModelsChanged'));
		this.collection.on('remove', this.bindSafely('collectionModelsChanged'));
	},
	collectionModelsChanged: function(oSender, oEvent, oAddedRemoved) {
		var cbk = this.bindSafely( 'showChanged' );
		this.collection.forEach(function( ch ) {
			ch.fetchShow(cbk);
		});
	},
	showChanged: function( show ) {
		this.add( show.get('category') );
	}
});

/*
* FavChCollection
*/
enyo.kind({
	name: 'FavChCollection',
	kind: 'FilterCollection',
	isFiltered: function( model ) {
		return  model.get('isFavorite');
	}
});

/*
* BckChCollection
*/
enyo.kind({
	name: 'BckChCollection',
	kind: 'FilterCollection',
	isFiltered: function( model ) {
		return model.get('isBlocked');
	}
});

enyo.kind({
	name: 'ChannelCategory',
	kind: 'enyo.Model',
	primaryKey: 'name',
});

function ChannelCategoriesCtor(session, favs) {
	var categories = new enyo.Collection({
		model: 'ChannelCategory',
		populate: function() {
			this.add({name: 'TODOS', id: 'all'});
		}
	});

	{
		// Add blocked category when session is Open
		var blockedModel = new ChannelCategory({name: 'BLOQUEADOS', id: 'blocked'});
		session.addObserver('isOpen', function(was, is) {
			var method = is? 'add' : 'remove';
			categories[method](blockedModel);
		});
	}

	{
		// Update collection when a channel is favoritized
		var favModel = new ChannelCategory({name: 'FAVORITOS', id: 'favs'});
		favs.on('add', categories.add.bind(categories, favModel));
		favs.on('remove', function() {
			if (favs.models.length === 0) {
				categories.remove(favModel);
			}
		});
	}

	return categories;
};

enyo.kind({
	name: 'AvailableChCollection',
	kind: 'FilterCollection',
	bindings: [
		{ from: '^session.isOpen', to: '.filterActive' }
	],
	init: function() {
		this.filterActiveChanged();
	},
	filterActiveChanged: function() {
		if (this.collection) {
			var toKeep = this.collection.filter(this.isFiltered, this);
			if (session.get('isOpen')) {
				this.add(toKeep);
			}
			else {
				this.remove(_.difference(this.collection.models, toKeep));
			}
		}
	},
	isFiltered: function( model ) {
		return !model.get('isBlocked') || session.get('isOpen');
	}
});
