"use strict";

var tvdutil = require('tvdutil');
var assert = require('assert');
var Repository = require('./repository');
var _ = require('lodash');

function doAsync( prom, cb ) {
	prom.then( _.partial(cb,undefined), cb );
}

function createMovieAPI(repo) {
	var self = {};

	//	Events
	tvdutil.forwardEventEmitter(self,repo);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	self.getAll = function(cb) { doAsync( repo.getMovies(), cb ); };
	self.getAll.isAsync = true;

	self.add = function( opts, cb ) { doAsync( repo.addMovie(opts), cb ); };
	self.add.isAsync = true;

	self.setCurrentTime = function(id,elapsed,cb) { doAsync( repo.setCurrentTime('movie',id,elapsed), cb ); };
	self.setCurrentTime.isAsync = true;

	self.markSeen = function(id,state,cb) { doAsync( repo.markSeen('movie', id, state ), cb ); };
	self.markSeen.isAsync = true;

	self.remove = function(id,cb) { doAsync( repo.remove('movie',id), cb ); };
	self.remove.isAsync = true;

	self.queueMovie = function(opts,cb) { doAsync( repo.queueMovie(opts), cb ); };
	self.queueMovie.isAsync = true;
	self.hasQueuedMovies = function(cb) { doAsync( repo.hasQueuedMovies(), cb ); };
	self.hasQueuedMovies.isAsync = true;
	self.getQueuedMovies = function(cb) { doAsync( repo.getQueuedMovies(), cb ); };
	self.getQueuedMovies.isAsync = true;
	self.resolveQueuedMovie = function( queuedID, add, cb ) { doAsync( repo.resolveQueuedMovie(queuedID,add), cb ); };
	self.resolveQueuedMovie.isAsync = true;

	self.getWebAPI = function() {
		return {
			'name': 'movie',
			'public': ['getAll', 'add', 'setCurrentTime', 'markSeen', 'remove', 'on', 'queueMovie', 'hasQueuedMovies', 'getQueuedMovies', 'resolveQueuedMovie' ]
		};
	};

	return self;
}

function createSerieAPI(repo) {
	var self = {};

	//	Events
	tvdutil.forwardEventEmitter(self,repo);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	self.getSeries = function(cb) { doAsync( repo.getSeries(), cb ); };
	self.getSeries.isAsync=true;

	self.getEpisodes = function(serieId, seasonName, cb) { doAsync( repo.getEpisodes(serieId,seasonName), cb ); };
	self.getEpisodes.isAsync=true;

	self.add = function( opts, cb ) { doAsync( repo.addEpisode(opts), cb ); };
	self.add.isAsync = true;

	self.setCurrentTime = function(id,elapsed,cb) { doAsync( repo.setCurrentTime('serie',id,elapsed), cb ); };
	self.setCurrentTime.isAsync = true;

	self.getNextEpisode = function(serieId,cb) { doAsync( repo.getNextEpisode(serieId), cb ); };
	self.getNextEpisode.isAsync = true;

	self.remove = function( serieId, cb ) { doAsync( repo.remove('serie', serieId), cb ); };
	self.remove.isAsync = true;

	self.getWebAPI = function() {
		return {
			'name': 'serie',
			'public': ['add','getSeries','getEpisodes', 'setCurrentTime', 'markSeen', 'getNextEpisode', 'remove', 'on']
		};
	};

	return self;
}

function createAPI( repo ) {
	var self = {};

	//	Events
	tvdutil.forwardEventEmitter(self,repo);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	self.getWebAPI = function() {
		return {
			'name': 'media',
			'modules': ['movie','serie'],
			'public': ['on']
		};
	};

	self.movie = createMovieAPI(repo);

	self.serie = createSerieAPI(repo);

	return self;
}

function ServiceImpl(adapter) {
	var self = {};
	var _cfg;
	var _repo;
	var _downloader;

	function cfgFile() {
		return 'media.json';
	}

	function defaultConfig() {
		return {
		};
	}

	function convertConfig( oldCfg ) {
		return oldCfg;
	}

	function initRepository() {
		_repo = new Repository({
			basePath: adapter.getDataPath(),
			sharePath: adapter.getSharePath(),
			downloader: _downloader,
			convertToVTT: true,
			supportedFiles: {
				subtitle: [ '.srt', '.vtt' ],
				video: [ '.avi', '.mkv', '.mp4' ]
			}
		});

		//	Ignore error
		_repo.on( 'error', () => {} );

		return _repo.start();
	}

	function finRepository() {
		return _repo.stop()
			.then( () => _repo = null );
	}

	self.start = function( cb ) {
		//	Load config
		_cfg = adapter.load( cfgFile(), defaultConfig(), convertConfig );
		assert(_cfg, 'Invalid config');

		//	Get downloader
		_downloader = adapter.registry().get('downloader');
		assert(_downloader, 'invalid downloader');

		return initRepository()
			.then(function() {
				cb( undefined, createAPI(_repo) );
			});
	};

	self.stop = function( cb ) {
		return finRepository()
			.then( () => cb() );
	};

	return self;
}

//	Service entry point
function Service(adapter) {
	var self = {};
	var _impl = null;

	self.onStart = function(cb) {
		log.info( 'Media', 'Start' );

		//	Create service implementation
		_impl = new ServiceImpl( adapter );

		//	Start
		_impl.start( cb );
	};

	self.onStop = function(cb) {
		log.info( 'Media', 'Stop' );
		_impl.stop(function() {
			_impl = null;
			cb();
		});
	};

	return Object.freeze(self);
}

module.exports = Service;

