'use strict';

var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var mockfs = require('mock-fs');
var Mocks = require('mocks');

var path = require('path');
var EventEmitter = require('events').EventEmitter;
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs-extra') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var tvdutil = require('tvdutil');

var setupRequest = null;
function RequestGetMock(uri) {
	var self = {};
	var _events = {};
	var _stream = null;
	self.defaultResponse = 200;
	self.defaultContentType = 'application/zip';
	self.on = function(evt,cb) {
		_events[evt] = cb;
		return self;
	};
	self.pipe = function(stream) {
		_stream = stream;
		self.emitResponse( self.defaultResponse, self.defaultContentType );
		return self;
	};

	self.emitResponse = function( st, ct ) {
		assert(_stream);
		_events['response']( {
			statusCode: st,
			headers: { 'content-type': ct }
		});
		_stream.end();
	};

	if (setupRequest) {
		setupRequest(self);
	}

	return self;
}

var RequestMock = {
	get: RequestGetMock
};

describe('Downloader', function() {
	var reg = null;
	var adapter = null;
	var MockedDownloader = null;

	beforeEach(function() {
		reg = Mocks.init('silly');

		setupRequest = null;
		MockedDownloader = Mocks.mockRequire( require, '../src/main', { 'request': RequestMock } );

		var sfs = {};
		mockfs(sfs);

		adapter = new Mocks.ServiceAdapter(reg,'downloader');

		fs.ensureDirSync( adapter.getDataPath() );
	});

	afterEach(function() {
		mockfs.restore();
		Mocks.fin();
		adapter = null;
		reg = null;
		MockedDownloader = null;
	});

	function createModule() {
		let helper = {
			name: 'pepe',
			type: 'pepe',
			start: function(opts) {
				assert( opts.onComplete );
				helper.onComplete = opts.onComplete;
				return bPromise.resolve();
			},
			stop: function() { return bPromise.resolve(); },
			fetch: function() { return bPromise.resolve(); },
		};

		return helper;
	}

	describe('constructor', function() {
		it('should construct basic', function() {
			let d = new MockedDownloader(adapter);
			assert(d);
			assert(d.onStart);
			assert(d.onStop);
		});

		it('basic start/stop', function(done) {
			let module = new MockedDownloader(adapter);
			module.onStart(function(err,api) {
				assert.equal(err);
				assert( api );
				module.onStop(done);
			});
		});

		it('should start/stop/start/stop', function(done) {
			let module = new MockedDownloader(adapter);
			module.onStart(function(err,api) {
				assert.equal(err);
				assert( api );
				module.onStop(function() {
					module.onStart(function(err1,api1) {
						assert.equal(err1);
						assert( api1 );
						assert( api !== api1 );
						module.onStop(done);
					});
				});
			});
		});
	});	//	constructor

	describe.only( 'methods_started', function() {
		var module;
		var api;

		beforeEach(function(done) {
			module = new MockedDownloader(adapter);
			module.onStart(function(err,a) {
				assert.equal(err);
				assert( a );
				api = a;
				done();
			});
		});

		afterEach(function(done) {
			module.onStop(done);
		});

		it('should start/stop a helper', function(done) {
			let started = 0;
			let stopped = 0;

			let helper = createModule();
			helper.start = bPromise.method(function(opts) {
				assert( opts.onComplete );
				assert.equal( opts.workDir, path.join(adapter.getDataPath(),helper.name) );
				started++;
			});
			helper.stop = bPromise.method(function() {
				stopped++;
			});

			api.addModule( helper )
				.then(function() {
					assert.equal( started, 1 );
					let mods = api.getModules();
					assert.equal( mods.length, 1 );
					assert.equal( mods[0], helper.name );

					let mod = api.get( helper.name );
					assert(mod);

					api.rmModule( helper.name )
						.then(function() {
							assert.equal( stopped, 1 );

							let mods1 = api.getModules();
							assert.equal( mods1.length, 0 );

							done();
						});
				});
		});

		it('should catch when a module start throwing error', function(done) {
			let helper = createModule();
			helper.start = function(otps) {
				throw new Error('pepe');
			};
			api.addModule( helper )
			   .then(function() {
					let mods = api.getModules();
					assert.equal( mods.length, 0 );

				   done();
			   });
		});

		it('should not addModule if reject with error', function(done) {
			let helper = createModule();
			helper.start = function() {
				return bPromise.reject(new Error('pepe'));
			};
			api.addModule( helper )
			   .then(function() {
					let mods = api.getModules();
					assert.equal( mods.length, 0 );

				   done();
			   });
		});

		it('should catch when a module stop throwing error', function(done) {
			api.rmModule( 'pepe' )
				.catch(function(err) {
					assert(err);
					assert.equal( err.message, 'Invalid module: name=pepe' );
					done();
				});
		});

		it('should catch when a module stop throwing error', function(done) {
			let helper = createModule();
			helper.stop = function() {
				throw new Error('pepe');
			};
			api.addModule( helper )
			   .then(function() {
					let mods = api.getModules();
				   assert.equal( mods.length, 1 );

				   api.rmModule( helper.name )
					   .then(function() {
						   let mods1 = api.getModules();
						   assert.equal( mods1.length, 0 );
						   done();
					   });
			   });
		});

		it('should ignore error when a module stop rejecting', function(done) {
			let helper = createModule();
			helper.stop = function() {
				return bPromise.reject( new Error('pepe') );
			};
			api.addModule( helper )
			   .then(function() {
					let mods = api.getModules();
				   assert.equal( mods.length, 1 );

				   api.rmModule( helper.name )
					   .then(function() {
						   let mods1 = api.getModules();
						   assert.equal( mods1.length, 0 );
						   done();
					   });
			   });
		});

		describe('fetch', function() {
			it('should download a file', function(done) {
				api.fetch( 'http://pepe.com' )
					.then( () => done() );
			});

			it('should fail fetch a file', function(done) {
				setupRequest = function(obj) {
					obj.defaultResponse = 201;
				};

				api.fetch( 'http://pepe.com' )
					.catch(function (err) {
						assert(err);
						assert.equal( err.message, 'Cannot download file; invalid status code: st=201' );
						assert.equal( fs.readdirSync( path.join(adapter.getDataPath(),'tmp') ).length, 0 );
						done();
					});
			});
		});

		describe('enqueueFetch', function() {
			beforeEach(function(done) {
				let helper = createModule();
				api.addModule( helper )
					.then( () => done() );
			});

			it('should fail when enqueue download', function(done) {
				let opts = {
					id: 'pepe1',
					type: 'basic1',
					uri: 'http://wwww.pepe.com'
				};
				api.enqueueFetch( opts )
					.catch(function (err) {
						done();
					});
			});

			it('should enqueue download file', function(done) {
				let helper = api.get( 'pepe' );
				helper.fetch = function(opts) {
					setTimeout(function() {
						helper.onComplete({ id: opts.id, path: '/pepe' });
					}, 10 );
					return bPromise.resolve();
				};

				let opts = {
					id: 'pepe1',
					type: 'pepe',
					uri: 'http://wwww.pepe.com'
				};

				let enqueued = 0;
				api.on( 'completed', function(info) {
					assert.equal( enqueued, 1 );
					assert(info);
					assert.equal( info.id, 'pepe1' );
					assert.equal( info.path, '/pepe' );
					done();
				});

				api.enqueueFetch( opts )
					.then(function () {
						enqueued++;
					});
			});
		});
	});
});

