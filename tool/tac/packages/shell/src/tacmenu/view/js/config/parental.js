/*
 * Kind Parental
*/
enyo.kind({
	name: 'ParentalControlView',
	classes: 'parental-control-view',
	spotlight: 'container',
	handlers: {
		onSpotlightContainerEnter: 'checkSession',
		onSpotlightContainerLeave: 'leaved',
		onLeavingBox: 'changeBoxOpacity',
		onSpotlightUp: 'onUp'
	},
	bindings: [
		{ from: '^session.isOpen', to: '.sessionOpen' },
		{ from: '^session.hasPassword', to: '.$.carousel.sideArrows' },
		{ from: '^session.hasPassword', to: '.$.carousel.blocked', transform: util.negate }
	],
	components: [
		{
			classes: 'layout-carousel-container',
			components: [
				{
					name: 'carousel',
					kind: 'Carousel',
					style: 'width: 100%',
					height: 340,
					width: 340,
					heightMultiplier: 3,
					detailed: false
				}
			]
		}
	],
	create: function() {
		this.inherited(arguments);
		var comps = [
			{ name: 'PasswordManagerView', kind: 'PasswordManager' },
			{ name: 'AgeView', kind: 'AgeRestriction' },
			{ name: 'ContentView', kind: 'ContentRestriction' }
		]
		this.$.carousel.$.container.createComponents(comps, {owner: this});
		this.$.carousel.$.posIndicator.hide();
	},
	leaved: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			this.changeBoxOpacity();
			this.$.carousel.resetToFirst();
		}
	},
	checkSession: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			this.$.carousel.waterfall('onEnableBox', {originator: this});
		}
	},
	changeBoxOpacity: function() {
		this.$.carousel.waterfall('onDisableBox', {originator: this});
	},
	sessionOpenChanged: function(was, is) {
		if (!is) {
			// Leave parental config, is protected!!
			this.bubble('onMenuRequest');
		}
	},
	onUp: function(oSender, oEvent) {
		if (this.$.carousel.$.container.getClientControls().indexOf(oEvent.originator) !== -1) {
			this.bubble('onMenuRequest');
			return true;
		}
	}
});

/*
* PasswordManager
*/
enyo.kind({
	name: 'PasswordManager',
	kind: 'ParentalBox',
	icon: assets('contrasena_min_act.png'),
	help: {txt: msg.help.config.tda.parental.passwd, img: 'carousel-parental.png'},
	title: 'Contraseña',
	handlers: {
		onSpotlightContainerEnter: 'onEnter',
		onSpotlightContainerLeave: 'onLeave'
	},
	components: [
		{
			name: 'details',
			classes: 'parental-details',
			content: 'Por favor cree una contraseña para hacer uso de las restricciones'
		},
		{
			kind: 'components.VerticalMenu',
			components: [
				{
					name: 'btnPass',
					kind: 'ParentalBtn',
					style: 'margin-top: 30px;',
					onSpotlightSelect: 'changePass',
				},
				{
					name: 'btnDelete',
					kind: 'ParentalBtn',
					text: 'ELIMINAR',
					onSpotlightSelect: 'deletePass',
					showing: false
				}
			]
		}
	],
	bindings: [
		{ from: '^session.hasPassword', to: '.$.btnPass.text', transform: function(v) { return v? 'CAMBIAR' : 'CREAR'; } },
		{ from: '^session.hasPassword', to: '.$.details.showing', transform: util.negate },
		{ from: '^session.hasPassword', to: '.$.btnDelete.showing' }
	],
	changePass: function() {
		var sessionExists = session.get('hasPassword');
		security.openSession(sessionExists).then(function(ok) {
			if (ok) {
				var opts = {title: sessionExists ? 'CAMBIAR CONTRASEÑA' : 'CREAR CONTRASEÑA'};
				return keyboard.show(opts).then(function(data) {
					if (data.status === 'accepted') {
						session.changePass(data.input, util.nop);
					}
				});
			}
			return false;
		});
		return true;
	},
	deletePass: function() {
		enyo.Spotlight.States.push('focus');
		dialogs.warn({desc: msg.popup.warn.removePasswd, owner: Context.owner()}).then(function(ok) {
			if (ok) {
				this.bubble('onMenuRequest');
				session.deletePass();
			}
		}.bind(this));
		return true;
	},
	onEnter: enyo.inherit(function(sup) {
		return function() {
			sup.apply(this, arguments);
			this.$.description.applyStyle('margin-top', '60px');
			this.$.description.applyStyle('margin-bottom', 'auto');
		};
	}),
	onLeave: enyo.inherit(function(sup) {
		return function() {
			sup.apply(this, arguments);
			this.$.description.applyStyle('margin-top','15px');
		};
	}),
});

/*
* AgeRestriction
*/
enyo.kind({
	name: 'AgeRestriction',
	kind: 'ParentalBoxFilter',
	icon: assets('edad_min_act.png'),
	iconOff: assets('edad_min.png'),
	title: 'Por edad',
	help: {txt: msg.help.config.tda.parental.age, img: 'carousel-parental.png'},
	published: {
		currentAge: undefined
	},
	handlers: {
		onCheckBoxSelect: 'onCheckBoxSelect'
	},
	bindings: [
		{ from: '^session.age', to: '.currentAge', oneWay: false },
		{ from: '.$.client.showing', to: '.smallIcon' }

	],
	components: [
		{ name: 'txt-details', classes: 'parental-details', content: 'Restringir programación para:' },
		{
			name: 'groupContainer',
			kind: 'Group',
			style: 'margin-top: 10px; margin-bottom: 10px;',
			spotlight: 'container',
			mixins: [CircularNavSupport],
			components: [
				{ kind: 'components.LabeledRadioButton', content: 'No restringir', value: 0 },
				{ kind: 'components.LabeledRadioButton', content: 'Mayores de 18', value: 18 },
				{ kind: 'components.LabeledRadioButton', content: 'Mayores de 16', value: 16 },
				{ kind: 'components.LabeledRadioButton', content: 'Mayores de 14', value: 14 },
				{ kind: 'components.LabeledRadioButton', content: 'Mayores de 12', value: 12 },
				{ kind: 'components.LabeledRadioButton', content: 'Mayores de 10', value: 10 },
			]
		}
	],
	onCheckBoxSelect: function(sender, evt) {
		console.log('evt pa %O', evt)
		session.age(evt.value);
		return true;
	},
	currentAgeChanged: function(was, is ) {
		this.$.groupContainer.getClientControls().forEach(function(checkbox) {
			checkbox.set('checked', checkbox.value === is);
		});
		return true;
	}
});

/*
* ContentRestriction
*/
enyo.kind({
	name: 'ContentRestriction',
	kind: 'ParentalBoxFilter',
	help: {txt: msg.help.config.tda.parental.content, img: 'carousel-parental.png'},
	handlers: {
		onCheckBoxSelect: 'onCheckBoxSelect',
	},
	icon: assets('contenido_min_act.png'),
	iconOff: assets('contenido_min.png'),
	title: 'Por contenido',
	components: [
		{ name: 'txt-details', classes: 'parental-details', content: 'Restringir contenido con: ' },
		{
			name: 'groupContainer',
			style: 'margin-bottom: 20px; margin-top: auto;',
			spotlight: 'container',
			mixins: [CircularNavSupport],
			components: [
				{ name: 'drugs', kind: 'components.LabeledCheckbox', content: 'Drogas', classes: 'content'},
				{ name: 'sex', kind: 'components.LabeledCheckbox', content: 'Sexo', classes: 'content' },
				{ name: 'violence', kind: 'components.LabeledCheckbox', content: 'Violencia', classes: 'content'},
			]
		}
	],
	bindings: [
		{ from: '^session.drugs', to: '.$.drugs.checked' },
		{ from: '^session.sex', to: '.$.sex.checked' },
		{ from: '^session.violence', to: '.$.violence.checked' }
	],
	onCheckBoxSelect: function(sender, evt) {
		switch(evt.content) {
			case 'Drogas':
				session.drugs(evt.checked);
				break;
			case 'Sexo':
				session.sex(evt.checked);
				break;
			case 'Violencia':
				session.violence(evt.checked);
				break;
		}
		return true;
	}
});

// TODO [nbaglivo]: Use CarouselItem to impl this kind
enyo.kind({
	name: 'ParentalBox',
	classes: 'parental-box',
	spotlight: 'container',
	events: {
		onLeavingBox: ''
	},
	published: {
		highlighted: false
	},
	bindings: [
		{ from: '.icon', to: '.$.icon.src' },
		{ from: '.title', to: '.$.txt.content' }
	],
	handlers: {
		onSpotlightUp: 'onUp',
		onSpotlightContainerEnter: 'onEnter',
		onSpotlightContainerLeave: 'onLeave',
		onEnableBox: 'enableBox',
		onDisableBox: 'disableBox'
	},
	tools: [
		{
			name: 'wrapper',
			classes: 'parentalbox-wrapper',
			components: [
				{
					classes: 'content',
					components: [
						{
							name: 'description',
							classes: 'description',
							components: [
								{
									name: 'iconContainer',
									classes: 'parental-icon-container',
									components: [
										{ name: 'icon', kind:'enyo.Image', classes: 'parental-icon' },
									]
								},
								{ name: 'txt', classes: 'txt' },
							]
						},
						{ name: 'client', showing: false, spotlight: 'container' }
					]
				}
			]
		},
		{ name: 'dummy', spotlight: true }
	],
	initComponents: function() {
		this.createChrome(this.tools);
		this.inherited(arguments);
	},
	onEnter: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			Context.setHelp(oSender.help);
			this.bubble('onRequestScroll', {dir: oEvent.dir || oEvent.direction });
			this.addClass('spotlight');
		}
	},
	onLeave: function(oSender, oEvent) {
		if (oEvent.originator === this) {
			this.removeClass('spotlight');
		}
	},
	highlightedChanged: function(was, is) {
		this.addRemoveClass('highlight', is);
		this.$.iconContainer.applyStyle('height', is? 'auto' : '124px');
		this.$.icon.applyStyle('max-height', is? '90px' : '130px');
		this.$.icon.applyStyle('min-width', is? '90px' : '130px');
		this.$.client.setShowing(is);
		enyo.Spotlight.States.push('focus', this.$.client);
		if (is && !Shell.helpDialog.showing) {
			enyo.Spotlight.spot(this.$.client);
		}
		this.$.dummy.spotlightDisabled = is;
	},
	enableBox: function(oSender, oEvent) {
		if (!this.$.wrapper.hasClass('enabled') && !this.hasClass('enabled')) {
			this.addClass('enabled');
			this.$.wrapper.addClass('enabled');
		}
		return true;
	},
	disableBox: function(oSender, oEvent) {
		this.$.dummy.spotlightDisabled = false;
		if (this.$.wrapper.hasClass('enabled') && this.hasClass('enabled')) {
			this.removeClass('enabled');
			this.$.wrapper.removeClass('enabled');
		}
		return true;
	}
});

/*
* ParentalBoxFilter
*/
enyo.kind({
	name: 'ParentalBoxFilter',
	kind: 'ParentalBox',
	active: false,
	_enabled: false,
	bindings: [
		{ from: '.iconOff', to: '.$.iconOff.src' },
		{ from: '^session.hasPassword', to: '.active' },
	],
	create: function() {
		this.inherited(arguments);
		this.$.iconContainer.createComponent(
			{
				name: 'iconOff',
				kind:'enyo.Image',
				classes: 'parental-icon',
				showing: false
			},
			{owner: this});
	},
	activeChanged: function(old) {
		if (this._enabled) {
			this.$.icon.set('showing', this.active );
			this.$.iconOff.set('showing', !this.active );
			this.$.txt.addRemoveClass('disabled', !this.active);
		}
	},
	enableBox: function(oSender, oEvent) {
		var result = session.get('hasPassword');
		this.$.icon.set('showing', result);
		this.$.iconOff.set('showing', !result);
		this.$.txt.addRemoveClass('disabled', !result);
		this._enabled = true;
		this.inherited(arguments);
	},
	disableBox: function(oSender, oEvent) {
		this.$.icon.show();
		this.$.iconOff.hide();
		this.$.txt.removeClass('disabled');
		this._enabled = false;
		return this.inherited(arguments);
	}
});

/*
* ParentalBtn
*/
enyo.kind({
	name: 'ParentalBtn',
	classes: 'parental-btn',
	spotlight: true,
	events: {
		onSpotlightContainerLeave: ''
	},
	bindings: [
		{ from: '.text', to: '.$.btn-txt.content' }
	],
	components: [
		{ name: 'btn-txt', classes: 'parental-btn-txt' }
	]
});
