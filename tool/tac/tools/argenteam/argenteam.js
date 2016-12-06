'use strict';

var path = require('path');
var assert = require('assert');
var tvdutil = require('tvdutil');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs-extra') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var _ = require('lodash');
var minimist = require("minimist");
var queryString = require('querystring');
var UrlParser = require('url');
var request = require('request');
var FeedParser = require('feedme');

var _argenteamURL = 'http://www.argenteam.net/';
var _argenteam = {
	serie: {
		feed: _argenteamURL + 'rss/portal_series_subtitles.xml',
		api: _argenteamURL + 'api/v1/episode',
		fields: [ 'title', 'season', 'episode', 'summary' ],
		apiMethod: () => 'add'
	},
	movie: {
		feed: _argenteamURL + 'rss/portal_movies_subtitles.xml',
		api: _argenteamURL + 'api/v1/movie',
		fields: [ 'title', 'country', 'genre', 'year', 'rating', 'runtime', 'alternativeTitle', 'poster', 'director', 'actors', 'summary' ],
		apiMethod: (queue) => queue ? 'queueMovie' : 'add'
	}
};

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

function parseFeed( feed ) {
	log.silly( 'argenteam', 'Parse feed: feed=%s', feed );

	return new bPromise(function(resolve,reject) {
		let items = [];

		var parser = new FeedParser();
		//parser.on('pubdate', function(pub) {	//	TODO: Check pubdate?
		parser.on('item', item => items.push(item) );
		parser.on('end', () => resolve(items) );
		parser.on('error', reject );

		request
			.get( feed )
			.on('response', function(response) {
				if (response.statusCode !== 200) {
					reject( tvdutil.warnErr( 'Repository', 'Cannot download file; invalid status code: st=%d', response.statusCode) );
				}
			})
			.on('error', reject )
			.pipe(parser);
	});
}

function getInfo( oper ) {
	log.silly( 'argenteam', 'getInfo: id=%s', oper.id );
	var opts = {
		url: oper.url + '?id='+oper.id,
		method: 'GET'
	};
	return tvdutil.doRequestAsync( opts )
		.then(function(res) {
			oper.episodeInfo = res.data;
			return oper;
		});
}

function chooseRelease( oper ) {
	assert(oper);
	log.silly( 'argenteam', 'Choose release: id=%s', oper.id );

	let best;
	let bestPoints=-1;
	var rels = oper.episodeInfo.releases;
	if (!rels) {
		throw tvdutil.warnErr( "argenteam", "No release available: id=%s, info=%j", oper.id, oper.episodeInfo );
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

	throw tvdutil.warnErr( "argenteam", "No release available: id=%s, info=%j", oper.id, oper.episodeInfo );
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

			if (item.uri && item.uri.indexOf('magnet:') == 0) {
				uri = item.uri;
				break;
			}

			if (item.alt && item.alt.indexOf('magnet:') == 0) {
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

	if (oper.episodeInfo.number) {
		oper.episodeInfo.episode = parseInt(oper.episodeInfo.number);
	}

	let fields = _argenteam[oper.type].fields;
	fields.forEach(function(field) {	//	TODO: Invertir asi sabemos que fields no estan mapeados!
		let val;
		if (oper.episodeInfo.info) {
			val = oper.episodeInfo.info[field];
		}
		if (!val) {
			val = oper.episodeInfo[field];
		}
		if (val) {
			oper.result[field] = val;
		}
	});

	if (oper.type === 'serie') {
		oper.result.serie = oper.serieName;
	}

	return oper;
}

function addVideo( oper ) {
	log.silly( 'argenteam', 'addVideo: is=%s, title=%s', oper.id, oper.result.title );
	var data = {
		'serviceID': 'ar.edu.unlp.info.lifia.tvd.media.main',
		'apiName': 'media.' + oper.type,
		'method': _argenteam[oper.type].apiMethod(oper.queue),
		'params': [oper.result]
	};

	if (oper.mockVideo) {
		let tmpName = mktemp.createFileSync( path.join('/tmp', 'movie_simpson_XXXXX.mp4') );
		fs.copySync( oper.mockVideo, tmpName );
		oper.result.src = {
			type: 'file',
			uri: tmpName
		};
	}

	if (oper.dryRun) {
		console.log( "send: oper=%j, data=%j", oper, data );
	}
	else {
		let tac_ip = oper.host;
		let tac_port = oper.port;
		var reqOpts = {
			method: 'POST',
			url: 'http://'+tac_ip+':'+tac_port+'/api',
			data: data
		};

		return tvdutil.doRequestAsync( reqOpts ).reflect();
	}
}

function processVideo( opts, id, name ) {
	log.info( 'argenteam', 'processVideo: id=%s', id );

	let oper = _.cloneDeep(opts);
	oper.id = id;
	if (!oper.serieName && name) {
		oper.serieName = name;
	}

	return getInfo(oper)
		.then(chooseRelease)
		.then(mapInfo)
		.then(addVideo)
		.catch(function(err) {
			log.warn( 'argenteam', 'Cannot add video: err=%s', err.message );
		});
}

function processFeedItem( opts, item ) {
	//	Check item
	if (!item.title || !item.link) {
		log.warn( 'argenteam', 'Invalid item: item=%j', item );
		return;
	}

	let iURL = item.link.indexOf(_argenteamURL);
	if (iURL < 0) {
		log.warn( 'argenteam', 'Invalid item link: link=%s', item.link );
		return;
	}

	//	Find episode ID
	let id;
	{
		let sub = item.link.substring(_argenteamURL.length);
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
		log.warn( 'argenteam', 'Invalid id: id=%s', id );
		return;
	}

	//	Find title
	let iSep = item.title.indexOf('|')
	let title = (iSep >= 0) ? item.title.substring(0,iSep-1) : item.title;
	if (!title) {
		log.warn( 'argenteam', 'Invalid title: title=%s', item.title );
		return;
	}

	return processVideo( opts, id, title );
}

function main() {
	let argv = minimist(process.argv.slice(2));
	tvdutil.setupLog( argv.logLevel || 'silly' );

	assert( argv.type === 'serie' || argv.type === 'movie');

	let opts = {
		type: argv.type,
		url: _argenteam[argv.type].api,
		mockVideo: argv.mockVideo,
		serieName: argv.name,
		host: argv.host || '127.0.0.1',
		port: argv.port || 2000,
		dryRun: argv.dryRun,
		queue: argv.queue || false
	};

	if (argv.feed) {
		return parseFeed( _argenteam[opts.type].feed )
			.map((items) => processFeedItem(opts,items));
	}
	else {
		if (argv.type === 'serie') {
			assert(argv.name);
		}

		let id = String(argv.id);
		if (id.indexOf(',') !== -1) {
			let videos = argv.id.split(',');
			return bPromise.map(videos, function(id) {
				return processVideo( opts, id );
			});
		}

		return processVideo( opts, id );
	}
}

main();
