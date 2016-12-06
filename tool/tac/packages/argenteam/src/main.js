'use strict';

var assert = require('assert');
var tvdutil = require('tvdutil');
var processFeed = require('feed');
var bPromise = require('bluebird');
var _ = require('lodash');
var WebModule = require('./webmodule');

var _fieldsRating = {
	source: {
		'BDRip': 10,
		'HDTV': 10,
		'DVDRip': 10,
		'WEB-DL': 0
	},
	size: [{
		type: 'serie',
		min: 600,
		max: 1400,
		points: 10
	},{
		type: 'serie',
		min: 1401,
		max: 100000,
		points: 5
	},{
		type: 'movie',
		min: 6001,
		max: 10000000,
		points: 5
	},{
		type: 'movie',
		min: 3000,
		max: 6000,
		points: 10
	}]
};

function getSize( str ) {
	var elem = str.split( ' ' );
	assert(elem.length === 2);
	var num = Number(elem[0]);
	switch (elem[1]) {
	case "MB":
		return num;
	case "GB":
		return num * 1024;
	case "KB":
		return num / 1024;
	}
}

function ServiceApi(impl) {
	var self = {};

	function doAsync( prom, cb ) {
		prom.then( _.partial(cb,undefined), cb );
	}

	self.hasSerie = impl.hasSerie.bind(impl);
	self.addSerie = impl.addSerie.bind(impl);
	self.removeSerie = impl.removeSerie.bind(impl);
	self.listSeries = impl.listSeries.bind(impl);

	self.getAvailableSeries = impl.getAvailableSeries.bind(impl);
	self.getAvailableSeries.isAsync = true;

	self.checkFeeds = impl.checkFeeds.bind(impl);
	self.addID = function( type, id, serieName, cb ) {
		if (!cb) {
			cb = serieName;
			serieName = undefined;
		}
		doAsync( impl.addID( type, id, serieName ), cb );
	};

	return Object.freeze(self);
}

function ServiceImpl(adapter) {
	var self = {};
	var _cfg = null;
	var _workCondition = null;
	var _timeout = null;

	function reg(srvID) {
		return adapter.registry().get(srvID);
	}

	//	Aux config
	function cfgFile() {
		return 'config.json';
	}

	function defaultCfg() {
		return {
			baseURL: 'http://www.argenteam.net/',
			serie: {
				feed: 'rss/portal_series_subtitles.xml',
				api: 'api/v1/episode',
				fields: [ 'title', 'season', 'episode', 'summary' ],
			},
			movie: {
				feed: 'rss/portal_movies_subtitles.xml',
				api: 'api/v1/movie',
				fields: [ 'title', 'country', 'genre', 'year', 'rating', 'runtime', 'alternativeTitle', 'poster', 'director', 'actors', 'summary' ],
			},
			timeout: {
				start: 5*60*1000,
				period: 8*60*60*1000
			},
			series: []
		};
	}

	function convertCfg( oldCfg ) {
		return oldCfg;
	}

	function saveCfg() {
		adapter.save( cfgFile(), _cfg );
	}

	//	Aux API
	function getUrl( type, field ) {
		return _cfg.baseURL + _cfg[type][field];
	}

	function getSerie( name ) {
		return _cfg.series.find( (spec) => spec.name === name );
	}

	function getInfo( oper ) {
		log.silly( 'argenteam', 'getInfo: id=%s', oper.id );
		var opts = {
			url: oper.apiURL + '?id='+oper.id,
			method: 'GET'
		};
		return tvdutil.doRequestAsync( opts )
			.then(function(res) {
				if (!res.data) {
					throw tvdutil.warnErr( "argenteam", "No information available: id=%s", oper.id );
				}
				oper.info = res.data;
				return oper;
			});
	}

	function chooseRelease( oper ) {
		assert(oper);
		log.silly( 'argenteam', 'Choose release: id=%s', oper.id );

		let best;
		let bestPoints=-1;
		var rels = oper.info.releases;
		if (!rels) {
			throw tvdutil.warnErr( "argenteam", "No release available: id=%s, info=%j", oper.id, oper.info );
		}

		for (var i=0; i<rels.length; i++) {
			var rel = rels[i];
			if (rel.torrents.length > 0 && rel.subtitles.length > 0) {
				log.verbose( 'argenteam', 'Found good release: %s, %s, %s, %s, %d', rel.source, rel.codec, rel.team, rel.size, rel.subtitles[0].count );

				var points = 0;
				points += _fieldsRating.source[rel.source] || 0;

				{	//	Check size
					let s = getSize(rel.size);

					//	Get range
					for (let rI=0; rI<_fieldsRating.size.length; rI++) {
						let r = _fieldsRating.size[rI];

						if (r.type === oper.type && r.min <= s && s <= r.max) {
							points += r.points;
							break;
						}
					}
				}

				if (points > bestPoints) {
					bestPoints = points;
					best = rel;
				}
			}
		}

		if (best) {
			oper.choosed = best;
			log.info( 'argenteam', 'Best release: points=%d, info=%s, %s, %s, %s, %d', bestPoints, best.source, best.codec, best.team, best.size, best.subtitles[0].count );
			return oper;
		}

		throw tvdutil.warnErr( "argenteam", "No release available: id=%s, info=%j", oper.id, oper.info );
	}

	function mapInfo( oper ) {
		assert(oper);
		log.silly( 'argenteam', 'mapInfo: id=%s', oper.id );

		//	Always select magnet
		let uri;
		{
			let index=0;
			while (index < oper.choosed.torrents.length) {
				let item = oper.choosed.torrents[index];

				if (item.uri && item.uri.indexOf('magnet:') === 0) {
					uri = item.uri;
					break;
				}

				if (item.alt && item.alt.indexOf('magnet:') === 0) {
					uri = item.alt;
					break;
				}

				index++;
			}
		}

		if (!uri) {
			throw tvdutil.warnErr( "argenteam", "No valid uri available: torrents=%j", oper.choosed.torrents );
		}

		oper.result = {
			src: {
				type: 'torrent',
				uri: uri
			},
			size: getSize(oper.choosed.size),
			subtitles: oper.choosed.subtitles.map( (sub) => ({ uri: sub.uri }) )
		};

		if (oper.type === 'serie') {
			oper.result.serie = oper.name;
			oper.info.episode = parseInt(oper.info.number);
		}

		//	Map fields
		let fields = _cfg[oper.type].fields;
		fields.forEach(function(field) {
			let val;
			if (oper.info.info) {
				val = oper.info.info[field];
			}
			if (!val) {
				val = oper.info[field];
			}
			if (val) {
				oper.result[field] = val;
			}
		});

		return oper;
	}

	function addVideo( oper ) {
		let fncName;
		if (oper.type === 'serie') {
			fncName = 'add';
		}
		else if (oper.mustAdd) {
			fncName = 'add';
		}
		else {
			fncName = 'queueMovie';
		}

		log.silly( 'argenteam', 'addVideo: type=%s, id=%s, fnc=%s', oper.type, oper.id, fncName );

		return new bPromise(function(resolve,reject) {
			let api = reg('ar.edu.unlp.info.lifia.tvd.media.main')[oper.type];
			api[fncName]( oper.result, function(err) {
				if (err) {
					reject(err);
				}
				else {
					resolve(oper.result);
				}
			});
		});
	}

	function processVideo( oper ) {
		log.info( 'argenteam', 'processVideo: id=%s', oper.id );

		oper.apiURL = getUrl(oper.type,'api');
		oper.mustAdd = oper.mustAdd || false;
		return getInfo(oper)
			.then(chooseRelease)
			.then(mapInfo)
			.then(addVideo)
			.catch(function(err) {
				log.warn( 'argenteam', 'Cannot add video: err=%s', err.message );
				throw err;
			});
	}

	function processFeedItem( type, item ) {
		log.silly( 'argenteam', 'Process feed item: type=%s, title=%s', type, item.title );

		//	Check item
		if (!item.title || !item.link) {
			throw tvdutil.warnErr( 'argenteam', 'Invalid item: item=%j', item );
		}

		let iURL = item.link.indexOf(_cfg.baseURL);
		if (iURL < 0) {
			throw tvdutil.warnErr( 'argenteam', 'Invalid item link: link=%s', item.link );
		}

		//	Find episode ID
		let id;
		{
			let sub = item.link.substring(_cfg.baseURL.length);
			sub = sub.substring(sub.indexOf('/')+1);
			let iSub = sub.indexOf('/');
			if (iSub < 0) {
				id = sub;
			}
			else {
				id = sub.substring(0,iSub);
			}
		}
		if (!id) {
			throw tvdutil.warnErr( 'argenteam', 'Invalid id: id=%s', id );
		}

		//	Find title
		let iSep = item.title.indexOf('|');
		let title = (iSep >= 0) ? item.title.substring(0,iSep-1) : item.title;
		if (!title) {
			throw tvdutil.warnErr( 'argenteam', 'Invalid title: title=%s', item.title );
		}

		if (type === 'serie') {
			let index=_cfg.series.findIndex(function (spec) {
				let patt = new RegExp(spec.filter);
				return patt.test(title);
			});

			if (index < 0) {
				return bPromise.resolve();
			}
		}

		return processVideo({
			type: type,
			id: id,
			name: title
		});
	}

	function checkFeed( type ) {
		log.silly( 'argenteam', 'checkFeed: type=%s', type );
		let opts = {
			url: getUrl(type,'feed'),
			lastPubDate: _cfg[type].lastCheck
		};
		return processFeed(opts)
			.then(function (result) {
				_cfg[type].lastCheck = result.lastPubDate;

				let proms = [];

				result.items.forEach(function(item) {
					try {
						proms.push( processFeedItem(type,item).reflect() );
					} catch(err) {}
				});

				return bPromise.all(proms);
			});
	}

	function checkFeedsAux() {
		log.silly( 'argenteam', 'checkFeeds' );
		let feeds = [ 'movie' ];
		if (_cfg.series.length > 0) {
			feeds.push( 'serie' );
		}
		bPromise.map( feeds, checkFeed )
			.catch(function(err) {
				log.warn( 'argenteam', 'A error ocurred when check feed: err=%s', err.message );
			})
			.finally(function() {
				checkFeedsTimeout();
			});
	}

	function checkFeedsTimeout( ms ) {
		if (_timeout) {
			clearTimeout(_timeout);
		}
		_timeout = setTimeout( checkFeedsAux, ms || _cfg.timeout.period );
	}

	//	Service API
	self.addSerie = function( spec ) {
		log.info( 'argenteam', 'Add serie: spec=%j', spec );
		if (!self.hasSerie(spec.name)) {
			_cfg.series.push({
				name: spec.name,
				filter: spec.filter
			});
			saveCfg();
		}
	};

	self.hasSerie = function( name ) {
		return getSerie( name ) ? true : false;
	};

	self.removeSerie = function(name) {
		log.info( 'argenteam', 'Remove serie: name=%s', name );
		var index = _cfg.series.findIndex( spec => spec.name === name );
		if (index >= 0) {
			_cfg.series.splice( index, 1 );
			saveCfg();
		}
	};

	self.listSeries = function() {
		return _cfg.series.map( (spec) => spec.name );
	};

	self.getAvailableSeries = function( cb ) {
		cb(undefined,[]);
	};

	self.addID = function( type, id, serieName ) {
		let oper = {
			type: type,
			id: id,
			name: serieName,
			mustAdd: true
		};
		return processVideo( oper );
	};

	self.checkFeeds = function() {
		checkFeedsTimeout( 100 );
	};

	//	Servie entry point
	self.start = function(cb) {
		//	Load configuration
		_cfg = adapter.load( cfgFile(), defaultCfg(), convertCfg() );
		assert(_cfg);

		//	Setup work condition
		_workCondition = new tvdutil.BasicWaitCondition();

		//	Check feeds if necesary
		checkFeedsTimeout( _cfg.timeout.start );

		reg('remoteapi').addModule( new WebModule(self) )
			.then( () => cb( undefined, new ServiceApi(self) ), cb );
	};

	self.stop = function(cb) {
		//	Wait until nothing async is working
		_workCondition.wait(function() {
			clearTimeout(_timeout);
			_timeout = null;

			reg('remoteapi').rmModule( 'argenteam' )
				.then(function() {
					saveCfg();
					cb();
				});
		});
	};

	return self;
}

function Service(adapter) {
	var self = {};
	var _impl = new ServiceImpl(adapter);

	self.onStart = function(cb) {
		log.info( 'argenteam', 'Start' );
		_impl.start(cb);
	};

	self.onStop = function(cb) {
		log.info( 'argenteam', 'Stop' );
		_impl.stop(cb);
	};

	return Object.freeze(self);
}

// Export module hooks
module.exports = Service;
