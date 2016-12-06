/* exported Channel */
/* globals SettingView, TvView */

function Channel(res) {

	function onView( chInfo ) {
		log.verbose( 'Channel', 'onView: id=%s', chInfo.id );
		ui.showView( TvView, chInfo );
	}

	function onFav( btn, chInfo, enable ) {
		log.verbose( 'Channel', 'onFav: id=%s', chInfo.id );
		assert( enable !== chInfo.isFavorite, 'Favorite toggle state differ from channel favorite state');
		model.get('channels').toogleFavorite(chInfo.id)
			.finally( () => btn.setActive( chInfo.isFavorite ) );
		return true;
	}

	function onBlock( btn, chInfo, enable ) {
		log.verbose( 'Channel', 'onBlock: id=%s', chInfo.id );
		assert( enable !== chInfo.isBlocked, 'Blocked toggle state differ from channel blocked state');
		model.get('channels').toogleBlocked(chInfo.id)
			.finally( () => btn.setActive( chInfo.isBlocked ) );
		return true;
	}

	var prot = {};

	prot.onconstruct = function(opts) {
		log.info( 'Channel', 'onconstruct: opts=%j', opts );

		// Call SettingView
		this._super(opts);
	};

	prot.oninit = function() {
		log.info( 'Channel', 'oninit' );

		// Call SettingView
		this._super();

		let chInfo = this.get('viewParam.channel');
		assert(chInfo);

		let svm = ui.getModels(SettingView);
		let actPanel = new svm.ActionPanel();

		{	// Create view button
			let btn = new svm.Button( res.txt('btnView'), res.img('view') );
			btn.setAction( $(onView,chInfo) );
			actPanel.addWidget( btn );
		}

		{	// Create favorite button
			let btn = new svm.ToggleButton( res.txt('btnFav'), res.img('fav') );
			btn.setActive( chInfo.isFavorite );
			btn.setAction( $(onFav,btn,chInfo) );
			actPanel.addWidget( btn );
		}

		{	// Create block button
			let btn = new svm.ToggleButton( res.txt('btnBlock'), res.img('block') );
			btn.setActive( chInfo.isBlocked );
			btn.setAction( $(onBlock,btn,chInfo) );
			let ses = model.get('session');
			btn.setVisible( ses.isEnabled() && !ses.isBlocked() );
			actPanel.addWidget( btn );
		}

		{	//	Left panal
			let leftPanel = this.model.getLeftPanel();
			leftPanel.setTitle( chInfo.name );
			leftPanel.setIcon( chInfo.logo );
			leftPanel.addDescription([
				res.txt('channel') + ': ' + chInfo.channel,
				res.txt('frecuency') + ': ' + chInfo.info.frecuency,
				res.txt('srvID') + ': ' + chInfo.info.nitID.toString(16) + '.' + chInfo.info.tsID.toString(16) + '.' + chInfo.info.srvID.toString(16),
			]);
		}

		this.model.pushActionPanel(actPanel);
	};

	return {
		extends: SettingView,
		proto: prot
	};
}

