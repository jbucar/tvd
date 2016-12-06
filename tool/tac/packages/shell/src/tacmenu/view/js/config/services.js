
(function() {

	var helpSpec = { txt: msg.help.config.services.system, img: 'submenu.png' };

	enyo.kind({
		name: 'ServicesView',
		kind: View,
		help: helpSpec,
		tabBindings: {
			system: 'content'
		},
		menuOptions: { classes: 'config-submenu view-in-view-menu' },
		handlers: {
			onSpotlightContainerLeave: '_leaved',
			onSpotlightSelect: '_selected'
		},
		menuComponents: [
			{
				name: 'system',
				kind: 'ConfigTabBtn',
				label: 'SISTEMA',
				icon: 'menu-sistema.png',
				onSpotlightFocused: '_update',
				onSpotlightDown: '_checkEnter'
			},
		],
		components: [
			{
				name: 'content',
				kind: 'CarouselDetail',
				collection: Services,
				carouselItemView: 'ServiceItem',
				detailItemView: 'ServicesInfo',
				help: {
					carousel: helpSpec,
					details: { txt: msg.help.config.services.details, img: 'submenu.png' }
				}
			}
		],
		_selected: function(sender, evt) {
			var orig = evt.originator;
			if (orig.isDescendantOf(this.$.content) && orig.kind === 'ServiceItem') {
				this.$.content.showDetail();
				return true;
			}
		},
		_update: function() {
			if (Services.length === 0) {
				Services.fetch();
			}
		},
		_leaved: function(sender, evt) {
			if (evt.originator === this) {
				Services.empty();
			}
		},
		_checkEnter: function() {
			return (Services.length === 0);
		}
	});

	enyo.kind({
		name: 'ServiceItem',
		kind: 'CarouselItem',
		classes: 'carousel-service-item',
		running: undefined,
		iconBar: [
			{ name: 'statusIcon', classes: 'services-status-icon' },
		],
		handlers: {
			onSpotlightFocus: 'onFocus',
			onSpotlightBlur: 'onBlur'
		},
		bindings: [
			{ from: '.model.name', to: '.$.txt.content' },
			{
				from: '.model.icon', to: '.$.img.src',
				transform: function(icon) {
					return icon ? icon : assets('servicios-tac.png');
				}
			},
			{ from: '.model.running', to: 'running' }
		],
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.txt.set('showing', true);
				this.$.txt.addClass('service-txt');
			}
		}),
		onFocus: function(oSender, oEvent) {
			this.bubble('onRequestScroll', {dir: oEvent.direction || oEvent.dir});
			this.$.txt.addClass( 'selected ' + Context.current() );
			this.$.txt.startMarquee();
		},
		onBlur: function(oSender, oEvent) {
			this.$.txt.stopMarquee();
			this.$.txt.removeClass( 'selected ' + Context.current() );
		},
		runningChanged: function(oSender, oEvent) {
			if ( this.running ) {
				this.$.statusIcon.removeClass('stoped');
				this.$.statusIcon.addClass('running');
			} else {
				this.$.statusIcon.removeClass('running');
				this.$.statusIcon.addClass('stoped');
			}
		}
	});

})();
