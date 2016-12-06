/**
 * Kind Help
 */
Shell.helpDialog = (function() {

	var colors = {};
	colors.home = 'rgb(31, 64, 112)';
	colors.ch = colors.tvlayer = 'rgb(241, 116, 0)';
	colors.imgs = 'rgb(0, 169, 227)';
	colors.apps = 'rgb(0, 145, 120)';
	colors.config = 'rgb(144, 38, 30)';

	enyo.kind({
		name: 'HelpDialog',
		kind: 'Dialog',
		classes: 'help-dialog',
		title: 'AYUDA',
		icon: 'help/ayuda.png',
		handler: {
			onSpotlightKeyDown: 'onSpotlightKeyDown'
		},
		actions: [
			{ name: 'accept', kind: 'DialogButton', classes: 'ok', action: 'accepted' },
		],
		create: function() {
			this.inherited(arguments);
			this.createComponent({ name: 'imagen', kind: 'enyo.Image', src: assets('default_detail.png') }, {owner: this});
			this.set('acceptLabel', 'CERRAR');
		},
		onSpotlightKeyDown: function(oSender, oEvent) {
			return (oEvent.keyCode !== tacKeyboardLayout.VK_OK);
		},
		show: function(customHelp) {
			if (Context.helpEnabled) {
				enyo.Spotlight.States.push('focus');
				if (!Context.empty()) {
					var ctx = Context.current();
					var help = customHelp ? customHelp : Context.help;
					var contextColor = colors[ctx];
					this.$.icon.applyStyle('background', contextColor);
					this.$.imagen.set('src', assets('help/'+ help.img));
					this.$.title.applyStyle('color', contextColor);
					this.$.accept.applyStyle('background', contextColor);
					if (help.txt) {
						this.set('description', help.txt);
					} else {
						log.warn('HelpDialog','updateHelp','Context help doesnt exist');
						this.set('description', msg.help.error );
					}
				}
				this.inherited(arguments);
			}
		}
	});

	var instance = new HelpDialog();
	return instance;

})();
