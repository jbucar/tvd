var MenuCollectionSupport = {
	name: MenuCollectionSupport,
	constructed: enyo.inherit(function (sup) {
		return function () {
			sup.apply(this, arguments);

			this.tabBindings = {};
			this._collectionHandlers = {
				add: this.bindSafely('_collectionSupportAdded'),
				remove: this.bindSafely('_collectionSupportRemoved'),
				reset: this.bindSafely('_collectionSupportReseted')
			};

			this.collectionChanged();
		};
	}),
	collectionChanged: function(was) {
		if (was) {
			Object.keys(this._collectionHandlers).forEach(function(hdlr) {
				was.off(hdlr, this._collectionHandlers[hdlr]);
			}, this);
		}

		if (this.collection) {
			Object.keys(this._collectionHandlers).forEach(function(hdlr) {
				this.collection.on(hdlr, this._collectionHandlers[hdlr]);
			}, this);

			var models = this.collection.models.slice();
			if (models.length) this._collectionHandlers.add({}, {}, {models: models});
		}
	},
	_collectionSupportAdded: function(sender, evtName, evt) {
		var models = evt.models;
		var bindings = {};

		var ctrols = this.$.menu.createComponents(models.map(function(m) {
			return { kind: this.menuKindOption, model: m };
		}, this), {owner: this});

		ctrols.forEach(function(c) {
			var ctrolName = c.get('name');
			bindings[ctrolName] = ctrolName + 'Content';
			c.render();
		});

		this.$.menu.stabilize();

		this.tabBindings = enyo.mixin(this.tabBindings, bindings);

		if (this.contentOptions) {
			var firstTab = Object.keys(this.tabBindings)[0];
			this.createComponents(ctrols.map(function(ctrol) {
				return enyo.mixin({
					name: ctrol.get('name') + 'Content',
					showing: firstTab === ctrol.get('name'),
					model: ctrol.get('model')
				}, this.contentOptions);
			}, this), {owner: this}).forEach(function(c) {
				c.render();
			});
		}
	},
	_collectionSupportRemoved: function(sender, evtName, evt) {
		var cur = enyo.Spotlight.getCurrent();
		var spot = cur && cur.isDescendantOf(this);

		if (spot) {

			enyo.Spotlight.freeze();
		}

		var ids = evt.models.map(function(m) { return m.get('id'); });
		var lost;

		if (this.contentOptions) {
			this.getClientControls().filter(function(c) {
				return evt.models.indexOf(c.get('model')) > -1;
			})
			.forEach(function(c) {
				lost = lost || (cur.isDescendantOf(c) ? c : undefined);
				c.destroy();
			});
		}

		this.$.menu.getClientControls().filter(function(c) {
			return ids.indexOf(c.model.get('id')) > -1;
		}).forEach(function(c) {
			lost = lost || (cur.isDescendantOf(c) ? c : undefined);
			this.$.menu.destroyComponent(c);
		}.bind(this));

		this.$.menu.stabilize();

		if (spot) {
			enyo.Spotlight.unfreeze();
			if (lost) this.bubble('onSpotLost', {originator: this});
		}
	},
	_collectionSupportReseted: util.nop // TODO
};

enyo.kind({
	name: 'View',
	showing: false, // TODO: Move this to the client
	classes: 'shell-fit view',
	spotlight: 'container',
	published: {
		menuComponents: null,
		tabBindings: {},
		layerClass: null,
		context: null,
		animEnabled: true,
	},
	handlers: {
		onTabWindowCloseRequest: 'viewClose',
		onSpotlightContainerEnter: 'onSelfEnter',
		onSpotlightContainerLeave: 'onSelfLeave',
		onSpotlightKeyDown: 'onKeyWrapper',
		onTabWindowClosed: 'onTabWindowClosed',
		onRequestLayer: 'requestLayer'
	},
	menuOption: null,
	menuConfig:	{
		name: 'menu',
		kind: Shell.Menu,
		displace: 190,
		childWidth: 190,
		onSpotlightContainerEnter: 'onMenuEnter',
		onSpotlightFocused: 'onMenuFocused',
		onSpotlightFocus: 'onMenuFocus',
		onSpotlightSelect: 'onMenuSelect',
		onSpotlightDown: 'onMenuDown',
	},
	tools: [
		{ name: 'layer', classes: 'view-layer', spotlight: false },
		{
			name: 'wrapper',
			classes: 'wrapper view-wrapper inactive',
			spotlight: 'container',
			onSpotlightContainerEnter: 'onWrapperEnter',
			onSpotlightContainerLeave: 'onWrapperLeave'
		},
		{
			name: 'menuAnim', kind: "Animator", duration: 200, easingFunction: enyo.easing.linear,
			onStep: 'menuAnimStep',
			onEnd: 'menuAnimComplete'
		}
	],
	_programaticallySelected: false,
	_adjusted: false,
	_inview: false,
	_firstEnter: true,
	menuAnimStep: function(oSender, oEvent) {
		this.$.menu.applyStyle('height', oSender.value + 'px');
	},
	requestLayer: function(oSender, oEvent) {
		this.$.layer.addRemoveClass('deactive', !oEvent.on);
	},
	onSelfEnter: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			log.trace('View', 'onSelfEnter', 'this.name=%s animEnabled=%s', this.name, this.animEnabled);
			if (this._firstEnter) {
				this.onFirstEnter();
				this._firstEnter = false;
			}

			if (this.context) {
				Context.help = this.help;
				Context.enter(this.context, this.$.wrapper);
			}

			this.$.menu.$.client.applyStyle('opacity', 1);
			if (this.animEnabled) {
				this.$.menuAnim.play({
					startValue: this._inview ? 0 : 7,
					endValue: 50,
					node: this.hasNode(),
					anim: 'expand',
				});
			}
			else {
				this.$.menu.applyStyle('height', '50px');
			}
		}
	},
	onSelfLeave: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			log.trace('View', 'onSelfLeave', 'this.name=', this.name);
			if (this.context) {
				Context.leave(this.context);
			}

			if (this.animEnabled) {
				this.$.menuAnim.play({
					startValue: 50,
					endValue: this._inview ? 0 : 7,
					node: this.hasNode(),
					anim: 'collapse',
				});
			}
			else {
				this.$.menu.applyStyle('height', this._inview ? 0 : 7 + 'px');
			}
			this.resetMenu();

			this.$.menu.removeClass('inactive');
			this.$.menu.removeClass('active');
		}
	},
	onMenuFocus: function(oSender, oEvent) {
		if (oEvent.originator.isDescendantOf(this.$.menu) && (oEvent.originator !== this.$.menu)) {
			if (this.lastFocused) {
				this.lastFocused.removeClass('freezed');
			}
			this.lastFocused = oEvent.originator;
			this.lastFocused.addClass('freezed');
		}
	},
	onMenuFocused: function(oSender, oEvent) {
		if (oEvent.originator.isDescendantOf(this.$.menu) && (oEvent.originator !== this.$.menu)) {
			this._selectTab(oEvent.originator.name, this._programaticallySelected);
			return true;
		}
	},
	onMenuEnter: function(oSender, oEvent) {
		if (oEvent.originator === this.$.menu) {
			this.activeMenu(true);
			if (Context.helpEnabled) {
				Context.setHelp(this.help);
			}
		}
	},
	onFirstEnter: util.nop,
	activeMenu: function(active) {
		this.$.menu.addRemoveClass('active', active);
		this.$.menu.addRemoveClass('inactive', !active);
	},
	activeWrapper: function(active) {
		this.$.wrapper.addRemoveClass('active', active);
		this.$.wrapper.addRemoveClass('inactive', !active);
	},
	animEnabledChanged: function(was, is) {
		if (!is) {
			this.$.menuAnim.stop();
		}
	},
	leaveMenu: function() {}, //TODO: Remove
	onWrapperEnter: function(oSender, oEvent) {
		if (oEvent.originator === this.$.wrapper) {
			this.activeMenu(false);

			if (!this.tabSelected.showing) {
				this.stopJob('doSelectTab');
				this.showTab();
			}

			this.activeWrapper(true);

			if (!(this.tabSelected instanceof (View) || this.tabSelected.wrapView)) {
				this.bubble('onRequestLayer', {on: false});
			}
		}
	},
	onWrapperLeave: function(oSender, oEvent) {
		log.trace('View', 'onWrapperLeave', 'this.name=', this.name);
		if (oEvent.originator === this.$.wrapper) {
			this.activeWrapper(false);
			this.bubble('onRequestLayer', {on: true});
		}
	},
	onMenuDown: function(oSender, oEvent) {
		if (this.tabBindings[oEvent.originator.name] !== 'none') {
			this.stopJob('doSelectTab');
			this.showTab();
			return false;
		}
		else {
			return this.tabSelected && !this.tabSelected.showing;
		}
	},
	onMenuSelect: function (oSender, oEvent) {
		if (oEvent.originator.isDescendantOf(this.$.menu) && oEvent.originator !== this.$.menu)	{
			enyo.Spotlight.onSpotlightEvent({type: 'onSpotlightDown', originator: oEvent.originator});
			return true;
		}
	},
	onKeyWrapper: function(sender, evt) {
		var back = (evt.keyCode === tacKeyboardLayout.VK_EXIT || evt.keyCode === tacKeyboardLayout.VK_BACK);
		if (back) {
			if (sender === this.$.wrapper) {
				enyo.Spotlight.spot(this.$.menu);
				this.waterfall('onTabWindowClose', {originator: evt.originator}, this.eventNode);
			}
			else {
				this.viewClose();
			}

			return true;
		}
	},
	onTabWindowClosed: function(oSender, oEvent) {
		if (oEvent.originator !== this) {
			enyo.Spotlight.spot(this.$.menu);
			return true;
		}
	},
	viewClose: function() {
		this.waterfall('onTabWindowClose', {originator: this},  this.eventNode);
		this.bubble('onTabWindowClosed', {originator: this},  this.eventNode);
		return true; // For use in events
	},
	create: function() {
		this.inherited(arguments);

		if (this.parent.getInstanceOwner() instanceof(View)) {
			this.addClass('view-in-view');
			this._inview = true;
		}

		if (this.menuComponents) {
			this.$.menu.createComponents(this.menuComponents, {owner: this});
		}
		if (this.layerClass) {
			this.$.layer.addClass( this.layerClass );
		}
	},
	initComponents: function() {
		this.createTools();
		this.controlParentName = 'wrapper';
		this.discoverControlParent();
		this.inherited(arguments);
	},
	createTools: function() {
		this.createChrome(this.tools);

		// Create the menu
		var menu = enyo.clone(this.menuConfig || {});
		menu.addBefore = this.$.wrapper;
		enyo.mixin(menu, this.menuOptions || this.menuOption);
		var comp = this.createComponent(menu, { owner : this });
		comp.addClass('view-menu');
		// Hide the menu content baceause it is collapsed at creation time
		this.$.menu.$.client.applyStyle('opacity', 0);

		this.blackChanged();
	},
	menuAnimComplete: function(oSender, oEvent) {
		var animation = oSender.anim;
		if (animation === 'collapse') {
			this.$.menu.$.client.applyStyle('opacity', 0);
			this.activeWrapper(false);
		}
		return (animation === 'expand' || animation === 'collapse');
	},
	_selectTab: function(tab, now) {
		var tabContent = this.$[this.tabBindings[tab]];
		if (this.tabSelected !== tabContent) {
			this.stopJob('doSelectTab');
			if (this.tabSelected) {
				this.tabSelected.hide();
			}
			this.tabSelected = tabContent;
			if (tabContent) {
				if (now) {
					this.showTab();
				}
				else {
					this.startJob('doSelectTab', this.bindSafely('showTab'), 750);
				}
			}
		}
	},
	showTab: function() {
		this.addRemoveClass('view-wrap-view', this.tabSelected instanceof (View) || this.tabSelected.wrapView);
		if (this.tabSelected) {
			this.tabSelected.show();
			if (this.tabSelected.contentSelected) {
				this.tabSelected.contentSelected();
			}
		}
	},
	resetMenu: function() {
		this.$.menu.unscroll();
		if (this.$.menu.first()) {
			this._selectTab(this.$.menu.first().name, true);
		}
	},
	selectTab: function(tab, enter) {
		this._programaticallySelected = true;
		var success = this.$.menu.scrollTo(function(c) {
			var m = c.get('model');
			return m? m.get('name') === tab : c.name === tab;
		});

		if (success && enter) {
			enyo.Spotlight.spot(this.$.wrapper, 'DOWN');
		}

		this._programaticallySelected = true;
		return this.tabSelected;
	},
	blackChanged: function() {
		this.$.wrapper.addRemoveClass('view-wrapper', !this.black);
	},
	adjust: function() {
		var height = parseInt(this.getComputedStyleValue('height'), 10);
		var mheight = 50;
		this.$.wrapper.applyStyle('height', (height - mheight) + 'px');
		this.$.wrapper.applyStyle('position', 'relative');
		this.$.wrapper.applyStyle('top',  mheight + 'px');
		this._adjusted = true;
	},
	showingChanged: function(was, is) {
		this.inherited(arguments);
		if (!this._adjusted) {
			// Set the wrapper height
			this.adjust();
		}
	},
});

(function() {

	enyo.kind({
		name: 'components.ViewContent',
		classes: 'view-content',
		spotlight: 'container',
		handlers: {
			onSpotlightContainerEnter: 'onContentEnter',
			onSpotlightContainerLeave: 'onContentLeave'
		},
		tools: [
			{ classes: 'upper-side' },
			{ name: 'client', classes: 'client'	},
			{ name: 'legend', kind: 'components.BackLegend', showing: false }
		],
		_firstEnter: true,
		focused : false,
		initComponents: enyo.inherit(function(sup) {
			return function() {
				this.createChrome(this.tools);
				sup.apply(this, arguments);
			};
		}),
		onContentEnter: function(sender, evt) {
			if (evt.originator === this) {
				this.set('focused', true);
				if (this._firstEnter) {
					if (this.onFirstEnter) {
						this.onFirstEnter();
					}
					_firstEnter = false;
				}
				if (this.help) {
					Context.help = this.help;
				}
			}
		},
		onContentLeave: function(sender, evt) {
			if (evt.originator === this) {
				this.set('focused', false);
			}
		},
		contentSelected: util.nop,
		focusedChanged: function(was, is) {
			this.$.legend.setShowing(is);
		},
	});

})();
