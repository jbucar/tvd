
(function() {
	'use strict';

	var GlueMixins = [DISupport, enyo.ObserverSupport, enyo.ComputedSupport, enyo.BindingSupport, enyo.EventEmitter];

	enyo.kind({
		name: 'LogConfigGlue',
		mixins: GlueMixins,
		// Attrs
		level: undefined,
		enabled: undefined,
		port: undefined,
		// Private
		_config: undefined,
		_levels: ['error', 'warn', 'http', 'info', 'verbose', 'silly'],
		fetch: function() {
			system.getLogConfig(util.catchError(function(cfg) {
				this._config = cfg;
				this.set('enabled', cfg.enabled);
				this.set('level', this._levels.indexOf(cfg.level));
			}.bind(this)));

			system.getRemotePort(util.catchError(this.set.bind(this, 'port')));

			return this;
		},
		enable: function(enabled) {
			this._config.enabled = enabled;
			this.enabled = enabled; // Raw set
			system.setLogConfig(this._config);
		},
		changeLevel: function(level) {
			if (this.get('level') !== level) {
				this._config.level = this._levels[level];
				this.level = level; // Raw set
				system.setLogConfig(this._config);
			};
		}
	});

	enyo.kind({
		name: 'DevelopmentModeGlue',
		mixins: GlueMixins,
		// Attrs
		enabled: undefined,
		init: function() {
			system.isDevelopmentMode(util.catchError(this.set.bind(this, 'enabled')));
			system.on('DevelopmentModeEnabled', this.set.bind(this, 'enabled'));
			return this;
		}
	});

})();
