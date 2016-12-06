'use strict';

var fs = require('fs');
var path = require('path');
var _ = require('lodash');

function checkBounds( bounds ) {
	var check = _.isPlainObject(bounds);
	check &= typeof bounds.x === 'number';
	check &= typeof bounds.y === 'number';
	check &= typeof bounds.w === 'number' && bounds.w >= 0;
	check &= typeof bounds.h === 'number' && bounds.h >= 0;
	return check;
}

function cleanDirectory( dir, removeRoot ) {
	if (fs.existsSync(dir)) {
		fs.readdirSync(dir).forEach( function (file) {
			var curPath = path.join(dir, file);
			if (fs.lstatSync(curPath).isDirectory()) {
				cleanDirectory(curPath, true);
			} else {
				fs.unlinkSync(curPath);
			}
		});
		if (removeRoot) {
			fs.rmdirSync(dir);
		}
	}
}
module.exports.cleanDirectory = cleanDirectory;

module.exports.checkJsApis = function( apis ) {
	if (_.isUndefined(apis)) {
		return true;
	}

	var check = false;
	try {
		check = (apis instanceof Array);
		apis.forEach( function (api) {
			check &= _.isPlainObject(api);
			check &= typeof api.id === 'string' && api.id.length > 0;
			check &= typeof api.name === 'string' && api.name.length > 0;
			check &= (api.exports instanceof Array);
			api.exports.forEach( function (method) {
				check &= typeof method === 'string' && method.length > 0;
			});
		});
	} catch (err) {
		check = false;
	}
	return check;
};

module.exports.fileExists = function( filename ) {
	try {
		return fs.statSync(filename).isFile();
	} catch( error ) {
		return false;
	}
};

module.exports.isValidUrl = function( url ) {
	var check = true;
	var res = /^(http|https|file|chrome):\/\/([^\?]+)(\?.*)?$/.exec(url);
	if (res && (res[2].length > 0)) {
		if (res[1] == 'file') {
			check &= module.exports.fileExists(res[2]);
		}
	} else {
		check = false;
	}
	return check;
};

module.exports.areValidOptions = function( options ) {
	var check = true;
	check &= _.isPlainObject(options);
	check &= checkBounds(options.bounds);
	check &= _.isString(options.js);
	check &= _.isBoolean(options.bgTransparent);
	check &= _.isBoolean(options.focus);
	check &= _.isBoolean(options.visible);
	check &= _.isNumber(options.zIndex);
	check &= _.isArray(options.plugins);
	if (check) {
		check &= options.plugins.every(function(plugin) {
			return _.isString(plugin);
		});
	}
	if (!check) {
		log.warn('BrowserService', '[checks] Invalid options: %j', options);
	}
	return check;
};
