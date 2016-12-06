
(function() {

	enyo.kind({
		name: 'Wizard',
		classes: 'wizard-window enyo-fit',
		spotlight: 'container',
		showLicense: true,
		handlers: {
			onMoveToPanel: 'moveToPanel',
			onTransitionFinish: 'updateBreadcrumb',
			onTransitionStart: 'transitionStarted',
		},
		bindings: [
			{ from: '.$.panels.index', to: '.index' }
		],
		components: [
			{
				name: 'steps',
				classes: 'steps',
				showing: false,
				components: [
					{
						name: 'breadcrumb',
						kind: 'Breadcrumb',
						components: [
							{ content: 'TÉRMINOS Y CONDICIONES', step: 'PASO 1 DE 3' },
							{ content: 'INTERNET', step: 'PASO 2 DE 3' },
							{ content: 'BUSCAR CANALES', step: 'PASO 3 DE 3' },
						]
					},
					{
						name: 'panels',	kind: 'moon.Panels', classes: 'wizard-panels', arrangerKind: 'CarouselArranger',
						handlers: { onSpotlightRight: util.nop,	onSpotlightLeft: util.nop },
						components: [
							{ name: 'step1', kind: 'Step1', prevTo: '', nextTo: 'step2' },
							{ name: 'step2', kind: 'Step2', prevTo: 'step1', nextTo: '', nextToOpt1: 'step4', nextToOpt2: 'step3' },
							{ name: 'step3', kind: 'Step3', prevTo: 'step2', nextTo: 'step4' },
							{ name: 'step4', kind: 'Step4', prevTo: 'step2', nextTo: '' }
						],
					}
				]
			},
			{
				name: 'welcome',
				classes: 'welcome',
				components: [
					{ kind: 'enyo.Image', src: assets('intro.png'), classes: 'img' },
					{ kind: 'WizardButton', content: 'COMENZAR', onSpotlightSelect: 'startSteps' }
				]
			}
		],
		showingChanged: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);

				if (this.showing) {
					scanModel.stop();
					this.$.welcome.show();
				}
				else {
					this.$.steps.hide();
				}

				this.bubble('onBlurResquest', {on: this.showing}, this);
			}
		}),
		indexChanged: function() {
			this.$.panels.getActive().set('spotlightDisabled', false);
		},
		moveToPanel: function(sender, name) {
			sender.set('spotlightDisabled', true);
			this.$.panels.selectPanelByName(name);
		},
		updateBreadcrumb: function() {
			this.$.breadcrumb.moveTo(this.$.panels.getActive().get('stepIndex'));
		},
		transitionStarted: function(semder, evt) {
			var to = this.$.panels.getClientControls()[evt.toIndex];
			if (to.transitionStarted) {
				to.transitionStarted();
				return true;
			}
		},
		startSteps: function() {
			this.$.steps.show();
			this.$.panels.reflow();
			this.$.step1.set('spotlightDisabled', !this.showLicense);
			this.$.step2.set('spotlightDisabled', this.showLicense);
			this.$.panels.setIndexDirect(this.showLicense? 0 : 1);
			this.$.welcome.hide();
			enyo.Spotlight.spot(this.$.steps);
		}
	});

	enyo.kind({
		name: 'BreadcrumbItem',
		classes: 'item',
		published: {
			current: '',
			step: '',
		},
		bindings: [
			{ from: '.content', to: '.$.description.content' },
			{ from: '.current', to: '.$.step.showing' },
			{ from: '.step', to: '.$.step.content' },
		],
		components: [
			{ name: 'description', classes: 'desc' },
			{ name: 'step', classes: 'step', content: 'Paso ' },
		],
		check: function(checked) {
			this.$.description.addRemoveClass('filled', checked);
		},
		reset: function() {
			this.check(false);
			this.setCurrent(false);
		}
	});

	enyo.kind({
		name: 'Breadcrumb',
		classes: 'breadcrumb',
		defaultKind: 'BreadcrumbItem',
		published: {
			current: 0
		},
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.currentChanged(undefined, 0);
			}
		}),
		currentChanged: function(was, is) {
			this.getClientControls().forEach(function(ctrol, index) {
				ctrol.check(index <= is);
				ctrol.setCurrent(index == is);
			});
		},
		moveTo: function( index ) {
			if (this.current !== index) {
				this.getClientControls()[this.current].setCurrent(false);
				this.setCurrent(index);
			}
		},
		reset: function() {
			this.getClientControls().slice(1).forEach(function(c) {
				c.reset(false);
			});
			this.setCurrent(0);
		}
	});

	enyo.kind({
		name: 'WizardPanel',
		spotlight: 'container',
		classes: 'wizard-panel',
		spotlightDisabled: true,
		published: {
			prev: 'ANTERIOR',
			next: 'CONTINUAR',
			nextTo: '',
			prevTo: '',
			stepIndex: 0,
		},
		events: {
			onMoveToPanel: ''
		},
		handlers: {
			onSpotlightKeyDown: 'handleKey',
			onSpotlightContainerEnter: 'onEnter'
		},
		bindings: [
			{ from: '.prev', to: '.$.leftBtn.content' },
			{ from: '.next', to: '.$.rightBtn.content' },
			{ from: '.prev', to: '.$.leftBtn.showing' },
			{ from: '.next', to: '.$.rightBtn.showing' },
		],
		tools: [
			{ name: 'client', classes: 'wizard-panel-client' },
			{
				name: 'btns', classes: 'wizard-buttons',
				spotlight: 'container',
				defaultSpotlightControl: 'rightBtn', onSpotlightContainerEnter: 'onBtnsEnter',
				components: [
					{
						name: 'leftBtn', kind: 'WizardButton', ontap: 'leftBtnAction', content: 'ANTERIOR',
					},
					{
						name: 'rightBtn', kind: 'WizardButton', ontap: 'rightBtnAction', content: 'CONTINUAR',
					}
				]
			}
		],
		initComponents: enyo.inherit(function(sup) {
			return function() {
				this.createChrome(this.tools);

				sup.apply(this, arguments);

				if (this.prev && this.next) {
					this.$.btns.applyStyle('justify-content', 'space-between');
				}
			}
		}),
		onEnter: function(oSender, oEvent) {
			if (oEvent.originator === this) {
				enyo.Spotlight.spot(this.$.rightBtn);
			}
		},
		onBtnsEnter: function(oSender, oEvent) {
			if (oEvent.originator === this.$.btns) {
				enyo.Spotlight.spot(this.$.rightBtn);
			}
		},
		leftBtnAction: function() {
			this.doMoveToPanel(this.prevTo ? this.prevTo : '');
		},
		rightBtnAction: function() {
			this.doMoveToPanel(this.nextTo ? this.nextTo : '');
		},
		handleKey: enyo.inherit(function(sup) {
			return function(sender, evt) {
				switch(evt.keyCode) {
					case tacKeyboardLayout.VK_BACK:
						if (this.prevTo !== 'step1') {
							this.leftBtnAction();
							return true;
						}
				}
				return sup.apply(this, arguments);
			};
		}),
	});

	enyo.kind({
		name: 'WizardButton',
		kind: 'Shell.SimpleButton',
		classes: 'wizard-button',
		fixed: true,
		style: 'width: 185px;'
	});

})();
