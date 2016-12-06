(function() {

	//TODO [nbaglivo]: Refactoring. Some code are duplicated.
	enyo.kind({
		name: 'components.Checkbox',
		published: {
			checked: false,
		},
		classes: 'checkbox',
		bindings: [
			{ from: '.checked', to: '.$.tick.showing' }
		],
		components: [
			{ name: 'tick', kind: enyo.Image, classes: 'tick', src: util.asset('checkbox.png') }
		],
		toggle: function() {
			this.set('checked', !this.get('checked'));
		}
	});

	enyo.kind({
		name: 'components.RadioButton',
		published: {
			checked: false,
			value: undefined,
		},
		classes: 'radio-button',
		bindings: [
			{ from: '.checked', to: '.$.tick.showing' }
		],
		components: [
			{ name: 'tick', classes: 'tick' }
		],
		toggle: function() {
			this.set('checked', !this.get('checked'));
		}
	});

	enyo.kind({
		name: 'components.LabeledCheckbox',
		classes: 'parental-checkbox',
		spotlight: true,
		handlers: {
			onSpotlightSelect: 'selectCheck',
		},
		events: {
			onCheckBoxSelect: ''
		},
		bindings: [
			{ from: '.content', to: '.$.text.content' },
			{ from: '.checked', to: '.$.check.checked' },
		],
		components: [
			{ name: 'check', kind: 'components.Checkbox' },
			{ name: 'text' }
		],
		selectCheck: function() {
			this.$.check.toggle();
			this.doCheckBoxSelect({check: this.$.check.getChecked()});
		}
	});

	enyo.kind({
		name: 'components.LabeledRadioButton',
		classes: 'parental-checkbox',
		spotlight: true,
		handlers: {
			onSpotlightSelect: 'selectCheck',
		},
		events: {
			onCheckBoxSelect: ''
		},
		bindings: [
			{ from: '.content', to: '.$.text.content' },
			{ from: '.checked', to: '.$.rb.checked' },
			{ from: '.value', to: '.$.rb.value' },
		],
		components: [
			{ name: 'rb', kind: 'components.RadioButton' },
			{ name: 'text' }
		],
		selectCheck: function() {
			this.$.rb.toggle();
			this.doCheckBoxSelect({value: this.$.rb.getValue()});
		}
	});

})();
