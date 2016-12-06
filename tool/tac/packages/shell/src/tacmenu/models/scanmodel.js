(function() {
	enyo.kind({
		name: 'ScanModel',
		kind: 'enyo.Model',
		mixins: [DISupport],
		attributes: {
			isScanning: false,
			percentage: 0,
			count: 0,
			network: 0,
		},
		// Private
		_requestsStart: false,
		// API
		initialize: function() {
			var self = this;
			var tvd = this.getAPI('tvd');

			tvd.tuner.isScanning(util.catchError(function( res ) {
				self.set( 'isScanning', res );
			}));

			tvd.on('channelAdded', function() {
				self.set('count', self.get('count') + 1);
			});

			tvd.on('scanChanged', function( evt ) {
				if (evt.state === 'begin') {
					if (!self._requestsStart) {
						self._cancelScan();
					}
					else {
						self._requestsStart = false;
					}
					self.emit('onStartScan', {});
					channels.empty(); //TODO: move this to channels
					self.set( 'isScanning', true );
				} else if (evt.state === 'end') {
					self.set('percentage', 100);
					var count = self.get('count');
					self._resetScanning();
					self.emit('onEndScan', {status: 'scanEnded', count: count});
				}
				else if (evt.state === 'network') {
					self.set({percentage: evt.percentage, network: evt.network});
				}
			});

			// Promisify cancelScan
			this._cancelScan = util.promisify(this.getAPI('tvd').tuner.cancelScan);

			return self;
		},
		start: function() {
			if (!this.get('isScanning')) {
				this._requestsStart = true;
				this.getAPI('tvd').tuner.startScan();
			}
		},
		stop: function() {
			if (this.get('isScanning')) {
				return this._cancelScan();
			}
			else {
				if (this._requestsStart) {
					this._requestsStart = false;
				}
				else {
					this.emit('onEndScan', {status: 'nonStarted'});
				}

				return Promise.resolve(true);
			}
		},
		// Private
		_resetScanning: function() {
			this.set({
				isScanning: false,
				percentage: 0,
				count: 0,
				network: 0,
			}, {silence: true});
		}
	});
})()
