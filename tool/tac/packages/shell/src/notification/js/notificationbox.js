/*
 * NotificationBox kind.
 */
enyo.kind({
	name: 'NotificationBox',
	classes: 'notification-box',
	published: {
		anim: true,
		position: undefined,
		sizeFixed: true
	},
	events: {
		onVanish: ''
	},
	bindings: [
		{ from: '.content', to: '.$.txt.content' },
		{ from: '.icon', to: '.$.icon.src' }
	],
	statics: {
		icons: {
			search: assets('notification_search.png'),
			check: assets('notification_check.png'),
			downloaded: assets('notification_downloaded.png'),
			error: assets('notification_error.png'),
			loading: assets('notification_loading.gif'),
			channels: assets('notification_channels.png'),
			favorite: assets('notification_favorite.png'),
			unfavorite: assets('notification_unfavorite.png'),
			locked: assets('block.png'),
			unlocked: assets('notification_unlocked.png'),
			parental: assets('submenu_restricciones.png'),
			ginga: assets('notification_ginga.png'),
			usb: assets('notification_usb.png'),
			geolocal: assets('geo-gde.png')
		}
	},
	components: [
		{ name: 'icon', classes: 'notification-box-image', kind: 'enyo.Image' },
		{ name: 'txt', classes: 'notification-box-txt', allowHtml: true },
		{ kind: "Animator", duration: 1500, onStep: 'animatorStep', onEnd: 'animationComplete', easingFunction: enyo.easing.linear },
	],
	animatorStep: function(oSender) {
		this.applyStyle('transform', 'translateX(' + oSender.value + '%)');
	},
	animationComplete: function() {
		if (this.__destroyed) {
			// Actually destroy, really, no kidding, for good
			this.doVanish();
			enyo.Control.prototype.destroy.call(this);
		}
	},
	destroy: function(now) {
		if (now) {
			if (this.__destroyed) {
				this.$.animator.stop();
			}
			else {
				this.doVanish();
			}
			enyo.Control.prototype.destroy.call(this);
		}
		else if (!this.__destroyed) {
			this.__destroyed = true;
			this.$.animator.play({
				startValue: 0,
				endValue: 100,
				node: this.hasNode()
			});
		}
	},
	sizeFixedChanged: function() {
		this.addRemoveClass('size-fixed', this.sizeFixed);
	},
	positionChanged: function() {
		this.applyStyle('order', this.position);
	},
	rendered: function() {
		this.inherited(arguments);
		this.sizeFixedChanged();
		if (this.anim) {
			this.$.animator.play({
				startValue: 100,
				endValue: 0,
				node: this.hasNode()
			});
		}
		else {
			this.applyStyle('transform', 'translateX(0%)');
		}

		if (this.timeout) {
			var timeout = this.timeout > 1500 ? this.timeout : 1500;
			this.startJob('selfDestroy', this.bindSafely('destroy'), timeout);
		}
	}
});

enyo.kind({
	name: 'NotificationStack',
	classes: 'notification-stack',
	components: [],
	stack: function(def, ext, cbk) {
		var ctrols = this.getClientControls();
		var exactgroup = ctrols.some(function(c2) {
			return def.group === c2.group;
		});

		if (exactgroup && def.discardOnOverlap) { return false; };

		var c = this.createComponent(def, ext);
		cbk();

		if (c.group) {
			var replace = c.replace;
			var g = ctrols.filter(function(c2) {
				return c.group.match(c2.group) && c !== c2;
			});

			c.set('anim', replace ? g.length === 0 : c.get('anim'));
			c.set('position', (replace && g.length) ? g[0].get('position') : ctrols.length);

			c.render();

			g.forEach(function(ctrol) {
				if (c !== ctrol) {
					ctrol.destroy(replace);
				}
			});
		}
		else {
			c.render();
		}

		return true;
	}
});
