(function() {

	enyo.kind({
		name: 'AppModel',
		kind: 'enyo.Model',
		attributes: {
			favorite: false,
			name: 'Cargando...',
			icon: assets('default_detail.png'),
			description: '',
			categories: []
		},
		fetch: function() {
			appmgr.isFavorite(this.get('id'), util.catchError(this.set.bind(this, 'favorite')));

			appmgr.get(this.get('id'), util.catchError(function getAppInfo(app) {
				for (attr in app) {
					this.set(attr, app[attr]);
				}
			}.bind(this)));
		},
		toggleFavorite: function() {
			appmgr.toggleFavorite(this.get('id'), util.catchError(function() {
				var toggled = !this.get('favorite');
				this.set('favorite', toggled);
				// Shows favorite application notification.
				notification.emit('favoriteApp', toggled);
			}.bind(this)));
		},
		uninstall: function() {
			pkgmgr.uninstall({'id': this.get('pkgID')}, util.informError());
		}
	});


	function finder(prop, id) {
		return function finder(app) {
			return app.get(prop) === id;
		}
	}

	enyo.kind({
		name: 'AppsCollection',
		kind: 'enyo.Collection',
		model: 'AppModel',
		create: function() {
			this.inherited(arguments);
			pkgmgr.on('uninstall', function pkgUninstalled(id) {
				notification.emit('uninstallApp', {state: 'uninstalled'});
				var res = this.find(finder('pkgID', id));

				if (res) res.destroy();
			}.bind(this));

			appmgr.on('install', function( id ) {
				var model = this.find(finder('id', id));
				if (!model) {
					model = this.add({id: id})[0];
				}

				model.fetch();
			}.bind(this));
		},
		fetch: function() {
			appmgr.getAll(util.catchError(function appsFetch(apps) {
				var models = this.add(apps);
				this.emit('loaded', {});
				models.forEach(function(m) {
					m.fetch();
				});
			}.bind(this)));
		}
	});

	enyo.kind({
		name: 'ApplicationCategoryCollection',
		kind: 'enyo.Collection',
		published: {
			collection: '',
			category: ''
		},
		collectionChanged: function() {
			this.collection.addListener('change', this.bindSafely('modelChanged'));
		},
		modelChanged: function(oSender, oEvent, oChanged) {
			var categories = oChanged.model.changed.categories;
			if (categories !== undefined) {
				if (oChanged.model.get('categories').indexOf(this.category) > -1) {
					this.add(oChanged.model);
				}
				else {
					this.remove(oChanged.model);
				}
			}
		}
	});

	enyo.kind({
		name: 'AppsCategory',
		kind: 'enyo.Model',
		primaryKey: 'name',
	});

	enyo.kind({
		name: 'AppCategoriesCollection',
		kind: 'enyo.Collection',
		model: 'AppsCategory',
		published: {
			collection: ''
		},
		collectionChanged: function() {
			this.collection.addListener('add', this.bindSafely('collectionModelsChanged'));
			this.collection.addListener('remove', this.bindSafely('collectionModelsChanged'));
		},
		collectionModelsChanged: function() {
			appmgr.getCategories(util.catchError(function addCategories(categories) {
				// Remove favorite if there is no favorited apps
				{
					var favs = this.collection.models.some(function(app) {
						return app.get('favorite');
					});

					if(!favs) {
						// If there is no Favs we need to remove FAVORITOS from categories.
						var index = categories.indexOf('FAVORITOS');
						categories.splice( index, 1 );
					}
				}

				categories = categories.map(function(name) {
					return {id: name, name: name};
				});

				categories.unshift({name: 'TODAS', id: 'all'}); // Add TODAS category

				this.add(categories, { purge: true });
			}.bind(this)));
		}
	});

	enyo.kind({
		name: 'FavAppsCollection',
		kind: 'enyo.Collection',
		published: {
			collection: ''
		},
		collectionChanged: function() {
			this.collection.addListener('change', this.bindSafely('modelChanged'));
		},
		modelChanged: function(oSender, oEvent, oChanged) {
			var fav = oChanged.model.changed.favorite;
			if (fav !== undefined) {
				if (fav) {
					this.add(oChanged.model);
				}
				else {
					this.remove(oChanged.model);
				}
			}
		}
	});

})();
