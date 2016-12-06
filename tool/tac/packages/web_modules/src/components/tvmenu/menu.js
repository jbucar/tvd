/* globals PopupBox, CarrouselList */
/* exported TvMenu */
function TvMenu(res) {
// models:
	class TvMenuModel {
		constructor() {
			this.data = {
				menuBgImg: '',
				modelReady: true,
				header: {},
				section: undefined,
				shortcuts: [],
				currentShortcut: -1,
			};
			this.backgroundImage = null;
			this.header = '';
			this.dialog = '';
			this.sections = [];
			this.currentSection = -1;
			this.main = {
				section: -1,
				opts: {},
			};
		}

		setMenuBgImage(img) {
			this.backgroundImage = img;
		}

		setMenuIcon(icon) {
			this.data.leftPopupIcon = icon;
		}

		setMenuBgColor(color) {
			this.data.leftPopupBgColor = color;
		}

		setMenuFocusColor( color ) {
			this.data.sectionFocusBgColor = color;
		}

		setHeader(tpl, data) {
			this.header = tpl;
			this.data.header = data;
		}

		getHeader() {
			return this.header;
		}

		setDialog(tpl) {
			this.dialog = tpl;
		}

		getDialog() {
			return this.dialog;
		}

		isReady() {
			return this.data.modelReady;
		}

		setReady(isReady) {
			this.data.modelReady = isReady;
		}

		createSection(className, ...args) {
			let ctor = menuModels[className];
			assert(ctor !== undefined, 'TvMenuModel.createSection called with invalid class name: ' + className);
			let section = new ctor(...args);
			this.addSection(section);
			return section;
		}

		addSection(section) {
			assert(section instanceof GenericMenu, 'TvMenuModel.addSection: Invalid section type: ' + typeof section);
			if (this.currentSection < 0) {
				this.currentSection = 0;
				this.data.section = section;
				this.main.section = 0;
			}
			this.sections.push(section);
		}

		setCurrentSection(section, opts, setMain) {
			log.verbose( 'TvMenuModel', 'setCurrentSection: section=%d, opts=%j', section, opts );
			assert(section >= 0 && section < this.sections.length, 'TvMenuModel.setCurrentSection: Invalid section index: ' + section);
			if (setMain === true) {
				this.main.section = section;
				this.main.opts = opts;
			}
			this.currentSection = section;
			this.data.section = this.getCurrentSection();
			this.updateCurrentShortcut(opts);
			this.data.section.onEnter(opts || {});
		}

		getCurrentSection() {
			assert(this.currentSection >= 0 && this.currentSection < this.sections.length, 'TvMenuModel.getCurrentSection: Invalid currentSection: ' + this.currentSection);
			return this.sections[this.currentSection];
		}

		inMainSection() {
			return this.main.section === this.currentSection;
		}

		focusCurrentSection(ractive) {
			log.verbose( 'TvMenuModel', 'focusCurrentSection' );
			let cur = this.getCurrentSection();
			this.data.menuBgImg = cur.backgroundImage ? cur.backgroundImage : this.backgroundImage;
			let focused = cur.focus(ractive);
			if (focused) {
				this.currentShortcut = cur.getCurrentShortcut();
			}
			return focused;
		}

		getCurrentShortcut() {
			return this.data.currentShortcut;
		}

		updateCurrentShortcut(opts) {
			if (this.hasShortcuts()) {
				this.data.currentShortcut = this.data.section.getShortcutFrom(opts);
			}
		}

		hasShortcuts() {
			return this.data.shortcuts.length > 0;
		}

		shortcutsCount() {
			return this.data.shortcuts.length;
		}

		getMainSection() {
			return this.main;
		}

		getViewData() {
			return this.data;
		}
	}

	class GenericMenu {
		constructor(tpl) {
			this.view = tpl;
			this.shortcuts = [];
			this.backgroundImage = null;
		}

		onEnter() {
			if (this.onEnterCallback) {
				this.onEnterCallback();
			}
		}

		setOnEnterCallback(cb) {
			this.onEnterCallback = cb;
		}

		focus(ractive) {
			ractive.nodes.MenuBody.focus();
			return true;
		}

		blur() {
		}

		setBackground( img ) {
			this.backgroundImage = img;
		}

		addShortcut( title, icon, rank, opts ) {
			let key = this.shortcutKeyFor(opts);
			assert(this.shortcuts[key] === undefined, 'GenericMenu.addShortcut: Already exists a shortcut for this section');
			log.verbose( 'GenericMenu', 'addShortcut: opts=%j, key=%j', opts, key );
			this.shortcuts[key] = {title: title, icon: icon, rank: rank, opts: opts, section: this};
		}

		// protected:
		getShortcutFrom(opts) {
			let shct = this.shortcuts[this.shortcutKeyFor(opts)];
			if (shct) {
				log.verbose( 'GenericMenu', 'getShortcutFrom(%j): %d', opts, shct.index );
				return shct.index;
			}
			return -1;
		}

		// returns the index of the shortcut to the actual section
		getCurrentShortcut() /*virtual*/ {
			return this.getShortcutFrom({});
		}

		// returns an unique key for the section represented by opts
		shortcutKeyFor(/*opts*/) /*virtual*/ {
			return 0;
		}
	}

	class CarrouselMenu extends GenericMenu {
		constructor() {
			super( res.tpl('carrousel_menu') );
			this.current = {
				carrousel: 0,
				item: -1,
			};
			this.carrousels = [];
			this.carrouselList = undefined;
		}

		onEnter(opts) {
			if (opts.crsl !== undefined) {
				this.current.carrousel = opts.crsl;
			}
			if (opts.item !== undefined) {
				this.current.item = opts.item;
			}
			super.onEnter(opts);
		}

		focus(ractive) {
			this.carrouselList = ractive.findComponent('CarrouselList');
			assert(this.carrouselList);
			return this.carrouselList.focus(this.current);
		}

		blur() {
			assert(this.carrouselList);
			this.current = this.carrouselList.getCurrentPosition();
		}

		addCarrousel( name, itemTpl, items, itemSelectedCb, shortcutIcon, rank ) {
			let crsl = {
				name: name,
				tpl: itemTpl,
				items: items,
			};
			if (itemSelectedCb) {
				crsl.onSelected = this.itemSelected.bind(this, itemSelectedCb);
			}
			let self = this;
			crsl.items.forEach( function(item) {
				if (item.onSelected) {
					let original = item.onSelected;
					item.onSelected = self.itemSelected.bind(self, original);
				}
			});
			this.carrousels.push(crsl);
			if (shortcutIcon !== undefined) {
				this.addShortcut( name, shortcutIcon, rank !== undefined ? rank : 0, {crsl: this.carrousels.length-1} );
			}
		}

		updateCarrousel(crslIdx, items) {
			assert(crslIdx < this.carrousels.length, 'Invalid carrousel index: ' + crslIdx);
			this.carrousels[crslIdx].items = items;
		}

		itemSelected(callback, item) {
			this.current = this.carrouselList.getCurrentPosition();
			callback(item);
		}

		// override
		shortcutKeyFor(opts) {
			return opts.crsl;
		}

		// override:
		getCurrentShortcut() {
			return this.getShortcutFrom({crsl: this.current.carrousel});
		}
	}

// impl:
	let menuModels = {
		CarrouselMenu: CarrouselMenu,
		GenericMenu: GenericMenu,
	};

	let prot = {
		template: res.tpl('menu'),
		css: res.style('menu'),
		components: ui.getComponents( PopupBox, CarrouselList ),
	};

	prot.onconstruct = function(opts) {
		log.info( 'TvMenu', 'onconstruct' );

		this.model = new TvMenuModel();
		opts.data = this.model.getViewData();

		opts.data.showLeftPopup = false;
		opts.partials.section = '';
		opts.partials.header = '';
	};

	prot.oninit = function() {
		log.info( 'TvMenu', 'oninit' );

		this.on({
			menukeyHandler: onMenuKeyDown,
			shortcutKeyHandler: onShortcutKeyDown,
		});
		this.observe( 'modelReady', onModelReadyChange, {init: false, defer: true} );

		this.model.sections.forEach( (section,idx) => {
			section.index = idx;
			let shortcuts = section.shortcuts;
			this.model.data.shortcuts.push(...shortcuts);
		});
		this.model.data.shortcuts.sort( (a,b) => a.rank - b.rank );
		this.model.data.shortcuts.forEach( (shortcut,idx) => shortcut.index = idx );

		let main = this.model.getMainSection();
		if (main.section >= 0) {
			this.model.updateCurrentShortcut(main.opts);
		}
	};

	prot.oncomplete = function() {
		log.info( 'TvMenu', 'oncomplete' );

		this.rendered = true;
		this.resetPartial('header', this.model.getHeader());
		this.resetPartial('dialog', this.model.getDialog());
		this.resetPartial('section', this.model.getCurrentSection().view);
		if (this.model.isReady()) {
			this.focus();
		}
	};

	prot.onunrender = function() {
		log.info( 'TvMenu', 'onunrender' );
		this.rendered = false;
		this.model.getCurrentSection().blur();
		this.resetPartial('section', '');
		this.resetPartial('header', '');
		this.resetPartial('dialog', '');
	};

	prot.focus = function() {
		log.verbose( 'TvMenu', 'focus' );
		if (!this.model.focusCurrentSection(this)) {
			if (this.model.hasShortcuts()) {
				showMenu(this, true);
			} else {
				this.nodes.MenuBody.focus();
			}
		} else {
			this.set('showLeftPopup', false);
		}
	};

	prot.focusSection = function(section, opts, setMain) {
		log.verbose( 'TvMenu', 'focusSection: section=%d, opts=%j, setMain=%s', section, opts, setMain );

		if (this.rendered) {
			this.resetPartial('section', '');
			this.model.setCurrentSection(section, opts, setMain);
			this.resetPartial('section', this.model.getCurrentSection().view);
			showMenu(this, false);
		} else {
			this.model.setCurrentSection(section, opts, setMain);
		}
	};


	return {
		proto: prot,
		models: menuModels,
	};

// private:
	function showMenu(self, show) {
		self.set('showLeftPopup', show);
		if (show) {
			self.model.getCurrentSection().blur();
			self.nodes.MenuSections.children[self.model.getCurrentShortcut()].focus();
		} else if (!self.model.focusCurrentSection(self)) {
			showMenu(self, true);
		}
	}

	function onModelReadyChange(value, oldValue, keyPath) {
		log.verbose( 'TvMenu', 'onModelReadyChange: %s change from "%s" to "%s"', keyPath, oldValue, value );
		if (value) {
			this.focus();
		}
	}

	function navigateSection(self, idx, up) {
		let cur = idx;
		let last = self.model.shortcutsCount() - 1;
		cur += up ? -1 : 1;
		cur = (cur < 0) ? last : ((cur > last) ? 0 : cur);
		self.nodes.MenuSections.children[cur].focus();
	}

	function onMenuKeyDown(event) {
		// log.verbose( 'Menu', 'onMenuKeyDown: code=%d', event.original.which );
		let propagate = true;
		switch (event.original.which) {
			case tacKeyboardLayout.VK_LEFT:
				if (this.model.hasShortcuts()) {
					showMenu(this, true);
					propagate = false;
				}
				break;
			case tacKeyboardLayout.VK_BACK:
			case tacKeyboardLayout.VK_EXIT:
				if (!this.model.inMainSection()) {
					let main = this.model.getMainSection();
					this.focusSection(main.section, main.opts);
					propagate = false;
				}
				break;
		}
		return propagate;
	}

	function onShortcutKeyDown(event, idx, section, opts) {
		// log.verbose( 'Menu', 'onShortcutKeyDown: code=%d focused=%d', event.original.which, focused );
		switch (event.original.which) {
			case tacKeyboardLayout.VK_RIGHT:
				showMenu(this, false);
				break;
			case tacKeyboardLayout.VK_UP:
			case tacKeyboardLayout.VK_DOWN:
				navigateSection( this, idx, event.original.which == tacKeyboardLayout.VK_UP );
				break;
			case tacKeyboardLayout.VK_ENTER:
				this.focusSection(section, opts);
				break;
			default:
				return true;
		}

		return false;
	}
}
