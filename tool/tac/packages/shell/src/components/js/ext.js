
var components = {};

(function(enyo, scope) {
	enyo.EventEmitter.once = function(e, fn, ctx) {
		var listener;
		listener = function() {
			fn.apply(ctx, arguments);
			this.off(e, listener, ctx);
		}.bind(this);
		this.on(e, listener, ctx);
	}

	function buildBinding(b, target) {
		var from = b.from || b.attribute;
		var to = b.to || b.attribute;
		var api = target.getAPI(b.api);
		var setter = target.set.bind(target, to);
		setter = b.transform? _.flow(b.transform, setter) : setter;
		return {
			connect: api.on.bind(api, from, setter),
			disconnect: api.off? api.off.bind(api, from, setter) : util.nop, // TODO: review this
			sync: b.sync ? function() {
				api[b.sync](util.catchError(function() {
					setter.apply(target, arguments);
				}));
			} : util.nop
		}
	}

	// NOTE: Always include this mixins before DISupport
	scope.APIBindingSupport = {
		name: 'APIBindingSupport',
		constructed: enyo.inherit(function (sup) {
			return function () {
				if (this.apiBindings) {
					this.apiBindings = this.apiBindings.map(function(b) {
						var bng = buildBinding(b, this);
						if (b.autoConnect) {
							bng.connect();
						}
						return bng;
					}, this);
				}
				sup.apply(this, arguments);
			};
		}),
		enableAPIsBinding: function(enable) {
			var method = enable? 'connect' : 'disconnect';
			this.apiBindings.forEach(function(b) {
				b[method]();
			});
		},
		sync: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.apiBindings.forEach(function(b) {
					b.sync();
				});
			}
		})
	}

	scope.DISupport = {
		name: 'DISupport',
		constructed: enyo.inherit(function (sup) {
			return function () {
				this.dependencies = this.dependencies || {};
				sup.apply(this, arguments);
			};
		}),
		addAPI: function(apiName, apiObj) {
			this.dependencies[apiName] = apiObj;
			return this;
		},
		addAPIs: function(apis) {
			this.dependencies = enyo.mixin(this.dependencies, apis);
			return this;
		},
		getAPI: function(apiName) {
			if (this.dependencies[apiName]) {
				return this.dependencies[apiName];
			}
			else {
				throw new Error('API ' + apiName + ' not found');
			}
		}
	};

	scope.NotificationAPI = {
		silenced: false,
		emit: function(evt, arg) {
			if (!this.silenced) {
				notification.emit(evt, arg);
			}
			return !this.silenced;
		},
		silence: function() { this.silenced = true },
		unsilence: function() { this.silenced = false }
	}

})(enyo, window);
