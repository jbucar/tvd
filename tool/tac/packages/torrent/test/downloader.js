'use strict';

var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var mockfs = require('mock-fs');
var Mocks = require('mocks');

var path = require('path');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs-extra') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var tvdutil = require('tvdutil');

var TorrentModule = require('../src/downloader');

var TorrentMock = function() {
	var self = {};

	self.load = function() {
		return bPromise.resolve();
	};

	self.start = function() {
		return bPromise.resolve();
	};

	self.stop = function() {
		return bPromise.resolve();
	};

	return self;
};

describe('TorrentDownloader', function() {
	var reg = null;
	var onComplete = function( info ) {};

	beforeEach(function() {
		reg = Mocks.init('silly');

		var sfs = {};
		mockfs(sfs);

		//	Create work dir
		var tmpDir = '/work';
		fs.ensureDirSync( tmpDir );
	});

	afterEach(function() {
		mockfs.restore();
		Mocks.fin();
		reg = null;
	});

	function createOptions() {
		return {
			workDir: '/work',
			onComplete: onComplete
		};
	}

	function createFetch( id, hash, uri ) {
		return {
			id: id || 'id1',
			type: 'torrent',
			hash: hash || 'C12FE1C06BBA254A9DC9F519B335AA7C1367A88A',
			uri: uri || 'magnet:?xt=urn:btih:c12fe1c06bba254a9dc9f519b335aa7c1367a88a&dn'
		};
	}

	function cfgFile( workDir ) {
		return path.join(workDir,'config.json');
	}

	function writeCfg( workDir, downloads ) {
		fs.writeFileSync( cfgFile(workDir), JSON.stringify(downloads, null, 4) );
	}

	function createTorrent( hash, complete ) {
		let torrentPath = '/work/test_serie';
		return {
			info: {
				path: torrentPath,
				hash: hash,
				complete: (complete !== undefined) ? complete : 1
			},
			movieUri: path.join( torrentPath, 'the_walking_dead_S02E06.avi' )
		};
	}

	function writeTorrent( torrentData ) {
		fs.ensureDirSync( torrentData.info.path );
		fs.writeFileSync( torrentData.movieUri, 'pepe' );
	}

	describe('constructor', function() {
		it('should construct basic', function() {
			let d = new TorrentModule( new TorrentMock() );
			assert(d);
			assert.equal(Object.keys(d).length,5);
			assert(d.name);
			assert(d.type);
			assert(d.fetch);
			assert(d.start);
			assert(d.stop);
		});

		it('should start/stop', function(done) {
			let d = new TorrentModule( new TorrentMock() );
			assert(d);
			d.start( createOptions() )
				.then( () => d.stop() )
				.then( () => done() );
		});

		it('should start pending torrents when start', function(done) {
			let currentTorrent = new TorrentMock();
			let torrent = new TorrentModule( currentTorrent );
			assert(torrent);

			let opts = createOptions();

			let firstTorrent = createFetch();
			let downloads = [firstTorrent];
			writeCfg( opts.workDir, downloads );

			let tStarted = 0;
			currentTorrent.start = function( uri ) {
				tStarted++;
				assert.equal( uri, firstTorrent.uri );
				return bPromise.resolve();
			};

			torrent.start( opts )
				.then(function() {
					assert.equal( tStarted, 1 );
					return torrent.stop().then(done);
				});
		});
	});

	describe('methods', function() {
		var currentTorrent;
		var torrent;
		var onComplete;
		var opts;

		beforeEach(function(done) {
			currentTorrent = new TorrentMock();
			torrent = new TorrentModule( currentTorrent );

			opts = createOptions();
			opts.onComplete = function(info) {
				if (onComplete) {
					onComplete(info);
				}
			};
			opts.checkPeriod = 100;
			torrent.start( opts )
				.then( () => done() );
		});

		afterEach(function(done) {
			torrent.stop().then(done);
		});

		it('should test basic fetch', function(done) {
			let firstTorrent = createFetch();

			let tStarted = 0;
			currentTorrent.start = function( uri ) {
				tStarted++;
				assert.equal( uri, firstTorrent.uri );
				return bPromise.resolve();
			};

			torrent.fetch( firstTorrent )
				.then(function() {
					assert.equal( tStarted, 1 );
					done();
				});
		});

		it('should test multiple fetch', function(done) {
			let firstTorrent = createFetch();
			let secondTorrent = createFetch('id2');

			let tStarted = 0;
			currentTorrent.start = function( uri ) {
				tStarted++;
				return bPromise.resolve();
			};

			bPromise.join(
				torrent.fetch( firstTorrent ),
				torrent.fetch( secondTorrent ),
				function() {
					assert.equal( tStarted, 2 );
					done();
				});
		});

		it('should reject if uri is invalid', function(done) {
			let firstTorrent = createFetch();
			firstTorrent.uri = 'apadsfa';
			torrent.fetch( firstTorrent )
				.catch(function(err) {
					assert(err);
					assert.equal(err.message.indexOf('Invalid magnet link'), 0);
					done();
				});
		});

		it('should check torrent completion', function(done) {
			let firstTorrent = createFetch();

			let torrentData = createTorrent( firstTorrent.hash, 0 );
			let torrentData2 = createTorrent( 'padsfadsfadsfadf', 0 );
			writeTorrent( torrentData );

			let getAllCount = 0;
			currentTorrent.getAll = function() {
				getAllCount++;
				if (getAllCount === 2) {
					torrentData.info.complete = 1;
				}
				return bPromise.resolve( [torrentData2.info,torrentData.info] );
			};

			onComplete = function( info ) {
				assert(info);
				assert.equal(info.err);
				assert.equal( info.id, firstTorrent.id );
				assert.equal( info.path, torrentData.info.path );
				done();
			};

			torrent.fetch( firstTorrent );
		});

		it('cannot stop torrent when check torrent completion', function(done) {
			let firstTorrent = createFetch();

			let torrentData = createTorrent( firstTorrent.hash );
			writeTorrent( torrentData );

			currentTorrent.stop = function() {
				return bPromise.reject(new Error('pepe') );
			};

			currentTorrent.getAll = function() {
				return bPromise.resolve( [torrentData.info] );
			};

			onComplete = function( info ) {
				assert(info);
				assert(info.err);
				assert.equal( info.id, firstTorrent.id );
				assert.equal( info.err.message, 'A error ocurred when process torrent: hash=C12FE1C06BBA254A9DC9F519B335AA7C1367A88A, err=pepe' );
				done();
			};

			torrent.fetch( firstTorrent );
		});

		it('should check torrent and client remove download', function(done) {
			let firstTorrent = createFetch();

			let torrentData2 = createTorrent( 'padsfadsfadsfadf', 0 );

			currentTorrent.getAll = function() {
				return bPromise.resolve( [torrentData2.info] );
			};

			onComplete = function( info ) {
				assert(info);
				assert(info.err);
				assert.equal( info.id, firstTorrent.id );
				assert.equal( info.err.message, 'Some error ocurred in torrent client; hash not found: hash=C12FE1C06BBA254A9DC9F519B335AA7C1367A88A' );
				done();
			};

			torrent.fetch( firstTorrent );
		});

		it('should check torrent and getAll returns with error and recheck', function(done) {
			let firstTorrent = createFetch();

			let torrentData = createTorrent( firstTorrent.hash );
			writeTorrent( torrentData );

			let getAllCount=0;
			currentTorrent.getAll = function() {
				getAllCount++;
				if (getAllCount === 1) {
					return bPromise.reject( new Error('pepe1') );
				}
				return bPromise.resolve( [torrentData.info] );
			};

			onComplete = function( info ) {
				assert(info);
				assert.equal( info.id, firstTorrent.id );
				assert.equal( info.path, torrentData.info.path );
				assert.equal( info.err );
				done();
			};

			torrent.fetch( firstTorrent );
		});
	});

});

