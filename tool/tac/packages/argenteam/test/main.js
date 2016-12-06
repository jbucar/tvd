'use strict';

var path = require('path');
var mockfs = require('mock-fs');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs-extra') );
var tvdutil = require('tvdutil');

var assert = require('assert');
var _ = require('lodash');
var Mocks = require('mocks');

var _mediaAPI = 'ar.edu.unlp.info.lifia.tvd.media.main';

var testInfo = {
	series: [{
		id: 85993,
		result: JSON.parse(fs.readFileSync(path.join(__dirname,'episode_85993.json')))
	}],
	movies: [{
		id: 90614,
		result: JSON.parse(fs.readFileSync(path.join(__dirname,'movie_90614.json')))
	}]
};

function getInfo( type, id ) {
	let info = testInfo[type].find( (data) => data.id === id );
	assert(info);
	return _.cloneDeep(info);
}

function MediaMock() {
	var self = {
		movie: {
			add: function(opts,cb) { cb(undefined); },
			queueMovie: function(opts,cb) { cb(undefined); }
		},
		serie: {
			add: function(opts,cb) { cb(undefined); },
		}
	};

	return self;
}

var TvdUtilMock = {
	BasicWaitCondition: tvdutil.BasicWaitCondition,
	warnErr: tvdutil.warnErr,
};

var processFeed = null;
var FeedMock = function(opts) {
	if (processFeed) {
		return processFeed(opts);
	}
	return bPromise.resolve({ items: [] });
};

describe('argenteam', function() {
	var _reg = null;
	var _adapter = null;
	var Argenteam;

	beforeEach( function() {
		processFeed = null;
		Argenteam = Mocks.mockRequire( require, '../src/main', { 'tvdutil': TvdUtilMock, 'feed': FeedMock } );

		mockfs({});
		fs.ensureDirSync('/system/data/config/net.argenteam');

		_reg = Mocks.init('silly');
		_adapter = new Mocks.ServiceAdapter(_reg,'net.argenteam');
		_reg.put( 'remoteapi', new Mocks.RemoteApi() );
		_reg.put( _mediaAPI, new MediaMock() );
	});

	afterEach(function() {
		Mocks.fin();
		_adapter = null;
		_reg = null;

		mockfs.restore();
	});

	it('should construct', function() {
		var module = new Argenteam(_adapter);
		assert( module );
	});

	it('should start/stop', function(done) {
		var module = new Argenteam(_adapter);
		module.onStart(function(err,api) {
			assert.equal(err);
			assert(api);
			module.onStop(done);
		});
	});

	it('should start/stop/start/stop', function(done) {
		var module = new Argenteam(_adapter);
		module.onStart(function(err,api) {
			assert.equal(err);
			assert(api);
			module.onStop(function() {
				module.onStart(function(err2,api2) {
					assert.equal(err2);
					assert(api2);
					assert( api !== api2 );
					module.onStop(done);
				});
			});
		});
	});

	describe( 'methods', function() {
		var module;
		var api = null;

		beforeEach( function() {
			module = new Argenteam(_adapter);
		});

		afterEach(function(done) {
			module.onStop(function() {
				api = null;
				module = null;
				done();
			});
		});

		function start(cb,load) {
			if (load) {
				_adapter.load = load;
			}
			module.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				cb();
			});
		}

		describe( 'started', function() {
			beforeEach(function(done) {
				start(done);
			});

			it( 'add/has/remove serie', function() {
				api.addSerie({
					name: 'pepe',
					filter: 'Pepe feed'
				});
				assert.equal( api.hasSerie( 'pepe' ), true );
				assert.equal( api.listSeries().length, 1 );
				api.removeSerie( 'pepe' );
				assert.equal( api.hasSerie( 'pepe' ), false );
				assert.equal( api.listSeries().length, 0 );
			});

			it( 'add twice', function() {
				api.addSerie({
					name: 'pepe',
					filter: 'Pepe feed'
				});
				assert.equal( api.hasSerie( 'pepe' ), true );
				assert.equal( api.listSeries().length, 1 );

				api.addSerie({
					name: 'pepe',
					filter: 'Pepe feed'
				});
				assert.equal( api.hasSerie( 'pepe' ), true );
				assert.equal( api.listSeries().length, 1 );
			});

			it( 'remove invalid', function() {
				api.removeSerie( 'pepe' );
				assert.equal( api.hasSerie( 'pepe' ), false );
				assert.equal( api.listSeries().length, 0 );
			});

			it( 'get availables', function(done) {
				api.getAvailableSeries(function(err,series) {
					assert.equal(err);
					assert(series);
					assert.equal(series.length, 0);
					done();
				});
			});

			describe('addID', function() {
				it( 'addMovie and test meta', function(done) {
					let movInfo = getInfo( 'movies', 90614 );

					TvdUtilMock.doRequestAsync = function(opts) {
						let res = {
							data: movInfo.result
						};
						return bPromise.resolve(res);
					};

					let movApi = _reg.get(_mediaAPI).movie;
					let addCalled=0;
					movApi.add = function( opts, cb ) {
						addCalled++;
						cb(undefined);
					};

					api.addID( 'movie', movInfo.id, function(err,info) {
						assert.equal(err);
						assert(info);

						assert.equal( addCalled, 1 );

						assert.equal(info.title, movInfo.result.info.title);
						assert.equal(info.country, movInfo.result.info.country);
						assert.equal(info.director, movInfo.result.info.director);
						assert.equal(info.actors, movInfo.result.info.actors);
						assert.equal(info.year, movInfo.result.info.year);
						assert.equal(info.rating, movInfo.result.info.rating);

						//	Test subs
						assert.equal( info.subtitles.length, 1 );

						//	Test src
						assert.equal(info.src.type,'torrent');
						assert.equal(info.src.uri,movInfo.result.releases[1].torrents[0].uri);
						done();
					});
				});

				it( 'addSerie and test meta', function(done) {
					let eInfo = getInfo( 'series', 85993 );

					TvdUtilMock.doRequestAsync = function(opts) {
						let res = {
							data: eInfo.result
						};
						return bPromise.resolve(res);
					};

					let serieApi = _reg.get(_mediaAPI).serie;
					let addCalled=0;
					serieApi.add = function( opts, cb ) {
						addCalled++;
						assert.equal( opts.src.type, 'torrent' );
						assert.equal( opts.src.uri, eInfo.result.releases[0].torrents[0].uri );
						cb(undefined);
					};

					api.addID( 'serie', eInfo.id, 'the walking thead', function(err,info) {
						assert.equal(err);
						assert(info);

						assert.equal( addCalled, 1 );

						assert.equal(info.season, eInfo.result.season);
						assert.equal(info.episode, eInfo.result.episode);

						assert.equal( info.title, eInfo.result.title);
						assert.equal( info.summary, eInfo.result.summary);

						//	Test subs
						assert.equal( info.subtitles.length, 1 );

						//	Test src
						assert.equal(info.src.type,'torrent');
						assert.equal(info.src.uri,eInfo.result.releases[0].torrents[0].uri);
						done();
					});
				});

				it( 'Should select alt uri if available', function(done) {
					let eInfo = getInfo( 'series', 85993 );
					eInfo.result.releases[0].torrents = [{
						uri: 'http://www.pepe.com'
					}, {
						uri: 'http://www.pepe.com',
						alt: 'magnet://pepe.asdfjas'
					}];

					TvdUtilMock.doRequestAsync = function(opts) {
						let res = {
							data: eInfo.result
						};
						return bPromise.resolve(res);
					};

					api.addID( 'serie', eInfo.id, 'the walking thead', function(err,info) {
						//	Test src
						assert.equal(info.src.type,'torrent');
						assert.equal(info.src.uri.indexOf('magnet:'), 0);
						done();
					});
				});

				it( 'No valid uri for a selected release', function(done) {
					let eInfo = getInfo( 'series', 85993 );
					eInfo.result.releases[0].torrents =  [{
						uri: 'http://www.pepe.com'
					}];
					eInfo.result.releases.splice(1,2);

					TvdUtilMock.doRequestAsync = function(opts) {
						let res = {
							data: eInfo.result
						};
						return bPromise.resolve(res);
					};

					api.addID( 'serie', eInfo.id, 'the walking thead', function(err,info) {
						assert(err);
						assert.equal( err.message.indexOf('No valid uri available: torrents='), 0 );
						done();
					});
				});

				it( 'Choose release fail', function(done) {
					let eInfo = getInfo( 'series', 85993 );
					eInfo.result.releases.splice(0,2);

					TvdUtilMock.doRequestAsync = function(opts) {
						let res = {
							data: eInfo.result
						};
						return bPromise.resolve(res);
					};

					api.addID( 'serie', eInfo.id, 'the walking thead', function(err,info) {
						assert(err);
						assert.equal( err.message.indexOf('No release available: id=85993'), 0 );
						done();
					});
				});

				it( 'getInfo fail', function(done) {
					let movInfo = getInfo( 'movies', 90614 );

					TvdUtilMock.doRequestAsync = function(opts) {
						return bPromise.resolve({});
					};

					api.addID( 'movie', movInfo.id, function(err) {
						assert(err);
						assert.equal(err.message,'No information available: id=90614');
						done();
					});
				});

				it( 'chooseRelease fail', function(done) {
					let movInfo = getInfo( 'movies', 90614 );

					TvdUtilMock.doRequestAsync = function(opts) {
						return bPromise.resolve({data:{}});
					};

					api.addID( 'movie', movInfo.id, function(err) {
						assert(err);
						assert.equal(err.message.indexOf('No release available: id=90614'), 0 );
						done();
					});
				});

				it( 'error on media api', function(done) {
					let movInfo = getInfo( 'movies', 90614 );

					TvdUtilMock.doRequestAsync = function(opts) {
						return bPromise.resolve({data: movInfo.result});
					};

					let movApi = _reg.get(_mediaAPI).movie;
					let addCalled=0;
					movApi.add = function( opts, cb ) {
						addCalled++;
						cb(new Error('pepe'));
					};

					api.addID( 'movie', movInfo.id, function(err) {
						assert(err);
						assert.equal(err.message.indexOf('pepe'), 0 );
						done();
					});
				});
			});

			describe('checkFeeds', function() {
				it( 'should check empty feeds', function(done) {
					processFeed = function(opts) {
						assert(opts.url);
						done();
						return bPromise.resolve({ items: [] });
					};
					api.checkFeeds();
				});

				it( 'should check with items', function(done) {
					let movInfo = getInfo( 'movies', 90614 );

					TvdUtilMock.doRequestAsync = function(opts) {
						return bPromise.resolve({data: movInfo.result});
					};

					let movApi = _reg.get(_mediaAPI).movie;
					let queueCount=0;
					movApi.queueMovie = function( opts, cb ) {
						queueCount++;
						cb(undefined);
						if (queueCount === 2) {
							done();
						}
					};

					processFeed = function(opts) {
						let items = [{
							id: 1,
							link: 'http://www.argenteam.net/episode/91475',
							pubDate: 'Fri, 01 Jul 2016 12:29:45 GMT'
						},{
							id: 2,
							title: 'Between | S02E01',
							pubDate: 'Fri, 01 Jul 2016 12:29:45 GMT'
						},{
							id: 3,
							title: 'Between',
							link: 'http://www.argenteam.net/episode/91475',
							pubDate: 'Fri, 01 Jul 2016 12:29:45 GMT'
						},{
							id: 3,
							title: '|',
							link: 'http://www.argenteam.net/episode/91475',
							pubDate: 'Fri, 01 Jul 2016 12:29:45 GMT'
						},{
							id: 3,
							title: 'Between | S02E01',
							link: 'http://www.argenteam.net/pepe/',
							pubDate: 'Fri, 01 Jul 2016 12:29:45 GMT'
						},{
							id: 4,
							title: 'Green Room',
							link: 'http://www.argenteam.net/movie/91462/Green.Room.%282015%29',
							pubDate: 'Sat, 02 Jul 2016 15:28:40 GMT'
						},{
							id: 3,
							title: 'Between | S02E01',
							link: 'http://www.pepe.com/episode/91475',
							pubDate: 'Fri, 01 Jul 2016 12:29:45 GMT'
						}];

						return bPromise.resolve({ items: items });
					};
					api.checkFeeds();
				});

				it( 'should check serie items', function(done) {
					let serInfo = getInfo( 'series', 85993 );

					TvdUtilMock.doRequestAsync = function(opts) {
						return bPromise.resolve({data: serInfo.result});
					};

					let serApi = _reg.get(_mediaAPI).serie;
					serApi.add = function( opts, cb ) {
						cb(undefined);
						done();
					};

					processFeed = function(opts) {
						var items = [];
						if (opts.url.indexOf('portal_series_subtitles.xml') >= 0) {
							items = [{
								id: 1,
								title: 'Between | S02E01',
								link: 'http://www.argenteam.net/episode/91475',
								pubDate: 'Fri, 01 Jul 2016 12:29:45 GMT'
							}];
						}

						return bPromise.resolve({ items: items });
					};

					api.addSerie({
						name: 'Between',
						filter: 'Between'
					});
					
					api.checkFeeds();					
				});

				it( 'should check for serie and fail', function(done) {
					let serInfo = getInfo( 'series', 85993 );

					TvdUtilMock.doRequestAsync = function(opts) {
						return bPromise.resolve({data: serInfo.result});
					};

					processFeed = function(opts) {
						var items = [];
						if (opts.url.indexOf('portal_series_subtitles.xml') >= 0) {
							items = [{
								id: 1,
								title: 'Between | S02E01',
								link: 'http://www.argenteam.net/episode/91475',
								pubDate: 'Fri, 01 Jul 2016 12:29:45 GMT'
							}];
						}
						else {
							done();
						}

						return bPromise.resolve({ items: items });
					};

					api.addSerie({
						name: 'Pepe',
						filter: 'pepe'
					});
					
					api.checkFeeds();					
				});				

				it( 'should handle processFeed error', function(done) {
					processFeed = function(opts) {
						done();
						return bPromise.reject(new Error('pepe'));
					};
					api.checkFeeds();
				});
			});
		});
	});
});
