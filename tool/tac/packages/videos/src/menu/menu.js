/* globals TvMenu, CarrouselList, Serie, Movie, Dialog */
/* exported VideosMenu */

function VideosMenu(res) {

	let GenericMenu = ui.getModels(TvMenu).GenericMenu;

	class DetailsMenu extends GenericMenu {
		constructor(extraProps, maxSummaryLines) {
			super( res.tpl('video_details') );
			this.carrouselList = null;
			this.extraProps = extraProps;
			this.maxSummaryLines = maxSummaryLines;
			this.selectedItem = {
				carrousel: 0,
				item: 0,
			};
			this.carrousels = [];
			this.focusedItem = null;
		}

		focus(ractive) {
			log.verbose( 'DetailsMenu', 'focus' );

			this.carrouselList = ractive.findComponent('CarrouselList');
			assert(this.carrouselList);
			this.carrouselList.on('focusChange', this.onItemFocused.bind(this, ractive) );
			this.carrouselList.on('refreshCarrousels', () => this.onItemFocused(ractive, this.carrouselList.getSelectedItem()) );
			return this.carrouselList.focus(this.selectedItem);
		}

		blur() {
			log.verbose( 'DetailsMenu', 'blur' );
			this.selectedItem = this.carrouselList.getCurrentPosition();
		}

		addCarrousel( name, view ) {
			log.verbose( 'DetailsMenu', 'addCarrousel: name=%s', name );
			this.carrousels.push({
				name: res.txt(name),
				tpl: res.tpl('video_item'),
				items: [],
				onSelected: item => ui.showView( view, item ),
			});
		}

		updateItems( idx, items ) {
			log.verbose( 'DetailsMenu', 'updateItems: items=%d', items.length );
			this.carrousels[idx].items.splice( 0, this.carrousels[idx].items.length, ...items );
		}

		//	private
		onItemFocused( ractive, item ) {
			log.verbose( 'DetailsMenu', 'onItemFocused: item=%j', item );
			this.focusedItem = item;
			if (!this.focusedItem) {
				ractive.focus();
			}
		}
	}

	//	Movies
	function fillMovies( self, section, movies ) {
		let mMovies = movies.getAll().map( getVideoInfo );
		section.updateItems( 0, mMovies );
		// TODO: change by real carrousel
		section.updateItems( 1, mMovies );
		if (!self.model.isReady()) {
			self.model.setReady(true);
			processQueuedMovies(self, section);
		}
	}

	function getVideoInfo(metainfo) {
		let txt = (resTxt, info, extra) => res.txt(resTxt) + ': ' + ((info && extra) ? info + extra : (info || ''));
		return {
			id: metainfo.id,
			title: metainfo.title,
			genre: txt('genre',metainfo.genre),
			director: txt('director', metainfo.director),
			actors: txt('actors',metainfo.actors),
			duration: txt('runtime', parseInt(metainfo.runtime / 60), ' min.'),
			summary: metainfo.summary,
			logo: metainfo.poster,
			src: metainfo.video.src,
			seen: metainfo.seen,
			currentTime: metainfo.currentTime,
			rating: metainfo.rating,
			subtitles: metainfo.subtitles.filter( (s) => s.type === 'vtt' ),
			codecs: metainfo.video.codecs,
		};
	}

	function getSerieInfo(metainfo) {
		return {
			id: metainfo.id,
			title: metainfo.title,
			summary: metainfo.summary,
			logo: metainfo.logo || res.img('def_serie'),
			seen: metainfo.seen,
			seasons: metainfo.seasons,
			nextEpisode: metainfo.nextEpisode,
			rating: metainfo.rating,
		};
	}

	function processQueuedMovie(ractive, sm, movies, index) {
		assert(index >= 0 && index < movies.length, 'processQueuedMovie: invalid index');

		let dlg = getVideoInfo(movies[index]);
		dlg.positive = res.txt('addQueued');
		dlg.negative = res.txt('discardQueued');
		dlg.ratingImg = res.img('rating');

		ractive.set({
			dialog: dlg,
			theme: {
				bgColor: 'rgb(44,44,44)',
				fgColor: 'rgb(65,65,65)',
				focusColor: 'rgb(128,128,128)'
			},
		});
	}

	function processQueuedMovies(ractive,sm) {
		let movModel = model.get('Movies');
		movModel.getQueuedMovies()
			.then(function(movies) {
				log.verbose( 'menu', 'On process queued movies: movies=%d', movies.length );
				if (movies.length > 0) {
					let index = 0;
					processQueuedMovie( ractive, sm, movies, index );

					let sigHandler=ractive.on( 'onDialogResult', function(result) {
						log.verbose( 'menu', 'On process queued item: result=%s, index=%d', result, index );

						movModel.resolveQueuedMovie( movies[index].id, result )
							.finally(function() {
								index++;
								if (index < movies.length) {
									processQueuedMovie( ractive, sm, movies, index );
								}
								else {
									ractive.set({
										dialog: undefined,
										theme: undefined,
									});
									sigHandler.cancel();
									sm.focus(ractive);
								}
							});
					});
				}
			});
	}

	let prot = {
		css: res.styles('menu', 'queuedmovies', '../items'),
		components: ui.getComponents( CarrouselList, Dialog ),
	};

	prot.onconstruct = function(opts) {
		log.verbose( 'VideosMenu', 'onconstruct: opts=%j', opts );
		this._super(opts);

		opts.data.ratingIcon = res.img('rating');
		opts.partials.dialog = '';
		this.model.setReady(false);
		this.model.setMenuIcon( res.img('videos') );
		this.model.setMenuBgColor( 'rgb(65,160,41)' );
		this.model.setMenuFocusColor( 'rgb(107,196,60)' );
		this.model.setDialog(res.tpl('qm_dialog'));

		let clearEvents = [];

		{	//	Movies
			let sm = new DetailsMenu(['genre','director','actors','duration'], 6);
			sm.setBackground( res.img('movies_background') );
			sm.addCarrousel( 'movies', Movie );
			sm.addCarrousel( 'movies', Movie );
			sm.addShortcut( res.txt('movies'), res.img('movies'), 1 );
			this.model.addSection( sm );

			let movies = model.get('Movies');
			clearEvents.push( movies.on( 'changed', () => fillMovies(this, sm, movies) ) );
		}

		{	//	Series
			let sm = new DetailsMenu([], 10);
			sm.setBackground( res.img('series_background') );
			sm.addCarrousel( 'series', Serie );
			sm.addShortcut( res.txt('series'), res.img('series'), 2 );
			this.model.addSection( sm );

			let series = model.get('Series');
			clearEvents.push( series.on( 'changed', () => sm.updateItems( 0, series.getSeries().map(getSerieInfo) ) ));
		}

		{	// Others
			let tpl = '<h1 style="color:black;font-size:5rem; margin-left: 10%;">TODO: Others</h1>';
			let others = this.model.createSection('GenericMenu', tpl );
			others.addShortcut( res.txt('others'), res.img('others'), 3 );
		}

		// Remove listeners
		this.on( 'teardown', () => clearEvents.forEach(clearFn => clearFn()) );

		this.focusSection(0, {}, true);
	};

	return {
		extends: TvMenu,
		proto: prot
	};
}
