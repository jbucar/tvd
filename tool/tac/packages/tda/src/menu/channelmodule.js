/* globals EventEmitter */
/* exported ChannelModel */

function ChannelModel() {
	var self = {};
	var _channels = [];
	var _events = new EventEmitter();
	var _res;

	function changed() {
		_events.emit( 'changed' );
	}

	function getChannels() {
		//	Get all apps
		return apiCallAsync( tvd.db.getAll )
			.map( chID => apiCallAsync(tvd.channel.get,chID) )
			.then(function(channels) {
				channels.forEach(function (ch) {
					if (!ch.logo) {
						ch.logo = ui.getResource('images','menu/default_channel');
					}
					_channels.push( ch );
				});
				changed();
			});
	}

	function showNotify( msgID, icon ) {
		notification.emit( 'ApplicationNotification', {
			msg: _res.txt(msgID),
			icon: _res.img(icon,true)
		});
	}

	self.init = function() {
		log.info( 'ChannelModel', 'Init' );

		_res = ui.getResources( 'menu' );

		return getChannels()
			.then(function() {
				tvd.on('scanChanged', function(evt) {
					if (evt.state === 'begin') {
						_channels.splice( 0, _channels.length );
						changed();
					}
					else if (evt.state === 'end') {
						getChannels();
					}
				});
			});
	};

	self.fin = function() {
	};

	forwardEventEmitter( self, _events );

	self.getAll = function() {
		return _channels;
	};

	self.get = function(chID) {
		return _channels.find( (ch) => ch.id === chID );
	};

	self.toogleFavorite = function( chID ) {
		log.verbose( 'ChannelModel', 'toggleFav: chID=%s', chID );
		let ch = self.get(chID);
		if (ch) {
			return apiCallAsync( tvd.channel.toggleFavorite, chID )
				.then(function() {
					ch.isFavorite = !ch.isFavorite;
					showNotify( ch.isFavorite ? 'notifyFavorite' : 'notifyNotFavorite', 'favorites' );
					changed();
				});
		}
		return Promise.reject( new Error('Invalid chID') );
	};

	self.toogleBlocked = function( chID ) {
		log.verbose( 'ChannelModel', 'toggleBlocked: chID=%s', chID );
		let ch = self.get(chID);
		if (ch) {
			return apiCallAsync( tvd.channel.toggleBlocked, chID )
				.then(function() {
					ch.isBlocked = !ch.isBlocked;
					showNotify( ch.isBlocked ? 'notifyBlocked' : 'notifyNotBlocked', 'blocked' );
					changed();
				});
		}
		return Promise.reject( new Error('Invalid chID') );
	};

	return self;
}
