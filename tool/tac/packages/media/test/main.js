'use strict';

var os = require('os');
var path = require('path');
var fs = require('fs-extra');
var util = require('util');
var tvdutil = require('tvdutil');
var _ = require('lodash');
var bPromise = require('bluebird');
var mktemp = bPromise.promisifyAll( require('mktemp') );

var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var mockfs = require('mock-fs');
var Mocks = require('mocks');

var MediaService = require('../src/main');

describe('media', function() {
	var reg = null;
	var adapter = null;
	var rootDir = null;

	beforeEach(function() {
		reg = Mocks.init('silly');
		var dest = path.join( os.tmpDir(), 'media.XXXXXXXX');
		rootDir = mktemp.createDirSync( dest );

		Mocks.rwPath( rootDir );
		adapter = new Mocks.ServiceAdapter(reg,'media');
		reg.put( 'diskmgr', new Mocks.DiskManager() );
		reg.put( 'downloader', new Mocks.Downloader() );
	});

	afterEach(function() {
		Mocks.fin();
		fs.remove( rootDir );
		rootDir = null;
		adapter = null;
	});

	describe('onStart/onStop', function() {
		var media = null;

		beforeEach(function() {
			media = new MediaService(adapter);
		});

		afterEach(function() {
			media = null;
		});

		it('should check api from media', function(done) {
			media.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				reg.put( 'media', api );

				assert.isFunction(api.getWebAPI);

				assert.isFunction( api.movie.getWebAPI );
				assert.isObject( api.movie.getWebAPI() );

				assert.isFunction( api.serie.getWebAPI );
				assert.isObject( api.serie.getWebAPI() );

				let webAPI = api.getWebAPI();
				assert.isObject(webAPI);
				media.onStop(done);

				reg.put( 'ar.edu.unlp.info.lifia.tvd.media.main', media );
			});
		});

		it('should convert old config', function(done) {
			adapter.load = function( file, defaultsValues, convert ) {
				var cfg = _.cloneDeep(defaultsValues);
				var newCfg = convert(cfg);
				assert.deepEqual( cfg, newCfg );
				return cfg;
			};

			media.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				media.onStop(done);
			});
		});

		it('should start/stop/start', function(done) {
			media.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				media.onStop(function() {
					media.onStart(function(err1,api1) {
						assert.equal(err1);
						assert.isObject(api1);
						assert(api1 != api);
						media.onStop(done);
					});
				});
			});
		});
	});

	describe('methods', function() {
		var media = null;
		var api = null;

		beforeEach(function(done) {
			media = new MediaService(adapter);
			media.onStart(function(err,obj) {
				api = obj;
				done();
			});
		});

		afterEach(function(done) {
			media.onStop(function() {
				media = null;
				api = null;
				done();
			});
		});

		it('should catch repository errors on movie.add', function(done) {
			api.movie.add({}, function(err) {
				assert.equal(err);
				done();
			});
		});

		it('should catch repository errors on serie.add', function(done) {
			api.serie.add({}, function(err) {
				assert.equal(err);
				done();
			});
		});

		it('should returns all movies', function(done) {
			api.movie.getAll(function(err,movies) {
				assert.equal(err);
				assert.equal(movies.length,0);
				done();
			});
		});

		it('should returns all episodes', function(done) {
			api.serie.getSeries(function(err,series) {
				assert.equal(err);
				assert.equal(series.length,0);
				done();
			});
		});

		it('should get all series', function(done) {
			api.serie.getSeries(function(err,series) {
				assert.equal(err);
				assert.equal(series.length,0);
				done();
			});
		});

		it('should handle error if serie not exists', function(done) {
			api.serie.getEpisodes('pepe', 'season1',function(err,episodes) {
				assert.equal(err);
				assert.equal(episodes.length,0);
				done();
			});
		});

		it('should handle error on setCurrentTime (serie)', function(done) {
			api.serie.setCurrentTime( 'pepe', 202.22 ,function(err) {
				assert(err);
				done();
			});
		});

		it('should handle error on setCurrentTime (movie)', function(done) {
			api.movie.setCurrentTime( 'pepe', 202.22 ,function(err) {
				assert(err);
				done();
			});
		});

		it('should handle error on movie.markSeen', function(done) {
			api.movie.markSeen( 'pepe', false ,function(err) {
				assert(err);
				done();
			});
		});

		it('should handle error on movie.remove', function(done) {
			api.movie.remove( 'pepe' ,function(err) {
				assert(err);
				done();
			});
		});

		it('should handle error on serie.remove', function(done) {
			api.serie.remove( 'pepe' ,function(err) {
				assert(err);
				done();
			});
		});

		it('should returns undefined', function(done) {
			api.serie.getNextEpisode('pepe', function(err,episode) {
				assert.equal(err);
				assert.equal(episode);
				done();
			});
		});

		it('should handler error on movie.queueMovie', function(done) {
			api.movie.queueMovie({}, function(err) {
				assert.equal(err);
				done();
			});
		});

		it('should test movie.hasQueuedMovies', function(done) {
			api.movie.hasQueuedMovies( function(err,has) {
				assert.equal(err);
				assert.isFalse(has);
				done();
			});
		});

		it('should test movie.getQueuedMovies', function(done) {
			api.movie.getQueuedMovies( function(err,movies) {
				assert.equal(err);
				assert.equal(movies.length,0);
				done();
			});
		});

		it('should test movie.getQueuedMovies', function(done) {
			api.movie.resolveQueuedMovie( 'pepe', true, function(err,movies) {
				assert(err);
				done();
			});
		});
	});
});

