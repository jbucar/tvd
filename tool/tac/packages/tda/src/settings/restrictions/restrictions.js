/* exported Restrictions */
/* globals SettingView */
function Restrictions(res) {
	let svm = ui.getModels(SettingView);

	function createBtn( label, img, actionAsPartial ) {
		let btn = new svm.Button( res.txt(label), res.img(img) );
		actionAsPartial.splice( 1, 0, btn );
		btn.setAction( $(...actionAsPartial) );
		return btn;
	}

	function onSetPassword( btn, ractive, params ) {
		log.info( 'Restrictions', 'onSetPassword' );
		assert(btn);
		assert(ractive);
		if (params.pin === params.pin2) {	//	TODO: Check in dialog!
			model.get('session').setPass( params.pin );
		}
	}

	function onCleanPassword( btn ) {
		log.info( 'Restrictions', 'onCleanPassword' );
		assert(btn);
		model.get('session').clearPass();
	}

	function onCloseSession( btn ) {
		log.info( 'Restrictions', 'onCloseSession' );
		assert(btn);
		model.get('session').expire();
	}

	function onSetYear( btn, ractive, year ) {
		log.info( 'Restrictions', 'onSetYear: year=%s', year );
		assert(btn);
		assert(ractive);
		apiCall( tvd.session.restrictAge, year );
		ractive.selectedBtn.setActionIcon( res.img('icons/radio_btn') );
		btn.setActionIcon( res.img('icons/radio_btn_sel') );
		ractive.selectedBtn = btn;
	}

	function createYearBtn( label, sel, ractive, year ) {
		let btn = createBtn( label, 'icons/m' + year, [onSetYear,ractive,year] );
		let actionImg = 'icons/radio_btn';
		if (sel === year) {
			actionImg += '_sel';
			ractive.selectedBtn = btn;
		}
		btn.setActionIcon( res.img(actionImg) );
		return btn;
	}

	function onSelectYear( btn, ractive ) {
		log.info( 'Restrictions', 'onSelectYear' );
		assert(ractive);
		assert(btn);

		apiCallAsync( tvd.session.ageRestricted )
			.then(function(year) {
				let actPanel = new svm.ActionPanel();

				actPanel.addWidget( createYearBtn('btnYearNone',year,ractive,0) );
				actPanel.addWidget( createYearBtn('btnYear18',year,ractive,18) );
				actPanel.addWidget( createYearBtn('btnYear16',year,ractive,16) );
				actPanel.addWidget( createYearBtn('btnYear14',year,ractive,14) );
				actPanel.addWidget( createYearBtn('btnYear12',year,ractive,12) );
				actPanel.addWidget( createYearBtn('btnYear10',year,ractive,10) );

				ractive.model.pushActionPanel(actPanel);
			});
	}

	function onSetContent( btn, fnc, state ) {
		log.info( 'Restrictions', 'onSetContent: state=%j', state );
		assert(btn);

		apiCallAsync( fnc, state )
			.finally( () => btn.setActive( state ) );

		return true;
	}

	function createContentBtn( label, img, state, fnc ) {
		let btn = new svm.ToggleButton( res.txt(label), res.img('icons/content_' + img) );
		btn.setAction( $(onSetContent,btn,fnc) );
		btn.setActive( state );
		return btn;
	}

	function onSelectContent( btn, ractive ) {
		assert(ractive);
		assert(btn);

		Promise.all([
			apiCallAsync( tvd.session.isSexRestricted ),
			apiCallAsync( tvd.session.isViolenceRestricted ),
			apiCallAsync( tvd.session.isDrugsRestricted )
		]).spread(function(hasSex,hasViolence,hasDrugs) {
			log.info( 'Restrictions', 'onSelectContent: hasSex=%s, hasViolence=%s, hasDrugs=%s', hasSex, hasViolence, hasDrugs );

			let actPanel = new svm.ActionPanel();

			actPanel.addWidget( createContentBtn('btnContentDrugs','drugs',hasDrugs,tvd.session.restrictDrugs) );
			actPanel.addWidget( createContentBtn('btnContentSex','sex',hasSex,tvd.session.restrictSex) );
			actPanel.addWidget( createContentBtn('btnContentViolence','violence',hasViolence,tvd.session.restrictViolence) );

			ractive.model.pushActionPanel(actPanel);
		});
	}

	function updateVisibles( ractive, isEnabled, isBlocked ) {
		log.verbose( 'Restrictions', 'updateVisibles: isEnabled=%s, isBlocked=%s', isEnabled, isBlocked );

		ractive.buttons.initSession.setVisible( isBlocked ? true : false );
		ractive.buttons.createPass.setVisible( isEnabled ? false : true );
		ractive.buttons.closeSession.setVisible( isEnabled && !isBlocked );
		ractive.buttons.restrictYear.setVisible( isEnabled && !isBlocked );
		ractive.buttons.restrictContent.setVisible( isEnabled && !isBlocked );
		ractive.buttons.cleanPass.setVisible( isEnabled && !isBlocked );

		ractive.resetFocus();
	}

	var prot = {};

	prot.oninit = function(opts) {
		log.info( 'Restrictions', 'oninit' );

		// Call SettingView
		this._super(opts);

		let session = model.get('session');
		let actPanel = new svm.ActionPanel();
		this.buttons = {};

		{	// Create initialize session
			let btn = new svm.Button( session.btnText(), session.btnImage() );
			btn.setAction( $(function() { model.get('session').unblock(); }, btn) );
			this.buttons.initSession = btn;
			actPanel.addWidget( btn );
		}

		{	// Create set password button
			let btn = createBtn( 'btnPass', 'pass', [onSetPassword,this] );
			let inputs = [{
				name: 'pin',
				label: res.txt('pin'),
				placeholder: res.txt('pin_description'),
				type: 'password',
			},{
				name: 'pin2',
				label: res.txt('pin2'),
				placeholder: res.txt('pin_description'),
				type: 'password',
			}];
			btn.setDialog( 'input', res.txt('pinTitle').toUpperCase(), inputs, res.txt('pinAccept'), validatePass);

			this.buttons.createPass = btn;
			actPanel.addWidget( btn );
		}

		{	// Restrict by year
			let btn = createBtn( 'btnYear', 'year', [onSelectYear,this] );
			this.buttons.restrictYear = btn;
			actPanel.addWidget( btn );
		}

		{	// Restrict by content
			let btn = createBtn( 'btnContent', 'content', [onSelectContent,this] );
			this.buttons.restrictContent = btn;
			actPanel.addWidget( btn );
		}

		{	// Close session button
			let btn = createBtn( 'btnCloseSession', 'close_session', [onCloseSession,this] );
			this.buttons.closeSession = btn;
			actPanel.addWidget( btn );
		}

		{	// Delete password button
			let btn = createBtn( 'btnCleanPass', 'clear', [onCleanPassword,this] );
			btn.setDialog( 'confirm', res.txt('btnCleanPass').toUpperCase(), res.txt('cleanpass_confirm_text') );

			this.buttons.cleanPass = btn;
			actPanel.addWidget( btn );
		}

		{	//	Left panal
			let leftPanel = this.model.getLeftPanel();
			leftPanel.setTitle( res.txt('title') );
			leftPanel.setIcon( res.img('restrictions') );
			leftPanel.addDescription([res.txt('description')]);
		}

		this.model.pushActionPanel(actPanel);

		{	//	Session handling
			updateVisibles( this, session.isEnabled(), session.isBlocked() );
			session.on('changed', $(updateVisibles,this) );
		}
	};

	return {
		extends: SettingView,
		proto: prot
	};

// private:
	function validatePass(data, cb) {
		log.verbose( 'Restrictions', 'validatePass: data=%j', data );
		let error;
		if (data.pin === '') {
			error = res.txt('emptyPassError');
		}
		else if (data.pin !== data.pin2) {
			error = res.txt('mismatchPassError');
		}
		cb(error);
	}
}
