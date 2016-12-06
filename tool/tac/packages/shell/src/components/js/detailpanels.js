(function() {

	// TODO [nbaglivo]: remove this
	function panelDef(kind) {
		return {
			kind: 'Control',
			classes: 'detail-panel',
			style: 'width: 100%',
			bindings: [
				{ from: '.model', to: '.$.detail.model' },
			],
			tools: [
				{ classes: 'arrow-bar' },
				{
					classes: 'detail-container',
					components: [
						{ name: 'detail', kind: kind },
					]
				},
				{ classes: 'arrow-bar end' },
			],
			initComponents: function() {
				this.createChrome(this.tools);
			}
		};
	}

	enyo.kind({
		name: 'detailpanels.impl.DetailsArrows',
		mixins: [components.FadeSupport],
		components: [
			{
				classes: 'arrow-bar',
				style: 'display: flex; justify-content: center; align-items: center; width: 7%;',
				components: [{ kind: 'Shell.LeftArrow', name: 'leftarrow' }]
			},
			{
				classes: 'arrow-bar end',
				style: 'display: flex; justify-content: center; align-items: center; width: 7%;',
				components: [{ kind: 'Shell.RightArrow', name: 'rightarrow' }]
			},
		],
		showingChanged: enyo.inherit(function(sup) {
			return function(was, is) {
				// Hack: There is a bug in enyo that make flex -> block when show/hide
				if (!is) this._display = 'flex';
			}
		})
	});

	enyo.kind({
		name: 'DetailPanels',
		spotlight: 'container',
		classes: 'detail-panels',
		events: {
			onIdxChanged: '',
			onTransitionStart: ''
		},
		published: {
			idx: undefined,
			collection: undefined,
			innerKind: undefined
		},
		bindings: [
			{ from: '.collection.length', to: '.$.arrows.showing' },
		],
		handlers: {
			onRequestScroll: 'scrollRequested'
		},
		components: [
			{ name: 'arrows', kind: detailpanels.impl.DetailsArrows, style: 'width: 100%; height: 100%; position: absolute; display: flex; justify-content: space-between;' },
			{ name: 'client', style: 'width: 100%; height: 100%; display: flex' },
			{ kind: "Animator", duration: 1000, onStep: '_step', onStop: '_reset', onEnd: '_reset', easing: enyo.easing.linear },
		],
		originals: [],
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);

				this.onAdd = this.bindSafely('_modelsAdd');
				this.onRemove = this.bindSafely('_modelsRemove');
				this.onReset = this.bindSafely('_collectionReset');

				this.collectionChanged();
			}
		}),
		collectionChanged: function(was, is) {
			if (was) {
				was.off('add', this.onAdd);
				was.off('remove', this.onRemove);
				was.off('reset', this.onReset);
			}

			if (is) {
				is.on('add', this.onAdd);
				is.on('remove', this.onRemove);
				is.on('reset', this.onReset);

				var models = this.collection.models.slice();
				if (models.length) this._modelsAdd({}, 'add', {models: models});
			}
		},
		_modelsRemove: function(sender, evtName, evt) {
			var toRemove = this.getClientControls().filter(function(c) {
				return evt.models.indexOf(c.get('model')) > -1;
			})

			if (this.originals) {
				this.originals.forEach(function(c) {
					c.order = 0;
					c.applyStyle('order', 0);
				});
				this._sort();
			}

			toRemove.forEach(function(c) {
				c.destroy();
			})

			this.originals = this.getClientControls();
			this.originals.forEach(function(c, idx) {
				c.idx = idx;
			});
			this._sort();

			this.$.arrows.setShowing(this.collection.length > 1);

			this._backToFirst();
		},
		_modelsAdd: function(sender, evtName, evt) {
			var innerKind = this.innerKind;
			var ctrols = this.getClientControls();
			var sortedBy = this.sortedBy;

			if (this.originals) {
				this.originals.forEach(function(c) {
					c.order = 0;
					c.applyStyle('order', 0);
				});
				this._sort();
			}

			this.createComponents(evt.models.map(function(model, idx) {
				var def = panelDef(innerKind);

				def.addBefore = sortedBy ? ctrols.find(function(c) {
					return c.get('model').get(sortedBy) > model.get(sortedBy);
				}) : undefined;

				def.model = model;
				return def;
			})).forEach(function(c) {
				c.render();
			});

			this.originals = this.getClientControls();
			this.originals.forEach(function(c, idx) {
				c.idx = idx;
				c.order = 0;
				c.applyStyle('order', 0);
			});
			this._sort();

			this.$.arrows.setShowing(this.collection.length > 1);

			this._backToFirst();

		},
		_backToFirst: function() {
			this.$.client.applyStyle('transform', 'translate(0px, 0px)');
			this.idx = 0;
			var cur = enyo.Spotlight.getCurrent();
			if (cur && cur.isDescendantOf(this)) {
				enyo.Spotlight.spot(this.originals[0]);
			}
		},
		_collectionReset: function() {
			this.idx = 0;
			this.destroyClientControls();
		},
		idxChanged: function(was, is) {
			if (this.idx !== undefined) {
				this.$.client.applyStyle('transform', 'translate(0px, 0px)');

				while(this.originals[0].idx !== this.idx) {
					this.scrollRight(false);
				}

				enyo.Spotlight.unspot();
				enyo.Spotlight.spot(this.originals[0]);
			}
		},
		scrollRequested: function(sender, evt) {
			if (this.collection.length > 1) {
				if (evt.dir === 'RIGHT') {
					this.scrollRight(true);
				}
				else if (evt.dir === 'LEFT') {
					this.scrollLeft();
				}
			}
		},
		_step: function(sender) {
			this.$.client.applyStyle('transform', 'translateX(' + sender.value + '%)');
		},
		reset: function() {
			this.$.animator.stop();
			this.set('idx', undefined);
		},
		_reorder: function() {
			if (this.originals[0].idx === 0) {
				this.originals.forEach(function(c) {
					c.applyStyle('order', '0');
					c.order = 0;
				});
			}
		},
		_sort: function() {
			this.originals.sort(function(a, b) {
				return (a.order !== b.order) ? a.order - b.order : a.idx - b.idx;
			});
		},
		scrollRight: function(animate) {
			if (animate) {
				this.$.arrows.hide();
				this.doTransitionStart({});
				this.$.animator.play({
					startValue: 0,
					endValue: -100,
					node: this.hasNode(),
					right: true,
					wasAnimated: true
				});
			}
			else {
				this._reset({right: true});
			}
		},
		scrollLeft: function() {
			this._reorder();
			this.originals[this.originals.length - 1].applyStyle('order', '-1');
			this.originals[this.originals.length - 1].order = -1;
			this._sort();

			this.$.client.applyStyle('transform', 'translate(100%, 0)');

			this.$.arrows.hide();
			this.doTransitionStart({});
			this.$.animator.play({
				startValue: -100,
				endValue: 0,
				node: this.hasNode(),
				right: false,
				wasAnimated: true
			});
		},
		_reset: function(sender) {
			if (sender.right) {
				this._reorder();
				this.originals[0].applyStyle('order', '2');
				this.originals[0].order = 2;
				this._sort();
			}
			this.$.client.applyStyle('transform', 'translate(0px, 0px)');

			if (sender.wasAnimated) {
				this.doIdxChanged({was: 0, is: this.originals[0].idx, direction: sender.right ? 'RIGHT' : 'LEFT'});
				this.$.arrows.show();
			}
		}
	});

})();
