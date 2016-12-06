/**
 * Volume kind.
 */
enyo.kind({
	name: 'ClosedCaption',
	events: {
		onVanish: ''
	},
	components: [
		{
			name: 'cc-background',
			classes: 'cc-background',
			components: [
				{ content: 'SUBTÍTULO:', classes: 'cc-title-text'},
				{ name: 'ccContent', classes: 'cc-text' }
			]
		}
	],
	show: function() {
		// Shows cc widget by 3 seconds.
		this.inherited(arguments);
		this.startJob('hideCCJob', this.bindSafely('hideCC'), 3000);
	},
	hideCC: function() {
		// Hides cc widget.
		this.hide();
		this.doVanish();  // sends event onVanish.
	},
	setCC: function(subID) {
		var id = subID + 1;
		this.$.ccContent.set('content', id === 0 ? 'DESACTIVADO' : 'CLOSED CAPTION ' + id);
	}
});