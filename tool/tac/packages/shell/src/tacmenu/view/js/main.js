
(function(scope) {

	enyo.kind({
		name: 'TVSourceResource',
		mixins:[DISupport, enyo.ComputedSupport, enyo.BindingSupport, enyo.EventEmitter],
		published: {
			value: null,
		},
		initialize: function() {
			tvd.on('playerMediaChanged', function playerMediaChanged(url, started) {
				log.info('TVSourceResource', 'playerMediaChanged', 'url=%s started=%s', url, started);
				this.set('value', started? url : null);
			}.bind(this));
			return this;
		}
	});

	scope.TVSource = new TVSourceResource().initialize();

	enyo.kind({
		name: 'TVideo',
		kind: 'enyo.Video',
		classes: 'shell-video',
		bindings: [
			{ from: '^TVSource.value', to: '.tv' }
		],
		components: [
			{ kind: 'Signals', onTVMode: 'onTvMode' }
		],
		play: function() {
			log.info('TVideo', 'play','this=%s src=%s', this.name, TVSource.get('value'));
			this.setSrc(TVSource.get('value'));
			this.inherited(arguments);
		},
		pause: function() {
			log.info('TVideo', 'pause', 'this=%s', this.name);
			this.setSrc(null);
			this.inherited(arguments);
		},
		tvChanged: function(was, is) {
			log.info('TVideo', 'tvChanged','this=%s src=%s playing=%s', this.name, is, this._prevCommand === 'play');
			if (this._prevCommand === 'play') {
				this.setSrc(is);
				if (is) {
					this.play();
				}
			}
		},
		onTvMode: function(oSender, oEvent)  {
			if(oEvent.mode === 'off' && this._prevCommand === 'play') {
				this.pause();
			}
		}
	});

})(this);


(function() {

	var focusStack = function() {
		var self = {};
		var stack = [];

		self.init = function() {
			stack = [];
			var cur = enyo.Spotlight.getCurrent();
			while(cur) {
				stack.push(cur);
				cur = enyo.Spotlight.getParent(cur);
			}

			return stack;
		}

		// Find the first spottable control in the stack
		self.firstSpottable = function() {
			return stack.find(function(ctrol) {
				return enyo.Spotlight.isSpottable(ctrol);
			});
		}

		return self;
	}();

	var fullscreenSize = { left: 0, top: 0, width: 1280, height: 720 };
	var CDA_APP = 'ar.gob.minplan.tac.cda';

	appmgr.on('application', function(evt) {
		var isGingaApp = /ar\.edu\.unlp\.info\.lifia\.tvd\.ginga.*/.test(evt.id);
		if (evt.state === 'starting' && !isGingaApp) {
			// When an app starts we need to shutdown the TV and close all dialogs
			dialogs.closeAll();
			tvdadapter.shutdown();
			enyo.Signals.send('onTVMode', {mode: 'off'});

			if (evt.id === CDA_APP) {
				app.goHome();
			}
		}
	});

	enyo.kind({
		name: 'TAC',
		handlers: {
			onSpotlightKeyDown: 'onKey',
			onBlurResquest: 'onBlurResquested'
		},
		components: [
			{
				name: 'blured',
				classes: 'home-background enyo-fit',
				kind: 'enyo.Image',
				src: assets('backgroundblured.png'),
				sizing: 'cover',
				showing: false,
			},
			{
				name: 'background',
				classes: 'home-background enyo-fit',
				kind: 'enyo.Image',
				src: assets('background.png'),
				sizing: 'cover',
				showing: true,
			},
			{ name: 'menuLayer', kind: 'TACMenu', showing: false },
			{ name: 'wizard', kind: 'Wizard', showing: false },
			{ name: 'tvLayer', kind: 'TVLayer', showing: false },
			{ kind: "Signals", onWizardRequest: "onWizard", onTVMode: "onTvMode" },
			{
				name: 'mainVideo',
				kind: 'TVideo',
				showing: false,
				style: 'position: absolute'
			}
		],
		curTvMode: 'off',
		_prevMode: 'off',
		_insessionTransaction: false,
		create: function() {
			this.inherited(arguments);

			var self = this;
			system.isFirstBoot(util.catchError(function(res) {
				if (res) {
					enyo.Signals.send('onWizardRequest', {on: true, firstBoot: true});
				}
				else {
					self.$.menuLayer.show();
					enyo.Spotlight.spot(self.$.menuLayer);
				}
			}));

			tvd.on('closedCaption', function(text) {
				if (!self.$.mainVideo.isPaused()) {
					var videoElement = self.$.mainVideo.hasNode();
					var textTrack = videoElement.textTracks[0];
					if (!textTrack) {
						textTrack = videoElement.addTextTrack('captions', 'Closed Caption', 'es-AR');
						textTrack.mode = 'showing';
					}
					else if (textTrack.cues.length > 0) {
						for (var i=0; i<textTrack.cues.length; i++) {
							textTrack.removeCue(textTrack.cues[i]);
						}
					}
					var t = self.$.mainVideo.getCurrentTime();
					textTrack.addCue(new VTTCue(t, t + 5.0, text));
				}
			});

			tvd.on('playerChanged', function() {
				var textTrack = self.$.mainVideo.hasNode().textTracks[0];
				if (textTrack) {
					for (var i=0; i<textTrack.cues.length; i++) {
						textTrack.removeCue(textTrack.cues[i]);
					}
				}
			});

			tvd.on('middleware', function(evt) {
				if (evt.type === 'mainVideo') {
					// TODO: Unhardcode screensize
					var rect;
					if (evt.method === 'resize') {
						rect = {
							left: (evt.rect.x / 720) * fullscreenSize.width,
							top: (evt.rect.y / 576) * fullscreenSize.height,
							width: (evt.rect.w / 720) * fullscreenSize.width,
							height: (evt.rect.h / 576) * fullscreenSize.height
						}
					}
					else if (evt.method === 'stop') {
						rect = fullscreenSize;
					}

					if (rect) {
						this.$.mainVideo.setBounds(rect);
					}
				}
			}.bind(this));
		},
		goHome: function() {
			this.$.menuLayer.selectComponent('home', undefined, true);
		},
		onBlurResquested: function(oSender, oEvent) {
			this.$.blured.setShowing(oEvent.on);
			this.$.background.setShowing(!oEvent.on);
		},
		onWizard: function(oSender, oEvent) {
			notification.emit('NotificationStatus', {enabled: !oEvent.on}); // No notification while the wizard is working
			if (oEvent.on) {
				this.$.wizard.showLicense = oEvent.firstBoot;
				this.$.menuLayer.hide();
				this.$.wizard.show();
				Context.clear();
				Context.enter('wizard', this.$.wizard);
				this.$.menuLayer.freezeContainer = true;
				enyo.Spotlight.spot(this.$.wizard);
			}
			else {
				Context.leave('wizard');
				this.$.wizard.hide();
				this.$.menuLayer.show();
				this.$.menuLayer.freezeContainer = false;
				this.goHome();
			}
		},
		onTvMode: function(oSender, oEvent) {
			log.trace('Main', 'onTvMode', 'mode=%s curMode=%s', oEvent.mode, this.curTvMode);

			if (oEvent.mode === 'previous') {
				oEvent.mode = this._prevMode;
				oEvent.bounds = this._prevBounds;
			}
			this._prevMode = oEvent.prev || this.curTvMode;
			this._prevBounds = oEvent.prevBounds;
			this.curTvMode = oEvent.mode;

			if (oEvent.mode === 'preview' || oEvent.mode === 'fullscreen') {
				if (oEvent.mode === 'preview' && oEvent.bounds) {
					this.$.mainVideo.setBounds(oEvent.bounds);
				}
				else { // Fullscreen
					this.$.mainVideo.setBounds(fullscreenSize);
				}

				this.$.mainVideo.show();
				this.$.mainVideo.play();
			}
			else if(oEvent.mode === 'off') {
				this.$.mainVideo.hide();
			}

			if (oEvent.mode === 'fullscreen') {
				focusStack.init();
				Context.enter('tvlayer', this.$.tvLayer);
				this.$.menuLayer.freezeContainer = true;
				this.$.tvLayer.show();
				this.$.menuLayer.hide();
			}
			else if (!this.$.menuLayer.showing) {
				Context.leave('tvlayer');
				this.$.menuLayer.freezeContainer = false;
				this.$.menuLayer.show();
				enyo.Spotlight.spot(focusStack.firstSpottable());
				this.$.tvLayer.hide();
			}

			if (oEvent.goto) {
				this.$.menuLayer.selectComponent(oEvent.goto, {tab: oEvent.tab, enter: true}, true);
			}

			enyo.Signals.send('TVModeActived', {mode: oEvent.mode, originator: oEvent.originator, prevMode: this._prevMode});
		},
		onKey: function(oSender, oEvent) {
			// No shortcuts in wizard
			if (this.$.wizard.showing) {
				return;
			}

			switch(oEvent.keyCode) {
				case tacKeyboardLayout.VK_YELLOW:
						this.openSession();
					break;
				case tacKeyboardLayout.VK_LIST:
					var chID = tvdadapter.current(false);
					tvdadapter.shutdown();
					enyo.Signals.send('onTVMode', {mode: 'off'});
					if (this.$.menuLayer.showing) {
						var sub = {tab: 'TODOS', enter: availableChannels.length > 0}; // Enter to TODOS only if there are channels
						var fromOutside = !oEvent.originator.isDescendantOf(this.$.menuLayer);
						this.$.menuLayer.selectComponent('tv', {tab: 'watchTV', enter: true, sub: sub}, fromOutside);
						this.$.menuLayer.$.tv.$.chContainer.spotCh(chID);
					}
					return true;

				case tacKeyboardLayout.VK_RECALL_FAVORITE_0:
					appmgr.run(CDA_APP, util.informError());
					return true;
			}
		},
		openSession: function() {
			if (!this._insessionTransaction) {
				this.$.tvLayer.closeWidgets();
				var isOpen = session.get('isOpen');
				this._insessionTransaction = true;
				log.trace('Main', 'openSession', 'isOpen=%s', isOpen);
				enyo.Spotlight.States.push('focus');
				if (!isOpen) {
					security.openCreateSession({
						owner: Context.owner(),
						title: 'USTED NO HA CREADO UNA SESIÓN',
						desc: '¿Desea crearla?',
						icon: 'dialog_warn.png'
					}).then(function() {
						this._insessionTransaction = false;
					}.bind(this));
				}
				else {
					dialogs.confirm({
						owner: Context.owner(),
						icon: 'dialog_bloq.png',
						title: 'CIERRE DE SESIÓN',
						desc: '¿Está seguro que desea cerrar sesión?',
						acceptLabel: 'ACEPTAR'
					}).then(function(ok) {
						if (ok) {
							session.close();
						}
						this._insessionTransaction = false;
					}.bind(this));
				}
			}
		},
	});

})();
