
(function() {
	'use strict';

	enyo.kind({
		name: 'components.TitledInput',
		classes: 'flex vertical start',
		published: {
			type: 'text'
		},
		bindings: [
			{ from: 'title', to: '.$.title.content' },
			{ from: 'title', to: '.$.input.vkTitle' },
			{ from: 'type', to: '.$.input.type' },
			{ from: 'value', to: '.$.input.value', oneWay: false },
		],
		components: [
			{ name: 'title', classes: 'oswald-light', style: 'font-size: 24px;' },
			{ name: 'input', kind: 'components.Input', style: 'background: white;' }
		],
		clear: function() {
			return this.$.input.clear.apply(this.$.input, arguments);
		}
	});

	enyo.kind({
		name: 'components.Input',
		kind: 'moon.InputDecorator',
		style: 'padding: 3px 15px !important; margin: 5px 0px !important;',
		published: {
			type: 'text',
			vkTitle: 'Ingrese texto'
		},
		bindings: [
			{ from: 'placeholder', to: '.$.input.placeholder' },
			{ from: 'selectOnFocus', to: '.$.input.selectOnFocus' },
			{ from: 'type', to: '.$.input.type' },
			{ from: 'value', to: '.$.input.value', oneWay: false },
		],
		handlers: {
			onSpotlightFocus: 'onFocus',
			onSpotlightSelect: 'onSelect',
			onSpotlightKeyDown: 'onKey'
		},
		components: [
			{ name: 'input', kind: 'moon.Input', style: 'width: 96%; color: inherit' }
		],
		onFocus: function(sender, evt) {
			if (evt.originator === this) {
				this.$.input.hasNode().onkeydown = function(evt) {
					if (evt.which === tacKeyboardLayout.VK_ENTER) {
						this.onSelect();
					}
				}.bind(this);

				enyo.asyncMethod(this, function() {
					this.$.input.hasNode().focus();
					this.$.input.focused();
				});
			}
		},
		onSelect: function(sender, evt) {
			this.$.input.clear();
			var opts = { title: this.get('vkTitle'), type: this.get('type') };
			keyboard.show(opts).then(function(data) {
				if (data.status === 'accepted') {
					this.$.input.setValue(data.input);
				}
			}.bind(this));
		},
		onKey: function(sender, evt) {
			var keyCode = evt.keyCode;
			if (keyCode === tacKeyboardLayout.VK_BACK || keyCode === tacKeyboardLayout.VK_EXIT) {
				enyo.Spotlight.unfreeze();
				this.$.input.blur();
				this.$.input.clear();
			}
		},
		clear: function() {
			return this.$.input.clear.apply(this.$.input, arguments);
		}
	});

})();
