(function() {

	enyo.kind({
		name: 'viewerDetail',
		kind: 'components.Detail',
		published: {
			content: 'No disponible'
		},
		bindings: [
			{ from: '.content', to: '.info', transform: function(content){
				return content ? content : 'No disponible';
			}},
		]
	});

	/**
	 * shell.imgs.DetailsPanel
	 */
	enyo.kind({
		name: 'shell.imgs.DetailsPanel',
		orient: 'h',
		kind: 'enyo.Drawer',
		classes: 'detailpanel',
		open: false,
		bindings: [
			{ from: '.model.name', to: '.$.imageName.content' },
			{ from: '.model.date', to: '.$.imageDate.content' },
			{ from: 'modelSize', to: '.$.imageSize.content' },
			{ from: '.model.camera', to: '.camera', transform: function(cam) {
				return !cam ? {model: undefined, make: undefined} : cam;
			} },
			{ from: '.model.gps', to: '.gps' },
			{ from: '.camera.model', to: '.$.imageCamera.content' },
			{ from: '.camera.make', to: '.$.imageCameraMaker.content' },
			{ from: '.model.orientation', to: '.$.imageOrientation.content', transform: 'orientationDecoder' },
			{ from: '.model.exposureTime', to: '.$.imageExposure.content', transform: 'exposure' },
			{ from: '.model.exposureCompensation', to: '.$.imageExposureComp.content', transform: function(comp){
				return comp !== undefined ? comp + ' EV' : comp;
			} },
			{ from: '.model.apertureValue', to: '.$.imageAperture.content', transform: function(aperture){
				return aperture ? 'f/' + aperture : aperture;
			} },
			{ from: '.model.iso', to: '.$.imageISO.content'},
			{ from: '.model.flash', to: '.$.imageFlash.content', transform: function(flash){
				if (flash !== undefined) {
					var activated = flash & 0x1;
					return activated ? 'Activado' : 'Desactivado';
				} else return flash;
			} },
			{ from: '.gps', to: '.$.latitude.content', transform: function(gps) {
				return gps && gps.latitude ? gps.latitude + ' ' + gps.latitudeRef : 'No disponible';
			}},
			{ from: '.gps', to: '.$.longitude.content', transform: function(gps) {
				return gps && gps.longitude ? gps.longitude + ' ' + gps.longitudeRef : 'No disponible';
			}},
			{ from: '.model.width', to: '.width' },
			{ from: '.model.height', to: '.height' },

		],
		computed: [
			{ method: "modelSize", path: ["width", "height"] },
		],
		components: [
			{
				name: 'contentWrapper', classes: 'img-viewer-panel-content',
				components: [
					{ content: 'INFORMACIÓN DE IMAGEN', classes: 'detailpanel-title'},
					{ classes: 'detailpanel-separator'},
					{ name: 'imageName', kind: 'viewerDetail', title: 'NOMBRE DE ARCHIVO' },
					{ name: 'imageDate', kind: 'viewerDetail', title: 'FECHA' },
					{ name: 'imageSize', kind: 'viewerDetail', title: 'DIMENSIONES' },
					{ name: 'imageCamera', kind: 'viewerDetail', title: 'CÁMARA' },
					{ name: 'imageCameraMaker', kind: 'viewerDetail', title: 'MARCA DE LA CÁMARA' },
					{ name: 'imageExposure', kind: 'viewerDetail', title: 'EXPOSICIÓN' },
					{ name: 'imageExposureComp', kind: 'viewerDetail', title: 'COMPENSACIÓN' },
					{ name: 'imageAperture', kind: 'viewerDetail', title: 'APERTURA' },
					{ name: 'imageISO', kind: 'viewerDetail', title: 'ISO' },
					{ name: 'imageOrientation', kind: 'viewerDetail', title: 'ORIENTACIÓN' },
					{ name: 'imageFlash', kind: 'viewerDetail', title: 'FLASH' },
					{ name: 'latitude', kind: 'viewerDetail', title: 'LATITUD' },
					{ name: 'longitude', kind: 'viewerDetail', title: 'LONGITUD' },
				]
			}
		],
		modelSize: function() {
			var size = 'No disponible';
			if (this.width && this.height) {
				size = this.width + ' x ' + this.height;
			}
			return size;
		},
		exposure: function(exp) {
			if (exp) {
				var secs = exp >= 1 ? exp.toString() : '1/' + (1/exp).toString();
				return secs + 's';
			} else return exp;
		},
		modelChanged: function(was, is) {
			if (is && this.showing) {
				this.model.fetch();
			}
		},
		toggle: function(inSender, inEvent) {
			this.setOpen(!this.open);
			if (this.open && this.model) {
				this.model.fetch();
			}
		},
		orientationDecoder: function(value) {
			switch (value) {
				case '1': return 'Arriba-Izquierda';
				case '2': return 'Arriba-Derecha';
				case '3': return 'Abajo-Derecha';
				case '4': return 'Abajo-Izquierda';
				case '5': return 'Izquierda-Arriba';
				case '6': return 'Derecha-Arriba';
				case '7': return 'Derecha-Abajo';
				case '8': return 'Izquierda-Abajo';
				default: return 'No disponible';
			}
		}
	});

	/**
	 * imgs.impl.ImageHolder
	 */
	enyo.kind({
		name: 'imgs.impl.ImageHolder',
		mixins: [components.FadeSupport],
		fadeDuration: 150,
		classes: 'imgs-item-full',
		published: {
			src: null
		},
		handlers: {
			onload: '_loaded',
		},
		components: [
			{ name: 'loading', kind: components.Loading, style: 'justify-content: center', showing: false },
			{ name: 'layer', showing: false, classes: 'viewer-layer', components: [
				{ name: 'img', kind: 'enyo.Image', classes: 'viewer scale' }
			]}
		],
		_rotation: 0,
		srcChanged: function(was, is) {
			this.reset();

			if (is) {
				this.$.layer.hide();
				this.$.loading.show();
				this.$.img.setSrc(is);
			}
		},
		_loaded: function() {
			this.$.loading.hide();
			this.$.layer.show();
		},
		scale: function(rotate, bounds) {
			if (rotate) {
				this.$.img.removeClass('scale');
				this.$.img.addClass('none');
			} else  {
				this.$.img.addClass('scale');
				this.$.img.removeClass('none');
			}
		},
		rotate: function(clockwise) {
			this._rotation += 90 * (clockwise? 1 : -1);
			var abs = Math.abs;

			if (abs(this._rotation % 360) === 0) {
				this._rotation = 0;
			}
			var resizeWidth = abs(this._rotation) === 90 || abs(this._rotation) === 270;
			var bounds = this.getBounds();
			var width = resizeWidth ? bounds.height : bounds.width;
			var height = resizeWidth ? bounds.width : bounds.height;
			this.$.img.applyStyle('max-width', width + 'px');
			this.$.layer.applyStyle('transform', 'rotate(' + this._rotation + 'deg)');
			this.scale(resizeWidth);
		},
		reset: function() {
			this._rotation = 0;
			this.resetScaled();
			this.$.layer.applyStyle('transform', null);
			this.scale(false);
		},
		resetScaled: function() {
			if (Math.abs(this._rotation) === 0 || Math.abs(this._rotation) === 180) {
				this.$.img.applyStyle('max-width','100%');
			}
		}
	});

	/**
	 * shell.imgs.ControlBar
	 */
	enyo.kind({
		name: 'shell.imgs.ControlBar',
		open: false,
		classes: 'imgs-controlbar',
		bindings: [
			{ from: '.playing', to : '.$.play.enabled', transform: util.negate },
			{ from: '.model.name', to : '.$.name.content', transform: util.toUpperCase }
		],
		components: [
			{
				classes: 'wrapper',
				components: [
					{
						name: 'play', kind: 'components.StateImage',
						classes: 'imgs-controlbar-btn play', imageClasses: 'imgs-controlbar-icon',
						enabledSrc: assets('play.png'), disabledSrc: assets('pause.png')
					},
					{ classes: 'imgs-controlbar-separator' },
					{ name: 'name', classes: 'imgs-controlbar-btn name oswald-light' }
				]
			},
			{ name: 'animator', kind: 'Shell.Collapser' }
		],
		setOpen: function(open) {
			if (open === this.open) return;
			this.open = open;

			if (open) {
				this.startJob('autoHide', this.bindSafely('setOpen', false), 5000);
				this.$.animator.expand(this, 0, '9.7301%');
			}
			else {
				this.$.animator.collapse(this, '9.7301%', 0);
			}
		}
	});

	/**
	 * shell.imgs.ImageViewer
	 */
	enyo.kind({
		name: 'shell.imgs.ImageViewer',
		kind: 'enyo.Popup',
		mixins: [enyo.EventEmitter],
		centered: true,
		floating: true,
		modal: true,
		classes: 'img-viewer',
		spotlight: true,
		published: {
			autoplayDelay: 4000,
		},
		handlers: {
			onSpotlightKeyDown: '_onKey',
		},
		bindings: [
			{ from: '.model', to: '.$.details.model' },
			{ from: '.model', to: '.$.control.model' },
			{ from: '._autoplay', to: '.$.control.playing' }
		],
		components: [
			{ name: 'loading', kind: components.Loading, style: 'justify-content: center', showing: false },
			{
				name: 'imgWrapper', classes: 'imgs-wrapper',
				components: [
					{ name: 'img', kind: 'imgs.impl.ImageHolder' },
					{ name: 'control', kind: 'shell.imgs.ControlBar', open: false }
				]
			},
			{ name: 'details', kind: 'shell.imgs.DetailsPanel', classes: 'img-viewer-panel', showing: false }
		],
		// private
		_autoplay: false,
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);

				this.$.img.addObserver('showing', this.bindSafely('_changeSource'));
			};
		}),
		showingChanged: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				if (!this.showing) {
					this.set('_autoplay', false);
					this.$.control.setOpen(false);
					this.$.img.reset();
				}
			};
		}),
		open: function(collection, index, loader) {
			this.loader = loader;
			this.set('collection', collection);

			this.show();

			this.set('index', index);
			this.$.img.reset();
			enyo.Spotlight.spot(this);
			Context.setHelp({txt: msg.help.imgViewer, img: 'imageviewer.png'});
		},
		collectionChanged: function(was, is) {
			var idx = this.get('index');
			if (this.get('paging')) {
				if (this.$.loading.showing) {
					idx++;
				}
				this.set('paging', false);
				this.set('index', idx);
			}
			this.loader(idx, idx + 3);
		},
		pagingChanged: function(was, is) {
			if (!is) {
				this.$.imgWrapper.show();
				this.$.loading.hide();
			}
		},
		_changeSource: function() {
			var idx = this.get('index');
			var mdl = this.get('collection')[idx];
			if (this.$.img.showing) {
				this.$.img.set('src', mdl.get('url'));
			}
			else {
				this.$.img.show();
			}
		},
		_onKey: function(sender, evt) {
			var keyCode = evt.keyCode;
			var kbd = tacKeyboardLayout;
			var handled = false;
			var openControl = false;

			switch(keyCode) {
				case kbd.VK_EXIT:
				case kbd.VK_BACK:
					enyo.Spotlight.States.pop('images');
					this.$.details.setOpen(false);
					this.hide();
					this.set('index', undefined);
					handled = true;
					break;

				case kbd.VK_RIGHT:
					if (this.get('paging')) {
						this.$.imgWrapper.hide();
						this.$.loading.show();
					}
					else{
						var index = this.index < this.get('collection').length-1 ? this.index +1 : 0;
						if (index === 0) {
							openControl = true;
						}
						this.set('index', index);
					}
					handled = true;
					break;

				case kbd.VK_LEFT:
					var index = this.index > 0 ? this.index - 1 : this.get('collection').length-1;
					if (index + 1 !== this.index) {
						openControl = true;
					}
					this.set('index', index);
					handled = true;
					break;

				case kbd.VK_INFO:
					if (!this.$.details.showing && !this.get('paging')) {
						this.$.details.show();
					}

					this.$.details.toggle();
					this.$.img.resetScaled();
					handled = true;
					break;

				case kbd.VK_PLAY:
				case kbd.VK_PAUSE:
				case kbd.VK_ENTER:
					openControl = true;
					handled = true;
					// We set the autoplay below, to ensure autoplay stops when any other key was pressed
					break;

				case kbd.VK_UP:
				case kbd.VK_DOWN:
					this.$.img.rotate(keyCode === kbd.VK_DOWN);
					handled = true;
					break;

				case kbd.VK_HELP:
					Shell.helpDialog.show();
					break;

			}

			if (handled) {
				var play = keyCode === kbd.VK_ENTER? !this.get('_autoplay') : keyCode === kbd.VK_PLAY;
				this.$.control.setOpen(openControl);
				this.set('_autoplay', play);
			}

			return handled;
		},
		indexChanged: function(was, is) {
			if (is !== undefined) {
				this.$.img.hide();
			}
			var images = this.get('collection');
			this.set('model', images[is]);
			this.loader(is + 2, is + 3);
			if (is === (images.length - 1)) {
				this.set('paging', true);
				this.emit('paging', {});
			}
		},
		_autoplayChanged: function(was, is) {
			log.debug('shell.imgs.ImageViewer', 'autoplayChanged', '%s', this.get('_autoplay'));

			if (is) {
				this.startJob('autoPlay', this.bindSafely('runAutoplay'), this.autoplayDelay);
			}
			else {
				this.stopJob('autoPlay');
			}
		},
		runAutoplay: function() {
			if (this._autoplay) {
				log.debug('shell.imgs.ImageViewer', 'runAutoplay', 'restarting autoplay');
				this.startJob('autoPlay', this.bindSafely('runAutoplay'), this.autoplayDelay);
				if (this.index < this.get('collection').length-1) {
					this.set('index', this.index + 1);
				} else {
					this.set('index', 0);
				}
			}
		}
	});

})();
