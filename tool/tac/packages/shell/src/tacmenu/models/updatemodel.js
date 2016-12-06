(function() {
	'use strict';

	enyo.kind({
		name: 'UpdateModel',
		mixins: [DISupport, enyo.BindingSupport, enyo.EventEmitter],
		// Attrs
		lastSearch: undefined,
		info: undefined,
		progress: undefined,
		dependencies: {
			updatemgr : updatemgr
		},
		init: function() {
			this.set('status', 'idle');

			var delayedSet = util.ensureDelay(this.set.bind(this, 'status'), 2000);

			this.getAPI('updatemgr').on('Firmware', function changeSt(evt) {
				var is = evt.status;

				if (is === 'available') {
					this.set('info', evt.info);
				}
				else if (is === 'downloading') {
					this.set('progress', evt.progress);
				}

				// Checking can be resolved (into idle or error) very quick, so we need to ensure that the model
				// don´t change the state that fast in order to show the transition.
				if ((is === 'checking') || (this.get('status') === 'checking' && (is === 'idle' || is === 'error'))) {
					delayedSet(is);
				}
				else {
					this.set('status', is);
				}
			}.bind(this));

			return this;
		},
		search: function() { this.getAPI('updatemgr').checkFirmwareUpdate(); },
		fetch: function() { this.getAPI('updatemgr').downloadFirmwareUpdate(); },
		install: function() { this.getAPI('updatemgr').updateFirmware(); },
		// Private,
		statusChanged: function(was, is) {
			if(is === 'checking') {
				this.set('lastSearch', new Date());
			}
			else if (is === 'downloaded') {
				this.emit('FirmwareUpdateFetched', {});
			}
		}
	});

})();
