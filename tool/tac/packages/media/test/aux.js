'use strict';

var path = require('path');
var bPromise = require('bluebird');

function createInfo(root,entry) {
	return {
		provider: 'disk1',
		id: path.join(root,entry),
		name: entry
	};
};

function createStats(oFile,oDir) {
	var self = {};
	if (oFile) {
		oDir = false;
	}
	self.isFile = function() {
		return oFile;
	};
	self.isDirectory = function() {
		return oDir;
	};
	self.size = 15;
	return self;
};

function createOper( info, root, stats, opts ) {
	return {
		provider: info.provider,
		id: info.id,
		name: info.name,
		root: root,
		stats: stats,
		extname: path.extname(info.name).toLowerCase(),
		opts: opts,
		info: info
	};
}

function callMap(del,opts,info,stats) {
	return new bPromise(function(resolve) {
		return resolve(del.map(createOper(info,del.root,stats,opts)));
	});
}

module.exports.createInfo = createInfo;
module.exports.createStats = createStats;
module.exports.createOper = createOper;
module.exports.callMap = callMap;

