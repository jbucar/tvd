"use strict";

var path = require('path');
var assert = require('assert');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var _ = require('lodash');

//	Movies should be in separate directories
//	Metadata in meta.json
//	Examples:
//		dir1
//			Pulp Fiction
//				meta.json
//				movie.avi
//				subtitle.srt
//				art1.png
//				art2.png
//			dir2
//				dir3
//					Reservoir Dogs
//						meta.json
//						movie.avi
//						subtitle.srt
//						art1.png
//						art2.png
//					Inglourious Basterds
//						meta.json
//						movie.avi
//						subtitle.srt
//						art1.png
//						art2.png

// returns [ 'Pulp Fiction', 'Reservoir Dogs', 'Inglourious Basterds']


function Delegate(id,name,root,type) {
	var self = {
		id: id,
		name: name,
		root: root,
		icon: path.join(__dirname,'imageIcon.png'),
		logo: path.join(__dirname,'imageLogo.png')
	};

	self.supportedExtensions = function() {
		return ['.avi', '.mp4', '.mkv', '.url'];
	};

	self.map = function(oper) {
		assert(oper.stats.isFile());

		oper.info.type = type;
		oper.info.url = 'file://' + oper.info.id;
		if (oper.opts.fields) {
			if (oper.opts.fields.indexOf('size') !== -1) {
				oper.info.size = oper.stats.size;
			}

			return fs.readFileAsync( path.join(oper.root,'meta.json') )
				.then(JSON.parse)
				.then(function(metaInfo) {
					oper.info = _.assign(oper.info,metaInfo);
					return oper.info;
				})
				.catch(function() {
					return oper.info;
				});
		}
		else {
			return oper.info;
		}
	};

	return self;
}

module.exports = Delegate;

