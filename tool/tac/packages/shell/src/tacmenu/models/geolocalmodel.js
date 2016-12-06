(function (scope) {
"use strict";

	enyo.kind({
		name: "GeoLocalModel",
		published: {
			enabled: false,
		},
		init: function() {
			var self = this;
			system.isGeolocationEnabled(function(err,result) {
				if (!err && result) {
				self.set('enabled', true);
				}
			});
			return this;
		},
		enable: function(enabled) {
			log.info('GeoLocalModel', 'enable', 'Enable geolocalization = %s', enabled);
			this.set('enabled', enabled);
			system.enableGeolocation(enabled);
			notification.emit('geolocalEnabled', enabled);
		}
	});
})(this);