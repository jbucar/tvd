'use strict';

var assert = require('assert');
var util = require('util');
var log = require('npmlog');
var path = require('path');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs-extra') );
var mktemp = bPromise.promisifyAll( require('mktemp') );
var _ = require('lodash');
var UglifyJS = require("uglify-js-harmony");
var Ractive = require('ractive');
Ractive.DEBUG=false;
var Esprima = require('esprima');

function getDefaultResources() {
	return {
		_parent: {},
		_templates: {},
		_styles: {},
		_strings: {},
		_images: {}
	};
}

function readContent( info, resKey, fnc ) {
	log.verbose( 'build', 'read (%s) path=%s => %s', resKey, info.relPath, info.resName );
	return fs.readFileAsync(info.path)
		.then(function(data) {
			info.resources[resKey][info.resName] = fnc ? fnc(data) : data.toString();
		});
}

function checkCode( info ) {
	return fs.readFileAsync( info.path )
		.then( (data) => Esprima.parse(data, {}) )
		.then(function (syntax) {
			syntax.body.forEach(function(b) {
				if (b.type !== 'FunctionDeclaration' && b.type !== 'ClassDeclaration') {
					throw Error( 'Invalid declaration: file=' + info.path + ', type=' + b.type );
				}

				if (b.id.name in info.state.resources.functions) {
					throw Error( 'Function already declarated: name=' + b.id.name );
				}

				let fncName = b.id.name.toLowerCase();
				info.state.resources.functions[fncName] = path.dirname(info.relPath);
			});
		});
}

function processPath( state, root, curPath ) {
	log.info( 'build', 'processPath: root=%s, curPath=%s', root, curPath );

	let resources = getDefaultResources();
	{	//	Assign resources to data path
		let objPath = curPath.split(path.sep).join('.');
		if (curPath === '.') {
			state.resources.data = resources;
		}
		else {
			_.set( state.resources.data, objPath, resources );
		}
	}

	return fs.readdirAsync( path.join(root,curPath) )
		.then(function(entries) {
			var aPromises = [];

			entries.forEach(function(entry) {
				let relPath = path.join(curPath,entry);
				var info = {
					resources: resources,
					state: state,
					name: entry,
					path: path.join(root,curPath,entry),
					root: root,
					relPath: relPath,
					dstPath: relPath
				};

				//log.silly( 'build', 'processPath: entry=%s, root=%s, rel=%s, dst=%s', entry, root, info.relPath, info.dstPath );

				var prom = fs.statAsync(info.path)
					.then(function(stats) {
						if (stats.isFile()) {
							info.size = stats.size;
							info.type = 'file';
							info.ext = path.extname(info.name);
							info.resName = path.basename(info.name,info.ext);

							if (info.ext === '.tpl') {
								return readContent( info, '_templates', (data) => Ractive.parse( data.toString() ) );
							}
							else if (info.ext === '.css') {
								return readContent( info, '_styles' );
							}
							else if (info.ext === '.txt') {
								return fs.readFileAsync( info.path )
									.then(JSON.parse)
									.then( (json) => _.assign(info.resources._strings,json) );
							}
							else if (info.ext === '.js') {
								state.files.push( info.path );
								if (!info.path.endsWith('mocks.js')) {
									return checkCode( info );
								}
							}
							else {
								if (info.ext === '.png' || info.ext === '.gif') {
									info.resources._images[info.resName] = info.dstPath;
								}

								//  copy
								log.verbose( 'build', 'Copy file: src=%s, dst=%s', info.path, info.dstPath );
								return fs.copyAsync( info.path, path.join(state.dst,info.dstPath) );
							}
						}
						else {
							return processPath( state, root, info.relPath );
						}
					});

				aPromises.push( prom );
			});

			return bPromise.all(aPromises);
		});
}

function uglify( state ) {
	log.verbose( 'build', 'uglify: script=%s', state.script );
	var result = UglifyJS.minify( state.files, {
		outSourceMap: state.srcMapName,
		warnings: true,
		mangle: false,
		mangleProperties: false,
		compress: false
	});
	return fs.appendFileAsync( state.script, result.code )
		.then( () => fs.writeFileAsync( state.srcMap, result.map ) );
}

function concat( src, dst ) {
	log.verbose( 'build', 'concat: src=%s', src );
	return fs.readFileAsync(src)
		.then( (data) => fs.appendFileAsync( dst, data ) );
}

function concatAll( state ) {
	log.verbose( 'build', 'concatAll: script=%s', state.script );
	return bPromise.map(state.files, function(fName) {
		return concat( fName, state.script );
	}, { concurrency: 1} );
}

function writeHeader( state ) {
	var text = '(function() {\n"use strict";\n';
	return fs.writeFileAsync( state.script, text );
}

function writeFooter( state ) {
	var text = "\n";
	text += 'function buildOptions() {\n';
	text += '\treturn ' + JSON.stringify(state.resources,4);
	text += '};\n\n';
	text += 'entry_point( window, buildOptions() );\n';
	text += "log.setLevel( '" + state.opts.logLevel + "' );\n";
	text += "})();\n";
	return fs.appendFileAsync( state.script, text );
}

function writeScriptOutput( state ) {
	return writeHeader( state )
		.then( () => state.opts.useConcat ? concatAll(state) : uglify( state ) )
		.then( () => writeFooter( state ) );
}

function writeHtmlOutput( state ) {
	let text = '';

	function addResources( res, strFormat ) {
		if (res) {
			res.forEach(function(res) {
				text += util.format( strFormat, 'resources/' + res );
			});
		}
	}

	function addScripts( mod ) { addResources( mod.libs, '\t<script type="text/javascript" src="%s"></script>\n' ); }
	function addStyles( mod ) { addResources( mod.css, '\t<link rel="stylesheet" type="text/css" href="%s">\n' ); }

	text += '<!doctype html>\n\n';
	text += '<head>\n';
	text += '\t<meta charset="utf-8">\n';
	text += '\t<title>' + state.opts.name + '</title>\n\n';

	text += '\t<!-- styles -->\n';
	addStyles( state.opts.web_modules );
	addStyles( state.opts.project );

	text += '\t<!-- thirdparty js libraries -->\n';
	addScripts( state.opts.web_modules );
	addScripts( state.opts.project );

	text += '\t<!-- js scripts -->\n';
	text += '\t<script type="text/javascript" src="' + state.opts.jsFile + '"></script>\n';

	text += '</head>\n';
	text += '<body>\n';
	text += '</body>\n\n';
	text += '</html>\n';

	return fs.writeFileAsync( state.html, text );
}

function copyResources( state, absPath, src ) {
	let extras = [];

	function addCopy( srcs ) {
		if (srcs) {
			extras.push.apply(extras,srcs.map(function (s) {
				return {
					src: path.join(absPath,s),
					dst: path.join(state.dst,'resources',s)
				};
			}));
		}
	}

	//	Process extra files form web_modules
	addCopy( src.libs );
	addCopy( src.css );
	addCopy( src.fonts );

	return bPromise.map( extras, function(extra) {
		log.verbose( 'build', 'Copy extra: src=%s', extra.src );
		return fs.copyAsync( extra.src, extra.dst );
	});
}

function processProject( state, absPath, src, resources ) {
	//	Parse all sources
	return bPromise.map( src.source, (comp) => processPath( state, absPath, comp, resources ) )
		.then( () => copyResources( state, absPath, src ) );
}

function processFiles( state ) {
	//	First, process project files
	return processProject( state, path.join(state.opts.absSrc,'src'), state.opts.project )
		.then( () => processProject( state, getModulesPath(), state.opts.web_modules ) );
}

function deploy( state ) {
	return fs.copyAsync( state.dst, state.opts.outPath )
		.then( () => fs.remove( state.dst ) );
}

function getModulesPath() {
	return path.join(path.dirname(path.dirname(__dirname)),"packages","web_modules","src");
}

function build( opts ) {
	log.level = opts.logLevel;

	log.info( 'build', 'Build project: outPath=%s', opts.outPath );

	let state = {
		opts: opts,
		files: [],
		resources: {
			functions: {},
			data: {}
		}
	};

	if (opts.useMock) {
		log.info( 'build', 'Add mock component' );
		opts.web_modules.source.push( "components/mock" );
		opts.useConcat = true;
	}

	return mktemp.createDirAsync( 'tmp.XXXXXXXX' )
		.then(function(tmpDir) {
			state.dst = tmpDir;
			state.script = path.join(tmpDir,opts.jsFile);
			state.html = path.join(tmpDir,opts.htmlFile);
			state.srcMapName = opts.jsFile + '.map';
			state.srcMap = path.join(tmpDir,state.srcMapName);
		})
		.then( () => processFiles( state ) )
		.then( () => writeScriptOutput( state ) )
		.then( () => writeHtmlOutput( state ) )
		.then( () => deploy( state ) )
		.catch(function(err) {
			log.error( 'build', 'err=%s', err.stack );
			if (state.dst) {
				fs.remove( state.dst );
			}
			process.exit(1);
		});
}

function parseOptions() {
	let argv = require('minimist')(process.argv.slice(2));
	assert(argv.config);
	return fs.readFileAsync( argv.config )
		.then(JSON.parse)
		.then(function( opts ) {
			function def( key, val ) {
				if (opts[key] === undefined) {
					opts[key] = val;
				}
				if (argv[key] !== undefined) {
					opts[key] = argv[key];
				}
			}

			//	Setup options
			opts.absSrc = path.dirname( argv.config );
			def( 'name', 'unknown' );
			def( 'outPath', 'out' );
			def( 'jsFile', 'build.js' );
			def( 'htmlFile', 'index.html' );
			def( 'useConcat', false );
			def( 'logLevel', 'WARN' );
			def( 'useMock', false );
			def( 'project', { source: [ "." ] } );

			return opts;
		});
}

parseOptions()
	.then( build );
