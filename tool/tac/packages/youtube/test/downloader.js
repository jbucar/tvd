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

var setupDl;
function YoutubeDl(uri,opts) {
	var self = {};
	var _events = {};
	var _stream = null;

	self.on = function(evt,cb) {
		_events[evt] = cb;
		return self;
	};

	self.pipe = function(stream) {
		_stream = stream;
		return self;
	};

	self.emitResponse = function() {
		assert(_stream);
		_stream.end();
	};

	self.emit = function( evt, msg ) {
		_events[evt]( msg );
	};

	if (setupDl) {
		setupDl(uri,opts,self);
	}

	return self;
};

describe('YoutubeModule', function() {
	var reg = null;
	var MockedModule = null;
	var onComplete = function( err, id, path ) {};

	beforeEach(function() {
		reg = Mocks.init('silly');

		MockedModule = Mocks.mockRequire( require, '../src/downloader', { 'ytdl-core': YoutubeDl } );

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
		MockedModule = null;
	});

	function createOptions() {
		return {
			workDir: '/work',
			onComplete: onComplete
		};
	}

	function createFetch( id, uri ) {
		return {
			id: id || 'id1',
			type: 'youtube',
			uri: uri || 'pepe'
		};
	}

	describe('constructor', function() {
		it('should construct basic', function() {
			let d = new MockedModule();
			assert(d);
			assert.equal(Object.keys(d).length,5);
			assert(d.name);
			assert(d.type);
			assert(d.fetch);
			assert(d.start);
			assert(d.stop);
		});

		it('should start/stop', function(done) {
			let d = new MockedModule();
			assert(d);
			d.start( createOptions() )
				.then( () => d.stop() )
				.then( () => done() );
		});
	});

	describe('methods', function() {
		var helper;
		var onComplete;
		var opts;

		beforeEach(function(done) {
			helper = new MockedModule();
			opts = createOptions();
			opts.onComplete = function(info) {
				if (onComplete) {
					onComplete(info);
				}
			};
			helper.start( opts )
				.then( () => done() );
		});

		afterEach(function(done) {
			helper.stop().then(done);
		});

		it('should test basic fetch', function(done) {
			let firstFetch = createFetch();

			let started = 0;
			let fetched = 0;
			onComplete = function( info ) {
				assert.equal( started, 1 );
				assert.equal( fetched, 1 );
				assert(info);
				assert.equal( info.err );
				assert.equal( info.id, firstFetch.id );
				assert( info.path );
				assert( info.meta );

				assert.equal( info.meta.title, 'pepe' );

				assert.isTrue( fs.statSync( info.path ).isDirectory() );
				done();
			};

			setupDl = function(uri,opts,dl) {
			 	assert.equal( uri, firstFetch.uri );
			 	fetched++;
				opts.filter( {format: {container:'mp4'} } );
				setTimeout(function() {
					dl.emit( 'info', { title: 'pepe', author: 'pepes', iurlsd: 'image.jpg' });
					dl.emitResponse();
				}, 10 );
			};

			helper.fetch( firstFetch )
				.then(function() {
					started++;
				});
		});

		it('should test reject in ydl', function(done) {
			let firstFetch = createFetch();

			onComplete = function( info ) {
				assert(info);
				assert.equal( info.id, firstFetch.id );
				assert( info.err );
				assert.equal( info.err.message, 'pepe' );
				assert.equal( fs.readdirSync( '/work' ).length, 0 );
				done();
			};

			setupDl = function(uri,opts,dl) {
			 	assert.equal( uri, firstFetch.uri );
				dl.pipe = function() { 	return dl; };
				let oldOn = dl.on;
				dl.on = function(evt,cb) {
					oldOn( evt, cb );
					if (evt === 'error') {
						dl.emit( 'error', new Error('pepe') );
					}
				};
			};

			helper.fetch( firstFetch );
		});
	});
});

