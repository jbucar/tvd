'use strict';

var assert = require('assert');
var path = require('path');
var _ = require('lodash');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs-extra') );
var tvdutil = require('tvdutil');
var util = require('./util');


var MetaFileName = 'meta.json';


class Delegate {
	constructor( evts ) {
		this._events = evts;
	}

	start() {
		return fs.ensureDirAsync( this._absPath );
	}

	add( info ) {
		//	Setup dst path
		let dstPath = this.getID(info);

		return fs.statAsync(dstPath)
			.then(function() {
				throw tvdutil.warnErr( 'Repository', 'Video already exists: dstPath=%s', dstPath );
			}, function() {
				//	Ingore error and create
				return fs.ensureDirAsync( path.dirname(dstPath) );
			})
			.then( () => fs.renameAsync( info.absPath, dstPath) )
			.then( () => {
				//	Save metadata to own directory
				log.silly( 'Repository', 'Add video: dst=%s', dstPath );

				info.absPath = dstPath;
				info.meta.currentTime = 0;
				info.meta.seen = false;

				return this.writeInfo( info )
					.then( () => this._events.emit( this.sigName('Added'), this.exportMeta( info ) ) );
			});
	}

	getID( info ) {
		return this.getAbsPath( this.getPathName(info.meta) );
	}

	setCurrentTime( id, elapsed ) {
		log.verbose( 'Repository', 'Set play elapsed time: id=%s, elapsed=%s', id, elapsed );
		return this.getInfo( id )
			.then((info) => {
				info.meta.currentTime = elapsed;
				let prom;
				let percentSeen = elapsed * 100 / info.meta.runtime;
				if (percentSeen > 90) {
					delete info.meta.currentTime;
					prom = this.updateSeen( info, true );
				}
				else {
					prom = bPromise.resolve();
				}
				return prom.then( () => this.update( info ) );
			});
	}

	markSeen( id, state ) {
		log.verbose( 'Repository', 'Mark seen: id=%s, state=%s', id, state );
		return this.getInfo( id )
			.then((info) => {
				return this.updateSeen( info, state )
					.then( () => this.update( info ) );
			});
	}

	remove( id ) {
		if (id.indexOf( this._absPath ) !== 0) {
			return bPromise.reject( tvdutil.warnErr( 'Repository', 'Invalid video id: id=%s', id ) );
		}

		return fs.removeAsync( id )
			.then( () => this._events.emit( this.sigName('Removed'), id ) );
	}

	updateSeen( info, state ) {
		info.meta.seen = state;
		return bPromise.resolve();
	}

	exportMeta( info ) {
		let ret = _.cloneDeep( info.meta );
		util.fixURI(info.absPath,ret.video.src,'uri');
		util.fixURI(info.absPath,ret,'poster');
		ret.subtitles.forEach( (s) => util.fixURI( info.absPath, s, 'uri' ) );

		ret.id = info.absPath;
		return ret;
	}

	checkInput() {
	}

	//	protected:
	sigName( sig ) {
		return this.type() + sig;
	}

	update( info ) {
		log.silly( 'Repository', 'Update video metadata: id=%s', info.absPath );
		return this.writeInfo( info )
			.then( () => this._events.emit( this.sigName('Updated'), this.exportMeta(info) ) );
	}

	getAbsPath( name ) {
		return path.join( this._absPath, name );
	}

	getInfo( id ) {
		return this.parseInfo( id )
			.catch(function (err) {
				throw tvdutil.warnErr( 'Repository', 'Video not exists: id=%s, err=%s', id, err.message );
			});
	}

	writeInfo( info ) {
		return this.writeJson( info.absPath, info.meta );
	}

	writeJson( path, meta ) {
		//	Save meta
		return fs.writeFileAsync( this.metaFile(path), JSON.stringify(meta, null, 4) );
	}

	parseInfo( absPath ) {
		let self = this;
		let metaPath = this.metaFile(absPath);
		log.silly( 'Repository', 'getInfo: absPath=%s', metaPath );
		return fs.readFileAsync( metaPath )
			.then(JSON.parse)
			.then(function (jsonData) {
				return {
					type: self.type(),
					absPath: absPath,
					meta: jsonData
				};
			});
	}

	metaFile( absPath ) {
		return path.join(absPath,MetaFileName);
	}

	findMeta( rootPath, maxLevel ) {
		let ret = [];
		let self = this;
		function find( info ) {
			if (info.stats.isFile() && info.entry === MetaFileName) {
				return self.parseInfo( path.dirname(info.absPath) )
					.then( (info) => ret.push(info) )
					.catch(function(err) {
						log.warn( 'Repository', 'Cannot parse metadata: meta=%s, err=%s', info.absPath, err.message );
					});
			}
			return bPromise.resolve();
		}

		let opts = {
			root: rootPath,
			level: maxLevel,
			cbEntry: find,
			cbEnd: () => ret
		};
		return tvdutil.findFiles( opts );
	}
}

class MovieDelegate extends Delegate {
	constructor( evts, basePath ) {
		super( evts );
		this._absPath = path.join(basePath,'movies');
	}

	getAll() {
		return this.findMeta( this._absPath, 1 );
	}

	count() {
		return fs.readdirAsync( this._absPath )
			.then( (entries) => entries.length );
	}

	//	protected:
	type() {
		return 'movie';
	}

	getFields() {
		return [ 'title', 'country', 'genre', 'year', 'rating', 'runtime', 'alternativeTitle', 'poster', 'plot', 'director', 'writer', 'actors', 'summary', 'language' ];
	}

	getPathName( meta ) {
		if (meta.title) {
			return meta.title;
		}

		var base = path.basename( meta.video.src.uri );
		return base.substring(0,base.indexOf('.'));
	}
}

class QueuedMovieDelegate extends MovieDelegate {
	constructor( evts, basePath ) {
		super( evts, basePath );
		this._absPath = path.join(basePath,'queued');
	}

	type() {
		return 'queuedMovie';
	}

	checkInput() {
	}
}

class SerieDelegate extends Delegate {
	constructor( evts, basePath ) {
		super( evts );
		this._absPath = path.join(basePath,'series');
	}

	getSeries() {
		let self = this;
		return fs.readdirAsync( self._absPath )
			.map(function(serieEntry) {
				let absPath = self.getSeriePath(serieEntry);
				return bPromise.join(
					self.getSerieInfo( absPath ),
					self.getSeasons( absPath ),
					function(serieInfo,seasons) {
						serieInfo.id = absPath;
						serieInfo.seasons = seasons;
						return serieInfo;
					});
			});
	}

	existsSerie( sID ) {
		assert(sID);
		return fs.statAsync( this.getSeriePath(sID) )
			.then( (stats) => stats.isDirectory() )
			.catch( () => false );
	}

	addSerie( sInfo ) {
		log.verbose( 'Repository', 'addSerie: title=%s', sInfo.title );
		let serieId = this.getSeriePath( sInfo.title );
		return fs.ensureDirAsync( serieId )
			.then( () => this.writeJson( serieId, sInfo ) );
	}

	getEpisodes( serieId, seasonName ) {
		let seasonPath = path.join(serieId,util.zeroFill(seasonName));
		log.silly( 'Repository', 'Get episodes: path=%s', seasonPath );
		return this.findMeta( seasonPath, 1 )
			.catch( () => [] );
	}

	nextEpisode( serieId ) {
		log.verbose( 'Repository', 'nextEpisode: serieId=%s', serieId );
		return this.getSerieInfo( serieId )
			.then( (info) => this.getLastSeen(serieId,info) )
			.then( (info) => this.nextEpisodeInfo( path.basename(serieId), info.lastSeen ) );
	}

	//	protected:
	type() {
		return 'serie';
	}

	updateSeen( info, state ) {
		return super.updateSeen( info, state )
			.then( () => this.updateLastSeen( info ) );
	}

	getEpisodePath( serieId, season, episode ) {
		return this.getAbsPath(this.getPathName({ serie: serieId, season: season, episode: episode }));
	}

	getLastSeen( serieId, serieInfo ) {
		if (serieInfo.lastSeen) {
			return serieInfo;
		}

		return this.getSeasons( serieId )
			.then( (seasons) => {
				let sSeasons = seasons.map( (season) => parseInt(season) ).sort( (a,b) => a-b );
				return this.getFirstEpisode( serieId, sSeasons )
					.then( (first) => {
						serieInfo.lastSeen = {
							season: first.season,
							episode: first.episode-1
						};
						return serieInfo;
					});
			});
	}

	getFirstEpisode( serieId, seasons ) {
		if (seasons.length === 0) {
			return bPromise.resolve({
				season: 0,
				episode: 0
			});
		}
		let season = seasons[0];
		assert(season);
		log.silly( 'Repository', 'getFirstEpisode: serieId=%s, seasons=%j, dir=%s', serieId, seasons, path.dirname(this.getEpisodePath( path.basename(serieId), season, 0 )) );
		return fs.readdirAsync( path.dirname(this.getEpisodePath( path.basename(serieId), season, 0 )) )
			.then( (episodes) => {
				if (episodes.length === 0) {
					seasons.shift();
					return this.getFirstEpisode( serieId, seasons );
				}
				else {
					let sEpisodes = episodes.map( (episode) => parseInt(episode) ).sort( (a,b) => a-b );
					return {
						season: season,
						episode: sEpisodes[0]
					};
				}
			});
	}

	nextEpisodeInfo( serieId, lastSeen ) {
		log.silly( 'Repository', 'nextEpisodeInfo: serieId=%s, lastSeen=%j', serieId, lastSeen );

		let nextEpisodePath = this.getEpisodePath(serieId,lastSeen.season,lastSeen.episode+1);
		return this.parseInfo( nextEpisodePath )
			.then(this.exportMeta, () => {
				let nextSeasonPath = this.getEpisodePath(serieId,lastSeen.season+1,1);
				return this.parseInfo( nextSeasonPath )
					.then(this.exportMeta, () => undefined );
			});
	}

	getSerieInfo( absPath ) {
		return this.parseInfo(absPath)
			.then( (info) => info.meta )
			.catch(function () {
				return {
					title: path.basename(absPath)
				};
			});
	}

	updateLastSeen( eInfo ) {
		let serieId = this.getSeriePath( eInfo.meta.serie );
		log.silly( 'Repository', 'updateLastSeen: serieId=%s', serieId );
		return this.getSerieInfo( serieId )
			.then( (sInfo) => {
				sInfo.lastSeen = {
					season: eInfo.meta.season,
					episode: eInfo.meta.episode
				};
				return this.writeJson( serieId, sInfo );
			});
	}

	getSeasons( absPath ) {
		let ret = [];
		let opts = {
			root: absPath,
			level: 0,
			cbEntry: bPromise.method( (info) => {
				if (info.stats.isDirectory()) {
					ret.push( info.entry );
				}
			}),
			cbEnd: () => ret
		};
		return tvdutil.findFiles( opts )
			.catch( () => [] );
	}

	getFields() {
		return [ 'title', 'serie', 'season', 'episode', 'summary' ];
	}

	getSeriePath( entry ) {
		return path.join(this._absPath,entry);
	}

	getPathName( meta ) {
		assert(meta.serie);
		let serie = meta.serie;
		let season = meta.season ? util.zeroFill(meta.season) : '01';
		let episode = meta.episode ? util.zeroFill(meta.episode) : '01';
		return path.join(serie,season,episode);
	}
}

module.exports.MovieDelegate = MovieDelegate;
module.exports.SerieDelegate = SerieDelegate;
module.exports.QueuedMovieDelegate = QueuedMovieDelegate;
