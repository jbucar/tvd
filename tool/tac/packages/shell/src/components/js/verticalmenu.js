
(function(scope) {

	function isSpotable(c) {
		return enyo.Spotlight.isSpottable(c);
	}

	scope.CircularNavSupport = {
		name: 'CircularNavSupport',
		constructor: enyo.inherit(function (sup) {
			return function () {
				var orig = this.handlers['onSpotlightFocused'];
				if (orig && orig !== '__cnsFocused' && _.isFunction(this[orig])) {
					this.__cnsOrigHandler = this[orig];
				}
				this.handlers['onSpotlightFocused'] = '__cnsFocused';
				sup.apply(this, arguments);
			};
		}),
		__cnsFocused: function(sender, evt) {
			var ret;
			if (this.__cnsOrigHandler) {
				ret = this.__cnsOrigHandler();
			}

			var evt = enyo.Spotlight.getLast5WayEvent();
			var dir = evt? evt.type.replace('onSpotlight', '') : '';
			if (enyo.Spotlight.getCurrent() === this && (dir === 'Up' || dir === 'Down')) {
				var children = dir === 'Down'? this.getClientControls() : this.getClientControls().reverse();
				enyo.Spotlight.spot(children.find(isSpotable));
				return ret || true;
			}

			return ret;
		}
	}

	enyo.kind({
		name: 'components.VerticalMenu',
		kind: 'components.flexLayout.Vertical',
		spotlight: 'container',
		mixins: [CircularNavSupport],
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.client.applyStyle('width', '100%');
			};
		})
	});

})(this);
