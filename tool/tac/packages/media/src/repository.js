"use strict";

var assert = require('assert');
var path = require('path');
var EventEmitter = require('events').EventEmitter;
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs-extra') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var tvdutil = require('tvdutil');
var srt2vttAsync = bPromise.promisify(require('srt2vtt'));
var ffprobe = require('node-ffprobe');
var MovieDelegate = require('./delegate').MovieDelegate;
var SerieDelegate = require('./delegate').SerieDelegate;
var QueuedMovieDelegate = require('./delegate').QueuedMovieDelegate;
var util = require('./util');

//	TODO
//	- Add serie information
//	- Add scraper when no summary/title/images found
//		* https://www.npmjs.com/package/node-tvdb
//		* https://www.npmjs.com/package/moviedb
//	- Download subtitles if necesary (no available when add)


function Repository( rOpts ) {
	assert(rOpts.basePath, 'Invalid basePath');
	assert(rOpts.sharePath, 'Invalid sharePath');
	assert(rOpts.downloader, 'Invalid downloader');
	assert(rOpts.supportedFiles, 'Invalid supported files');

	var self = {};
	var _events = new EventEmitter();
	var _workCondition = new tvdutil.BasicWaitCondition();
	let _tmpPath = path.join(rOpts.basePath,'tmp');
	let _repos = {
		movie: new MovieDelegate(_events,rOpts.sharePath),
		serie: new SerieDelegate(_events,rOpts.sharePath),
		queuedMovie: new QueuedMovieDelegate(_events,rOpts.basePath)
	};

	//--------------------- -----------------------------------------------------------------------
	//    Basic aux
	//--------------------- -----------------------------------------------------------------------
	function getRepo(info) {
		return _repos[info.type];
	}

	function setField( aDst, aSrc, dstField ) {
		var srcField = dstField;
		if (aSrc[srcField]) {
			aDst[dstField] = aSrc[srcField];
		}
	}

	function moveFile( absName, dstPath, newUri ) {
		var dst = path.join(dstPath, newUri || path.basename(absName));
		log.silly( 'Repository', 'Move file: src=%s, dst=%s', absName, dst );
		return fs.moveAsync( absName, dst )
			.then( () => dst );
	}

	function unpackFile( downloadInfo ) {
		log.silly( 'Repository', 'UnpackFile: info=%j', downloadInfo );

		let prom;
		if (/^application\/zip/.test(downloadInfo.mime)) {
			prom = tvdutil.unzipFile( _tmpPath, downloadInfo.path );
		}
		else {
			throw tvdutil.warnErr( 'Repository', 'Unsupported content type: mime=%s', downloadInfo.mime );
		}

		return prom;
	}

	function scanPath( rootPath ) {
		let entries = [];
		function addEntries( info ) {
			if (info.stats.isFile()) {
				entries.push( info );
			}
			return bPromise.resolve();
		}

		let opts = {
			root: rootPath,
			cbEntry: addEntries,
			cbEnd: () => entries
		};
		return tvdutil.findFiles( opts );
	}

	function resolveResource( dstPath, uri, dstName ) {
		assert(uri, 'resolveResource: Invalid uri');
		log.silly( 'Repository', 'Resolve resource: uri=%s, dstName=%s', uri, dstName );
		
		var uriObject = util.parseURI(uri);
		if (!uriObject.isValid()) {
			return bPromise.reject( tvdutil.warnErr( 'Repository', 'Cannot download resource; Invalid uri: uri=%s', uri ) );
		}

		if (uriObject.isLocal()) {
			return moveFile( uriObject.pathname(), dstPath );
		}

		return downloadFile( uri )
			.then(function (dInfo) {
				let ext;
				switch (dInfo.mime) {
				case 'image/jpeg':
					ext = '.jpg';
					break;
				case 'image/png':
					ext = '.png';
					break;
				default:
					log.warn( 'Repository', 'Unknown resource type: mime=%s', dInfo.mime );
					ext = '.png';
					break;
				}

				return moveFile( dInfo.path, dstPath, dstName+ext );
			});
	}

	function resolveResourceUrl( info, key ) {
		let data = info.meta[key];
		if (data) {
			return resolveResource( info.absPath, data, key )
				.then((newUri) => info.meta[key] = path.basename(newUri) );
		}
		return bPromise.resolve();
	}

	function resolveMetaAux( info, input ) {
		if (!input) {
			return bPromise.resolve();
		}

		//	Map fields
		let mapFields = getRepo(info).getFields();
		mapFields.forEach(function(field) {
			setField( info.meta, input, field );
		});

		//	Resolve resources
		return resolveResourceUrl( info, 'poster' );
	}

	//--------------------- -----------------------------------------------------------------------
	//    Subtitle aux
	//--------------------- -----------------------------------------------------------------------

	function probeFile( meta, absName ) {
		function setField( obj, key, val, force ) {
			if (val !== undefined) {
				let overwrite = (force === undefined) ? true : force;
				let set = (obj[key] !== undefined) ? overwrite : true;
				if (set) {
					obj[key] = val;
				}
			}
		}

		return new bPromise(function(resolve,reject) {
			ffprobe( absName, function(err,data) {
				if (err) {
					reject(err);
				}
				else {
					if (data.format) {
						setField( meta, 'runtime', parseInt(data.format.duration) );
						setField( meta.video, 'size', data.format.size );
					}

					if (data.metadata) {
						setField( meta, 'genre', data.metadata.genre, false );
						setField( meta, 'title', data.metadata.title, false );
					}

					if (data.streams) {
						data.streams.forEach(function(s) {
							if (s.codec_type === 'video' && s.avg_frame_rate !== '0/0') {
								let aFps = s.avg_frame_rate.split('/');
								assert(aFps.length === 2, 'probeFile: Invalid avg_frame_rate');
								let fps = parseInt(aFps[0]) / parseInt(aFps[1]);

								setField( meta.video.codecs.video, "name", s.codec_name );
								setField( meta.video.codecs.video, "profile", s.profile );
								setField( meta.video.codecs.video, "width", s.width );
								setField( meta.video.codecs.video, "height", s.height );
								setField( meta.video.codecs.video, "fps", fps );
							}
							else if (s.codec_type === 'audio') {
								setField( meta.video.codecs.audio, "name", s.codec_name );
								setField( meta.video.codecs.audio, "profile", s.profile );
								setField( meta.video.codecs.audio, "channels", s.channels );
							}
						});
					}

					log.silly( 'repository', 'probeFile: data=%j', meta );

					resolve(meta);
				}
			});
		});
	}

	function addLocalVideoEntry( info, absName ) {
		return moveFile( absName, info.absPath )
			.then(function(newUri) {
				log.silly( 'Repository', 'Add video entry: name=%s', path.basename(absName) );
				info.meta.video.src = {
					type: 'file',
					uri: path.basename(newUri)
				};
				return probeFile(info.meta,newUri);
			});
	}

	//--------------------- -----------------------------------------------------------------------
	//    Subtitle aux
	//--------------------- -----------------------------------------------------------------------
	function addSubtitleMeta( info, pName, lang ) {
		var uri = util.parseURI(pName);
		var type;
		var name;
		var resolvedUri;
		if (uri.isLocal()) {
			var ext = path.extname(pName).toLowerCase();
			name = path.basename(pName);
			type = getSubtitleType(ext);
			resolvedUri = path.basename(pName);
		}
		else {
			name = pName;
			type = 'unknown';
			resolvedUri = pName;
		}
		var subInfo = {
			name: name,
			type: type,
			uri: resolvedUri,
			lang: lang || 'en'
		};
		info.meta.subtitles.push( subInfo );

		if (info.oper.convertToVTT && (type !== 'vtt' && type !== 'unknown')) {
			return convert2VttSubtitle( info, pName, lang );
		}
	}

	function convert2VttSubtitle( info, pName, lang ) {
		log.silly( 'Repository', 'Convert to vtt subtitle: pName=%s', path.basename(pName) );
		return fs.readFileAsync( pName )
			.then( srt2vttAsync )
			.then(function(vttSubData) {
				let name = path.join(path.dirname(pName),path.basename(pName,path.extname(pName))+".vtt");
				return fs.writeFileAsync( name, vttSubData )
					.then( () => addSubtitleMeta( info, name, lang ) );
			});
	}

	function addSubtitle( info, uri, lang ) {
		let dstName = path.basename(uri);
		log.silly( 'Repository', 'Add subtitle: uri=%s', dstName );

		//	Move to info.absPath
		return moveFile( uri, info.absPath )
			.catch(function(err) {
				if (err.code === 'EEXIST') {
					dstName = util.zeroFill(info.meta.subtitles.length+1) + '-' + dstName;
					return moveFile( uri, info.absPath, dstName );
				}
				else {
					throw err;
				}
			})
			.then( (newUri) => addSubtitleMeta( info, newUri, lang ) );
	}

	function getSubtitleType( ext ) {
		switch (ext) {
		case '.srt':
			return "srt";
		case '.vtt':
			return "vtt";
		default:
			log.warn( "Repository", "Unknown subtitle file: ext=%s", ext );
			return "unknown";
		}
	}

	function downloadSubtitle( info, subInfo ) {
		return bPromise.using(
			downloadFile( subInfo.uri ).disposer(function(downloadInfo) {
				return fs.removeAsync( downloadInfo.path );
			}), function(downloadInfo) {
				return unpackFile(downloadInfo)
					.then( (outDir) => processPath( info, outDir ) );
			});
	}

	//--------------------- -----------------------------------------------------------------------
	//    Downloader aux
	//--------------------- -----------------------------------------------------------------------

	function processPath( info, lPath ) {
		log.silly( 'Repository', 'Process entries: lPath=%s', lPath );

		return scanPath( lPath )
			.then(function(entries) {
				let files = [];
				let maxSize = -1;
				let mainVideo;
				for (let i=0; i<entries.length; i++) {
					let info = entries[i];
					var ext = path.extname(info.entry).toLowerCase();

					if (rOpts.supportedFiles.subtitle.indexOf(ext) >= 0) {
						files.push({
							name: info.absPath,
							method: addSubtitle
						});
					}
					else if (rOpts.supportedFiles.video.indexOf(ext) >= 0) {
						if (info.stats.size > maxSize) {
							mainVideo = info.absPath;
							maxSize = info.stats.size;
						}
					}
				}

				//	Add main video!
				if (mainVideo) {
					files.push({
						name: mainVideo,
						method: addLocalVideoEntry
					});
				}

				return bPromise.map( files, function(file) {
					return file.method( info, file.name );
				});
			})
			.finally(function() {
				return fs.removeAsync( lPath );
			});
	}

	function downloadFile( uri ) {
		return rOpts.downloader.fetch( uri );
	}

	function onDownloadCompleted( downloadInfo ) {
		assert( downloadInfo.id, 'onDownloadCompleted: Invalid id' );
		log.silly( 'Repository', 'On download complete: info=%j', downloadInfo );

		getPartial( downloadInfo.id )
			.then(function(info) {
				if (downloadInfo.err) {
					throw downloadInfo.err;
				}
				return processPath( info, downloadInfo.path )
					.then( () => resolveMetaAux( info, downloadInfo.meta ) )
					.then( () => getRepo(info).add( info ) );
			})
			.catch(function(err) {
				log.error( 'Repository', 'A error ocurred when process downloaded video: err=%s', err.message );
				return fs.removeAsync( downloadInfo.id )
					.then(function () {
						_events.emit( 'error', err );
					});
			});
	}

	//--------------------- -----------------------------------------------------------------------
	//    Partial videos
	//--------------------- -----------------------------------------------------------------------

	function partialFile( absPath ) {
		return path.join(absPath,'meta.json');
	}

	function getPartial( absPath ) {
		log.silly( 'Repository', 'Get partial video: absPath=%s', absPath );
		return fs.readFileAsync( partialFile(absPath) )
			.then(JSON.parse);
	}

	function storePartial( info ) {
		log.silly( 'Repository', 'Store partial video: absPath=%s', info.absPath );
		return fs.writeFileAsync( partialFile(info.absPath), JSON.stringify(info, null, 4) );
	}

	//--------------------- -----------------------------------------------------------------------
	//    Main operations
	//--------------------- -----------------------------------------------------------------------
	function createWorkDir(info) {
		assert(info, 'createWorkDir: Invalid info');
		log.silly( 'Repository', 'Create working directory: src=%s', info.oper.src.uri );

		var dest = path.join( _tmpPath, info.type + '.XXXXXXXX' );
		return mktemp.createDirAsync( dest )
			.then(function(tmpDir) {
				info.id = path.basename(tmpDir);
				info.absPath = tmpDir;
				return info;
			});
	}

	function resolveVideo(info) {
		assert(info, 'resolveVideo: Invalid info');
		assert(info.oper.src.type, 'resolveVideo: invalid src.type');
		assert(info.oper.src.uri, 'resolveVideo: invalid src.uri');

		log.verbose( 'Repository', 'Resolve video: id=%s', info.id );

		info.meta.video = {
			codecs: {
				audio: {},
				video: {}
			}
		};
		setField( info.meta.video, info.oper, 'size' );

		let prom;
		if (info.oper.src.type === 'file') {
			let uriObj = util.parseURI(info.oper.src.uri);
			assert( uriObj.isLocal(), 'Invalid local uri' );
			let localPath = uriObj.pathname();
			prom=addLocalVideoEntry( info, localPath );
		}
		else if (info.oper.download) {
			assert(info.absPath, 'resolveVideo: Invalid absPath');
			let opts = {
				type: info.oper.src.type,
				uri: info.oper.src.uri,
				id: info.absPath
			};
			prom = rOpts.downloader.enqueueFetch( opts );
		}
		else {
			//	TODO: Handle online uris ...
			info.meta.video.src = {
				type: info.oper.src.type,
				uri: info.oper.src.uri
			};
			prom = bPromise.resolve();
		}

		return prom.then( () => info );
	}

	function resolveSubtitles(info) {
		assert(info, 'resolveSubtitles: Invalid info');

		log.verbose( 'Repository', 'Resolve subtitles: id=%s', info.id );

		//	Initialize subtitles
		info.meta.subtitles = [];

		//	Resolve each subtitles
		if (info.oper.subtitles && info.oper.subtitles.length > 0) {
			return bPromise.all(info.oper.subtitles.map(function( subInfo ) {
				log.verbose( 'Repository', 'Resolve subtitle: id=%s, subInfo=%j', info.id, subInfo );

				if (!subInfo.uri) {
					throw tvdutil.warnErr( 'Repository', 'Invalid uri: uri=%s', subInfo.uri );
				}

				var uri = util.parseURI(subInfo.uri);
				if (!uri.isValid()) {
					throw tvdutil.warnErr( 'Repository', 'Invalid uri: uri=%s', subInfo.uri );
				}

				if (uri.isLocal()) {
					return addSubtitle( info, uri.pathname(), subInfo.lang );
				}
				else if (info.oper.download) {
					return downloadSubtitle( info, subInfo );
				}
				else {
					return addSubtitleMeta( info, subInfo.uri, subInfo.lang );
				}
			})).then( () => info );
		}
		return info;
	}

	function resolveMetadata(info) {
		assert(info, 'resolveMetadata: Invalid info');
		log.verbose( 'Repository', 'Resolve metadata: id=%s', info.id );
		return resolveMetaAux( info, info.oper ).then( () => info );
	}

	function storeVideo(info) {
		assert(info, 'storeVideo: Invalid info');
		if (info.meta.video.src) {
			return getRepo(info).add( info );
		}
		else {
			return storePartial( info );
		}
	}

	function checkInput( info ) {
		log.silly( 'Repository', 'Check input' );
		return new bPromise(function(resolve) {
			if (!info.oper.src || !info.oper.src.uri) {
				throw tvdutil.warnErr( 'Repository', 'Cannot add %s; No src defined', info.type);
			}

			getRepo(info).checkInput( info.oper );

			resolve(info);
		});
	}

	function addVideo( opts, type ) {
		if (opts.download === undefined) {
			opts.download = true;
		}
		if (opts.convertToVTT === undefined) {
			opts.convertToVTT = true;
		}

		let info = {
			oper: opts,
			type: type,
			meta: {}
		};
		return checkInput(info)
			.then(createWorkDir)
			.then(resolveMetadata)
			.then(resolveSubtitles)
			.then(resolveVideo)
			.then(storeVideo)
			.catch(function(err) {
				log.error( 'Repository', 'Error adding video: err=%s', err.message );
				let prom = (info.absPath) ? fs.removeAsync( info.absPath ) : bPromise.resolve();
				return prom.then( () => _events.emit( 'error', err ) );
			});
	}

	function exportMeta( info ) {
		return getRepo(info).exportMeta( info );
	}

	function addSerie( serieName ) {
		if (!serieName) {
			return bPromise.reject( tvdutil.warnErr( 'Repository', 'Cannot add serie; No serie name defined' ) );
		}

		let rSerie = _repos.serie;
		return rSerie.existsSerie( serieName )
			.then(function(exists) {
				if (exists) {
					return bPromise.resolve();
				}
				return rSerie.addSerie({
					title: serieName
				});
			});
	}

	//	API
	tvdutil.forwardEventEmitter(self,_events);

	self.addMovie = function( movieInfo ) {
		log.info( 'Repository', 'Add movie: info=%j', movieInfo );
		return addVideo( movieInfo, 'movie' );
	};

	self.getMovies = function() {
		return _repos.movie.getAll().map(exportMeta);
	};

	self.queueMovie = function( movieInfo ) {
		log.info( 'Repository', 'Queue movie: info=%j', movieInfo );
		movieInfo.download = false;
		movieInfo.convertToVTT = false;
		return addVideo( movieInfo, 'queuedMovie' );
	};

	self.hasQueuedMovies = function() {
		return _repos.queuedMovie.count()
			.then( (size) => size > 0 );
	};

	self.getQueuedMovies = function() {
		return _repos.queuedMovie.getAll().map(exportMeta);
	};

	self.resolveQueuedMovie = function( queuedID, add ) {
		let prom;

		if (add) {
			prom = _repos.queuedMovie.getInfo( queuedID )
				.then(exportMeta)
				.then(function (movieInfo) {
					let info = {
						src: movieInfo.video.src,
						subtitles: movieInfo.subtitles
					};
					_repos.queuedMovie.getFields().forEach( (field) => info[field] = movieInfo[field] );
					return self.addMovie(info);
				});
		}
		else {
			prom = bPromise.resolve();
		}

		return prom.then( () => _repos.queuedMovie.remove(queuedID) );
	};

	self.addEpisode = function( episodeInfo ) {
		log.info( 'Repository', 'Add episode: info=%j', episodeInfo );
		return addSerie( episodeInfo.serie )
			.then( () => addVideo( episodeInfo, 'serie' ) )
			.catch(function(err) {
				log.error( 'Repository', 'Error adding episode: err=%s', err.message );
				_events.emit( 'error', err );
			});
	};

	self.getSeries = function() {
		return _repos.serie.getSeries();
	};

	self.getEpisodes = function(serie,season) {
		return _repos.serie.getEpisodes( serie, season ).map(exportMeta);
	};

	self.getNextEpisode = function( serie ) {
		return _repos.serie.nextEpisode( serie );
	};

	self.setCurrentTime = function( type, id, elapsed ) {
		assert(type, 'setCurrentTime: Invalid type');
		assert(id, 'setCurrentTime: Invalid id');
		return _repos[type].setCurrentTime( id, elapsed );
	};

	self.markSeen = function(type,id,state) {
		assert(type, 'markSeen: Invalid type');
		assert(id, 'markSeen: Invalid id');
		return _repos[type].markSeen( id, state );
	};

	self.remove = function(type,id) {
		assert(type, 'remove: Invalid type');
		assert(id, 'remove: Invalid id');
		log.info( 'Repository', 'Remove: type=%s, id=%s', type, id );
		return _repos[type].remove(id);
	};

	self.start = function() {
		log.info( 'Repository', 'Start' );

		rOpts.downloader.on( 'completed', onDownloadCompleted );

		//	Start repos
		return fs.ensureDirAsync( _tmpPath )
			.then( () => bPromise.map( Object.keys(_repos), function(rName) {
				return _repos[rName].start();
			}));
	};

	self.stop = function() {
		log.info( 'Repository', 'Stop' );
		return _workCondition.waitAsync()
			.then(function() {
				rOpts.downloader.removeListener( 'completed', onDownloadCompleted );
			});
	};

	return self;
}

module.exports = Repository;
