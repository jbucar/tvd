/* global TvMenu, Scan, Ginga, Restrictions, Channel */
/* exported Settings */
function Settings(res) {

	let sessionModel = model.get('session');
	let prot = {
		css: res.styles( 'settings' ),
		components: ui.getComponents( Scan, Ginga, Restrictions, Channel ),
	};

	prot.onconstruct = function(opts) {
		log.verbose( 'Settings', 'onconstruct' );
		this._super(opts);

		this.headerData = {
			icon: res.img('icon'),
			category: res.txt('config'),
			session: sessionModel.isActive() ? res.img('../menu/sesion') : undefined,
		};
		this.model.setHeader( res.tpl('header'), this.headerData);
		this.model.setMenuBgImage( res.img('../menu/background') );

		this.model.createSection('GenericMenu', '<Scan />');
		this.model.createSection('GenericMenu', '<Ginga />');
		this.model.createSection('GenericMenu', '<Restrictions />');
		this.model.createSection('GenericMenu', '<Channel />');

		opts.data.theme = ui.getTheme();
		opts.data.ThemeVirtualKeyboardBgImage = res.img('../menu/background', true);

		let clearEvent = sessionModel.on( 'changed', () => this.headerData.session = sessionModel.isActive() ? res.img('../menu/sesion') : undefined );
		this.on('teardown', clearEvent);

		this.focusSection( 0, {} );

		this.on('render', () => this.model.setReady(false));
		this.on('complete', () => {
			this.focusSection( this.get('viewParam.section'), {}, true );
			this.model.setReady(true);
		});
	};

	return {
		extends: TvMenu,
		proto: prot,
	};
}
