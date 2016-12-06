(function(scope) {
	'use strict';

	var imagesFields = [
		'width', 'height', 'camera',
		'orientation', 'exposureTime',
		'exposureCompensation', 'apertureValue',
		'iso', 'flash', 'date', 'gps'
	];

	var defaultValues = {
		width: undefined, height: undefined, camera: undefined,
		orientation: undefined, exposureTime: undefined,
		exposureCompensation: undefined, apertureValue: undefined,
		iso: undefined, flash: undefined, date: undefined, gps: undefined
	};

	enyo.kind({
		name: 'models.imgs.impl.ImageItemModel',
		kind: 'enyo.Model',
		_fetched: false,
		fetch: function(force) {
			var self = this;
			return new Promise(function(resolve, reject) {
				if (!self._fetched || force) {
					self.set(defaultValues, {force: true}); //clear old data
					media.image.get({
						provider: self.get('provider'),
						root: self.get('root'),
						id: self.get('id'),
						fields: imagesFields
					}, function(err, data) {
						if (err) {
							reject();
						}
						else {
							self.set(data);
							self._fetched = true;
							resolve(true);
						}
					});
				}
				resolve(true);
			});
		},
		isDirectory: function() {
			return this.get('type') === 'album';
		},
		get: enyo.inherit(function(sup) {
			return function(path) {
				if (path === 'url') {
					if (this.isDirectory()) {
						return assets('folder_icon.png');
					}
					else if (this.attributes.back) {
						return assets('back_arrow.png');
					}
				}

				return sup.apply(this, arguments);
			};
		})
	});

	var imagesSource = new tac.APISource({api: media.image});

	enyo.kind({
		name: 'models.imgs.impl.ImagesCollection',
		kind: 'enyo.Collection',
		source: imagesSource,
		model: 'models.imgs.impl.ImageItemModel',
		prepareModel: enyo.inherit(function(sup) {
			return function(attrs, opts) {
				if (opts) {
					attrs.root = opts.root;
				}
				return sup.apply(this, arguments);
			};
		})
	});

	function createProvider(provider) {
		var model = new enyo.Model(provider);
		var imgs = new models.imgs.impl.ImagesCollection();

		model.set('imgs', imgs);
		return model;
	}

	scope.initImageProviderCollection = function(collection) {
		media.image.getProviders(util.catchError(function(providers) {
			collection.add(providers.map(createProvider));
		}));

		media.image.on('providerAdded', function(providerID) {
			media.image.getProviders(util.catchError(function(providers) {
				var provider = providers.find(function(p) { return p.id === providerID; });
				assert(provider);
				collection.add(createProvider(provider));
			}));
		});

		media.image.on('providerRemoved', function(providerID) {
			var provider = collection.find(function(mdl) { return mdl.get('id') === providerID; });
			assert(provider);
			collection.remove(provider);
		});
	};

})(window);
