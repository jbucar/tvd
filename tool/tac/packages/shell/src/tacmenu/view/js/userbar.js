
(function() {

enyo.kind({
	name: 'StatusIcon',
	classes: 'status-icon',
	mixins: [components.FadeSupport],
	published: {
		label: '',
		color: '',
	},
	fadeDuration: 1500,
	bindings: [
		{ from: '.label', to: '.$.label.content' },
		{ from: '.icon', to: '.$.icon.src', transform: util.asset }
	],
	components: [
		{
			name: 'iconWrapper',
			classes: 'icon-wrapper',
			components: [
				{ name: 'icon', classes: 'icon', kind: 'enyo.Image' }
			]
		},
		{ name: 'label' },
	],
	create: function() {
		this.inherited(arguments);
		this.colorChanged();
	},
	colorChanged: function() {
		this.$.iconWrapper.applyStyle('background-color', this.color);
	}
});

var zeroFill = util.zeroFill;

enyo.kind({
	name: 'UserBar',
	spotlight: false,
	classes: 'userbar-mainview',
	bindings: [
		{ from: '^session.isOpen', to: '.$.sessionIcon.showing' },
		{ from: '^Connectivity.state', to: '.$.noConnectivityIcon.showing', transform: util.negate },
	],
	components: [
		{
			classes: 'logo',
			components: [
				{
					name: 'logo',
					kind: 'enyo.Image',
					src: assets('logo-tac-tv.png')
				},
			]
		},
		{
			kind: 'IconsBar',
			classes: 'icon-bar',
			defaultKind: 'StatusIcon',
			components: [
				{ name: 'sessionIcon', label: 'SESIÓN INICIADA', icon: 'notification_unlocked.png', color: '#d5a700', showing: false },
				{ name: 'noConnectivityIcon', kind: enyo.Image, src: util.asset('no-connectivity-icon.png'), showing: false },
				// { name: 'notificationIcon', label: 'NOTIFICACIONES', icon: 'no_notification_icon.png', color: '#009eb1' },
				{ name: 'date', icon: 'infobox-calendario.png', classes: 'date' },
				{ name: 'time', icon: 'infobox-reloj.png', classes: 'time' },
			]
		},
		{ kind: 'enyo.Signals', onlocalechange: 'updateDate' }
	],
	create: enyo.inherit(function(sup) {
		return function() {
			sup.apply(this, arguments);
			this.refreshJob();
		}
	}),
	updateDate: function() {
		var d = new Date(Date.now());
		this.$.date.setLabel(util.formatDate(d, '.'));
		this.$.time.setLabel('{0}:{1}'.format(zeroFill(d.getHours()), zeroFill(d.getMinutes())));
	},
	refreshJob: function() {
		this.updateDate();
		this.startJob('refresh', this.bindSafely('refreshJob'), 60000);
	},
});

})();
