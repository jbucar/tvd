var keyboard = (function(scope) {

	var showVK = util.promisify(appmgr.showVirtualKeyboard);

	enyo.kind({
		name: 'Keyboard',
		mixins: [enyo.BindingSupport],
		// attr
		showing: false,
		show: function (opts) {
			var ctx = Context.current();
			scope.keyboard.set('showing', true);
			return showVK({
				title: opts.title,
				placeholder: opts.placeholder,
				type: opts.type || 'password',
				styles: {
					'actionButtons': '{0}-keyboard-action'.format(ctx),
					'buttons': '{0}-keyboard-btn'.format(ctx),
					'shiftImage': 'shift_{0}.png'.format(ctx),
					'bar': '{0}-btn-bar-color'.format(ctx),
				}
			}).then(function(data) {
				scope.keyboard.set('showing', false); // Not showing anymore
				return data;
			});
		}
	});

	var inst = new Keyboard();

	return inst;

})(this);
