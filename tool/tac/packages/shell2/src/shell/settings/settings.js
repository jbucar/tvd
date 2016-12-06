/* global model, TvMenu, Inet, About, Geolocalization, ApplicationSetting, ServiceSetting */
/* exported Settings */
function Settings(res) {

	function goToScreen(self, category, section, params) {
		log.verbose( 'Settings', 'goToScreen: section=%d', section );
		self.headerData.category = category;
		self.set('screenParams', params);
		self.focusSection(section);
	}

	function onconstruct(opts) {
		log.info( 'Settings', 'onconstruct' );
		this._super(opts);

		opts.data.theme = ui.getTheme();
		opts.data.ThemeVirtualKeyboardBgImage = res.img('background');

		this.headerData = {
			icon: res.img('icon'),
			category: res.txt('config'),
		};
		this.model.setMenuBgImage( res.img('background') );
		this.model.setHeader( res.tpl('header'), this.headerData );

		{	// Main section
			let tpl = res.tpl('items');
			let sysTxt = res.txt('system');
			let sysItems = ['inet', 'geolocalization', 'about'].map( (item,idx) => {
				let itemRes = ui.getResources('shell/settings/'+ item);
				return {
					name: itemRes.txt('carrouselName'),
					icon: itemRes.img(item),
					onSelected: () => goToScreen(this, sysTxt, idx+1),
				};
			});

			let main = this.model.createSection('CarrouselMenu');
			main.addCarrousel( res.txt('system'), tpl, sysItems );
			main.addCarrousel( res.txt('applications'), tpl, model.get('apps').getAll(), $(goToScreen, this, res.txt('applications'), 4) );
			main.addCarrousel( res.txt('services'), tpl, [], $(goToScreen, this , res.txt('services'), 5) );
			main.setOnEnterCallback(() => this.headerData.category = res.txt('config'));

			apiCallAsync( srvmgr.getAll )
				.map( id => apiCallAsync(srvmgr.get, id) )
				.map(function(srv) {
					if (!srv.icon) {
						srv.icon = ui.getResource('images','shell/settings/service/default');
					}
					return srv;
				})
				.then( srvs => main.updateCarrousel(2, srvs) );
		}

		{	// Subsections
			this.model.createSection('GenericMenu', '<Inet />');
			this.model.createSection('GenericMenu', '<Geolocalization />');
			this.model.createSection('GenericMenu', '<About />');
			this.model.createSection('GenericMenu', '<ApplicationSetting />');
			this.model.createSection('GenericMenu', '<ServiceSetting />');
		}

		this.on( 'goToMenu', $(goToScreen, this, res.txt('config'), 0) );
		this.on( 'complete', () => this.focusSection(0, {crsl: 0, item: 0}) );
	}

	return {
		extends: TvMenu,
		proto: {
			css: res.styles( 'settings', '../items' ),
			components: ui.getComponents( Inet, About, Geolocalization, ApplicationSetting, ServiceSetting ),
			onconstruct: onconstruct,
		}
	};
}
