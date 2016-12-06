/**
 * Tooltip
 */
var Tooltip = (function() {

	enyo.kind({
		name: 'Shell.Tooltip',
		kind: enyo.Popup,
		classes: 'tooltip',

		floating: true,
		modal: true,
		autoHideOnCancel: true,

		text: '',
		customStyle: '',
		showing: false,

		components: [
			{
				name: 'texttip',
				classes: 'msg'
			}
		],
		bindings: [
			{ from: '.text', to: '.$.texttip.content' }
		],

		_lastContext: '',
		_defaultClass: 'tooltip',

		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.addClass( this.customStyle ? this.customStyle : '' );
			};
		}),
		show: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				// set new style
				var actualContext = Context.current();
				if ( actualContext && this._lastContext !== actualContext ) {
					this.removeClass(this.getClasses());
					this._lastContext = actualContext;
					this.addClass('{0} {1} {2}'.format( this._defaultClass, this.get('customStyle'), actualContext ));
				}
			};
		}),
		showTooltip: function() {
			// Start job to show the tooltip in 2 seconds.
			this.startJob('showTooltipJob', this.bindSafely('show'), 2000);
		},
		hideTooltip: function() {
			this.stopJob('showTooltipJob');
			this.hide();
		},
		customStyleChanged: function(oldValue, newValue) {
			this.removeClass(oldValue);
			this.addClass(newValue);
		}
	});

	var instance = new Shell.Tooltip();
	return instance;

})();