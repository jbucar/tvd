
(function() {

	function tooltipMesg(providers) {
		return providers > 0? msg.tooltip.config : msg.tooltip.images.emptySection;
	}

	/**
	 * Images
	 */
	enyo.kind({
		name: 'Images',
		kind: View,
		help: { txt: msg.help.menu.img, img: 'menu.png' },
		classes: 'imgs-view',
		model: imgsProviders,
		black: true,
		context: 'imgs',
		menuOptions: { classes: 'images-menu' },
		tabBindings: {
			imgs: 'providers',
			admin: 'none',
			back: 'none',
		},
		handlers: {
			onSpotLost: '_spotLosted'
		},
		bindings: [
			{ from: 'model.length', to: '.$.imgs.tooltipMsg', transform: tooltipMesg },
			{ from: 'model.length', to: '.$.nocontent.showing', transform: function(len) { return len === 0; } },
			{ from: 'model.length', to: '.$.providersView.showing', transform: function(len) { return len > 0; } },
			{ from: 'model', to: '.$.providersView.collection' },
		],
		menuComponents: [
			{
				name: 'imgs',
				kind: 'ImagesTabBtn',
				label: 'IMÁGENES',
				icon: 'imgn-blanco.png',
				tooltipEnabled: true,
			},
			/*{
				name: 'admin',
				kind: 'ImagesTabBtn',
				label: 'ADMINISTRADOR',
				icon: 'apps.png'
			},*/
			{
				name: 'back',
				kind: 'ImagesTabBtn',
				label: 'MENÚ',
				icon: 'menu_menu.png',
				tooltipEnabled: true,
				tooltipStyle: 'menu tab',
				tooltipMsg: msg.tooltip.menu,
				onSpotlightSelect: 'viewClose',
				onSpotlightDown: 'viewClose',
			},
		],
		components: [
			{
				name: 'providers',
				style: 'width: 100%; height: 100%; min-height: 100%;',
				wrapView: true,
				components: [
					{ name: 'providersView', kind: 'shell.imgs.Providers', classes: 'providers-view' },
					{
						name: 'nocontent',
						style: 'width: 100%; height: 100%; justify-content: center',
						classes: 'flex vertical center',
						components: [
							{ kind: 'enyo.Image', src: util.asset('img-no-available.png') },
							{ content: 'No hay imágenes disponibles.', classes: 'no-content oswald-light' }
						]
					}
				]
			}
		],
		_spotLosted: function() {
			return enyo.Spotlight.spot(this.$.menu);
		},
		onSelfEnter: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);
				this.$.providersView.adjust();
			}
		})
	});

	var Viewer = new shell.imgs.ImageViewer({showing: false});

	enyo.kind({
		name: 'shell.imgs.Providers',
		kind: View,
		context: 'imgs',
		help: { txt: msg.help.submenu.img, img: 'menu.png' },
		mixins: [MenuCollectionSupport],
		menuOptions: { classes: 'images-submenu view-in-view-menu' },
		menuKindOption: 'ImagesTabBtnProvider',
		contentOptions: { kind: 'shell.imgs.Provider' },
		handlers: {
			onSpotLost: '_spotLosted'
		},
		_spotLosted: function() {
			return enyo.Spotlight.spot(this.$.menu);
		}
	});

	/**
	 * shell.imgs.Item
	 */
	enyo.kind({
		name: 'shell.imgs.Item',
		classes: 'imgs-item',
		spotlight: true,
		published: {
			enabled: false
		},
		bindings: [
			{ from: 'model.url', to: '.url' }
		],
		handlers: {
			onSpotlightFocus: '_focused',
			onload: '_imgLoaded'
		},
		components: [
			{ name: 'img', kind: 'enyo.Image', imageSizing: 'cover', classes: 'img', showing: false }
		],
		_focused: function (sender, evt) {
			if (evt.originator === this) {
				this.bubble('onRequestScrollIntoView', {dir: evt.direction || evt.dir});
			}
		},
		_imgLoaded: function() {
			this.$.img.show();
		},
		urlChanged: function(was, is) {
			if (is && this.get('enabled')) {
				this._load();
			}
		},
		enabledChanged: function(was, is) {
			this.$.img.set('src', is? this.get('url') : null);
		},
		_load: function() {
			// TODO: Use canvas for scale off screen
			var image = new Image();
			image.onload = function() {
				if (this.$.img) {
					this.$.img.set('src', is);
				}
				image = null;
			}.bind(this);
			image.src = is;
		},
	});

	function getThumbnail(value) {
		return (value ? value : assets('imgn-default.png'));
	}

	/**
	 * shell.imgs.Provider
	 */
	enyo.kind({
		name: 'shell.imgs.Provider',
		kind: 'components.ViewContent',
		handlers: {
			onSpotlightFocus: '_focused',
			onNavigate: '_navigated',
			onSpotLost: '_spotLosted'
		},
		bindings: [
			{ from: '.model', to: '.$.grid.model' },
			{ from: '.model.logo', to: '.$.imgPreview.src', transform: getThumbnail },
			{ from: '.model.logo', to: '.$.thumbnail-img.src', transform: getThumbnail }
		],
		components: [
			{
				name: 'preview',
				classes: 'imgs-provider-preview',
				components: [
					{
						name: 'imgPreview',
						kind: enyo.Image,
						classes: 'imgpreview',
						onSpotlightFocused: '_prevFocused',
						spotlight: true
					},
					{ name: 'txtLabel', classes: 'txtlabel' }
				]
			},
			{
				name: 'sidebar',
				kind: 'components.flexLayout.Vertical',
				classes: 'viewcomponent-layout-left img-sidebar',
				style: 'max-width: 30%;',
				showing: false,
				components: [
					{
						name: 'thumbnail',
						classes: 'imgs-thumbnail flex horizontal center',
						components: [
							{
								name: 'thumbnail-img',
								style: 'width: 54.16%; height: 68.42%; object-fit: contain',
								kind: 'enyo.Image',
							}
						]
					},
					{ name: 'title', kind: 'moon.MarqueeText', mixins: ['moon.MarqueeSupport'], classes: 'imgs-sidebartxt title' },
					{ name: 'description', kind: 'moon.MarqueeText', mixins: ['moon.MarqueeSupport'], classes: 'imgs-sidebartxt' }
				]
			},
			{ name: 'grid', kind: 'shell.imgs.ProviderGrid', showing: false }
		],
		//private
		_providerLogo: undefined,

		onContentEnter: enyo.inherit(function(sup) {
			return function(sender, evt) {
				sup.apply(this, arguments);
				Context.setHelp({txt: msg.help.grid.img, img: 'grid-images.png'});
				this.$.title.startMarquee();
			};
		}),
		onContentLeave: enyo.inherit(function(sup) {
			return function(sender, evt) {
				sup.apply(this, arguments);
				if (evt.originator === this) {
					this._enableView(false);
					this.$.title.stopMarquee();
					this.$.description.stopMarquee();
				}
			};
		}),
		_spotLosted: function(sender, evt) {
			return enyo.Spotlight.spot(this.$.grid.$.loader);
		},
		_prevFocused: function() {
			this.$.title.setContent(this.get('model').get('name').toUpperCase());
			this._enableView(true);
			return true;
		},
		_focused: function(sender, evt) {
			var m = evt.originator.get('model');
			if (m) {
				this.$.description.stopMarquee();
				this.$.description.setContent(m.get('name'));
				this.$.description.startMarquee();
			}
		},
		_navigated: function(sender, evt) {
			if (!this._providerLogo) {
				this._providerLogo = this.get('model').get('logo');
			}

			var cur = evt.current;
			var src = cur? cur.get('url') : getThumbnail(this.get('model').get('logo'));
			this.$['thumbnail-img'].setSrc(src);
			this.$.title.stopMarquee();
			this.$.description.stopMarquee();
			this.$.description.setContent('');
			this.$.title.setContent(cur? cur.get('name').toUpperCase() : this.get('model').get('name').toUpperCase());
			return true;
		},
		_enableView: function(enabled) {
			this.$.sidebar.setShowing(enabled);
			this.$.imgPreview.set('spotlighDisabled', enabled);
			this.$.grid.setShowing(enabled);

			if(enabled) {
				enyo.Spotlight.spot(this.$.thumbnail);
				this.$.grid._load();
			}
			else {
				if (this._providerLogo) {
					this.$.imgPreview.set('src', this._providerLogo);
					this.$['thumbnail-img'].set('src', this._providerLogo);
					this._providerLogo = undefined;
				}
				this.$.grid.disable();
			}
			this.$.preview.setShowing(!enabled);
		}
	});

	var disable = function(c) { c.set('enabled', false); };
	var enable = function(c) { c.set('enabled', true); };

	/**
	 * shell.imgs.ImgsGrid
	 */
	enyo.kind({
		name: 'shell.imgs.ImgsGrid',
		style: 'width: 100%; height: 100%;',
		published: {
			collection: null,
			rows: 3,
			itemsPerRow: 7,
			childSize: 123,
		},
		handlers: {
			onScroll: '_scrolled',
			onSpotlightContainerEnter: '_clientEntered'
		},
		components: [
			{
				name: 'scroller',
				kind: 'ShellScroller',
				style: 'width: 100%; height: 100%;',
				vertical: 'scroll',
				horizontal: 'hidden',
				components: [
					{
						name: 'client',
						spotlight: 'container',
						memory: true,
						style: 'width: 100%; height: 100%; display: flex; flex-wrap: wrap;',
					}
				]
			}
		],
		_base: 0,
		_currentStep: 0,
		childForIndex: function(idx) {
			return this.$.client.getClientControls()[idx];
		},
		_clientEntered: function(sender, evt) {
			if (evt.originator.isDescendantOf(this.$.client)) {
				var ctrol = this.$.client.getClientControls()[this._currentStep * this.itemsPerRow];
				enyo.Spotlight.spot(ctrol);
			}
		},
		_scrolled: function(sender, evt) {
			var scrollBounds = evt.scrollBounds;
			var ctrols = this.$.client.getClientControls();

			// Calculate how much scroll we did
			var steps = Math.round(scrollBounds.top / this.childSize);
			var multiplier = Math.abs(this._currentStep - steps);
			this._currentStep = steps;

			if (scrollBounds.yDir < 0) {
				this._base -= this.itemsPerRow * multiplier;
			}

			var	firstLine = {
				start: this._base,
				end: this._base + this.itemsPerRow * multiplier
			};

			var lastLine = {
				start: this._base + this.rows * this.itemsPerRow,
				end: this._base + (this.rows + 1) * this.itemsPerRow * multiplier,
			};

			if (scrollBounds.yDir > 0) {
				this._base += this.itemsPerRow * multiplier;
				var nextLine = this._base + (this.itemsPerRow * this.rows);

				if (ctrols.length <= nextLine) {
					this.bubble('onPaging', {
						onSuccess: _.partial(this._preload, nextLine + this.itemsPerRow, this.itemsPerRow)
					});
				}
				else {
					this._preload(nextLine, this.itemsPerRow);
				}
			}

			var o = scrollBounds.yDir > 0 ? firstLine : lastLine;
			var out = ctrols.slice(o.start, o.end);
			out.forEach(disable);

			var i = scrollBounds.yDir > 0 ? lastLine : firstLine;
			var setin = ctrols.slice(i.start, i.end);
			setin.forEach(enable);
		},
		create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);

				this.onAdd = this.bindSafely('_addModls');
				this.onReset = this.bindSafely('_reset');

				this.collectionChanged(undefined, this.collection);
			};
		}),
		collectionChanged: function(was, is) {
			this._base = 0;
			this._currentStep = 0;
			if (was) {
				was.off('add', this.onAdd);
				was.off('reset', this.onReset);
			}

			if (is) {
				is.on('add', this.onAdd);
				is.on('reset', this.onReset);
			}
		},
		_addModls: function(sender, evtName, evt) {
			var inViewPort = this._inViewPort.bind(this);
			var prevItems = this.$.client.getClientControls();
			var items = this.$.client.createComponents(evt.models.map(function(m, i) {
				return { kind: 'shell.imgs.Item', model: m };
			}), {owner: this});

			items.forEach(function(c, i) {
				c.set('enabled', inViewPort(evt.index + i));
				if (i < items.length -1) {
					c.defaultSpotlightRight = items[i+1].name;
				}
				if (i > 0) {
					c.defaultSpotlightLeft = items[i-1].name;
				}
				c.render();
			});
			if (prevItems.length > 0) {
				prevItems[prevItems.length -1].defaultSpotlightRight = items[0].name;
				items[0].defaultSpotlightLeft = prevItems[prevItems.length -1].name;
			}

			var start = this._base + this.rows * this.itemsPerRow;
			this._preload(start, this.itemsPerRow);
		},
		_reset: function() {
			this.$.scroller.setScrollTop(0);
			this._base = 0;
			this._currentStep = 0;
			this.$.client.destroyClientControls();
		},
		_inViewPort: function(idx) {
			var end = this._base + this.rows * this.itemsPerRow;
			return this._base <= idx && idx < end;
		},
		stabilize: function() {
			var inViewPort = this._inViewPort.bind(this);
			this.$.client.getClientControls().forEach(function(ctrol, position) {
				ctrol.set('enabled', inViewPort(position));
			});
		}
	});

	function getImgs(collection) {
		return collection.filter(function(c) {
			return c.get('type') === 'image';
		});
	}

	var errorLbl = '<h1>Se ha producido un error desconocido</h1> Vuelva a intentar más tarde.';

	enyo.kind({
		name: 'shell.imgs.ProviderGrid',
		spotlight: 'container',
		classes: 'provider-grid',
		bindings: [
			{ from: 'model.imgs', to: 'images' },
			{ from: 'images', to: '$.resultList.collection' }
		],
		events: {
			onNavigate: '',
		},
		handlers: {
			onSpotlightSelect: '_selected',
			onSpotlightKeyDown: '_onKey',
			onPaging: '_paging',
			onSpotlightContainerLeave: '_leaved'
		},
		components: [
			{ name: 'loader', kind: components.Loading, classes: 'loader', spotlight: true, onSpotlightKeyDown: '_loaderKeyDown' },
			{ name: 'resultList', kind: 'shell.imgs.ImgsGrid' },
			{
				name: 'message',
				spotlight: true,
				showing: false,
				classes: 'loader flex vertical center',
				components: [
					{ name: 'icon', kind: enyo.Image, src: util.asset('warning-icon.png'), showing: false },
					{ name: 'label', allowHtml: true, classes: 'label' },
				]
			},
		],
		itemsPerRow: 7,
		rows: 3,
		_page: 0,
		_breadcrumb: [],
	    create: enyo.inherit(function(sup) {
			return function() {
				sup.apply(this, arguments);

				this.onSuccess = this.bindSafely('_loaded');
				this.onError = this.bindSafely('_error');
				this.onViewerClose = this.bindSafely('_viewerOpenClose');
				this.onPaging = this.bindSafely('_onViewerPaging');
			};
		}),
		imagesChanged: function(was, is) {
			if (is) {
				this._preload = ImageLoader(is);
				this.$.resultList._preload = this._preload;
			}
		},
		_paging: function(sender, evt) {
			this._page = this._page + 1;
			this._doFetch(this._page, evt.onSuccess);
		},
		_onViewerPaging: function() {
			this._page = this._page + 1;
			this._doFetch(this._page, function(sender, evt, models) {
				if (models.length) {
					Viewer.set('collection', getImgs(this.get('images')));
				}
				else {
					Viewer.set('paging', false);
				}
			}.bind(this));
		},
		disable: function() { //TODO Use this?? needed? showingChanged?
			this._page = 0;
			this._breadcrumb.length = 0; // Erase bradcrumb
			this.$.resultList.hide();

			this.$.icon.hide();
			this.$.message.hide();

			this.$.loader.show();
		},
		_selected: function(sender, evt) {
			var mdl = evt.originator.get('model');
			if (mdl) {
				if (mdl.isDirectory()) {
					this._breadcrumb.push(mdl);
					this.doNavigate({current: mdl});
					this._load();
					return true;
				}
				else if (mdl.get('back')) {
					this._navBack();
				}
				else {
					enyo.Spotlight.States.push('images', this.$.resultList);

					var imgs = getImgs(this.get('images'));

					Viewer.addObserver('showing', this.onViewerClose);
					Viewer.on('paging', this.onPaging);
					Viewer.open(imgs, imgs.indexOf(mdl), this._preload);
				}

				return true;
			}
		},
		fetch: function() {
			var imgs = this.get('images');
			if (imgs.get('status') === enyo.States.READY) {
				imgs.empty();
				imgs.clearError();

				if (this._breadcrumb.length) { // Add back model
					imgs.add({name: 'Atrás', back: true});
				}

				this._doFetch(0);
			}
		},
		_doFetch: function(page, cbk) {
			var imgs = this.get('images');
			var path = this._breadcrumb.reduce(function(accu, mdl) {
				return (accu.length? accu + '/' : accu) + mdl.get('name');
			}, '');

			imgs.params = {
				provider: this.get('model').get('id'),
				root: path ? path : null,
				fields: ['name', 'url', 'type', 'id', 'provider'],
				page: page, perPage: this.itemsPerRow * (this.rows + 1)
			};

			imgs.fetch({
				success: page? cbk : util.ensureDelay(this.onSuccess),
				error: this.onError,
				modelOptions: { root: path },
			});
		},
		_error: function(sender, evtName, obj, err) {
			log.error('shell.imgs.ProviderGrid', 'error while fetching=%s', err.message);
			this.get('images').clearError();

			this.showMessage(true);

			this.$.loader.hide();
		},
		showMessage: function(isError) {
			this.$.icon.setShowing(isError);
			var content = isError? errorLbl : 'No hay imágenes disponibles';
			this.$.label.set('content', content);
			this.$.message.show();
			enyo.Spotlight.spot(this.$.message);
		},
		_loaded: function() {
			if (this.showing) {
				if (this.get('images').length > 0) {
					this.$.resultList.show();

					if (enyo.Spotlight.getCurrent() === this.$.loader) {
						enyo.Spotlight.spot(this.$.resultList);
					}

					// Update the scroll position/size
					{
						var needScroll = this.get('images').length < (this.itemsPerRow * this.rows);
						this.$.resultList.$.scroller.$.strategy.$.vColumn.setShowing(!needScroll);
						if (!needScroll) {
							this.$.resultList.$.scroller.resize();
							this.$.resultList.$.scroller.scrollToTop();
						}
					}
				}
				else {
					this.showMessage(false);
				}
			}

			this.$.loader.hide();
		},
		_viewerOpenClose: function() {
			if (!Viewer.showing) {
				Viewer.removeObserver('showing', this.onViewerClose);
				Viewer.off('paging', this.onPaging);
				var index = this.get('images').indexOf(Viewer.get('model'));
				var selected = this.$.resultList.childForIndex(index);
				enyo.Spotlight.spot(selected);
				// TODO [nbaglivo] Don't use stabilize and optimize?
				this.$.resultList.stabilize();
			}
		},
		_load: function() {
			this._page = 0;
			this.$.loader.show();
			enyo.Spotlight.spot(this.$.loader);
			this.$.resultList.hide();
			this.fetch();
		},
		_navBack: function() {
			var len = this._breadcrumb.length;
			if (len) {
				this._breadcrumb.pop();
				this.doNavigate({current: this._breadcrumb[len-2]});
				this._load();
				return true;
			}
			return false;
		},
		_onKey: function(sender, evt) {
			if (evt.keyCode === tacKeyboardLayout.VK_UP) {
				var imgs = this.$.resultList.$.client.getClientControls().slice(0,7);
				var index = imgs.indexOf(evt.originator);
				return index !== -1;
			}
			if (evt.keyCode === tacKeyboardLayout.VK_PAGE_UP) {
				this.movePage(false);
			}
			if (evt.keyCode === tacKeyboardLayout.VK_PAGE_DOWN) {
				this.movePage(true);
			}
			if (evt.keyCode === tacKeyboardLayout.VK_BACK) {
				return this._navBack();
			}
		},
		movePage: function(downside) {
			var bounds = this.$.resultList.$.scroller.getScrollBounds();
			var move = downside ? 420 : -420;
			this.$.resultList.$.scroller.scrollTo(bounds.left, bounds.top + move);
			var current = enyo.Spotlight.getCurrent();
			var childs = this.$.resultList.$.client.getClientControls();
			var index = childs.findIndex( function(element) {
				return element === current;
			});
			index += downside ? 21 : -21;
			/*if (index >= childs.length - 7 ) {
				//TODO cargar siguiente pag
			}*/
			enyo.Spotlight.spot(childs[index]);
			return true;
		},
		destroy: enyo.inherit(function(sup) {
			return function() {
				Viewer.removeObserver('showing', this.onViewerClose);
				Viewer.off('paging', this.onPaging);
				sup.apply(this, arguments);
			};
		}),
		_leaved: function(sender, evt) {
			if (evt.originator === this) {
				this.$.loader.show();
				this.get('images').empty();
				this._page = 0;
				this._breadcrumb.length = 0; // Erase bradcrumb
			}
		},
		_loaderKeyDown: function(oSender, oEvent) {
			return (oEvent.keyCode !== tacKeyboardLayout.VK_EXIT);
		}
	});

	/**
	 * Kind ImagesTabBtnProvider
	 */
	enyo.kind({
		name: 'ImagesTabBtnProvider',
		kind: Shell.TabButton,
		colorClass: 'images-button',
		bindings: [
			{ from: 'model.name', to: '.label', transform: function(value) { return value.toUpperCase() } } ,
			{ from: 'model.icon', to: '.$.icon.src', transform: function(value) { return value ? value : assets('imgn-default-submenu.png')} },
		]
	});

	/**
	 * Kind ImagesTabBtn
	 */
	enyo.kind({
		name: 'ImagesTabBtn',
		kind: Shell.TabButton,
		colorClass: 'images-button',
		tooltipStyle: 'menu',
		tooltipMsg: msg.tooltip.config,
		tooltipEnabled: false
	});

})();
