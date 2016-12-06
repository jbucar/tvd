'use strict';

var assert = require('assert');
var bPromise = require('bluebird');
var crypto = require('crypto');
var fs = bPromise.promisifyAll( require('fs') );
var http = require('http');
var log = require('npmlog');
log.level = 'verbose';
var mailer = require('nodemailer');
var moment = require('moment');
var path = require('path');
var uniformResourceLocator = require('url');

var TAC_MIN_VERSION = '1.0.5195';

var BLACKLISTS = {
	ip: [ '192.168.0.234', '192.168.0.235', '192.168.0.238', '192.168.0.241', '192.168.0.243' ],
	id: []
};

// Convert a string of the form 'X.Y.Z' to a number of the form XXYYZZZZZZ
// X must be 2 digits at the most
// Y must be 2 digits at the most
// Z must be 6 digits at the most
function version2Int(v) {
	var parts = v.split('.');
	assert( parts.length === 3, 'Bad formated version="' + v + '"');
	assert( parts[0].length > 0 && parts[0].length <= 2, "Invalid major version length" );
	assert( parts[1].length > 0 && parts[1].length <= 2, "Invalid minor version length" );
	assert( parts[2].length > 0 && parts[2].length <= 6, "Invalid commit number length" );
	var major = parseInt(parts[0]);
	var minor = parseInt(parts[1]);
	var commit = parseInt(parts[2]);
	return major * 100000000 + minor * 1000000 + commit;
}

Array.prototype.contains = function(element){
	return this.indexOf(element) > -1;
};

String.prototype.insert = function (index, string) {
	if (index > 0)
		return this.substring(0, index) + string + this.substring(index, this.length);
	else
		return string + this;
};

var smtpTransport = mailer.createTransport({
	service: 'gmail',
	auth: {
		user: 'denethor.tvdlifia@gmail.com',
		pass: 'Tvd03034567'
	}
});

function parseCrashes(params) {
	var root = params[0];
	var entries = params[1];

	log.info('main', 'Scanning crashes in: %s', root);

	return bPromise.settle(
		entries.map( function(entry) {
			var info = {
				name: entry,
				path: path.join(root, entry),
				source: root
			};
			return fs.statAsync(info.path)
				.then( function(stats) {
					assert( stats.isFile() );
					var dump = JSON.parse(fs.readFileSync( info.path ));
					assert( version2Int(dump.systemInfo.system.version) >= version2Int(TAC_MIN_VERSION) );
					info.size = stats.size;
					info.type = 'file';
					info.ip = info.name.split('-')[1];
					if (('ar.edu.unlp.info.lifia.tvd.network' in dump.services) && dump.services['ar.edu.unlp.info.lifia.tvd.network'].state && dump.services['ar.edu.unlp.info.lifia.tvd.network'].state.ip) {
						info.ip = dump.services['ar.edu.unlp.info.lifia.tvd.network'].state.ip.split("/")[0];
					}
					info.timestamp = info.name.split('-')[2].insert(4, '-').insert(7, '-').insert(13,':').insert(16, ':').replace('_', ' ');
					return {
						info: info,
						dump: dump
					};
				});
		}))
		.filter(function(prom) {
			return prom.isFulfilled();
		})
		.map(function(prom) {
			return prom.value();
		});
}

function sendMail( result ) {
	var processed_log = '';
	if(Array.isArray(result.dump.log)) {
		result.dump.log.forEach( function(logLine) {
			processed_log += logLine;
			processed_log += '\n';
		});
	} else {
		processed_log = result.dump.log;
	}

	var systemInfo = result.dump.systemInfo;
	var ip = result.info.ip;
	var text = 'version: ' + systemInfo.system.version.trim() + ' (' +  systemInfo.system.commit.trim() + ')\n';
	text += 'platform: ' + systemInfo.platform.name.trim() + ' - ' +  systemInfo.platform.version.trim() + ' (' + systemInfo.platform.commit.trim()  + ')\n';
	text += 'ip: ' + ip + '\n';
	text += 'id: ' + systemInfo.id + '\n';
	text += 'source: ' + result.info.source + '\n';
	text += 'time: ' + result.info.timestamp + '\n\n';
	text += 'stack: ' + result.dump.stack + '\n\n';
	text += 'log: ' + processed_log + '\n';
	var mail = {
		from: 'Crash reporter <denethor.tvdlifia@gmail.com>',
		to: 'tvd-lifia-pieza3@googlegroups.com',
		subject: 'Crash report on '+ip,
		text: text,
		attachments: [{
			filename: result.info.name,
			content: JSON.stringify(result.dump, null, '\t'),
			contentType: 'text/plain'
		}]
	};

	processCrashes({
		platform: result.dump.systemInfo.platform.name,
		version: result.dump.systemInfo.system.version,
		dumps: result.dump.services['ar.edu.unlp.info.lifia.tvd.browser'].state.crashes,
	})
	.then( function(crashes) {
		mail.text += crashes;
	})
	.catch( function(e) {
		mail.text += 'Fail to process htmlshell crashes! Error: ' + e.message + '\n';
	})
	.finally( function() {
		smtpTransport.sendMail(mail, function(error, info){
			if(error){
				// Message temporarily deferred, restry later
				if( error.responseCode === 421 || error.responseCode === 454) {
					log.info( 'main', 'Delaying mail=%s', mail.subject );
					setTimeout(function() {
						return( sendMail(result));
					}, 10*60*1000);
				} else {
					log.error( 'main', 'Error enviando mail: err=%s', error.toString() );
				}
			}else{
				log.info('main', 'Message sent: ' + info.response);
				if( result.info.path ) {
					// delete report
					fs.unlink(result.info.path);
				}
			}
		});
	});
}

function postProcessDumps( results ) {
	var dumps = [];
	var processed = [];
	results.forEach( function( result ) {
		var hash = crypto.createHash('md5').update(result.dump.stack).digest('hex');
		if( !(dumps.contains(hash)) && !(BLACKLISTS.ip.contains(result.info.ip)) && !(BLACKLISTS.id.contains( result.dump.systemInfo.id)) ) {
			dumps.push( hash );
			processed.push( result.info.name );
			sendMail( result );
		} else {
			if( result.info.path ) {
				// delete report
				fs.unlink(result.info.path);
			}
		}
	});
	return processed;
}

function scanDir( root ) {
	fs.readdirAsync( root )
		.then(function(entries) { return [root, entries]; })
		.then( parseCrashes )
		.then( postProcessDumps )
		.then( function(processed) {
			log.info('main', 'Crashes procesados: %j', processed);
		})
		.catch( function(err) {
			log.error('main', 'Error procesando archivos: err=%s', err.message );
		});
}

function getUrl( url ) {
	return new bPromise(function(resolve, reject) {
		http.get(url, function(res) {
			var data = '';
			res.on('data', function(chunk) {
				data += chunk.toString();
			});
			res.on('end', function(){
				resolve(data);
			});
		}).on('error', function(error) {
			log.error( 'main', 'Error obteniendo URL %s: err=%s', url, error.toString() );
			reject(error);
		});
	});
}

function downloadDump( dump_url, dumpInfo, source ) {
	log.info('main', 'downloading dump url=%s', dump_url);
	return getUrl( dump_url ).then (function( data ) {
		var crash_time = moment(dumpInfo.creationTimestamp, 'YYYY-M-DDTHH:mm:ss.Z').format('YYYY-M-DD HH:mm:ss');
		var dump = JSON.parse(data);
		var result = { 
			info: {
				name: dumpInfo.path,
				id: dumpInfo.tac,
				source: source,
				timestamp: crash_time,
			},
			dump: dump
		};
		if (('ar.edu.unlp.info.lifia.tvd.network' in dump.services) && dump.services['ar.edu.unlp.info.lifia.tvd.network'].state && dump.services['ar.edu.unlp.info.lifia.tvd.network'].state.ip) {
			result.info.ip = dump.services['ar.edu.unlp.info.lifia.tvd.network'].state.ip.split("/")[0];
		}
		return result;
	});
}

function parseUrlJsonList( jsonList ) {
	var promises = [];
	jsonList.forEach(function ( dumpInfo ) {
		var crash_time = moment(dumpInfo.creationTimestamp, 'YYYY-M-DDTHH:mm:ss.Z');
		var current_time = moment(0, 'HH');
		if( (current_time.diff(crash_time, 'hours') < 24) && ! BLACKLISTS.id.contains(dumpInfo.tac)  ) {
		log.info('main', 'Found valid crash info: %s', dumpInfo.path);
			promises.push(dumpInfo);
		}
	});
	return promises;
}

function processUrl( url, auth, dumpInfos ) {
	log.info('main', 'Processing url=%s', url);
	var options = uniformResourceLocator.parse(url);
	options.auth = auth;
	return getUrl( options )
	.then( function( data ){
		var jsondata = JSON.parse(data);
		dumpInfos = dumpInfos.concat(parseUrlJsonList(jsondata._embedded.error));
		if( jsondata._links.next) {
			return processUrl(jsondata._links.next.href, auth, dumpInfos);
		}
		return dumpInfos;
	});
}

function scanUrl( url ) {
	// Must keep auth info, because links.next removes it :(
	var options = uniformResourceLocator.parse(url);
	var auth = options.auth;
	bPromise.all(processUrl( url + '/error', auth, []))
	.map(function (dumpInfo) {
		return downloadDump( url + '/files/error/get/' + dumpInfo.path, dumpInfo, url );
	})
	.all()
	.then( postProcessDumps )
	.then( function(processed) {
		log.info('main', 'Crashes procesados: %j', processed);
	})
	.error(function( error ) {
		log.error( 'main', 'Error explorando URL %s: err=%s', url, error.toString() );
	});
}

/*****************************************************************************
 *************** Parsing && formating of htmlshell crash dumps ***************
 *****************************************************************************/
var glob = require('glob');
var tar = require('tar');
var mktemp = bPromise.promisifyAll( require('mktemp') );
var zlib = require('zlib');
var run = bPromise.promisify( require('child_process').execFile );

function processCrashes(info) {
	log.info('main', 'Processing of htmlshell crashes start!' );
	return createTmpDir( info )
		.then( extractTacSymbols )
		.then( extractBrowserSymbols )
		.then( processHtmlshellDumps )
		.then( formatBacktraces )
		.finally( function() {
			removeTreeSync( info.tmpDir );
			log.info('main', 'Processing of htmlshell crashes end!' );
		});
}

function createTmpDir(info) {
	return mktemp.createDirAsync('/tmp/TAC_CRASH_REPORT_XXXXXX')
		.then( function(path) {
			info.tmpDir = path;
		})
		.return(info);
}

function extractTacSymbols(info) {
	var pattern1 = '/mnt/repositorio/builds/AML_MICROTROL/tac/image-*-' + info.version + '.tac.sym';
	var pattern2 = '/mnt/repositorio/entregas/tac/images/'+info.platform+'/'+info.version+'/image-*-' + info.version + '.tac.sym';
	return findAndExtractFile(pattern1, info.tmpDir, false).then(function(){
		return info;
	}, function() {
		return findAndExtractFile(pattern2, info.tmpDir, false).return(info);
	});
}

function extractBrowserSymbols(info) {
	var pattern = path.join( info.tmpDir, 'browser-' + info.version + '.*.syms' );
	return findAndExtractFile(pattern, info.tmpDir, true).return(info);
}

function findAndExtractFile(pattern, destDir, gziped) {
	return new bPromise( function(fullfill, reject) {
		log.verbose('main', 'Looking files matching: "%s"', pattern);
		var matches = glob.sync( pattern );
		if (matches.length > 0) {
			var read_stream = fs.createReadStream( matches[0] ).on( 'error', reject );
			if (gziped) {
				log.verbose('main', 'Uncompressing gzipped file: ' + matches[0]);
				read_stream = read_stream.pipe( zlib.createGunzip() );
			}
			log.verbose('main', 'Extracting file: ' + matches[0]);
			read_stream.pipe( tar.Extract({'path': destDir}).on( 'error', reject ).on( 'end', fullfill ) );
		}
		else {
			reject( new Error( 'Fail to get symbols!!! Pattern "' + pattern + '" do not match any file!') );
		}
	});
}

function processHtmlshellDumps(info) {
	log.info('main', 'Processing htmlshell core dumps');
	return bPromise.settle(
		Object.keys(info.dumps).map( function(key) {
			return processDump(info,key);
		}))
		.then( function(processed) {
			info.backtraces = processed;
			return info;
		});
}

function processDump(info, key) {
	var parts = key.split('/');
	var filename = parts[parts.length-1];
	var fullpath = path.join( info.tmpDir, filename );

	log.info('main', 'Writing dump %s to file: %s', filename, fullpath);

	return fs.writeFileAsync( fullpath, new Buffer(info.dumps[key], 'base64'), {'mode':384} )
		.then( function() {
			return run('./htmlshell_bt.js', [info.tmpDir, fullpath]);
		})
		.then( function(bt) {
			return {
				filename: filename,
				backtrace: bt[0]
			};
		});
}

function formatBacktraces(info) {
	log.info('main', 'Formating backtraces');
	var text = 'htmlshell crashes: ' + Object.keys(info.dumps).length + '\n\n';
	info.backtraces.forEach( function(st, i) {
		text += 'Crash [' + i + '] ';
		if (st.isFulfilled()) {
			text += 'Backtrace generated from ' + st.value().filename + ':\n' + st.value().backtrace + '\n\n';
		} else {
			text += 'Fail to generate backtrace!';
			if (st.isRejected()) {
				text += ' Reason: ' + st.reason();
			}
			text += '\n\n';
		}
	});
	return text;
}

function removeTreeSync(dir) {
	if(fs.lstatSync(dir).isDirectory()) {
		fs.readdirSync(dir).forEach(function(file) {
			removeTreeSync( path.join(dir,file) );
		});
		fs.rmdirSync( dir );
	} else {
		fs.unlinkSync( dir );
	}
}

/*****************************************************************************
 ******************************** Entry point ********************************
 *****************************************************************************/
var options = {
	'--dir': {
		func: scanDir,
		help: 'Scan directory report.'
	},
	'--url': {
		func: scanUrl,
		help: 'Scan and download URL reports.'
	},
};

function usage() {
	console.log(path.basename(process.argv[1])+':\n');
	Object.keys(options).forEach( function( option ) {
		console.log( '\t'+option+': '+options[option].help);
	});
	console.log();
}

// urls:
// 	http://lifia:jvr135@tiendatac.minplan.gob.ar/api/v1
// 	http://lifia:lifia@tiendatac_dev.minplan.gob.ar/api/v1 == http://lifia:lifia@190.210.150.121/api/v1
var mode=process.argv[2];
if(Object.keys(options).contains(mode)) {
	options[mode].func( process.argv[3] );
} else {
	usage();
}
