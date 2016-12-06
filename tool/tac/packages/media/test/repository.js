'use strict';

var path = require('path');
var util = require('util');
var tvdutil = require('tvdutil');
var _ = require('lodash');
var bPromise = require('bluebird');
var mktemp = bPromise.promisifyAll( require('mktemp') );
var EventEmitter = require('events').EventEmitter;

var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var mockfs = require('mock-fs');
var fs = bPromise.promisifyAll( require('fs-extra') );
var Mocks = require('mocks');

var workDir = '/repositories/work';
var shareDir = '/repositories/work/share';
var srtExample = fs.readFileSync( path.join(__dirname,'example.srt') );

var TvdUtilMock = {
	unzipFile: function() {
		var dest = path.join( workDir, 'unziped.XXXXXXXX' );
		return mktemp.createDirAsync( dest );
	},
	forwardEventEmitter: tvdutil.forwardEventEmitter,
	warnErr: tvdutil.warnErr,
	findFiles: tvdutil.findFiles,
	BasicWaitCondition: tvdutil.BasicWaitCondition
};

var resolveFFProbe;
function defaultProbeInfo() {
	return {
		format: {
			duration: (124.124 * 60),
			size: 4
		},
		metadata: {
			title: 'pepe',
			genre: "gen"
		},
		streams: [{
			codec_type: 'audio',
			channels: 2,
			codec_name: 'mp3',
			profile: 'some'
		},{
			codec_type: 'video',
			codec_name: 'h264',
			profile: 'high',
			width: 1920,
			height: 1080,
			avg_frame_rate: "24000/1001",
		},{
			codec_type: 'srt',
		}]
	};
}

function ffprobeMock( uri, cb ) {
	if (!resolveFFProbe) {
		cb( undefined, defaultProbeInfo() );
	}
	else {
		resolveFFProbe(uri,cb);
	}
}

function DownloaderMock() {
	var self = {};

	var _events = new EventEmitter();
	tvdutil.forwardEventEmitter(self,_events);

	self.ct = 'image/jpeg';

	self.fetch = function( uri ) {
		return mktemp.createFileAsync( path.join('/repositories/work/tmp', 'fetch_XXXXXXXX' ) )
			.then(function(tmpFile) {
				return {
					mime: self.ct,
					path: tmpFile
				};
			});
	};

	self.enqueueFetch = function( opts ) {
		return bPromise.resolve();
	};

	return self;
}

describe('Repository', function() {
	var MockedRepository = null;
	var currDownloaderMock = null;
	var reg = null;

	function defaultOpts() {
		return {
			basePath: workDir,
			sharePath: path.join(workDir,'share'),
			downloader: currDownloaderMock,
			supportedFiles: {
				subtitle: [ '.srt', '.vtt' ],
				video: [ '.avi', '.mkv', '.mp4' ]
			}
		};
	}

	function startRepo( opts ) {
		let repoOpts = opts || defaultOpts();
		let repo = new MockedRepository( repoOpts );
		//repo.on( 'error', function() {} );
		return repo.start().then( () => repo );
	}

	function stopRepo(repo) {
		return repo.stop();
	}

	beforeEach(function() {
		reg = Mocks.init('silly');

		currDownloaderMock = new DownloaderMock();
		resolveFFProbe = null;

		MockedRepository = Mocks.mockRequire( require, '../src/repository', { 'tvdutil': TvdUtilMock, 'node-ffprobe': ffprobeMock } );

		var sfs = {};
		mockfs(sfs);

		//	Create work dir
		fs.ensureDirSync( workDir );
		fs.ensureDirSync( shareDir );
	});

	afterEach(function() {
		mockfs.restore();
		Mocks.fin();
		MockedRepository = null;
		reg = null;
	});

	describe('constructor', function() {
		it('should construct basic', function(done) {
			startRepo().then(function(repo) {
				assert.equal(Object.keys(repo).length,19);
				assert(repo.start);
				assert(repo.stop);
				assert(repo.addMovie);
				assert(repo.getMovies);
				assert(repo.queueMovie);
				assert(repo.getQueuedMovies);
				assert(repo.resolveQueuedMovie);
				assert(repo.addEpisode);
				assert(repo.on);
				stopRepo(repo).then(done);
			});
		});
	});

	describe('methods', function() {
		function tempPath() {
			return path.join(workDir,'tmp');
		}

		function moviePath( movieName ) {
			return path.join(shareDir,'movies', movieName );
		}

		function movieMeta( movieName ) {
			return path.join(shareDir,'movies', movieName, 'meta.json' );
		}

		function zeroFill(n) {
			return ('0'+n).slice(-2);
		}

		function serieId( serieName ) {
			return path.join(shareDir,'series', serieName );
		}

		function seriePath( serieName ) {
			return path.join(shareDir,'series', serieName);
		}

		function serieMeta( serieName, season, episode ) {
			let nSeason = season ? zeroFill(season) : '01';
			let nEpisode = episode ? zeroFill(episode) : '01';
			return path.join(shareDir,'series', serieName, nSeason, nEpisode, 'meta.json' );
		}

		let pulpMeta = movieMeta( 'Pulp fiction' );

		function createDir( dir ) {
			if (dir === '/repositories/work') {
				return;
			}
			createDir( path.dirname(dir) );
			try {
				fs.mkdirSync( dir );
			} catch(err) {};
		}

		afterEach(function() {
			if (fs.exists( tempPath() )) {
				assert.equal( fs.readdirSync( tempPath() ).length, 0 );
			}
		});

		it('should start without download to resume (movies)', function(done) {
			let meta = {
				subtitles: [],
				video: {
					src: { type: 'file', uri: 'pepe.avi' },
					size: 4
				}
			};

			createDir( path.dirname(pulpMeta) );
			fs.writeFileSync( pulpMeta, JSON.stringify(meta, null, 4) );

			var repoOpts = defaultOpts();
			startRepo(repoOpts)
				.then(function(repo) {
					return repo.getMovies()
						.then(function(all) {
							assert.equal( all.length, 1 );
							done();
						});
				});
		});

		it('Should load all series', function(done) {
			let serie1 = serieMeta('pepe');
			let serie2 = serieMeta('pepe1');

			createDir( path.dirname(serie1) );
			createDir( path.dirname(serie2) );

			let serieMetaInfo = {
				title: 'The pepe',
				summary: 'The pepe is ...',
				logo: 'logo.png'
			};
			let s1Path = seriePath('pepe');
			let s2Path = seriePath('pepe1');
			fs.writeFileSync( path.join(s1Path,'meta.json'), JSON.stringify(serieMetaInfo, null, 4) );

			for (let s=0; s<5; s++) {
				for (let e=0; e<10; e++) {
					let episodeMetaInfo = {
						serie: 'pepe',
						subtitles: [],
						video: {
							src: {
								type: 'file',
								uri: 'pepe.avi'
							},
							size: 4
						}
					};

					let episodePath = serieMeta('pepe', s+1, e+1);
					createDir( path.dirname(episodePath) );

					//	Create episode info
					fs.writeFileSync( episodePath, JSON.stringify(episodeMetaInfo, null, 4) );
				}
			}

			var repoOpts = defaultOpts();
			startRepo(repoOpts).then(function(repo) {
				repo.getSeries()
					.then(function(series) {
						assert.equal( series.length, 2 );

						assert.deepEqual( series[0], { id: s1Path, title: 'The pepe', summary: 'The pepe is ...', logo: 'logo.png', seasons: ['01', '02', '03', '04', '05'] } );
						let seasons = series[0].seasons;
						assert.equal( seasons.length, 5 );

						assert.deepEqual( series[1], { id: s2Path, title: 'pepe1', seasons: ["01"] } );

						let proms = [];
						for (let s=0; s<5; s++) {
							assert.equal( seasons[s], zeroFill(s+1) );
							let prom = repo.getEpisodes( series[0].id, seasons[s] )
								.then(function(episodes) {
									assert.equal( episodes.length, 10 );
									for (let e=0; e<10; e++) {
										assert.equal( episodes[e].video.src.uri, 'file://' + path.join(path.dirname(serieMeta('pepe',s+1, e+1)),'pepe.avi') );
										assert.deepEqual( episodes[e], {
											id: path.join(path.dirname(serieMeta('pepe',s+1, e+1))),
											subtitles: [],
											video: {
												src: {
													type: 'file',
													uri: 'file://' + path.join(path.dirname(serieMeta('pepe',s+1, e+1)),'pepe.avi'),
												},
												size: 4
											},
											serie: 'pepe'
										});
									}
								});

							proms.push(prom);
						}

						bPromise.all(proms)
							.then( () => stopRepo(repo) )
							.then( () => done() );
					});
			});
		});

		it('should returns movies', function(done) {
			let meta = {
				subtitles: [],
				video: {
					src: { type: 'file', uri: 'pepe.avi' },
					size: 4
				}
			};

			createDir( path.dirname(pulpMeta) );
			fs.writeFileSync( pulpMeta, JSON.stringify(meta, null, 4) );

			var repoOpts = defaultOpts();
			startRepo(repoOpts).then(function(repo) {
				repo.getMovies().then(function(movies) {
					assert.isArray(movies);
					assert.equal(movies.length,1);
					stopRepo(repo).then(done);
				});
			});
		});

		it('should ignore invalid movies', function(done) {
			function setup( name, m ) {
				let movPath = moviePath(name);
				createDir( movPath );
				fs.writeFileSync( movieMeta(name), m );
			}

			let meta = {
				subtitles: [],
				video: {
					src: { type: 'file', uri: 'pepe.avi' },
					size: 4
				}
			};

			setup( 'movie1', '{adsfa' );
			setup( 'movie2', JSON.stringify(meta,null,4) );

			var repoOpts = defaultOpts();
			startRepo(repoOpts).then(function(repo) {
				repo.getMovies().then(function(movies) {
					assert.isArray(movies);
					assert.equal(movies.length,1);
					stopRepo(repo).then(done);
				});
			});
		});

		describe( 'methods (already started)', function() {
			var repo;
			var repoOpts;

			beforeEach(function(done) {
				repoOpts = defaultOpts();
				startRepo(repoOpts).then(function(rep) {
					assert(rep);
					repo = rep;
					done();
				});
			});

			afterEach(function(done) {
				stopRepo(repo).then(done);
			});

			function movieInput( src ) {
				return {
					src: src,
					title: 'Pulp fiction',
					size: 190
				};
			}

			function episodeInput( src ) {
				return {
					src: src,
					serie: 'the_walking_dead'
				};
			}

			function movieBasicInput() {
				return movieInput({
					type: 'basic',
					uri: 'http://www.pepe.com',
				});
			}

			function movieFileInput() {
				return movieInput({
					type: 'file',
					uri: '/repositories/Pulp fiction.avi',
				});
			}

			function episodeLocalInput() {
				return episodeInput({
					type: 'file',
					uri: '/repositories/the_walking_dead.avi',
				});
			}

			function movieTorrentInput() {
				return movieInput({
					type: 'torrent',
					uri: 'magnet:?xt=urn:btih:c12fe1c06bba254a9dc9f519b335aa7c1367a88a&dn',
				});
			}

			describe( 'addMovie', function() {
				it('should not add a movie if no uri present', function(done) {
					repo.addMovie( {} ).catch(function(err) {
						assert(err);
						assert.equal(err.message,'Cannot add movie; No src defined');
						done();
					});
				});

				it('should not create working directory', function(done) {
					var info = movieBasicInput();
					tvdutil.removeTreeSync( workDir );

					repo.addMovie( info ).catch(function(err) {
						assert(err);
						assert(err.message.indexOf('ENOENT, no such file or directory') === 0);
						done();
					});
				});

				it('should add a local (file://) movie', function(done) {
					let aviName = 'Pulp fiction.avi';
					var input = movieInput({
						type: 'file',
						uri: 'file:///repositories/' + aviName
					});
					fs.writeFileSync( '/repositories/Pulp fiction.avi', 'pepe' );
					repo.addMovie(input).then(function() {
						var info = JSON.parse(fs.readFileSync( pulpMeta ));
						assert.equal( info.title, input.title );

						assert.equal( info.video.src.type, 'file' );
						assert.equal( info.video.src.uri, aviName );
						assert.equal( info.video.size, 4 );

						assert.equal( info.subtitles.length, 0 );


						repo.getMovies().then(function(movies) {
							assert.equal( movies.length, 1 );
							assert.equal( movies[0].video.src.uri, 'file://' + path.join(moviePath('Pulp fiction'),aviName) );

							done();
						});
					});
				});

				it('should add a remote movie without download', function(done) {
					var input = movieBasicInput();
					input.download = false;
					repo.addMovie(input).then(function() {
						var info = JSON.parse(fs.readFileSync( pulpMeta ));
						assert.equal( info.title, input.title );

						assert.equal( info.video.src.uri, input.src.uri );
						assert.equal( info.video.size, 190 );

						assert.equal( info.subtitles.length, 0 );


						repo.getMovies().then(function(movies) {
							assert.equal( movies.length, 1 );
							assert.equal( movies[0].video.src.uri, input.src.uri );

							done();
						});
					});
				});

				it('should add a remote movie (torrent) and download', function(done) {
					let hash = 'C12FE1C06BBA254A9DC9F519B335AA7C1367A88A';
					var input = movieTorrentInput();

					let torrentPath = '/repositories/test_movie';
					let movieUri = path.join(torrentPath,'movie_pulp_fiction.avi');
					fs.mkdirSync( torrentPath );
					fs.writeFileSync( movieUri, 'pepe' );
					fs.writeFileSync( path.join(torrentPath,'readme.txt') );

					let curDownloadID;
					currDownloaderMock.enqueueFetch = function( opts ) {
						assert.equal( opts.type, input.src.type );
						assert.equal( opts.uri, input.src.uri );
						assert( opts.id );
						curDownloadID = opts.id;
						return bPromise.resolve();
					};

					repo.on( 'movieAdded', function(meta) {
						var info = JSON.parse( fs.readFileSync( pulpMeta ));
						assert.equal( info.title, input.title );

						assert.equal( info.subtitles.length, 0 );

						assert.equal( path.basename(meta.video.src.uri), path.basename(movieUri) );
						assert.equal( meta.video.size, 4 );

						assert.equal( fs.existsSync( torrentPath ), false );

						done();
					});

					repo.addMovie(input).then(function() {
						assert(curDownloadID);
						currDownloaderMock.emit( 'completed', {
							path: torrentPath,
							id: curDownloadID
						});
					});
				});

				it('should add a remote movie (torrent), try download and fail when complete processed files', function(done) {
					let hash = 'C12FE1C06BBA254A9DC9F519B335AA7C1367A88A';
					var input = movieTorrentInput();
					input.download = true;

					let torrentPath = '/repositories/test_movie';
					let movieUri = path.join(torrentPath,'movie_pulp_fiction.avi');
					fs.mkdirSync( torrentPath );
					fs.writeFileSync( movieUri, 'pepe' );
					fs.writeFileSync( path.join(torrentPath,'readme.txt') );

					let curDownloadID;
					currDownloaderMock.enqueueFetch = function( opts ) {
						assert.equal( opts.type, input.src.type );
						assert.equal( opts.uri, input.src.uri );
						assert( opts.id );
						curDownloadID = opts.id;
						return bPromise.resolve();
					};

					repo.on( 'error', function(err) {
						assert.equal( fs.existsSync( torrentPath ), false );
						assert.equal( fs.existsSync( curDownloadID ), false );
						done();
					});

					repo.addMovie(input).then(function() {
						assert(curDownloadID);
						fs.unlinkSync( path.join(torrentPath,'readme.txt') );
						fs.unlinkSync( movieUri );
						fs.rmdirSync( torrentPath );
						currDownloaderMock.emit( 'completed', {
							path: torrentPath,
							id: curDownloadID
						});
					});
				});

				it('should add a remote movie (torrent), try download and fail', function(done) {
					var input = movieTorrentInput();
					input.download = true;

					let torrentPath = '/repositories/test_movie';
					let movieUri = path.join(torrentPath,'movie_pulp_fiction.avi');
					fs.mkdirSync( torrentPath );
					fs.writeFileSync( movieUri, 'pepe' );
					fs.writeFileSync( path.join(torrentPath,'readme.txt') );

					currDownloaderMock.enqueueFetch = function( opts ) {
						return bPromise.reject( new Error('pepe') );
					};

					repo.addMovie(input).catch(function(err) {
						assert(err);
						assert.equal( err.message, 'pepe' );
						done();
					});
				});

				it('should add a remote movie (torrent), try download and fail2', function(done) {
					var input = movieTorrentInput();
					input.download = true;

					let torrentPath = '/repositories/test_movie';
					let movieUri = path.join(torrentPath,'movie_pulp_fiction.avi');
					fs.mkdirSync( torrentPath );
					fs.writeFileSync( movieUri, 'pepe' );
					fs.writeFileSync( path.join(torrentPath,'readme.txt') );

					let torrentID;
					currDownloaderMock.enqueueFetch = function( opts ) {
						torrentID = opts.id;
						return bPromise.resolve();
					};

					repo.on('error', function(err) {
						assert(torrentID);
						assert(err);
						assert.equal( err.message, 'pepe' );
						done();
					});

					repo.addMovie(input).then(function() {
						assert(torrentID);
						currDownloaderMock.emit( 'completed', {
							id: torrentID,
							err: new Error('pepe')
						});
					});
				});

				it('should add a remote movie with multiple files via magnet', function(done) {
					let hash = 'C12FE1C06BBA254A9DC9F519B335AA7C1367A88A';
					var input = movieTorrentInput();
					input.download = true;
					input.convertToVTT = false;

					let torrentPath = '/repositories/test_movie';
					let movieUri = 'movie_pulp_fiction.avi';
					let testMovieUri = 'test_pulp_fiction.avi';
					let movieSrt = 'movie_pulp_fiction.srt';
					let movieInfo = 'the.beauty.inside.2015.bdrip.x264-rovers.nfo';
					let movieSubIdx = 'The.Beauty.Inside.2015.BDRip.x264-ROVERS.idx';
					let movieSub = 'The.Beauty.Inside.2015.BDRip.x264-ROVERS.sub';

					fs.mkdirSync( torrentPath );
					fs.writeFileSync( path.join(torrentPath,movieUri), 'pepe_mas_grande!' );
					fs.writeFileSync( path.join(torrentPath,movieInfo), 'info'  );

					fs.mkdirSync( path.join(torrentPath,'test') );
					fs.writeFileSync( path.join(torrentPath,'test',testMovieUri), 'pepe' );

					fs.mkdirSync( path.join(torrentPath,'sub') );
					fs.writeFileSync( path.join(torrentPath,'sub',movieSrt), srtExample  );
					fs.writeFileSync( path.join(torrentPath,'sub',movieSubIdx), 'subidx'  );
					fs.writeFileSync( path.join(torrentPath,'sub',movieSub), 'sub'  );

					let curDownloadID;
					currDownloaderMock.enqueueFetch = function( opts ) {
						assert.equal( opts.type, input.src.type );
						assert.equal( opts.uri, input.src.uri );
						assert( opts.id );
						curDownloadID = opts.id;
						return bPromise.resolve();
					};

					repo.on( 'movieAdded', function(meta) {
						var info = JSON.parse(fs.readFileSync( pulpMeta ));
						assert.equal( info.title, input.title );

						assert.equal( path.basename(meta.video.src.uri), path.basename(movieUri) );
						assert.equal( meta.video.size, 4 );
						assert.equal( meta.video.hash );

						assert.equal( meta.subtitles.length, 1 );

						done();
					});

					repo.addMovie(input).then(function() {
						assert(curDownloadID);
						currDownloaderMock.emit( 'completed', {
							path: torrentPath,
							id: curDownloadID
						});
					});
				});
			});

			describe( 'addEpisode', function() {
				it('should add a local episode', function(done) {
					var input = episodeLocalInput();

					bPromise.join(
						repo.getNextEpisode(input.serie),
						repo.getSeries(),
						repo.getEpisodes('pepe','pepe1'),
						function(nextEpisode,series, episodes) {
							assert.deepEqual( nextEpisode );
							assert.deepEqual( series, [] );
							assert.deepEqual( episodes, [] );

							fs.writeFileSync( input.src.uri, 'pepe' );
							repo.addEpisode(input).then(function() {
								var info = JSON.parse(fs.readFileSync( serieMeta('the_walking_dead') ));

								assert.equal( path.basename(info.video.src.uri), path.basename(input.src.uri) );
								assert.equal( info.video.size, 4 );

								assert.equal( info.subtitles.length, 0 );
								done();
							});
						});
				});

				it('should add a local episode with existing serie', function(done) {
					var input = episodeLocalInput();

					fs.ensureDirSync( seriePath('the_walking_dead') );

					bPromise.join(
						repo.getNextEpisode(input.serie),
						repo.getSeries(),
						repo.getEpisodes('pepe','pepe1'),
						function(nextEpisode,series, episodes) {
							assert.deepEqual( nextEpisode );
							assert.deepEqual( series.length, 1 );
							assert.deepEqual( episodes, [] );

							fs.writeFileSync( input.src.uri, 'pepe' );
							repo.addEpisode(input).then(function() {
								var info = JSON.parse(fs.readFileSync( serieMeta('the_walking_dead') ));

								assert.equal( path.basename(info.video.src.uri), path.basename(input.src.uri) );
								assert.equal( info.video.size, 4 );

								assert.equal( info.subtitles.length, 0 );
								done();
							});
						});
				});

				it('should add a remote episode && download (by default)', function(done) {
					let input = episodeInput({
						type: 'torrent',
						uri: 'magnet:?xt=urn:btih:c12fe1c06bba254a9dc9f519b335aa7c1367a88a&dn',
					});

					let torrentPath = '/repositories/test_movie';
					let movieUri = path.join(torrentPath,'movie_pulp_fiction.avi');
					fs.mkdirSync( torrentPath );
					fs.writeFileSync( movieUri, 'pepe' );
					fs.writeFileSync( path.join(torrentPath,'readme.txt') );

					let torrentID;
					currDownloaderMock.enqueueFetch = function( opts ) {
						torrentID = opts.id;
						return bPromise.resolve();
					};

					repo.on( 'serieAdded', function(meta) {
						repo.getSeries()
							.then(function(series) {
								assert.equal( series.length, 1 );
								repo.getEpisodes( series[0].id,'01')
									.then(function(episodes) {
										assert.equal( episodes.length, 1 );
										let episode = episodes[0];
										assert( episode.video.src.uri.indexOf('/repositories') >= 0 );
										done();
									});
							});
					});

					repo.addEpisode(input).then(function() {
						currDownloaderMock.emit( 'completed', {
							path: torrentPath,
							id: torrentID
						});
					});
				});

				it('should add a local episode with season && episode numbers', function(done) {
					var input = episodeLocalInput();
					input.season = 1;
					input.episode = 5;

					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addEpisode(input).then(function() {
						var info = JSON.parse(fs.readFileSync( serieMeta(input.serie, '01', '05') ));

						assert.equal( path.basename(info.video.src.uri), path.basename(input.src.uri) );
						assert.equal( info.video.size, 4 );

						assert.equal( info.subtitles.length, 0 );
						done();
					});
				});

				it('should fail add a local episode', function(done) {
					var input = episodeLocalInput();
					delete input.serie;
					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addEpisode(input).catch(function(err) {
						assert(err);
						assert.equal(err.message, 'Cannot add serie; No serie name defined' );
						done();
					});
				});
			});

			describe('poster', function() {
				it('should add a movie with a remote poster', function(done) {
					currDownloaderMock.ct = 'image/jpeg';

					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.poster = 'http://www.pepe.com/asdfadsf.jpg';

					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( pulpMeta ));

						assert.equal( info.poster, 'poster.jpg' );

						repo.getMovies().then(function(movies) {
							assert.equal(movies.length,1);
							let movie = movies[0];

							//	Should add abs path
							assert(movie.poster !== 'poster.jpg' );
							assert.equal( path.basename(movie.poster), path.basename('poster.jpg') );

							done();
						});
					});
				});

				it('should add a movie with a png as remote poster', function(done) {
					currDownloaderMock.ct = 'image/png';

					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.poster = 'http://www.pepe.com/asdfadsf.png';
					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( pulpMeta ));

						assert.equal( info.poster, 'poster.png' );

						done();
					});
				});

				it('should add a movie with a unknown type remote poster', function(done) {
					currDownloaderMock.ct = 'image/pepe';

					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.poster = 'http://www.pepe.com/asdfadsf.png';
					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( pulpMeta ));

						assert.equal( info.poster, 'poster.png' );

						done();
					});
				});

				it('should add a movie with a invalid remote poster', function(done) {
					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.poster = 'pepe://www.pepe.com/asdfadsf.jpg';
					repo.addMovie(input).catch(function(err) {
						assert.equal( err.message, 'Cannot download resource; Invalid uri: uri=pepe://www.pepe.com/asdfadsf.jpg' );
						done();
					});
				});

				it('should add a movie with a local poster', function(done) {
					let img = '/repositories/pepe.jpg';
					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.poster = img;
					fs.writeFileSync( img, {} );
					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( pulpMeta ));
						assert.equal( path.basename(info.poster), path.basename(img) );

						done();
					});
				});
			});

			describe( 'subtitles', function() {
				it('should add a movie with a remote subtitle', function(done) {
					TvdUtilMock.unzipFile = function(req) {
						var dest = path.join( workDir, 'unziped.XXXXXXXX' );
						return mktemp.createDirAsync( dest )
							.then(function(outDir) {
								var proms = [];
								proms.push( fs.writeFileAsync( path.join(outDir,'pepe.srt'), 'data1' ) );
								proms.push( fs.writeFileAsync( path.join(outDir,'pepe.xua'), 'data2' ) );
								return bPromise.all(proms).then( () => outDir );
							});
					}
					currDownloaderMock.ct = 'application/zip';

					var input = movieFileInput();
					input.subtitles = [{
						uri: 'http://www.pepe.com/subtitle'
					}];
					input.convertToVTT = false;
					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( pulpMeta ));
						assert.equal( info.title, input.title );

						assert.equal( info.subtitles.length, 1 );
						assert.equal( info.subtitles[0].uri, 'pepe.srt' );
						assert.isTrue( tvdutil.isFile( path.join(basePath,info.subtitles[0].uri) ) );

						assert.equal( fs.readdirSync( tempPath() ).length, 0 );

						done();
					});
				});

				it('should add a movie with a remote subtitle without download', function(done) {
					var input = movieFileInput();
					input.subtitles = [{
						uri: 'http://www.pepe.com/subtitle'
					}];
					input.download = false;
					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( pulpMeta ));
						assert.equal( info.title, input.title );

						assert.equal( info.subtitles.length, 1 );
						assert.equal( info.subtitles[0].uri, input.subtitles[0].uri );
						assert.isFalse( tvdutil.isFile( path.join(basePath,info.subtitles[0].uri) ) );

						assert.equal( fs.readdirSync( tempPath() ).length, 0 );

						done();
					});
				});

				it('should add a movie with a invalid subtitle', function(done) {
					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.subtitles = [{ lang: 'Español' }];
					repo.addMovie(input).catch(function(err) {
						assert(err);
						assert.equal( err.message, 'Invalid uri: uri=undefined' );
						done();
					});
				});

				it('should add a movie with a local subtitle', function(done) {
					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.subtitles = [{
						uri: '/repositories/pepe.srt'
					}, {
						uri: 'file:///repositories/pepe2.srt'
					}];
					input.convertToVTT = false;

					fs.writeFileSync( '/repositories/pepe.srt', {} );
					fs.writeFileSync( '/repositories/pepe2.srt', {} );

					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( pulpMeta ));
						assert.equal( info.title, input.title );

						assert.equal( info.subtitles.length, 2 );
						assert.equal( info.subtitles[0].uri, 'pepe.srt' );
						assert.isTrue( tvdutil.isFile( path.join(basePath,info.subtitles[0].uri) ) );

						assert.equal( info.subtitles[1].uri, 'pepe2.srt' );
						assert.isTrue( tvdutil.isFile( path.join(basePath,info.subtitles[1].uri) ) );
						done();
					});
				});

				it('should catch error when subtitle not exists', function(done) {
					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.subtitles = [{
						uri: 'file:///repositories/pepe.srt'
					}];
					repo.addMovie(input).catch(function(err) {
						assert(err);
						assert.equal( err.code, 'ENOENT' );
						done();
					});
				});

				it('should add a movie with two local subtitles (with the same name)', function(done) {
					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.subtitles = [{
						uri: 'file:///repositories/pepe2.srt'
					}, {
						uri: 'file:///pepe2.srt'
					}];
					input.convertToVTT = false;

					fs.writeFileSync( '/repositories/pepe2.srt', {} );
					fs.writeFileSync( '/pepe2.srt', {} );

					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( pulpMeta ));
						assert.equal( info.title, input.title );

						assert.equal( info.subtitles.length, 2 );
						assert.equal( info.subtitles[0].uri, 'pepe2.srt' );
						assert.isTrue( tvdutil.isFile( path.join(basePath,info.subtitles[0].uri) ) );

						assert.equal( info.subtitles[1].uri, '02-pepe2.srt' );
						assert.isTrue( tvdutil.isFile( path.join(basePath,info.subtitles[1].uri) ) );
						done();
					});
				});

				it('should add a movie with a local subtitle and convert srt to vtt', function(done) {
					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.subtitles = [{
						uri: '/repositories/pepe.srt'
					}];
					input.convertToVTT = true;
					fs.writeFileSync( '/repositories/pepe.srt', srtExample );

					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( path.join(basePath,'meta.json') ));
						assert.equal( info.title, input.title );

						assert.equal( info.subtitles.length, 2 );
						assert.equal( info.subtitles[0].uri, 'pepe.srt' );
						assert.isTrue( tvdutil.isFile( path.join(basePath,info.subtitles[0].uri) ) );

						assert.equal( info.subtitles[1].uri, 'pepe.vtt' );
						assert.isTrue( tvdutil.isFile( path.join(basePath,info.subtitles[1].uri) ) );
						done();
					});
				});

				it('should add a movie with a vtt local subtitle', function(done) {
					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.subtitles = [{
						uri: '/repositories/pepe.vtt'
					}];
					fs.writeFileSync( '/repositories/pepe.vtt', {} );

					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( path.join(basePath,'meta.json') ));
						assert.equal( info.title, input.title );

						assert.equal( info.subtitles.length, 1 );
						assert.equal( info.subtitles[0].uri, 'pepe.vtt' );
						assert.isTrue( tvdutil.isFile( path.join(basePath,info.subtitles[0].uri) ) );

						done();
					});
				});

				it('should add a movie with a unknown local subtitle', function(done) {
					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.subtitles = [{
						uri: '/repositories/pepe.xxx'
					}];
					fs.writeFileSync( '/repositories/pepe.xxx', {} );

					repo.addMovie(input).then(function() {
						var basePath = moviePath('Pulp fiction');
						var info = JSON.parse(fs.readFileSync( path.join(basePath,'meta.json') ));
						assert.equal( info.title, input.title );

						assert.equal( info.subtitles.length, 1 );
						assert.equal( info.subtitles[0].uri, 'pepe.xxx' );
						assert.isTrue( tvdutil.isFile( path.join(basePath,info.subtitles[0].uri) ) );

						done();
					});
				});

				it('should no add subtitle if uri not supported', function(done) {
					var input = movieFileInput();
					fs.writeFileSync( input.src.uri, 'pepe' );
					input.subtitles = [{uri: 'xxx:///repositories/pepe.srt'}];
					repo.addMovie(input).catch(function(err) {
						assert(err);
						assert.equal(err.message,'Invalid uri: uri=xxx:///repositories/pepe.srt');
						done();
					});
				});

				it('should handle error if packed subtitle isnt available', function(done) {
					currDownloaderMock.ct = 'application/pepe';
					var input = movieBasicInput();
					input.subtitles = [{
						uri: 'http://www.pepe.com/subtitle'
					}];

					repo.addMovie(input).catch(function(err) {
						assert(err);
						assert.equal(err.message,'Unsupported content type: mime=application/pepe');
						done();
					});
				});

				it('should handle mime with charset', function(done) {
					currDownloaderMock.ct = 'application/zip; charset=iso-8859-7';

					var input = movieBasicInput();
					input.subtitles = [{
						uri: 'http://www.pepe.com/subtitle'
					}];

					repo.addMovie(input).then(function() {
						done();
					});
				});
			});

			describe( 'ffprobe', function() {
				var input;
				beforeEach(function() {
					input = {
						src: { type: 'file', uri: '/repositories/movie_pulp_fiction.avi' },
					};
				});

				it('should add a local movie and get info from ffprobe', function(done) {
					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addMovie(input).then(function() {
						var info = JSON.parse(fs.readFileSync( movieMeta('pepe') ));

						assert.equal( info.runtime, 7447 );
						assert.equal( info.title, 'pepe' );
						assert.equal( info.genre, 'gen' );

						assert.equal( info.video.size, 4 );
						assert.equal( path.basename(info.video.src.uri), path.basename(input.src.uri) );

						assert.equal( info.video.codecs.video.name, 'h264' );
						assert.equal( info.video.codecs.video.profile, 'high' );
						assert.equal( info.video.codecs.video.width, 1920 );
						assert.equal( info.video.codecs.video.height, 1080 );

						assert.equal( info.video.codecs.audio.name, 'mp3' );
						assert.equal( info.video.codecs.audio.profile, 'some' );
						assert.equal( info.video.codecs.audio.channels, 2 );

						assert.equal( info.subtitles.length, 0 );
						done();
					});
				});

				it('should add a local movie and fail in ffprobe', function(done) {
					resolveFFProbe = function( f, cb ) {
						cb( new Error('pepe') );
					};

					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addMovie(input).catch(function(err) {
						assert(err);
						assert.equal( err.message, 'pepe' );
						done();
					});
				});

				it('should add a local movie and empty info', function(done) {
					resolveFFProbe = function( f, cb ) {
						cb( undefined, {} );
					};

					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addMovie(input).then(function(err) {
						var info = JSON.parse(fs.readFileSync( movieMeta('movie_pulp_fiction') ));
						done();
					});
				});

				it('should add a local movie and some empty info', function(done) {
					resolveFFProbe = function( f, cb ) {
						let info = defaultProbeInfo();
						info.metadata.title = undefined;
						cb( undefined, info );
					};

					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addMovie(input).then(function() {
						var info = JSON.parse(fs.readFileSync( movieMeta('movie_pulp_fiction') ));
						done();
					});
				});

				it('should not overwrite metadata info', function(done) {
					input.title = 'the pepe';

					resolveFFProbe = function( f, cb ) {
						cb( undefined, defaultProbeInfo() );
					};

					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addMovie(input).then(function() {
						var info = JSON.parse(fs.readFileSync( movieMeta('the pepe') ));
						assert.equal( info.title, 'the pepe' );
						done();
					});
				});
			});

			describe( 'with a movie', function() {
				let movieInfo;
				var input = {
					src: {
						type: 'file',
						uri: '/repositories/movie_pulp_fiction.avi',
					},
					runtime: 120*60
				};

				function getMovieInfo() {
					return repo.getMovies().then(function(movies) {
						assert.equal( movies.length, 1 );
						return movies[0];
					});
				}

				beforeEach(function(done) {
					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addMovie(input).then(function() {
						getMovieInfo().then(function(info) {
							movieInfo = info;
							done();
						});
					});
				});

				it('should remove a movie', function(done) {
					assert.isTrue( fs.existsSync(movieMeta('pepe')) );
					repo.remove( 'movie', movieInfo.id )
						.then( () => repo.getMovies() )
						.then(function(movs) {
							assert.equal( movs.length, 0 );
							assert.isFalse( fs.existsSync(movieMeta('movie_pulp_fiction')) );
							done();
						});
				});

				it('should throw if setCurrentTime a invalid movie', function(done) {
					repo.setCurrentTime( 'movie', 'pepe' )
						.catch(function(err) {
							assert(err);
							assert( err.message.indexOf('Video not exists:') === 0 );
							done();
						});
				});

				it('should set currentTime to a movie and not set as seen', function(done) {
					assert.equal( movieInfo.currentTime, 0 );
					repo.setCurrentTime( 'movie', movieInfo.id, 10.200 )
						.then(getMovieInfo)
						.then(function(movie2) {
							assert.equal( movie2.currentTime, 10.200 );
							assert.equal( movie2.seen, false );
							done();
						});
				});

				it('should set currentTime to a movie and set as seen', function(done) {
					assert.equal( movieInfo.currentTime, 0 );
					repo.setCurrentTime( 'movie', movieInfo.id, 7000 )
						.then(getMovieInfo)
						.then(function(movie2) {
							assert.equal( movie2.currentTime );
							assert.equal( movie2.seen, true );

							done();
						});
				});

				it('should set seen state to a movie', function(done) {
					assert.equal( movieInfo.seen, false );
					repo.markSeen( 'movie', movieInfo.id, true )
						.then(getMovieInfo)
						.then(function(movie2) {
							assert.equal( movie2.seen, true );
							done();
						});
				});

				it('should throw error if already exists', function(done) {
					fs.writeFileSync( input.src.uri, 'pepe' );
					repo.addMovie(input).catch(function(err) {
						assert( err );
						assert( err.message.indexOf('Video already exists:') === 0);
						done();
					});
				});
			});

			describe('queueMovie', function() {
				it( 'should queue a local movie', function(done) {
					var input = {
						src: {
							type: 'file',
							uri: '/repositories/movie_pulp_fiction.avi',
						},
						subtitles: [{
							uri: '/repositories/pepe.srt'
						}]
					};

					fs.writeFileSync( input.src.uri, 'pepe' );
					fs.writeFileSync( input.subtitles[0].uri, 'pepe' );

					repo.queueMovie(input).then(function(){
						return bPromise.join(
							repo.hasQueuedMovies(),
							repo.getQueuedMovies(),
							function(has,movies) {
								assert.equal( has, true );
								assert.equal( movies.length, 1 );

								let mov = movies[0];
								assert.equal( path.basename(mov.video.src.uri.substring(7)), path.basename(input.src.uri) );
								assert.equal( mov.subtitles.length, 1 );
								assert.equal( path.basename(mov.subtitles[0].uri.substring(7)), path.basename(input.subtitles[0].uri) );
								done();
							});
					});
				});

				it( 'should queue and add a remote movie', function(done) {
					currDownloaderMock.ct = 'application/zip';

					var input = {
						poster: 'http://pepe.com',
						src: {
							type: 'basic',
							uri: 'http://pepe.com/movie_pulp_fiction.avi',
						},
						subtitles: [{
							uri: 'http://pepe.com/repositories/pepe.srt'
						}]
					};

					repo.queueMovie(input).then(function(){
						repo.getQueuedMovies().then(function(movies) {
							assert.equal( movies.length, 1 );
							let mov = movies[0];
							assert.equal( mov.video.src.uri, input.src.uri );
							assert.equal( fs.existsSync( path.dirname(mov.poster.substring(7)) ), true );
							assert.equal( mov.poster, 'file:///repositories/work/queued/movie_pulp_fiction/poster.png' );

							repo.resolveQueuedMovie( mov.id, true )
								.then(repo.getMovies)
								.then(function(movies) {
									assert.equal( movies.length, 0 );

									assert.equal( fs.existsSync( path.dirname(mov.poster.substring(7)) ), false );
									
									done();
								});
						});
					});
				});

				it( 'should queue and add a local movie', function(done) {
					var input = {
						country: 'countryArg',
						year: 2004,
						poster: '/repositories/pepe.png',
						src: {
							type: 'file',
							uri: '/repositories/movie_pulp_fiction.avi',
						},
						subtitles: [{
							uri: '/repositories/pepe.srt'
						}]
					};

					fs.writeFileSync( input.src.uri, 'pepe' );
					fs.writeFileSync( input.subtitles[0].uri, 'pepe' );
					fs.writeFileSync( input.poster, 'pepe' );

					repo.queueMovie(input).then(function(){
						repo.getQueuedMovies().then(function(movies) {
							assert.equal( movies.length, 1 );

							let mov = movies[0];
							assert.equal( mov.country, input.country );
							assert.equal( mov.year, input.year );
							assert.equal( fs.existsSync(mov.poster.substring(7)), true );
							
							repo.resolveQueuedMovie( mov.id, true )
								.then(repo.getMovies)
								.then(function(movies) {
									assert.equal( movies.length, 1 );
									let mov2 = movies[0];

									let posterPath = mov2.poster.substring( 7 );
									assert.equal( fs.existsSync(posterPath), true );
									
									assert.equal( mov2.country, input.country );
									assert.equal( mov2.year, input.year );

									assert.equal( mov2.subtitles.length, 2 );
									assert.equal( fs.existsSync( mov.id ), false );
									done();
								});
						});
					});
				});

				it( 'should queue and discard a local movie', function(done) {
					var input = {
						src: {
							type: 'file',
							uri: '/repositories/movie_pulp_fiction.avi',
						},
						subtitles: [{
							uri: '/repositories/pepe.srt'
						}]
					};

					fs.writeFileSync( input.src.uri, 'pepe' );
					fs.writeFileSync( input.subtitles[0].uri, 'pepe' );

					repo.queueMovie(input).then(function(){
						repo.getQueuedMovies().then(function(movies) {
							assert.equal( movies.length, 1 );

							let mov = movies[0];
							repo.resolveQueuedMovie( mov.id, false )
								.then(repo.getMovies)
								.then(function(movies) {
									assert.equal( movies.length, 0 );
									done();
								});
						});
					});
				});
			});
		});

		describe( 'nextEpisode', function() {
			function createEpisode( serie, season, episode ) {
				return {
					title: 'The pepe episode',
					serie: serie || 'pepe',
					season: season || 1,
					episode: episode || 1,
					summary: 'the summary is',
					subtitles: [],
					runtime: 60,
					video: {
						src: { type: 'file', uri: 'pepe.avi' },
						size: 4
					}
				};
			}

			function writeSerie( name, lastSeenSeason, lastSeenEpisode ) {
				let serieMetaInfo = {
					title: 'The pepe',
					summary: 'The pepe is ...',
					logo: 'logo.png'
				};
				if (lastSeenSeason !== undefined && lastSeenEpisode !== undefined) {
					serieMetaInfo.lastSeen = {
						season: lastSeenSeason,
						episode: lastSeenEpisode
					};
				}

				createDir( serieId(name) );
				fs.writeFileSync( path.join(serieId(name),'meta.json'), JSON.stringify(serieMetaInfo, null, 4) );
			}

			function writeEpisode( info ) {
				let episodePath = serieMeta( info.serie, info.season, info.episode );
				createDir( path.dirname(episodePath) );
				fs.writeFileSync( episodePath, JSON.stringify(info, null, 4) );
			}

			it( 'should returns undefined if serie not found', function(done) {
				startRepo().then(function(repo) {
					repo.getNextEpisode( 'other' )
						.then(function(episodeInfo) {
							assert.equal(episodeInfo);
							stopRepo(repo).then(done);
						});
				});
			});

			it( 'should returns first episode, when no episode was seen', function(done) {
				let serieName = 'pepe';
				writeSerie( serieName );
				writeEpisode( createEpisode( serieName, 1, 1 ) );
				writeEpisode( createEpisode( serieName, 1, 2 ) );

				startRepo().then(function(repo) {
					repo.getNextEpisode( serieId(serieName) )
						.then(function(episodeInfo) {
							assert( episodeInfo );
							assert.equal( episodeInfo.episode, 1 );
							stopRepo(repo).then(done);
						});
				});
			});

			it( 'should returns first episode, when no episode was seen 2', function(done) {
				let serieName = 'pepe';
				writeSerie( serieName );
				createDir( path.dirname(path.dirname(serieMeta( serieName, 2, 0 ))) );
				writeEpisode( createEpisode( serieName, 4, 6 ) );
				writeEpisode( createEpisode( serieName, 3, 6 ) );
				writeEpisode( createEpisode( serieName, 3, 3 ) );

				startRepo().then(function(repo) {
					repo.getNextEpisode( serieId(serieName) )
						.then(function(episodeInfo) {
							assert( episodeInfo );
							assert.equal( episodeInfo.season, 3 );
							assert.equal( episodeInfo.episode, 3 );
							stopRepo(repo).then(done);
						});
				});
			});

			it( 'should returns second episode, when episode was seen', function(done) {
				let serieName = 'pepe';
				writeSerie( serieName, 1, 1 );
				writeEpisode( createEpisode( serieName, 1, 1 ) );
				writeEpisode( createEpisode( serieName, 1, 2 ) );

				startRepo().then(function(repo) {
					repo.getNextEpisode( serieId(serieName) )
						.then(function(episodeInfo) {
							assert( episodeInfo );
							assert.equal( episodeInfo.season, 1 );
							assert.equal( episodeInfo.episode, 2 );
							stopRepo(repo).then(done);
						});
				});
			});

			it( 'should returns s02e01, when s01e01 was seen', function(done) {
				let serieName = 'pepe';
				writeSerie( serieName, 1, 1 );
				writeEpisode( createEpisode( serieName, 1, 1 ) );
				writeEpisode( createEpisode( serieName, 2, 1 ) );

				startRepo().then(function(repo) {
					repo.getNextEpisode( serieId(serieName) )
						.then(function(episodeInfo) {
							assert( episodeInfo );
							assert.equal( episodeInfo.season, 2 );
							assert.equal( episodeInfo.episode, 1 );
							stopRepo(repo).then(done);
						});
				});
			});

			it( 'should returns second episode, when set seen to episode 1', function(done) {
				let serieName = 'pepe';
				writeSerie( serieName );
				writeEpisode( createEpisode( serieName, 1, 1 ) );
				writeEpisode( createEpisode( serieName, 1, 2 ) );

				startRepo().then(function(repo) {
					let e1Id = path.dirname(serieMeta( serieName, 1, 1 ));
					repo.setCurrentTime( 'serie', e1Id, 59 )
						.then( () => repo.getNextEpisode( serieId(serieName) ) )
						.then(function(episodeInfo) {
							assert( episodeInfo );
							assert.equal( episodeInfo.episode, 2 );
							stopRepo(repo).then(done);
						});
				});
			});
		});
	});
});
