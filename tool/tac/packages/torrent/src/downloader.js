'use strict';

var assert = require('assert');
var _ = require('lodash');
var path = require('path');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs-extra') );
var tvdutil = require('tvdutil');
var mURI = require('magnet-uri');

function TorrentModule( torrentClient ) {
	assert( torrentClient, 'Invalid torrent client' );
	
	var _workDir;
	var _torrentCheck;
	var _onComplete;
	var _downloads = [];
	var _timeout = null;

	function cfgFile() {
		return path.join(_workDir,'config.json');
	}

	function readCfg() {
		return fs.readFileAsync( cfgFile() )
			.then(JSON.parse)
			.catch(function(err) {
				log.verbose( 'TorrentModule', 'Cannot read torrent config: err=%s', err.message );
				return [];
			});
	}

	function writeCfg() {
		return fs.writeFileAsync( cfgFile(), JSON.stringify(_downloads, null, 4) );
	}

	function restartTimeout( mult ) {
		assert(!_timeout, 'torrent.restartTimeout: Timeout not released');
		_timeout = setTimeout( () => checkTorrentHashes(), _torrentCheck * (mult || 1) );
	}

	function processTorrentFinished( info, torrent ) {
		assert(info, 'torrent.processTorrentFinished: Invalid info');
		assert(torrent, 'torrent.processTorrentFinished: Invalid torrent');
		log.silly( 'TorrentModule', 'Torrent completed: id=%s, hash=%s', info.id, torrent.hash );
		return torrentClient.stop( torrent.hash, true )
			.then( () => _onComplete({ id: info.id, path: torrent.path }) )
			.catch(function (e) {
				let err = tvdutil.warnErr( 'TorrentModule', 'A error ocurred when process torrent: hash=%s, err=%s', torrent.hash, e.message );
				_onComplete({ err: err, id: info.id });
			});
	}

	function checkTorrentHashes() {
		log.silly( 'TorrentModule', 'Check torrent hashes: hashes=%d', _downloads.length );

		_timeout = null;

		torrentClient.getAll()
			.then(function(torrents) {
				function findTorrent(hash) {
					return torrents.find( (t) => t.hash === hash );
				}

				let proms = [];

				for (let i=0; i<_downloads.length; i++) {
					let info = _downloads[i];
					let prom;
					let remove = false;

					let torrent = findTorrent(info.hash);
					if (torrent) {
						if (parseInt(torrent.complete) === 1) {
							remove=true;
							prom = processTorrentFinished( info, torrent );
						}
					}
					else {
						remove=true;
						let err = tvdutil.warnErr( 'TorrentModule', 'Some error ocurred in torrent client; hash not found: hash=%s', info.hash );
						_onComplete({ err: err, id: info.id });
						prom = bPromise.resolve();	//	Force write config!
					}

					if (remove) {
						_downloads.splice( i, 1 );
					}

					if (prom) {
						//	Write config file
						proms.push( prom.then(writeCfg).reflect() );
					}
				}

				return bPromise.all(proms);
			})
			.then(function() {
				if (_downloads.length > 0) {
					restartTimeout();
				}
			}, function(err) {
				log.warn( 'TorrentModule', 'A error ocurred when get torrent state: err=%s', err.message );
				restartTimeout( 5 );
			});
	}

	function startTorrent( info ) {
		log.silly( 'TorrentModule', 'Start torrent check: hash=%s', info.hash );
		return torrentClient.start( info.uri )
			.then(function() {
				_downloads.push( info );
				if (_downloads.length === 1) {
					restartTimeout();
				}
			});
	}

	//--------------------- -----------------------------------------------------------------------
	//    API
	//--------------------- -----------------------------------------------------------------------
	var self = {};

	self.name = 'torrent';
	self.type = 'torrent';

	self.fetch = function( src ) {
		assert(src, 'TorrentModule.fetch: Invalid src');
		assert(src.uri, 'TorrentModule.fetch: Invalid src.uri');
		assert(src.type === self.type, 'TorrentModule.fetch: Invalid src.type');
		log.silly( 'TorrentModule', 'Download torrent video: uri=%s', src.uri );

		let magnet = mURI.decode(src.uri);
		if (!magnet || !magnet.infoHash) {
			return bPromise.reject(tvdutil.warnErr( 'TorrentModule', "Invalid magnet link: uri=%s", src.uri ));
		}

		let info = _.clone(src);
		info.hash = magnet.infoHash.toUpperCase();

		return startTorrent( info )
			.then(writeCfg);
	};

	self.start = function( opts ) {
		assert(opts.workDir, 'TorrentModule.start: Invalid workDir');
		assert(opts.onComplete, 'TorrentModule.start: Invalid onComplete');

		_workDir = opts.workDir;
		_onComplete = opts.onComplete;
		_torrentCheck = opts.checkPeriod || (60*1000);
		_downloads = [];
		_timeout = null;

		return readCfg()
			.map( (info) => startTorrent(info) );
	};

	self.stop = bPromise.method(function() {
		clearTimeout(_timeout);
		_timeout = null;
	});

	return self;
}

module.exports = TorrentModule;

