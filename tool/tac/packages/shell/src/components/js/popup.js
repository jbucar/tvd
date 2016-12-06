
var dialogs = (function(enyo) {

	enyo.kind({
		name: 'DialogButton',
		classes: 'action',
		spotlight: true,
		action: undefined, // The event`s name to emit
	});

	var defaultAcceptLabel = 'ACEPTAR';

	// TODO: move detail to client.
	// TODO: bubble evetns?
	enyo.kind({
		name: 'Dialog',
		kind: 'enyo.Popup',
		centered: true,
		floating: true,
		modal: true,
		classes: 'enyo-fit dialog',
		mixins: [enyo.EventEmitter],
		handlers: {
			onSpotlightSelect: 'actionSelected',
			onSpotlightKeyDown: 'keyPressed'
		},
		published: {
			context: '', // TODO: check publish this
			acceptLabel: defaultAcceptLabel
		},
		bindings: [
			{ from: '.icon', to: '.$.icon.src', transform: assets },
			{ from: '.title', to: '.$.title.content' },
			{ from: '.description', to: '.$.desc.content' },
			{ from: '.acceptLabel', to: '.$.accept.content' },
		],
		tools: [
			{
				name: 'wrapper', classes: 'wrapper',
				components: [
					{
						name: 'detail',
						components: [
							{ name: 'icon', kind: 'enyo.Image', classes: 'icon' },
							{ name: 'title', classes: 'title' },
							{
								name: 'descriptionContainer', classes: 'description-container',
								components: [
									{ name: 'desc', classes: 'description' },
								]
							}
						]
					},
					{ name: 'client' },
					{ name: 'actions', classes: 'actions' }
				]
			}
		],
		actions: [
			{ name: 'accept', kind: 'DialogButton', classes: 'ok', action: 'accepted' }
		],
		_exitAction: 'accepted',
		initComponents: function() {
			this.createChrome(this.tools);
			this.$.actions.createComponents(this.actions, {owner: this});
			this.inherited(arguments);
		},
		create: function() {
			this.inherited(arguments);
			this.contextChanged(undefined, this.get('context'));
		},
		show: function() {
			log.trace('Dialog', 'show');
			this.inherited(arguments);
			enyo.Spotlight.spot(this.$.actions);
		},
		actionSelected: function(oSender, oEvent) {
			var action = oEvent.originator.action;
			if (action) {
				this.doAction(action);
			}
			return true;
		},
		doAction: function(action) {
			var owner = this.parent;
			this.emit(action);
			try {
				enyo.Spotlight.States.pop('focus');
			}
			catch(err) {
				log.debug('Dialog', 'doAction', 'No control to focus');
			}

			if (enyo.Spotlight.getCurrent().isDescendantOf(this)) {
				owner.bubble('onSpotLost', {});
			}

			this.hide();
		},
		keyPressed: function(oSender, oEvent) {
			var keyCode = oEvent.keyCode;
			if (keyCode === tacKeyboardLayout.VK_BACK || keyCode === tacKeyboardLayout.VK_EXIT) {
				this.doAction(this._exitAction);
				return true;
			}

			if (keyCode === tacKeyboardLayout.VK_HOME) {
				return true;
			}

			// 5 Way Move only in the container!
			var keyId = oEvent.keyIdentifier;
			if (keyId === 'Right' || keyId === 'Left' || keyId === 'Up' || keyId === 'Down') {
				var ctrol = enyo.Spotlight.NearestNeighbor.getNearestNeighbor(keyId.toUpperCase());
				return !(ctrol && ctrol.isDescendantOf(this));
			}
		},
		contextChanged: function(was, is) {
			this.$.actions.removeClass(was);
			this.$.actions.addClass(is);
			this.$.title.removeClass(was);
			this.$.title.addClass(is);
			this.$.icon.removeClass(was);
			this.$.icon.addClass(is);
			this.$.wrapper.removeClass(was);
			this.$.wrapper.addClass(is);
		},
		close: function() {
			this.doAction(this._exitAction);
		}
	});

	enyo.kind({
		name: 'ConfirmDialog',
		kind: 'Dialog',
		published: {
			denyLabel: 'CANCELAR'
		},
		bindings: [
			{ from: '.denyLabel', to: '.$.deny.content' },
		],
		actions: [
			{ name: 'deny', kind: 'DialogButton', classes: 'exit', action: 'denied', content: 'CANCELAR', defaultSpotlightLeft: 'accept', defaultSpotlightRight: 'accept' },
			{ name: 'accept', kind: 'DialogButton', classes: 'ok', action: 'accepted', defaultSpotlightLeft: 'deny', defaultSpotlightRight: 'deny' },
		],
		_exitAction: 'denied',
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.gingaRunningListener = this.bindSafely('onGingaRunning');
			};
		}),
		showingChanged: enyo.inherit(function(sup) {
			return function(was, is) {
				sup.apply(this, arguments);
				if (is) {
					gingaModel.on('runningGinga', this.gingaRunningListener);
				} else {
					gingaModel.off('runningGinga', this.gingaRunningListener);
				}
			};
		}),
		onGingaRunning: function(oSender, oEvent, oInfo) {
			if (oInfo.running && this.showing) {
				this.doAction('denied');
			}
		}
	});

	var module = {};
	var alertWidget = new Dialog();
	var confirmWidget = new ConfirmDialog();

	function showWidget(widget, opts) {
		// assert(!widget.showing, 'Cannot open a dialog that is already open');
		widget.removeAllListeners();
		widget.setContext(Context.current());
		widget.set('parent', opts.owner); // Hack to spotlight
		var iconPreffix = Context.current() === 'tvlayer' ? 'tvlayer_' :'';
		widget.set('icon', iconPreffix + opts.icon);
		widget.set('title', opts.title);
		widget.set('description', opts.desc);
		widget.set('acceptLabel', opts.acceptLabel ? opts.acceptLabel : defaultAcceptLabel);

		return new Promise(function(resolve, reject) {
			widget.on('accepted', function() {
				widget.set('parent', null);
				resolve(true);
			});
			// Only in ConfirmDialog.
			widget.on('denied', function() {
				widget.set('parent', null);
				resolve(false);
			});
			widget.show();
		});
	}

	module.confirm = showWidget.bind(undefined, confirmWidget);

	module.warn = function warn(opts) {
		opts.title = 'ADVERTENCIA';
		opts.icon = 'dialog_warn.png';
		return showWidget(confirmWidget, opts);
	}

	module.error = function error(opts) {
		opts.title = 'ERROR';
		opts.icon = 'dialog_error.png';
		return showWidget(alertWidget, opts);
	}

	module.confirmOpen = function() {
		return confirmWidget.showing;
	}

	module.closeAll = function() {
		alertWidget.close();
		confirmWidget.close();
	}

	return module;
})(enyo);
