'use strict';

var fs = require('fs');
var tvdutil = require('tvdutil');
var logos = require('./logos');

//	Sample from webservice
// {
// 	"provincia":null,
// 	"id_provincia":null,
// 	"id_productora":"3",
// 	"nombre":"Encuentro",
// 	"solo_en":"",
// 	"enlace":"http://www.encuentro.gov.ar/",
// 	"nombre_imagen":"http://bocetos.minplan.gob.ar/grilla/public/images/productoras/3_82.svg",
// 	"alt_img":"Encuentro",
// 	"title_img":"Encuentro",
// 	"numeros_virtuales_calidad":"MV,SD",
// 	"numeros_virtuales": ["2231","2201"],
// 	"frequency":null,
// 	"service_id":null,
// 	"ts_id":null,
// 	"on_id":null
// }

//	Logo Sample: {"name":"Encuentro","frequency":"-1","nitID":"-1","tsID":"-1","srvID":"-1","channelID":"22.01","logo":"images/encuentro.jpg"}
//	Channel info sample: {"id":0,"channel":"18.01","name":"MOD4-HD","info":{"type":"dvb","frequency":"497","nitID":1934,"tsID":1934,"srvID":61888}}

function createLogos(adapter) {
	var self = {};
	var _db = null;
	var _tvd = null;
	var _cfg = null;
	var _updateTimeout = null;
	var _stopCondition = null;

	function defaultBase() {
		return adapter.getInstallPath();
	}

	function defaultConfig() {
		return {
			dbBase: defaultBase(),
			firstCheck: 5*1000,
			url: 'http://ws.tda.gob.ar/rest/?method=restTac2&service_id=0&ts_id=0&on_id=0&frequency=0',
			lastChanged: '2015-04-24 16:20:23'
		};
	}

	function loadDB( dbBase ) {
		var db = logos.dbName(dbBase);
		try {
			_db = JSON.parse(fs.readFileSync(db));
		} catch(error) {
			log.verbose("tdalogos", "Cannot read/parse logos database: db=%s", db);
			return false;
		}
		return true;
	}

	function finUpdate() {
		loadLogos();
		_tvd.updateLogos();
		_stopCondition.dec();
	}

	function checkForUpdate() {
		log.verbose( 'tdalogos', 'Check for update' );
		var oParams = {
			user: _cfg.user,
			password: _cfg.password,
			url: _cfg.url,
			basePath: adapter.getDataPath(),
			lastChanged: _cfg.lastChanged
		};

		//	Wait
		_stopCondition.inc();

		logos.fetchLogos( oParams )
			.then(function(result) {
				log.info( 'tdalogos', 'DB fetched: len=%d, path=%s',
							 result.logos.length, result.path );

				//	Save config
				var oldBase = _cfg.dbBase;
				_cfg.dbBase = result.path;
				_cfg.lastChanged = result.lastChanged;
				adapter.save( 'config.json', _cfg );

				if (oldBase !== defaultBase()) {
					//	Remove old
					tvdutil.removeTreeAsync( oldBase )
						.then(finUpdate,finUpdate);
				}
				else {
					//	Reload logos
					finUpdate();
				}
			},function(err) {
				if (err.message !== 'No changes in DB') {
					log.warn( 'tdalogos', 'Error fetching new logos: err=%s', err.message );
				}
				finUpdate();
			});
	}

	function loadLogos() {
		var result = loadDB( _cfg.dbBase );
		if (!result) {
			result=loadDB( defaultBase() );
		}
		return result;
	}

	function makeResult(img,priority) {
		return { 'logo': logos.imageName(_cfg.dbBase,img), 'priority': priority };
	}

	function findLogo(ch) {
		log.silly( "tdalogos", "Find logo: ch=%j", ch );

		for (var i=0; i<_db.length; i++) {
			var chLogo = _db[i];

			//	Find by IDs
			if (chLogo.nitID == ch.info.nitID &&
				chLogo.tsID == ch.info.tsID &&
				chLogo.srvID == ch.info.srvID &&
				chLogo.frequency == ch.info.frequency)
			{
				return makeResult( chLogo.logo, 10 );
			}

			//	Find by name
			if (ch.name.toLowerCase().search( chLogo.name.toLowerCase() ) >= 0) {
				return makeResult( chLogo.logo, 5 );
			}

			//	Find by #channel
			if (chLogo.channelID === ch.channel) {
				return makeResult( chLogo.logo, 5 );
			}
		}

		return null;
	}

	self.onStart = function(cb) {
		_cfg = adapter.load('config.json', defaultConfig());
		if (loadLogos()) {
			//	Add provider to tvd
			_tvd = adapter.registry().get('tvd');
			_tvd.addLogoProvider( findLogo );

			//	Setup wait condition on checkForUpdate
			_stopCondition = new tvdutil.BasicWaitCondition();

			//	Check for logos update
			_updateTimeout = setTimeout( function() {
				checkForUpdate();
			}, _cfg.firstCheck );

			cb( undefined, {} );
		}
		else {
			cb( new Error('Cannot load logos configuration') );
		}
	};

	self.onStop = function(cb) {
		clearTimeout( _updateTimeout );
		_updateTimeout = null;

		_stopCondition.wait(function() {
			//	Remove provider
			_tvd.removeLogoProvider( findLogo );
			_tvd = null;
			cb();
		});
	};

	return Object.freeze(self);
}

// Export module hooks
module.exports = createLogos;

