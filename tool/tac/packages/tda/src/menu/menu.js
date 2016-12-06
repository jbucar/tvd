/* globals TvMenu, TvView, Settings */
/* exported TDAMenu */
function TDAMenu(res) {
	let sessionModel = model.get('session');
	let channelsModel = model.get('channels');

	function updateChannels(header, section, channels, updateFavs) {
		log.verbose( 'TDAMenu', 'updateChannels: channels=%d', channels.length );
		if (sessionModel.isActive()) {
			header.logo = res.img('sesion');
			section.updateCarrousel(1, channels.filter(ch => ch.isBlocked));
		} else {
			header.logo = undefined;
			section.updateCarrousel(1, []);
		}
		if (updateFavs) {
			section.updateCarrousel(0, channels.filter(ch => ch.isFavorite));
		}
	}

	function onconstruct(opts) {
		log.verbose( 'TDAMenu', 'onconstruct' );
		this._super(opts);

		let headerData = {logo: undefined};
		let all = channelsModel.getAll();

		this.model.setReady(false);
		this.model.setHeader( res.tpl('header'), headerData );
		this.model.setMenuBgImage( res.img('background') );
		this.model.setMenuIcon( res.img('tda') );
		this.model.setMenuBgColor( 'rgb(255,108,0)' );
		this.model.setMenuFocusColor( 'rgb(255,161,0)' );

		{	// EPG
			let epg = this.model.createSection('GenericMenu', res.tpl('epg'));
			epg.addShortcut( res.txt('epg'), res.img('epg'), 2 );
		}

		{	// Channels
			let chTpl = res.tpl('item_channel');
			let showChannel = ch => ui.showView( TvView, ch );
			let cfgItem = {
					name: res.txt('setting_config'),
					icon: res.img('config'),
					onSelected: () => this.focusSection(2, {crsl: 0})
			};

			let chs = this.model.createSection('CarrouselMenu');
			chs.addCarrousel( res.txt('favorites'), chTpl, [], showChannel, res.img('favorites'), 1 );
			chs.addCarrousel( res.txt('blocked'), chTpl, [], showChannel, res.img('blocked'), 3 );
			chs.addCarrousel( res.txt('all'), chTpl, all, showChannel, res.img('all'), 4 );
			chs.addCarrousel( '', res.tpl('item_config'), [cfgItem] );
			updateChannels( headerData, chs, all, true );

			let events = [
				sessionModel.on( 'changed', () => updateChannels(headerData, chs, all) ),
				channelsModel.on( 'changed', () => {
					updateChannels(headerData, chs, all, true);
					this.model.setReady(true);
				}),
			];
			this.on('teardown', () => events.forEach(clearFn => clearFn()));
		}

		{	// Configs
			let cfgItems = ['scan', 'ginga', 'restrictions'].map( (item, idx) => {
				let itemRes = ui.getResources('settings/'+ item);
				return {
					name: itemRes.txt('carrouselName'),
					icon: itemRes.img(item),
					section: idx,
				};
			});

			let cfg = this.model.createSection('CarrouselMenu');
			cfg.addCarrousel( res.txt('settings'), res.tpl('item_settings'), cfgItems, cfg => ui.showView(Settings, cfg), res.img('settings'), 5 );
			cfg.addCarrousel( res.txt('channel'), res.tpl('item_channel'), all, ch => ui.showView( Settings, {section: 3, channel: ch} ) );
		}

		this.focusSection(1, {crsl: 0, item: 0}, true);
	}

	return {
		extends: TvMenu,
		proto: {
			css: res.styles('menu', '../items'),
			onconstruct: onconstruct,
		},
	};
}
