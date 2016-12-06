enyo.kind({
	name: 'TVLayer',
	classes: 'enyo-fit',
	handlers: {
		onSpotlightKeyDown: 'keyPressed',
		onRestoreSpotlightDummy: 'onRestoreSpotlightDummy',
		onRequestGingaRun: 'onRequestGingaRun'
	},
	components: [
		{ name: 'spotlightDummy', spotlight: true },
		{ name: 'infoBox', kind: 'InfoBox', showing: false, defaultSpotlightDisappear: 'spotlightDummy' },
		{ name: 'gingaApps', kind: 'GingaAppSelector', showing: false, defaultSpotlightDisappear: 'spotlightDummy' },
		{ name: 'gingaBar', kind: 'DownloadGinga', showing: false, defaultSpotlightDisappear: 'spotlightDummy' },
		{ kind: 'Signals', TVModeActived: 'TVModeActived' },
	],
	_protectionWarn: false,
	create: enyo.inherit(function(sup) {
		return function() {
			sup.apply(this, arguments);
			this.blockedListener = this.bindSafely('channelBlocked');
			this.chChangedListener = this.bindSafely('channelChanged');
			this.gingaDownloadListener = this.bindSafely('onGingaDownload');
			this.gingaRunningListener = this.bindSafely('onGingaRunning');
			this.loadKeyBindings();
			this.$.gingaBar.connectBinding(false);
		};
	}),
	loadKeyBindings: function() {

		var kbLayout = tacKeyboardLayout;
		var self = this;
		var infoBox = self.$.infoBox;

		var sendTvModeSignal = function(params) {
			enyo.Signals.send('onTVMode', params);
		};

		var numberPressed = function(keyCode) {
			var offset = 48;
			infoBox.changeToChannel(keyCode - offset);
		};

		var enter = function() {
			if (infoBox.showing) {
				infoBox.onEnter();
				return true;
			}
		};

		this.keyActions = [];

		this.keyActions[kbLayout.VK_INFO] = function() {
			infoBox.showDetailsInfo();
			enyo.Spotlight.spot(infoBox);
		};

		this.keyActions[kbLayout.VK_EXIT] = function() {
			sendTvModeSignal({mode: 'previous', originator: self});
		};

		this.keyActions[kbLayout.VK_HOME] = function() {
			tvdadapter.shutdown();
			sendTvModeSignal({mode: 'off', goto: 'home', originator: self});
		};

		this.keyActions[kbLayout.VK_GUIDE] = function() {
			tvdadapter.shutdown();
			sendTvModeSignal({mode: 'off', goto: 'tv', tab:'guide', originator: self});
		};

		this.keyActions[kbLayout.VK_ENTER] = enter;
		this.keyActions[kbLayout.VK_OK] = enter;

		this.keyActions[kbLayout.VK_CHANNEL_UP] = function() {
			if (!infoBox.showing || infoBox.details) {
				tvdadapter.channelUp(false);
			}
		};

		this.keyActions[kbLayout.VK_CHANNEL_DOWN] = function() {
			if (!infoBox.showing || infoBox.details) {
				tvdadapter.channelDown(false);
			}
		};

		this.keyActions[kbLayout.VK_NEXT_FAVORITE_CHANNEL] = function() {
			tvdadapter.nextFavorite();
		};

		this.keyActions[kbLayout.VK_LAST] = function() {
			tvdadapter.previous();
		};

		this.keyActions[kbLayout.VK_UP] = function(keyCode, orig) {
			if (orig === self.$.spotlightDummy || orig === infoBox) {
				tvdadapter.channelUp(true);
			}
		};

		this.keyActions[kbLayout.VK_DOWN] = function(keyCode, orig) {
			if (orig === self.$.spotlightDummy || orig === infoBox) {
				tvdadapter.channelDown(true);
			}
		};

		this.keyActions[kbLayout.VK_TRACK_NEXT] = function() {
			tvd.player.nextAudio();
		};

		this.keyActions[kbLayout.VK_SUBTITLE] = function() {
			tvd.player.nextSubtitle( util.catchError(function( sid ) {
				notification.emit( 'CC', { id: sid } );
			}));
		};

		this.keyActions[kbLayout.VK_RED] = function() {
			if (gingaModel.get('enabled')) {
				if (infoBox.get('showing')) {
					infoBox.hide();
				}
				self.$.gingaApps.show();
				enyo.Spotlight.spot(self.$.gingaApps);
				self.$.spotlightDummy.set('spotlight', false);
			}
		};

		this.keyActions[kbLayout.VK_0] = numberPressed;
		this.keyActions[kbLayout.VK_1] = numberPressed;
		this.keyActions[kbLayout.VK_2] = numberPressed;
		this.keyActions[kbLayout.VK_3] = numberPressed;
		this.keyActions[kbLayout.VK_4] = numberPressed;
		this.keyActions[kbLayout.VK_5] = numberPressed;
		this.keyActions[kbLayout.VK_6] = numberPressed;
		this.keyActions[kbLayout.VK_7] = numberPressed;
		this.keyActions[kbLayout.VK_8] = numberPressed;
		this.keyActions[kbLayout.VK_9] = numberPressed;

	},
	closeWidgets: function() {
		this.$.infoBox.hide();
		this.$.gingaApps.hide();
	},
	onRestoreSpotlightDummy: function() {
		this.$.spotlightDummy.set('spotlight', true);
	},
	onRequestGingaRun: function(oInfo, oEvent) {
		this.$.gingaApps.hide();
		tvd.middleware.runApplication(oEvent);
		var downloadModel = gingaModel.get('download');
		if( !downloadModel.get('downloaded') ) {
			this.$.gingaBar.show();
		}
		return true;
	},
	showingChanged: enyo.inherit(function(sup) {
		return function(was, is) {
			sup.apply(this, arguments);
			if (is) {
				tvdadapter.on('changed', this.chChangedListener);
				channels.on('onBlockedCh', this.blockedListener);
				gingaModel.get('download').on('downloadGinga', this.gingaDownloadListener);
				gingaModel.on('runningGinga', this.gingaRunningListener);
				tvd.player.isProtected(util.catchError(function(isProtected) {
					if (isProtected) {
						this.channelBlocked();
					}
					else {
						enyo.Spotlight.spot(this);
						this.$.infoBox.showInfo(false);
					}
				}.bind(this)));
			}
			else {
				channels.off('onBlockedCh', this.blockedListener);
				tvdadapter.off('changed', this.chChangedListener);
				gingaModel.off('downloadGinga', this.gingaDownloadListener);
				gingaModel.off('runningGinga', this.gingaRunningListener);
				this._protectionWarn = false;
				this.$.infoBox.hide();
				this.$.gingaApps.hide();
				this.onRestoreSpotlightDummy();
				if(this.$.gingaBar.showing) {
					this.stopJob('hide');
					this.$.gingaBar.hide();
				}
			}
		};
	}),
	onGingaDownload: function(oSender, oEvent, oInfo) {
		log.info('TVLayer', 'onGingaDownload', 'Value=%d', oInfo.downloading);
		if (oInfo.downloading) {
			if (!this.$.gingaBar.showing) {
				this.$.gingaBar.show();
			}
			this.$.gingaBar.connectBinding(true);
		} else {
			var self = this;
			this.$.gingaBar.connectBinding(false);
			this.startJob('hideGingaBar', function() {
				self.$.gingaBar.hide();
			}, 1000);
		}
	},
	onGingaRunning: function(oSender, oEvent, oInfo) {
		log.info('TVLayer', 'onGingaRunning', 'Value=%d', oInfo.running);
		if (oInfo.running && this.$.gingaApps.showing) {
			this.$.gingaApps.hide();
			this.onRestoreSpotlightDummy();
			enyo.Spotlight.spot(this.$.spotlightDummy);
		} else if (oInfo.running && this.$.infoBox.showing) {
			this.$.infoBox.hide();
		}
	},
	channelBlocked: function(oSender, oEvent, reason) {
		if (!this._protectionWarn) {
			this._protectionWarn = true;
			var infobox = this.$.infoBox;
			infobox.hide();
			enyo.Spotlight.States.push('focus');
			var self = this;
			dialogs.confirm({
				owner: Context.owner(),
				icon: 'dialog_bloq.png',
				title: 'CANAL BLOQUEADO',
				desc: reason === 'content' ? msg.channels.contentBlocked : msg.channels.channelBlocked,
				acceptLabel: 'DESBLOQUEAR',
			}).then(function(ok) {
				if (ok) {
					enyo.Spotlight.States.push('focus');
					security.openSession().then(function(open) {
						if (open) {
							infobox.onChannelChanged(undefined, undefined, { isVirtual: false, chID: tvdadapter.current(false) });
						}
					});
				} else {
					self._protectionWarn = false;
				}
			});
		}
	},
	channelChanged: function( oSender, oEvent, oInfo ) {
		if (dialogs.confirmOpen()) { return true; }
		if (oInfo.chID !== -1) {

			var chModel = channels.getModelFromID( oInfo.chID );
			if (!chModel.get('isBlocked') || session.get('isOpen')) {
				if (!this.$.infoBox.get('zapping')) {
					this.$.infoBox.changeToZappingMode();
				}
				this.$.infoBox.showInfo( oInfo.isVirtual );
			}
			else {
				this.$.infoBox.hide();
			}

			if (!oInfo.isVirtual) {
				if (this.$.gingaApps.showing) {
					this.$.gingaApps.hide();
				}

				if (this.$.gingaBar.showing) {
					this.$.gingaBar.hide();
				}
			}
		}
	},
	TVModeActived: function(oSender, oEvent) {
		if (oEvent.mode === 'off' && oEvent.originator === this) {
			tvdadapter.shutdown();
		}
	},
	keyPressed: function(oSender, oEvent) {
		var action = this.keyActions;
		var keyCode = oEvent.keyCode;
		if (action[keyCode]) {
			action[keyCode](keyCode, oEvent.originator);
		}
	}
});
