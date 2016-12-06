"use strict";

var util = require('util');
var bodyParser = require('body-parser')
var _ = require("lodash");
var url = require('url');
var fs = require('fs');
var path = require('path');
var express = require('express');
var printf = require('printf');
var mkdirp = require('mkdirp');

//	Definitions
var SERVICE_CONFIG = '/tacconfig/configuration';
var SERVICE_UPDATE_FIRMWARE = '/firmware/update';
var SERVICE_UPDATE_PACKAGE = '/package/update';
var SERVICE_CRASH = '/tacerror';

//	Global variables
var server_cfg = null;
var app = express();

function log() {
	if (server_cfg.enable_log) {
		console.log.apply( console.log, arguments );
	}
}

function loadConfig( cfg ) {
	try {
		var str=fs.readFileSync( cfg );
		return JSON.parse( str );
	} catch(err) {
		log( "No se pudo cargar el archivo: %s", cfg );
		return null;
	}
}

function getServerUrl( srv ) {
	var url = server_cfg.url;
	if (server_cfg.port) {
		url += ':' + server_cfg.port;
	}
	return url + srv;
}

function makeNewSerial() {
	var now = new Date();
	var year = now.getFullYear();
	var month = now.getMonth()+1;
	var day = now.getDate();
	var hours = now.getHours();
	var min = now.getMinutes();
	var sec = now.getSeconds();
	return printf( '%04d%02d%02d_%02d%02d%02d',
						year, month, day, hours, min, sec );
}

function getPlatform( platform ) {
	var p = null;
	if (platform == 'aml_isdbt_1l') {
		p = 'AML_MICROTROL';
	}
	else if (platform == 'aml_isdbt_0') {
		p = 'AML_CORADIR';
	}
	else if (platform == 'UNKNOWN') {
		return 'DESKTOP';
	}
	return p;
}

function getFirmwareConfig( p ) {
	return path.join(server_cfg.build_path,p,'tac','LATEST.json');
}

function getPackagesConfig( p ) {
	return server_cfg.packages;
}

function getPlatformConfig( p ) {
	return path.join( 'configs', p, 'config.json' );
}

function savePlatformConfig( cfgFile, cfg ) {
	var copy = _.cloneDeep(cfg);
	delete copy.config_server.url;
	delete copy.firmware_update_server.url;
	delete copy.package_update_server.url;
	delete copy.urlCrashServer;
	mkdirp.sync( path.dirname(cfgFile) );
	fs.writeFileSync( cfgFile, JSON.stringify(copy, null, 4) );
}

function getModifiedTime( configFile ) {
	try {
		var st = fs.statSync( configFile );
		return st.mtime;
	}
	catch (err) {
		log( 'Error reading stats from config file: file=%s, err=%s', configFile, err );
		return null;
	}
}

function makeConfig( cfgFile ) {
	var def = {
		serial: makeNewSerial(),
        log: {
            enabled: true,
            maxHistorySize: 1000,
            maxWidth: null, // Disable maxWidth
            level: 'verbose',
            port:3045
        },
        config_server: {
            'retryTimeout': (5*60*1000),    //      5 minutes
            'checkTimeout': (60*60*1000)    //      1 hour
        },
        firmware_update_server: {
            'retryTimeout': (5*60*1000),    //      5 minutes
            'checkTimeout': (60*60*1000)    //      1 hour
        },
        package_update_server: {
            'retryTimeout': (5*60*1000),    //      5 minutes
            'checkTimeout': (60*60*1000)    //      1 hour
        },
        system_packages: []
    };

	//log( 'Creando archivo de configuracion: file=%s', cfgFile );
	savePlatformConfig( cfgFile, def );
	return def;
}

function getConfig( configFile ) {
	//	Load config from file
	var cfg = loadConfig( configFile );
	if (!cfg) {
		//	Create config
		cfg = makeConfig( configFile );
	}

	//	Set static fields
	cfg.config_server.url = getServerUrl( SERVICE_CONFIG );
	cfg.firmware_update_server.url = getServerUrl( SERVICE_UPDATE_FIRMWARE );
	cfg.package_update_server.url = getServerUrl( SERVICE_UPDATE_PACKAGE );
	cfg.urlCrashServer = getServerUrl( SERVICE_CRASH );

	//log( 'get config: serial=%s', cfg.serial );
	return cfg;
}

function getVersion(v) {
	var comps = v.split('.');
	return comps[2];
}

function needUpdate(v1,v2) {
	var v1i = parseInt(v1);
	var v2i = parseInt(v2);
	log( 'Need update: stb=%s, last=%s', v1, v2 );
	return v1i < v2i;
}

function fixFirmwareURL(urlstr) {
	var oURL = url.parse(urlstr);
	oURL.auth = 'buildbot:0303456';
	return url.format(oURL);
}

function checkRequest(req,node) {
	if (!req.body.platform || !req.body.version) {
		log( "Error en request, no platform/version: request=%s, ip=%s, body=%j", node, req.connection.remoteAddress, req.body );
		return null;
	}

	var plat = getPlatform(req.body.platform);
	if (!plat) {
		log( "Error en request, no platform: request=%s, ip=%s, body=%j", node, req.connection.remoteAddress, req.body );
		return null;
	}

	return plat;
}

app.get( SERVICE_CONFIG, bodyParser.json(), function(req, res){
	var plat = checkRequest(req,SERVICE_CONFIG);
	if (!plat) {
		return;
	}
	//	Get stb config
	var cfg = getConfig( getPlatformConfig( plat ) );
	log( 'Send config: cfg=%j', cfg );
	res.send( cfg );
});

app.get( SERVICE_UPDATE_FIRMWARE, bodyParser.json(), function(req, res){
	var plat = checkRequest(req,SERVICE_UPDATE_FIRMWARE);
	if (!plat) {
		return;
	}

	// if (plat == "DESKTOP") {
	// 	log( 'DESKTOP platform not supported' );
	// 	return;
	// }

	//	Parse firmware version
	var cfg = {};
	try {
		var data = JSON.parse( fs.readFileSync( getFirmwareConfig(plat) ));
		if (needUpdate(getVersion(req.body.version),data.version)) {
			cfg.url = fixFirmwareURL(data.firmware);
			cfg.version = data.version;
			cfg.name = "Sin nombre";
			cfg.description = data.description;
			cfg.md5 = data.md5;
			cfg.size = data.size;
			cfg.mandatory = data.mandatory ? data.mandatory : false;
		}
	} catch( error ) {
		log( 'Se produjo un error: err=%s, stack=%s', error, error.stack );
	}

	log( 'Send firmware update info: %j', cfg  );
	res.send( cfg );
});

//  Request: {
//      "id": info.id,
//      "platform": info.platform.name,
//      "version": info.system.version,
//      "pkgID": pkgID,
//      "pkg-version": version
//  }
app.get( SERVICE_UPDATE_PACKAGE, bodyParser.json(), function(req, res){
	var plat = checkRequest(req,SERVICE_UPDATE_PACKAGE);
	if (!plat) {
		return;
	}

	var cfg = {};
	try {
		//	Parse package version
		var data = JSON.parse( fs.readFileSync( getPackagesConfig(plat) ));

		log( 'body=%j', req.body );

		//	Exists package?
		var pkg = data[req.body.pkgID];
		if (pkg) {
			log( 'pkg=%j', pkg );

			//	TODO: Check platform!
			if (req.body['pkg-version'] !== pkg.version) {
				cfg = pkg;
			}
		}
	} catch(error) {
		log( 'Se produjo un error: err=%s, stack=%s', error, error.stack );
	}

	log( 'Send package update info: %j', cfg  );
	res.send( cfg );
});

app.post( SERVICE_CRASH, bodyParser.json({limit: 1050620}), function(req, res) {
	var name = 'crash-' + req.connection.remoteAddress + '-' + makeNewSerial();
	log( 'Get crash report: name=%s', name );
	fs.writeFile(path.join(server_cfg.crash_path,name), JSON.stringify(req.body, null, 4));
	res.status(200).end();
});

//	main
server_cfg = loadConfig( 'server.json' );
if (server_cfg) {
	app.listen( server_cfg.port );
	log('Listening ' + server_cfg.port );
}
