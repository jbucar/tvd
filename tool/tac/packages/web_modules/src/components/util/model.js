/* exported Model */

function Model() {
	var self = {};
	var _models = {};

	self.init = function( models ) {
		log.verbose( 'Model', 'Init: models=%j', models );

		_models = models;
		return Promise.map(Object.keys(models), function(mName) {
			log.verbose( 'Model', 'Init: model=%s', mName );
			return models[mName].init();
		});
	};

	self.fin = function() {
		return Promise.map(Object.keys(_models), function(mName) {
			log.verbose( 'Model', 'Fin: model=%s', mName );
			return _models[mName].fin();
		}).then(() => _models = null);
	};

	self.get = function( mName ) {
		let mod = _models[mName];
		assert(mod);
		return mod;
	};

	return self;
}
