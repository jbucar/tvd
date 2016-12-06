
(function() {

	var INVALID_CH = -1;

	enyo.kind({
		name: 'TvdAdapter',
		mixins:[DISupport, enyo.ObserverSupport, enyo.ComputedSupport, enyo.BindingSupport, enyo.EventEmitter],
		_channel: INVALID_CH,
		_virtual: INVALID_CH,
		_previous: INVALID_CH,
		init: function() {
			// Promisify change
			this.getAPI('tvd').player.change = util.promisify(this.getAPI('tvd').player.change);
			return this;
		},
		channelUp: function( isVirtual ) {
			this.changeFactor(isVirtual, 1);
		},
		channelDown: function ( isVirtual ) {
			this.changeFactor(isVirtual, -1);
		},
		change: function( chID ) {
			log.info('TvdAdapter', 'change', 'channel=%s', chID);
			this.set('_virtual', INVALID_CH);
			var self = this;
			return self.getAPI('tvd').player.change(chID).then(function(current) {
				self.set('_channel', chID === INVALID_CH ? INVALID_CH : current);
			})
			.catch(function() {
				self.set('_channel', INVALID_CH);
			});
		},
		shutdown: function() {
			this.change(INVALID_CH);
		},
		previous: function() {
			var chID = this.get('_previous');
			if (chID !== INVALID_CH) {
				this.change(chID);
			}
		},
		nextFavorite: function() {
			this.getAPI('tvd').player.nextFavorite(this.current(false), 1, util.catchError(function( chID ) {
				if (chID !== INVALID_CH) {
					this.change(chID);
				}
			}.bind(this)));
		},
		current: function( isVirtual ) {
			var virtual = this.get('_virtual');
			if (isVirtual && virtual !== INVALID_CH) {
				return virtual;
			}
			else {
				return this.get('_channel');
			}
		},
		changeFactor: function( isVirtual, factor ) {
			this.getAPI('tvd').player.nextChannel(this.current(isVirtual), factor, util.catchError(function( ch ) {
				if (!isVirtual) {
					this.change(ch);
				}
				else {
					this.set('_virtual', ch);
					this.emit('changed', {isVirtual: true, chID: ch});
				}
			}.bind(this)));
		},
		resetVirtualChannel: function() {
			this.set('_virtual', INVALID_CH);
		},
		// Private
		_channelChanged: function(was, is) {
			log.info('TvdAdapter', '_channelChanged', 'channel=%s', is);
			if (was !== INVALID_CH) {
				this.set('_previous', was);
			}

			this.emit('changed', {isVirtual: false, chID: is});
		}
	});

})();
