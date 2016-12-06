(function() {
	'use strict';

	enyo.kind({
		name: 'components.flexLayout.Vertical',
		classes: 'flexlayout vertical',
		center: true,
		tools: [
			{ name: 'client', classes: 'client', style: 'max-width: 100%;' }
		],
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.applyStyle('align-items', this.center? 'center' : 'flex-start');
			}
		}),
		initComponents: enyo.inherit(function(sup) {
			return function() {
				this.createChrome(this.tools);
				sup.apply(this, arguments);
			}
		})
	});

})();
