(function() {
	enyo.kind({
		name: 'tac.APISource',
		kind: 'enyo.Source',
		useCache: function() {
			this.memoized = util.asyncMemoize(this.api.get.bind(this.api));
		},
		fetch: function (model, opts) {
			// util.assert(this.api, 'There is no point for a source without an api');
			var getter = this.memoized || this.api.get.bind(this.api);
			getter(model.params || {}, function(err, res) {
				if (!err) {
					opts.success(res);
				}
				else if (opts.error) {
					opts.error(err);
				}
			});
		}
	});
})();
