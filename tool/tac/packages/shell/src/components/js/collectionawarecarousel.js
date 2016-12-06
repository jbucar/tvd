enyo.kind({
	name: 'Shell.CollectionAwareCarousel',
	kind: 'Carousel',
	published: {
		collection: undefined,
		componentView: undefined,
		sortedBy: undefined
	},
	events: {
		onCollectionChanged: ''
	},
	collectionChanged: function() {
		this.collection.addListener('add', this.bindSafely('onModelsAdded'));
		this.collection.addListener('remove', this.bindSafely('onModelsRemoved'));
		this.collection.addListener('reset', this.bindSafely('onCollectionReset'));
		var models = this.collection.models.slice();
		if (models.length) this.onModelsAdded({}, {}, {models: models});
	},
	onModelsAdded: function(oSender, oEvent, oAdded) {
		var cur = enyo.Spotlight.getCurrent();
		var spot = cur && cur.isDescendantOf(this.$.container);
		if (spot) {
			enyo.Spotlight.unspot();
			this.onLeave(this, {originator: this});
		}
		this.resetToFirst();
		var component = this.get('componentView');

		this.$.container.createComponents(oAdded.models.map(function(model) {

			var nextItem = this.sortedBy ? this.$.container.getClientControls().find(function(c) {
				return c.get('model').get(this.sortedBy) > model.get(this.sortedBy);
			}, this) : undefined;

			return { kind: component, model: model, addBefore: nextItem };
		}, this), { owner: this }).forEach(function(c) { c.render(); });

		if (spot) {
			enyo.Spotlight.spot(this.$.container);
			this.onEnter(this, {originator: this});
		}

		this.set('sideArrows', !(this.$.container.getClientControls().length === 1));
		this.doCollectionChanged({type: 'added', originator: this});
	},
	onModelsRemoved: function(oSender, oEvent, oRemoved) {
		var cur = enyo.Spotlight.getCurrent();
		var spot = cur && cur.isDescendantOf(this.$.container);

		if (spot) {
			this.onLeave(this, {originator: this});
		}

		enyo.Spotlight.freeze();

		var idsRemoved = oRemoved.models.map(function(m) { return m.get('id') });
		this.$.container.getClientControls().filter(function(c) {
			return idsRemoved.indexOf(c.model.get('id')) > -1;
		})
		.forEach(this.$.container.destroyComponent.bind(this.$.container));

		enyo.Spotlight.unfreeze();
		var length = this.$.container.getClientControls().length;
		if (spot && length) {
			enyo.Spotlight.spot(this.$.container);
			this.onEnter(this, {originator: this});
		}

		this.set('sideArrows', !(length === 1));
		this.doCollectionChanged({type: 'removed', originator: this, empty: length === 0, ids: idsRemoved});
	},
	onCollectionReset: function() {
		this.$.container.destroyClientControls();
		this.doCollectionChanged({type: 'removed', originator: this});
	}
});
