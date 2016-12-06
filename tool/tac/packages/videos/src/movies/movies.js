/* exported Movie */
/* globals VideoPlayer, SettingView */

function Movie(res) {

	function onPlay( ractive, mInfo, fromTime ) {
		log.verbose( 'Movie', 'onPlay' );
		assert(ractive);
		mInfo.currentTime = fromTime;
		let vPlayer = ui.showView( VideoPlayer, mInfo );
		vPlayer.once('unrender', function() {
			model.get('Movies').setCurrentTime( mInfo.id, mInfo.currentTime );
		});
	}

	function onView( btn, mInfo, setState ) {
		log.verbose( 'Movie', 'onView: setState=%s', setState );
		assert(btn);
		model.get('Movies').markSeen( mInfo.id, setState );
	}

	function onSummary( /*btn, mInfo*/ ) {
		log.verbose( 'Movie', 'onSummary' );
	}

	function onLabels( /*btn, mInfo*/ ) {
		log.verbose( 'Movie', 'onLabels' );
	}

	function onRemove( btn, mInfo ) {
		log.verbose( 'Movie', 'onRemove' );
		assert(btn);
		model.get('Movies').remove( mInfo.id );
		ui.hideView();
	}

	function createButton(panel, title, icon, isToogle) {
		let ctor = isToogle ? svm.ToggleButton : svm.Button;
		let btn = new ctor( res.txt(title), res.img(icon) );
		panel.addWidget(btn);
		return btn;
	}

	function setupButton(btn, action, visible, active ) {
		btn.setAction( $(action,btn) );
		if (visible !== undefined) {
			btn.setVisible( visible );
		}
		if (active !== undefined) {
			btn.setActive( active );
		}
	}

	let svm = ui.getModels(SettingView);
	let prot = {
		css: res.styles('../header'),
	};

	prot.onconstruct = function(opts) {
		log.info( 'Movie', 'onconstruct' );
		this._super(opts);

		opts.data.theme = ui.getTheme();
		opts.data.bgImage = res.img('background');

		let actPanel = new svm.ActionPanel();

		this.buttons = {
			resume: createButton(actPanel, 'resume', '../resume'),
			play: createButton(actPanel, 'play', '../play'),
			seen: createButton(actPanel, 'view', 'view', true),
			summary: createButton(actPanel, 'summary', 'summary'),
			labels: createButton(actPanel, 'labels', 'labels'),
			remove: createButton(actPanel, 'remove', '../remove'),
		};
		this.buttons.remove.setDialog( 'confirm', res.txt('remove_confirm_title'), res.txt('remove_confirm_text') );

		this.model.setHeader(res.tpl('../header'), {
			title: res.txt('movies'),
			icon: res.img('../menu/movies'),
		});
		this.model.pushActionPanel(actPanel);
	};

	prot.onrender = function() {
		log.info( 'Movie', 'onrender' );
		this._super();

		let mInfo = this.get('viewParam');
		assert(mInfo);

		{	//	Left panal
			let leftPanel = this.model.getLeftPanel();
			let aInfo = mInfo.codecs.audio;
			let vInfo = mInfo.codecs.video;
			leftPanel.setTitle( mInfo.title );
			leftPanel.setSubTitle( sprintf('%3.1f / %s / %s / %s', aInfo.channels, aInfo.name.toUpperCase(), vInfo.name.toUpperCase(), vInfo.height < 720 ? 'SD' : 'HD') );
			leftPanel.setIcon( mInfo.logo, true );
			leftPanel.setDescriptions( [mInfo.summary] );
		}

		setupButton( this.buttons.resume, () => onPlay(this,mInfo,mInfo.currentTime), mInfo.currentTime ? true : false);
		setupButton( this.buttons.play, () => onPlay(this,mInfo,0) );
		setupButton( this.buttons.seen, (btn,st) => onView(btn,mInfo,st), undefined, mInfo.seen );
		setupButton( this.buttons.summary, btn => onSummary(btn,mInfo) );
		setupButton( this.buttons.labels, btn => onLabels(btn,mInfo) );
		setupButton( this.buttons.remove, btn => onRemove(btn,mInfo) );

		{	//	On model changed?
			let removeListener = model.get('Movies').on( 'updated', (movieInfo) => {
				if (movieInfo.id === mInfo.id) {
					mInfo.currentTime = movieInfo.currentTime;
					mInfo.seen = movieInfo.seen;
					mInfo.codecs = movieInfo.codecs;

					this.buttons.seen.setActive( movieInfo.seen );
					let wasVisible = this.buttons.resume.isVisible();
					let isVisible = movieInfo.currentTime > 0.0;
					this.buttons.resume.setVisible( isVisible );
					if (wasVisible != isVisible) {
						this.resetFocus();
					}
				}
			});
			this.on('teardown', removeListener );
		}
	};

	return {
		extends: SettingView,
		proto: prot
	};
}
