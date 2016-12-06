/* exported Serie */
/* globals VideoPlayer, SettingView */

function Serie(res) {

	let svm = ui.getModels(SettingView);

	function zeroFill(n) {
		return ('0'+n).slice(-2);
	}

	function title( episode ) {
		return 'S' + zeroFill(episode.season) + 'E' + zeroFill(episode.episode);
	}

	function getDescriptions(info) {
		let season = zeroFill(info.season);
		let descL1 = res.txt('season') + ' ' + season + ' - ' + res.txt('episode') + ' ' + zeroFill(info.episode);
		let descL2 = res.txt('title') + ': ' + info.title;
		let descL3 = res.txt('runtime') + ': ' + parseInt(info.runtime/60) + ' ' + res.txt('min');
		let descs = [ descL1, descL2, descL3, info.summary ];
		return descs;
	}

	function updateButtons( ractive, sInfo ) {
		model.get('Series')
			.nextEpisode( sInfo.id )
			.then( (episodeInfo) => {
				ractive.nextEpisode = episodeInfo;

				let playVisible = episodeInfo ? true : false;
				ractive.buttons.play.setVisible( playVisible );
				ractive.buttons.resume.setVisible( playVisible ? episodeInfo.currentTime : false );
				ractive.buttons.episodes.setVisible( playVisible );

				if (playVisible) {
					let descs = getDescriptions( episodeInfo );

					ractive.buttons.play.setName( res.txt('play') + ' ' + title(episodeInfo) );
					ractive.buttons.play.getLeftPanel().setDescriptions( descs );
					if (episodeInfo.video.codecs) {
						let aInfo = episodeInfo.video.codecs.audio;
						let vInfo = episodeInfo.video.codecs.video;
						ractive.buttons.play.getLeftPanel().setSubTitle( sprintf('%3.1f / %s / %s / %s', aInfo.channels, aInfo.name.toUpperCase(), vInfo.name.toUpperCase(), vInfo.height < 720 ? 'SD' : 'HD') );
					}

					ractive.buttons.resume.setName( res.txt('resume') + ' ' + title(episodeInfo) );
					ractive.buttons.resume.getLeftPanel().setDescriptions( descs );
					if (episodeInfo.video.codecs) {
						let aInfo = episodeInfo.video.codecs.audio;
						let vInfo = episodeInfo.video.codecs.video;
						ractive.buttons.resume.getLeftPanel().setSubTitle( sprintf('%3.1f / %s / %s / %s', aInfo.channels, aInfo.name.toUpperCase(), vInfo.name.toUpperCase(), vInfo.height < 720 ? 'SD' : 'HD') );
					}

					ractive.buttons.episodes.setName( res.txt('season') + ' ' + parseInt(episodeInfo.season) );
				}

				ractive.resetFocus();
			});
	}

	function createEpisodeBtn( ractive, serieInfo, episode ) {
		assert(ractive);
		assert(serieInfo);
		assert(episode);
		let btn = new svm.Button( title(episode), res.img('play'), true );
		let panel = new svm.Panel(serieInfo.title, serieInfo.logo, true);
		panel.setDescriptions( getDescriptions(episode) );
		btn.setLeftPanel( panel );
		btn.setAction( $(onPlay,ractive,serieInfo,episode,0) );
		return btn;
	}

	function createSeasonBtn( ractive, serieInfo, season ) {
		assert(ractive);
		assert(serieInfo);
		assert(season);
		let btn = new svm.Button( season, res.img('season') );
		btn.setAction( $(onSeason,btn,ractive,serieInfo,season) );
		return btn;
	}

	function onPlay( ractive, sInfo, episode, currentTime ) {
		log.verbose( 'Serie', 'onPlay' );
		assert(ractive);
		assert(sInfo);
		assert(episode);

		let param = {
			title: episode.title,
			summary: episode.summary,
			logo: sInfo.logo,
			src: episode.video.src,
			subtitles: episode.subtitles.filter( (s) => s.type === 'vtt' ),
			currentTime: currentTime !== undefined ? currentTime : episode.currentTime
		};
		let ractiveView = ui.showView( VideoPlayer, param );
		ractiveView.once('unrender', function() {
			model.get( 'Series' ).setCurrentTime( episode.id, param.currentTime );
		});
	}

	function onSeason( btn, ractive, serieInfo, season ) {
		log.verbose( 'Serie', 'onSeason' );
		assert(btn);
		assert(ractive);
		assert(serieInfo);
		assert(season);

		let actionPanel = new svm.ActionPanel();

		let series = model.get('Series');

		series.getEpisodes( serieInfo.id, season )
			.then(function(episodes) {
				if (episodes.length > 0) {
					episodes.sort( (a,b) => a.episode-b.episode );
					episodes.forEach( (episode) => actionPanel.addWidget( createEpisodeBtn(ractive,serieInfo,episode) ) );
				}
				else {
					actionPanel.addWidget( new svm.Label( res.txt('nfound'), res.img('../remove') ) );
				}
				ractive.model.pushActionPanel(actionPanel);
			})
			.finally( () => btn.actionCompleted() );

		return true;
	}

	function onSeasons( btn, ractive, sInfo ) {
		log.verbose( 'Serie', 'onSeasons' );
		assert(btn);
		assert(ractive);
		assert(sInfo);

		let actionPanel = new svm.ActionPanel();
		sInfo.seasons.forEach( (season) => actionPanel.addWidget( createSeasonBtn(ractive,sInfo,season) ) );
		ractive.model.pushActionPanel(actionPanel);
	}

	function onRemove( btn, id ) {
		log.verbose( 'Series', 'onRemove' );
		assert(btn);
		assert(id);
		model.get('Series').remove( id );

		ui.hideView();
	}

	function createButton(panel, title, icon, isToogle) {
		let ctor = isToogle ? svm.ToggleButton : svm.Button;
		let btn = new ctor( res.txt(title), res.img(icon) );
		panel.addWidget(btn);
		return btn;
	}

	function setupButton( btn, action, panel, visible ) {
		btn.setAction( () => action(btn) );
		if (panel !== undefined) {
			btn.setLeftPanel( panel );
		}
		if (visible !== undefined) {
			btn.setVisible( visible );
		}
	}

	var prot = {
		css: res.styles('../header'),
	};

	prot.onconstruct = function(opts) {
		log.info( 'Serie', 'onconstruct' );
		this._super(opts);

		opts.data.theme = ui.getTheme();
		opts.data.bgImage = res.img('background');

		let actPanel = new svm.ActionPanel();

		this.buttons = {
			resume: createButton(actPanel, 'resume', '../resume'),
			play: createButton(actPanel, 'play', '../play'),
			episodes: createButton(actPanel, 'season', 'seasons'),
			seasons: createButton(actPanel, 'seasons', 'seasons'),
			remove: createButton(actPanel, 'remove', '../remove'),
		};
		this.buttons.remove.setDialog( 'confirm', res.txt('remove_confirm_title'), res.txt('remove_confirm_text') );

		this.model.setHeader(res.tpl('../header'), {
			title: res.txt('series'),
			icon: res.img('../menu/series'),
		});
		this.model.pushActionPanel(actPanel);
	};

	prot.onrender = function() {
		log.info( 'Serie', 'onrender' );
		this._super();

		let sInfo = this.get('viewParam');
		assert(sInfo);

		{	//	Left panal
			let leftPanel = this.model.getLeftPanel();
			leftPanel.setTitle( sInfo.title );
			leftPanel.setIcon( sInfo.logo, true );
			leftPanel.setDescriptions([ sInfo.summary ]);
		}

		setupButton(this.buttons.resume, () => onPlay(this,sInfo,this.nextEpisode,this.nextEpisode.currentTime), new svm.Panel(sInfo.title, sInfo.logo, true), false);
		setupButton(this.buttons.play, () => onPlay(this,sInfo,this.nextEpisode, 0), new svm.Panel(sInfo.title, sInfo.logo, true), false );
		setupButton(this.buttons.episodes, btn => onSeason(btn,this,sInfo,this.nextEpisode.season), undefined, false );
		setupButton(this.buttons.seasons, btn => onSeasons(btn,this,sInfo), undefined, sInfo.seasons.length > 0 );
		setupButton(this.buttons.remove, btn => onRemove(btn,sInfo.id) );

		updateButtons( this, sInfo );
	};

	return {
		extends: SettingView,
		proto: prot
	};
}

