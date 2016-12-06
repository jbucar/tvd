'use strict';

var assert = require('assert');
var path = require('path');
var mURL = require('url');
var queryString = require('querystring');

function zeroFill(n) {
	return ('0'+n).slice(-2);
}

function parseURI( uri ) {
	assert(uri);
	var obj = {
		uri: uri,
		url: mURL.parse(uri)
	};

	obj.isValid = function() {
		if (obj.url.protocol &&
			obj.url.protocol != 'file:' &&
			obj.url.protocol != 'http:' &&
			obj.url.protocol != 'https:')
		{
			return false;
		}
		return true;
	};
	obj.isLocal = function() {
		return obj.url.protocol === null || obj.url.protocol === 'file:';
	};
	obj.pathname = function() {
		if (obj.url.protocol === 'file:') {
			return queryString.unescape(obj.url.pathname);
		}
		return uri;
	};
	return obj;
}

function isLocal(uri) {
	return parseURI(uri).isLocal();
}

function fixURI( absPath, obj, key ) {
	let uri = obj[key];
	if (!uri) {
		return;
	}

	if (isLocal(uri)) {
		obj[key] = 'file://' + path.join(absPath,uri);
	}
}

module.exports.zeroFill = zeroFill;
module.exports.fixURI = fixURI;
module.exports.isLocal = isLocal;
module.exports.parseURI = parseURI;



