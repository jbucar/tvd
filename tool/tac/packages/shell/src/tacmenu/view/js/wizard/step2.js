enyo.kind({
	name: 'Step2',
	kind: 'WizardPanel',
	stepIndex: 1,
	prev: false,
	events: {
		onNetworkSelect: ''
	},
	bindings: [
		{ from: '.$.interfaces.interfaceSelected', to: '.nextTo', transform: 'nextStep' }
	],
	components: [
		{ name: 'interfaces', kind: 'Interfaces', onContainerLeave: 'spotToBtn' }
	],
	nextStep: function() {
		this.doNetworkSelect( this.$.interfaces.get('interfaceSelected') );
		return this.$.interfaces.get('interfaceSelected') === 'wired' ? this.nextToOpt1 : this.nextToOpt2;
	},
	onEnter: enyo.inherit(function (sup) {
		return function (oSender, oEvent) {
			sup.apply(this, arguments);
			if (oEvent.originator === this) {
				this.$.interfaces.reset();
			}
		};
	}),
	spotToBtn: function() {
		enyo.Spotlight.spot(this.$.interfaces);
	}
});

/* Interfaces Selection */
enyo.kind({
	name: 'Interfaces',
	classes: 'wizard-network-view',
	spotlight: 'container',
	published: {
		intefaceSelected: ''
	},
	bindings: [
		{ from: '.$.interfaces.active', to:'.interfaceSelected', transform: 'activeInterfaceChanged' },
	],
	components: [
		{ name: 'title', classes: 'wizard-title', content: 'INTERNET' },
		{ name: 'description', classes: 'wizard-content', content: 'Seleccione el tipo de conexión que desee y presione OK' },
		{ name: 'interfaces', kind: 'Group', highlander: true, classes: 'wizard-network-interfaces', components: [
			{ name: 'wired', kind: 'InterfaceCheckbox', content: 'CABLE DE RED', active: true },
			{ name: 'wireless', kind: 'InterfaceCheckbox', content: 'WIFI' },
		] }
	],
	reset: function() {
		this.$.wired.setActive(true);
	},
	activeInterfaceChanged: function(value) {
		return value.name;
	}
});

/* InterfaceCheckbox */
enyo.kind({
	name: 'InterfaceCheckbox',
	classes: 'wizard-network-checkbox',
	spotlight: true,
	published: {
		active: false,
	},
	handlers: {
		onSpotlightSelect: 'check',
	},
	events: {
		onActivate: ''
	},
	bindings: [
		{ from: '.content', to: '.$.text.content' },
		{ from: '.active', to: '.$.check.checked' },
	],
	components: [
		{ name: 'text', classes: 'wizard-network-text' },
		{ name: 'check', classes: 'wizard-network-check', kind: 'WizardCheckbox', checkImg: 'check_act.png', uncheckImg: 'check_in.png' },
	],
	create: function() {
		this.inherited(arguments);
		this.activeChanged();
	},
	check: function(oSender, oEvent) {
		this.setActive(true);
	},
	activeChanged: function() {
		this.bubble('onActivate');
	}
});

/* CheckboxImage */
enyo.kind({
	name: 'WizardCheckbox',
	published: {
		checked: false,
	},
	checkImg: '',
	uncheckImg: '',
	bindings: [
		{ from: '.checked', to: '.$.imgChecked.showing' },
		{ from: '.checked', to: '.$.imgUnChecked.showing', transform: util.negate },
		{ from: '.checkImg', to: '.$.imgChecked.src', transform: util.asset },
		{ from: '.uncheckImg', to: '.$.imgUnChecked.src', transform: util.asset }
	],
	components: [
		{ name: 'imgChecked', kind: 'enyo.Image' },
		{ name: 'imgUnChecked', kind: 'enyo.Image' }
	]
});
