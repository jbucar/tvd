/* globals TvMenu, model, Settings, Welcome */
/* exported ShellMenu */
function ShellMenu(res) {

	let prot = {
		css: res.styles('menu', '../items'),
		components: ui.getComponents( Welcome ),
	};

	prot.onconstruct = function(opts) {
		log.verbose( 'ShellMenu', 'onconstruct: opts=%j', opts );
		this._super(opts);

		let headerData = {
			logo: res.img('logo-tac-tv'),
			calendar: res.img('calendario'),
			clock: res.img('reloj'),
			date: '01.01.1990',
			time: '12:00',
			inet: '',
		};

		{	// Header
			this.model.setReady( false );
			this.model.setMenuBgImage( res.img('background') );
			this.model.setHeader( res.tpl('header'), headerData );
			updateDateTime( headerData );
			updateHeader( headerData );
			setInterval( $(updateDateTime, headerData), 10 * 1000 );
		}

		{	// Main section
			let apps = model.get('apps').getAll();
			let action = app => apiCall( appmgr.run, app.id );
			let appTpl = res.tpl('item_app');
			let cfgItem = {
				name: res.txt('setting_config'),
				icon: res.img('config'),
				onSelected: () => ui.showView( Settings ),
			};

			let main = this.model.createSection('CarrouselMenu');
			main.addCarrousel( res.txt('recomended'), res.tpl('item_recomended'), [] );
			main.addCarrousel( res.txt('favorites'), appTpl, apps.filter(app => app.isFavorite), action );
			main.addCarrousel( res.txt('applications'), appTpl, apps, action );
			main.addCarrousel( res.txt('config'), res.tpl('item_settings'), [cfgItem] );

			let clearAppEvent = model.get('apps').on( 'onChanged', () => {
				log.verbose( 'ShellMenu', 'On applications changed: apps=%d', apps.length );
				main.updateCarrousel(1, apps.filter(app => app.isFavorite));
				this.model.setReady(true);
			});
			this.on( 'teardown', () => clearAppEvent() );
		}

		{	// Fisrt boot
			this.model.createSection('GenericMenu', '<Welcome />');
		}

		apiCall( system.isFirstBoot, res => { if (res) this.focusSection(1); } );
		network.on( 'connection', evt => evt.type === 'state' ? updateHeader(headerData) : null );
		// TODO: this.on('teardown', () => network.removeListener('connection') );
	};

	return {
		extends: TvMenu,
		proto: prot,
	};

// private:
	function updateDateTime(header) {
		let dt = currentDateTime();
		header.date = dt.date;
		header.time = dt.time;
	}

	function updateHeader(header) {
		apiCallAsync( network.isConnected ).then( connected => header.inet = connected ? '' : res.img('disconnected') );
	}
}
