
(function() {

	enyo.kind({
		name: 'VirtualKeyboard',
		classes: 'vkeyboard',
		published: {
			btnStyle: '',
			actionBtnStyle: '',
			placeholder: '',
		},
		events: {
			onCancel:'',
			onAccept:''
		},
		bindings: [
			{ from: '.shiftImageOver', to: '.$.shiftImageOver.src', transform: assets },
			{ from: '.barClass', to: '.$.btnOk.barClass'},
			{ from: '.barClass', to: '.$.btnExit.barClass'},
			{ from: '.passwordMaskEnabled', to: '.$.input.passwordMaskEnabled'},
			{ from: '.placeholder', to: '.$.input.placeholder'},
		],
		handlers:{
			ontap: 'ontap',
			onSpotlightKeyUp: 'handleKey'
		},
		components:[
			{ kind: 'enyo.Signals', onkeypress: 'keypressed' },
			{ kind: 'KeyInput', name: 'input' },
			{ name: 'numbers', classes: 'vkeyboard-rows', components:[
				{ name: 'numbersFirt', kind: 'KeyButton', content: '1', defaultSpotlightLeft: 'numbersLast', defaultSpotlightUp: 'shift' },
				{ kind: 'KeyButton', content: '2', defaultSpotlightUp: 'shift' },
				{ name: 'numberThree', kind: 'KeyButton', content: '3', defaultSpotlightUp: 'btnSpecial' },
				{ kind: 'KeyButton', content: '4', defaultSpotlightUp: 'spacebar' },
				{ kind: 'KeyButton', content: '5', defaultSpotlightUp: 'spacebar' },
				{ kind: 'KeyButton', content: '6', defaultSpotlightUp: 'btnExit' },
				{ name: 'numberSeven', kind: 'KeyButton', content: '7', defaultSpotlightUp: 'btnExit' },
				{ kind: 'KeyButton', content: '8', defaultSpotlightUp: 'btnExit' },
				{ name: 'numberNine', kind: 'KeyButton', content: '9', defaultSpotlightUp: 'btnOk' },
				{ name: 'numbersLast', kind: 'KeyButton', content: '0', defaultSpotlightRight: 'numbersFirt', defaultSpotlightUp: 'btnOk' },
			]},

			{ name: 'AlphabeticRow1', classes: 'vkeyboard-rows', components:[
				{name: 'arFirst', kind: 'KeyButtonSpecial', content: 'q', special: '@', defaultSpotlightLeft: 'arLast' },
				{kind: 'KeyButtonSpecial', content: 'w', special: '#' },
				{kind: 'KeyButtonSpecial', content: 'e', special: '_' },
				{kind: 'KeyButtonSpecial', content: 'r', special: '%' },
				{kind: 'KeyButtonSpecial', content: 't', special: '(' },
				{kind: 'KeyButtonSpecial', content: 'y', special: ')' },
				{kind: 'KeyButtonSpecial', content: 'u', special: '&' },
				{kind: 'KeyButtonSpecial', content: 'i', special: '$' },
				{kind: 'KeyButtonSpecial', content: 'o', special: '+' },
				{name: 'arLast', kind: 'KeyButtonSpecial', content: 'p', special: ':', defaultSpotlightRight: 'arFirst' },
			]},
			{ name: 'AlphabeticRow2', classes: 'vkeyboard-rows', components:[
				{name: 'ar2First', kind: 'KeyButtonSpecial', content: 'a', special: ';', defaultSpotlightLeft: 'ar2Last' },
				{kind: 'KeyButtonSpecial', content: 's', special: '"' },
				{kind: 'KeyButtonSpecial', content: 'd', special: '¡' },
				{kind: 'KeyButtonSpecial', content: 'f', special: '!' },
				{kind: 'KeyButtonSpecial', content: 'g', special: '/' },
				{kind: 'KeyButtonSpecial', content: 'h', special: '¿' },
				{kind: 'KeyButtonSpecial', content: 'j', special: '?' },
				{kind: 'KeyButtonSpecial', content: 'k', special: '-' },
				{kind: 'KeyButtonSpecial', content: 'l', special: '.' },
				{name: 'ar2Last', kind: 'KeyButtonSpecial', content: '\'', special: '*', defaultSpotlightRight: 'ar2First' },
			]},
			{ name: 'AlphabeticRow3', classes: 'vkeyboard-rows', components:[
				{name: 'ar3First', kind: 'KeyButtonSpecial', content: 'z', special: '', defaultSpotlightLeft: 'ar3Last' },
				{kind: 'KeyButtonSpecial', content: 'x', special: '' },
				{kind: 'KeyButtonSpecial', content: 'c', special: '' },
				{kind: 'KeyButtonSpecial', content: 'v', special: '' },
				{kind: 'KeyButtonSpecial', content: 'b', special: '' },
				{kind: 'KeyButtonSpecial', content: 'n', special: '' },
				{kind: 'KeyButtonSpecial', content: 'm', special: '' },
				{kind: 'KeyButtonSpecial', content: ',', special: '' },
				{kind: 'KeyButtonSpecial', content: '.', special: '' },
				{name: 'ar3Last', kind: 'KeyButtonSpecial', content: '?', special: '', defaultSpotlightRight: 'ar3First' },
			]},
			{ name: 'AlphabeticRow4', classes: 'vkeyboard-rows', components:[
				{
					name: 'shift',
					kind: 'KeyButton', classes: 'vkeyboard-special',
					ontap: 'upChars',
					onSpotlightFocus: 'showFocusedShiftImage',
					onSpotlightBlur: 'showNotFocusedShiftImage',
					components: [
						{ name: 'shiftImage', kind: 'enyo.Image', src: assets('shift.png') },
						{ name: 'shiftImageOver', kind: 'enyo.Image', showing: false }
					],
					defaultSpotlightLeft: 'ar4Last',
					defaultSpotlightDown: 'numbers'
				},
				{ name: 'btnSpecial', kind: 'KeyButton', classes: 'vkeyboard-special', content: '#$%', ontap: 'changeToSpecialSimbol', defaultSpotlightDown: 'numberThree' },
				{ name: 'spacebar', kind: 'KeyButton', classes: 'vkeyboard-special space', content: 'Espacio', ontap: 'space' },
				{ name: 'ar4Last', kind: 'KeyButton', classes: 'vkeyboard-special delete', content: 'Borrar', ontap: 'delete', defaultSpotlightRight: 'shift'  },
			]},
			{ name: 'AlphabeticRow5', classes: 'vkeyboard-rows-reverse', components:[
				{ name: 'btnOk', kind: 'ActionButton', content: 'ACEPTAR', ontap: 'ok', defaultSpotlightRight: 'btnExit', defaultSpotlightDown: 'numberNine' },
				{ name: 'btnExit', kind: 'ActionButton', content: 'CANCELAR', ontap: 'cancel', defaultSpotlightLeft: 'btnOk', defaultSpotlightDown: 'numberSeven' },
			]},
		],

		//* private */
		_lowerCase:  true,
		_specialCharsOn: false,
		_allowedKeys: {},
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this,arguments);

				this.$.btnOk.addClass(this.actionBtnStyle);
				this.$.btnExit.addClass(this.actionBtnStyle);
				this.applyStyleToBtns(this.$.numbers, this.btnStyle);
				this.applyStyleToBtns(this.$.AlphabeticRow1, this.btnStyle);
				this.applyStyleToBtns(this.$.AlphabeticRow2, this.btnStyle);
				this.applyStyleToBtns(this.$.AlphabeticRow3, this.btnStyle);
				this.applyStyleToBtns(this.$.AlphabeticRow4, this.btnStyle);

				var nothing = util.nop;
				var del = this.bindSafely('delete');
				var cancel = this.bindSafely('cancel');
				this._allowedKeys[tacKeyboardLayout.VK_LEFT] = nothing;
				this._allowedKeys[tacKeyboardLayout.VK_UP] = nothing;
				this._allowedKeys[tacKeyboardLayout.VK_RIGHT] = nothing;
				this._allowedKeys[tacKeyboardLayout.VK_DOWN] = nothing;
				this._allowedKeys[tacKeyboardLayout.VK_ENTER] = nothing;
				this._allowedKeys[tacKeyboardLayout.VK_EXIT] = cancel;
				this._allowedKeys[tacKeyboardLayout.VK_BACK] = cancel;;
				this._allowedKeys[tacKeyboardLayout.VK_BACKSPACE] = del;
			}
		}),
		btnStyleChanged: function( oldStyle, newStyle ) {
			this.changeStyleToBtns(this.$.numbers, oldStyle, newStyle);
			this.changeStyleToBtns(this.$.AlphabeticRow1, oldStyle, newStyle);
			this.changeStyleToBtns(this.$.AlphabeticRow2, oldStyle, newStyle);
			this.changeStyleToBtns(this.$.AlphabeticRow3, oldStyle, newStyle);
			this.changeStyleToBtns(this.$.AlphabeticRow4, oldStyle, newStyle);
		},
		actionBtnStyleChanged: function( oldStyle, newStyle ) {
			this.changeStyleToBtns(this.$.AlphabeticRow5, oldStyle, newStyle);
		},
		handleKey: function(oSender, oEvent) {
			var key = oEvent.keyCode;
			if (this._allowedKeys[key]) {
				return this._allowedKeys[key]();
			}
			else {
				return true;
			}
		},
		keypressed: function(sender, evt) {
			if (enyo.Spotlight.getCurrent().isDescendantOf(this) && (evt.keyCode !== tacKeyboardLayout.VK_ENTER)) {
				this.$.input.putChar(String.fromCharCode(evt.keyCode));
				return true;
			}
		},
		barClassChanged:  function( oldStyle, newStyle ) {
			this.$.btnOk.removeClass(oldStyle);
			this.$.btnExit.removeClass(oldStyle);

			this.$.btnOk.addClass(newStyle);
			this.$.btnExit.addClass(newStyle);
		},
		shiftImageOverChanged: function(oldValue, newValue) {
			this.$.shiftImageOver.src = newValue;
		},
		applyStyleToBtns: function(l, aStyle) {
			var list = l.getClientControls();
			for( i=0; i< list.length; i++) {
				list[i].addClass(aStyle);
			}
		},
		changeStyleToBtns: function(l, oldStyle, newStyle) {
			var list = l.getClientControls();
			for( i=0; i< list.length; i++) {
				list[i].removeClass(oldStyle);
				list[i].addClass(newStyle);
			}
		},
		ontap: function(anSender, anEvent) {
			this.$.input.putChar( anEvent.originator.getContent() );
		},
		delete: function() {
			this.$.input.delChar();
			return true;
		},
		space: function() {
			this.$.input.putChar(' ');
			return true;
		},
		upChars: function() {
			var l = this.$.AlphabeticRow1.children.concat(this.$.AlphabeticRow2.children, this.$.AlphabeticRow3.children);
			for(i=0; i<l.length; i++) {
				l[i].setCase(!l[i].isLower);
			}
			this._lowerCase = l[1].isLower;
			return true;
		},
		ok: function(oSender, oEvent) {
			log.trace('VirtualKeyboard', 'ok', 'val=%s', this.$.input.getValue());
			this.doAccept({value: this.$.input.getValue()});
			this.$.input.clear();
			return true;
		},
		cancel: function(oSender, oEvent) {
			log.trace('VirtualKeyboard', 'cancel', 'val=%s', this.$.input.getValue());
			this.doCancel({});
			return true;
		},
		changeToSpecialSimbol: function() {
			this._specialCharsOn = !this._specialCharsOn;
			if (this._specialCharsOn) {
				this.$.btnSpecial.setContent('Abc');
			}
			else {
				this.$.btnSpecial.setContent('#$%');
			}
			var l = this.$.AlphabeticRow1.children.concat(this.$.AlphabeticRow2.children, this.$.AlphabeticRow3.children);
			for(i=0; i<l.length; i++) {
				l[i].toggleChar();
				l[i].setCase(true);
			}
			this._lowerCase = true;
			return true;
		},
		showingChanged: enyo.inherit(function(sup) {
			 return function(was, is) {
				log.trace('VirtualKeyboard', 'showingChanged', 'val=%s', is);
				if (!is) {
					this.reset();
				}
				sup.apply(this, arguments);
			}
		}),
		reset: function() {
			this.$.input.clear();
			var l = this.$.AlphabeticRow1.children.concat(this.$.AlphabeticRow2.children, this.$.AlphabeticRow3.children);
			if (this._specialCharsOn) {
				this._specialCharsOn = false;
				this.$.btnSpecial.setContent('#$%');
				for(i=0; i<l.length; i++) {
					l[i].toggleChar();
				}
			}

			if (!this._lowerCase) {
				this._lowerCase = true;
				for(i=0; i<l.length; i++) {
					l[i].setCase(this._lowerCase);
				}
			}
			this.showNotFocusedShiftImage();
		},
		showFocusedShiftImage: function() {
			this.$.shiftImage.hide();
			this.$.shiftImageOver.show();
		},
		showNotFocusedShiftImage: function() {
			this.$.shiftImageOver.hide();
			this.$.shiftImage.show();
		}
	});

	//* KeyButton */
	enyo.kind({
		name: 'KeyButton',
		classes: 'vkeyboard-button',
		spotlight: true,
		isLower: true,
		setCase: function(aBoolean) {
			this.isLower = aBoolean;
		},
		isLower: function() {
			return this.isLower;
		},
	})

	//* KeyButtonSpecial */
	enyo.kind({
		name: 'KeyButtonSpecial',
		kind: 'KeyButton',
		special: '',

		//* private */
		_original:'',
		_special: true,

		create: function() {
			this.inherited(arguments);
			this._original = this.content;
		},
		toggleChar: function() {
			this.setContent( (this._special ? this.special : this._original) );
			this._special = !this._special;
		},
		resetChar: function() {
			this.setContent( this._original );
			this._special = true;
		},
		setCase: function(aBoolean) {
			this.isLower = aBoolean;
			if (this.isLower) {
				this.setContent( this.content.toLowerCase() );
			}	else {
				this.setContent( this.content.toUpperCase() );
			}
			this.render();
		},
	});

	//* KeyInput */
	enyo.kind({
		name: 'KeyInput',
		kind: 'enyo.Input',
		placeholder: 'Por favor, introduzca la contraseña',
		content: '',
		password: '',
		mask: '',
		published: {
			passwordMaskEnabled: true,
		},
		create: function() {
			this.inherited(arguments);
			this.removeClass( this.getClasses() );
			this.addClass('vkeyboard-input');
		},
		putChar: function(value) {
			if (this.get('passwordMaskEnabled')) {
				this.stopJob('passwordMask');
				this.maskOn();
				this.updateValue(value);
				this.startJob('passwordMask', this.bindSafely('maskOn'), 1000);
			} else {
				this.updateValue(value);
			}
		},
		delChar: function() {
			this.password = this.password.slice(0, this.password.length-1);
			this.mask = this.mask.slice(0, this.mask.length-1);
			this.set('value',this.mask);
		},
		clear: function() {
			this.password = '';
			this.mask = this.password;
			this.set('value', this.password);
		},
		maskOn: function() {
			if (this.password.length > 0) {
				this.mask = this.mask.slice(0, this.mask.length-1) + '*';
			}
			this.setValue(this.mask);
		},
		updateValue: function(value) {
			this.password += value;
			this.mask += value;
			this.set('value', this.mask);
		},
		getValue: function() {
			return this.get('passwordMaskEnabled') ? this.password : this.mask;
		}
	});

	/* ActionButton */
	enyo.kind({
		name: 'ActionButton',
		classes: 'vkeyboard-action-btn',
		spotlight: true
	});

	enyo.kind({
		name: 'VirtualKeyboardPopup',
		kind: 'enyo.Popup',
		classes: 'virtual-keyboard-popup',
		spotlight: 'container',
		floating: true,
		modal: true,
		components: [
			{
				name: 'wrapper', classes: 'wrapper',
				components: [
					{ name: 'description', classes: 'description' },
					{ name: 'vk', kind: 'VirtualKeyboard' }
				]
			}
		],
		show: enyo.inherit(function(sup) {
			return function(opts) {

				if (opts.defineStyles) {
					this._sheet = document.createElement('style');
					this._sheet.innerHTML = opts.defineStyles;
					document.body.appendChild(this._sheet);
				}

				this.$.vk.set('actionBtnStyle', opts.styles.actionButtons);
				this.$.vk.set('btnStyle', opts.styles.buttons);
				this.$.vk.set('shiftImageOver', opts.styles.shiftImage);
				this.$.vk.set('barClass', opts.styles.bar);

				this.$.vk.setPlaceholder(opts.placeholder || 'Ingrese el texto');
				this.$.vk.set('passwordMaskEnabled', opts.type === 'password');

				this.$.description.setContent(opts.title || 'TAC Virtual Keyboard');

				sup.apply(this, arguments);
			}
		}),
		showingChanged: enyo.inherit(function(sup) {
			 return function(was, is) {
				log.trace('VirtualKeyboard', 'showingChanged', 'val=%s', is);
				if (!is) {
					if (this._sheet) {
						this._sheet.parentNode.removeChild(this._sheet);
						this._sheet = undefined;
					}
					this.$.vk.reset();
				}
				sup.apply(this, arguments);
			}
		}),
	});

})();
