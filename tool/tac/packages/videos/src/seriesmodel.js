/* globals EventEmitter */
/* exported SeriesModel */

function SeriesModel() {
	var self = {};
	var _events = new EventEmitter();
	var _series = [];

	function changed() {
		_events.emit( 'changed' );
	}

	function fetchSeries() {
		return apiCallAsync( media.serie.getSeries )
			.then(function(series) {
				_series = series;
				changed();
			});
	}

	self.init = function() {
		log.info( 'SeriesModel', 'Init' );

		media.serie.on( 'serieAdded', function() {
			fetchSeries();
		});

		media.serie.on( 'serieUpdated', function( meta ) {
			_events.emit( 'updated', meta );
			changed();
		});

		return fetchSeries();
	};

	self.fin = function() {
	};

	forwardEventEmitter( self, _events );

	self.getSeries = function() {
		return _series;
	};

	self.getEpisodes = function( serie, season ) {
		return apiCallAsync( media.serie.getEpisodes, serie, season );
	};

	self.setCurrentTime = function( eID, elapsed ) {
		return apiCallAsync( media.serie.setCurrentTime, eID, elapsed );
	};

	self.nextEpisode = function( serieId ) {
		return apiCallAsync( media.serie.getNextEpisode, serieId );
	};

	self.remove = function( serieID ) {
		return apiCallAsync( media.serie.remove, serieID )
			.then(function() {
				let index = _series.findIndex( (s) => s.id === serieID );
				assert(index >= 0);
				_series.splice( index, 1 );
				changed();
			});
	};

	return self;
}
