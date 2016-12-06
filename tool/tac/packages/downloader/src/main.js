'use strict';

var assert = require('assert');
var path = require('path');
var EventEmitter = require('events').EventEmitter;
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var fse = bPromise.promisifyAll( require('fs-extra') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var request = require('request');
var tvdutil = require('tvdutil');

//	TODO:
//	- Add queue

function DownloaderImpl( adapter ) {
	var _modules = {};
	var self = {};

	function sendComplete( info ) {
		assert(info.id);
		log.verbose( 'downloader', 'Send torrent completed: info=%j', info );
		_events.emit( 'completed', info );
	}

	function tmpDir() {
		return adapter.getDataPath('tmp');
	}

	function startModule( mod ) {
		log.silly( 'downloader', 'Start module: name=%s', mod.name );
		let basePath = adapter.getDataPath(mod.name);
		return fse.ensureDirAsync( basePath )
			.then( () => mod.start({
				onComplete: sendComplete,
				workDir: basePath
			}));
	}

	function stopModule( name ) {
		log.silly( 'downloader', 'Stop module: name=%s', name );
		return bPromise.method(_modules[name].stop)()
			.catch(function(err) {
				log.warn( 'downloader', 'A error ocurred when stop module: name=%s, err=%s', name, err.message );
			});
	}

	//	Api
	var _events = new EventEmitter();
	tvdutil.forwardEventEmitter(self,_events);

	self.addModule = function( helper ) {
		assert(helper, 'Invalid helper');
		assert(helper.name, 'Helper has no name property');
		assert(helper.fetch, 'Helper has no fetch function');
		assert(helper.type, 'Helper has no type property');

		log.info( 'downloader', 'Add module: %s', helper.name );
		return startModule( helper )
			.then(function() {
				_modules[helper.type] = helper;
			})
			.catch(function(err) {
				log.warn( 'downloader', 'A error ocurred when start a module: name=%s, err=%s', helper.name, err.message );
			});
	};

	self.rmModule = function(name) {
		assert(name, 'Invalid helper name');

		let mod = _modules[name];
		if (!mod) {
			return bPromise.reject( tvdutil.warnErr( 'downloader', 'Invalid module: name=%s', name ) );
		}
		return stopModule( name )
			.then(function() {
				delete _modules[name];
			});
	};

	self.getModules = function() {
		return Object.keys(_modules).map( (m) => _modules[m].name );
	};

	self.get = function( name ) {
		return _modules[name];
	};

	self.start = function() {
		_modules = {};
		return fse.ensureDirAsync( tmpDir() );
	};

	self.stop = function() {
		return bPromise.map( Object.keys(_modules), stopModule );
	};

	self.fetch = function( uri ) {
		log.silly( 'downloader', 'Fetch file: uri=%s', uri );
		return mktemp.createFileAsync( path.join(tmpDir(),'fetch_XXXXXXXX') )
			.then(function(tmpFile) {
				return new bPromise(function(resolve,reject) {
					var ct;

					//	Setup stream
					var stream = fs.createWriteStream( tmpFile );
					stream.on('finish', function() {
						resolve( {
							mime: ct,
							path: tmpFile
						});
					});
					stream.on('error', reject );

					request
						.get( uri )
						.on('response', function(response) {
							if (response.statusCode === 200) {
								ct = response.headers['content-type'];
							}
							else {
								reject( tvdutil.warnErr( 'downloader', 'Cannot download file; invalid status code: st=%d', response.statusCode) );
							}
						})
						.on('error', reject )
						.pipe(stream);
				})
				.catch(function(err) {
					return fse.removeAsync( tmpFile ).then( () => {throw err;} );
				});
			});
	};

	self.enqueueFetch = function( opts ) {
		assert(opts.id);
		assert(opts.type);
		assert(opts.uri);

		let helper = _modules[opts.type];
		if (!helper) {
			return bPromise.reject( tvdutil.warnErr( 'downloader', 'Cannot fetch src; invalid type: src.type=%s', opts.type ) );
		}

		log.silly( 'downloader', 'Enqueue fetch: opts=%j', opts );
		return helper.fetch( opts );
	};

	return self;
}

function DownloaderApi(impl) {
	var self = {};

	tvdutil.forwardEventEmitter(self,impl);

	self.addModule = impl.addModule.bind(impl);
	self.rmModule = impl.rmModule.bind(impl);
	self.getModules = impl.getModules.bind(impl);
	self.get = impl.get.bind(impl);
	self.fetch = impl.fetch.bind(impl);
	self.enqueueFetch = impl.enqueueFetch.bind(impl);

	return self;
}

//	Service entry point
function DownloadService(adapter) {
	var self = {};
	var _impl = null;

	self.onStart = function(cb) {
		log.info( 'downloader', 'Start' );
		_impl = new DownloaderImpl(adapter);
		_impl.start()
			.then( () => cb(undefined,new DownloaderApi(_impl)), cb );
	};

	self.onStop = function(cb) {
		log.verbose( 'downloader', 'Stop' );
		_impl.stop()
			.then( () => cb(), cb );
	};

	return Object.freeze(self);
}

// Export module hooks
module.exports = DownloadService;

