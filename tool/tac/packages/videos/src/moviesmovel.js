/* globals EventEmitter */
/* exported MoviesModel */

function MoviesModel() {
	var self = {};
	var _events = new EventEmitter();
	var _videos = [];

	function changed() {
		_events.emit( 'changed' );
	}

	function getVideos() {
		return apiCallAsync( media.movie.getAll )
			.then(function(movies) {
				_videos = movies;
			});
	}

	function get( movieID, fnc ) {
		return fnc.call( _videos, (vid) => vid.id === movieID );
	}

	function getMovieIndex( movieID ) {
		return get( movieID, Array.prototype.findIndex );
	}

	self.init = function() {
		log.info( 'MoviesModel', 'Init' );

		media.movie.on( 'movieAdded', function( meta ) {
			_videos.push( meta );
			changed();
		});

		media.movie.on( 'movieUpdated', function( meta ) {
			let index = getMovieIndex( meta.id );
			if (index >= 0) {
				_videos[index] = meta;
				changed( meta.id );
				_events.emit( 'updated', meta );
			}
			changed();
		});

		return getVideos()
			.then( changed );
	};

	self.fin = function() {
	};

	forwardEventEmitter( self, _events );

	self.getAll = function() {
		return _videos;
	};

	self.getQueuedMovies = function() {
		return apiCallAsync( media.movie.getQueuedMovies );
	};

	self.resolveQueuedMovie = function( movieID, needAdd ) {
		return apiCallAsync( media.movie.resolveQueuedMovie, movieID, needAdd );
	};

	self.setCurrentTime = function( movieID, elapsed ) {
		return apiCallAsync( media.movie.setCurrentTime, movieID, elapsed );
	};

	self.markSeen = function( movieID, seenState ) {
		return apiCallAsync( media.movie.markSeen, movieID, seenState );
	};

	self.remove = function( movieID ) {
		return apiCallAsync( media.movie.remove, movieID )
			.then(function() {
				let index = _videos.findIndex( (m) => m.id === movieID );
				assert(index >= 0);
				_videos.splice( index, 1 );
				changed();
			});
	};

	return self;
}
