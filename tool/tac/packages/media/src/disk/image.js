"use strict";

var path = require('path');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var _ = require('lodash');
var ExifParser = require('exif-parser');

var exifFields = ['width','height','camera','orientation','exposureTime','exposureCompensation','apertureValue','iso','flash','gps','date'];

function parseExif( img ) {
	log.silly( 'diskimage', 'Parse exif: path=%s', img );
	var oper = {};
	return fs.openAsync( img, 'r' )
		.then(function(fd) {
			log.silly( 'diskimage', 'Open file: fd=%d', fd );
			oper.fd = fd;
			return oper;
		})
		.then(function(oper) {
			log.silly( 'diskimage', 'Reading file content: oper=%j', oper );
			var exifDataSize = 65535;
			oper.buf = new Buffer(exifDataSize);
			return fs.readAsync( oper.fd, oper.buf, 0, exifDataSize, 0 )
				.then(function() {
					return oper;
				});
		})
		.then(function(oper) {
			log.silly( 'diskimage', 'Parsing exif data: oper=%j', oper.fd );
			//	Parse exif information
			var parser = ExifParser.create( oper.buf );
			oper.exif = parser.parse();
			return oper;
		})
		.then(function(oper) {
			log.silly( 'diskimage', 'Closing file: oper=%j', oper.fd );
			return fs.closeAsync(oper.fd)
				.then(function() {
					oper.fd = undefined;
					return oper;
				});
		})
		.then(function(oper) {
			return oper.exif;
		}, function(err) {
			log.silly( 'diskimage', 'Error parsing exif data: name=%s, err=%s', img, err.message );
			var prom;
			if (oper.fd) {
				log.silly( 'diskimage', 'Closing file: oper=%j', oper.fd );
				prom=fs.closeAsync(oper.fd);
			}
			else {
				prom=bPromise.resolve();
			}
			return prom.finally(function() {
				throw err;
			});
		});
}

function mapExif(opts,info) {
	return parseExif( info.id )
		.then(function(exifData) {
			function setField( f, p2, p3 ) {
				var val = _.get( exifData, p3 ? p3 : p2 );
				if (val !== undefined && opts.fields.indexOf(f) >= 0) {
					if (typeof val === 'string') {
						val = val.trim();
					}
					if (p3) {
						info[f][p2] = val;
					}
					else {
						info[f] = val;
					}
				}
			}

			log.silly( 'diskimage', 'Map exif data: exif=%j', exifData );
			setField( 'width', 'imageSize.width' );
			setField( 'height', 'imageSize.height' );
			if (exifData.tags) {
				if (opts.fields.indexOf('camera') >= 0) {
					info.camera = {};
					setField( 'camera', 'make', 'tags.Make' );
					setField( 'camera', 'model', 'tags.Model' );
				}
				setField( 'exposureTime', 'tags.ExposureTime' );
				setField( 'exposureCompensation', 'tags.ExposureCompensation' );
				setField( 'orientation', 'tags.Orientation' );
				setField( 'apertureValue', 'tags.ApertureValue' );
				setField( 'iso', 'tags.ISO' );
				setField( 'flash', 'tags.Flash' );
				setField( 'date', 'tags.CreateDate' );
				setField( 'date', 'tags.ModifyDate' );

				if (opts.fields.indexOf('gps') >= 0) {
					info.gps = {};
					setField( 'gps', 'latitudeRef', 'tags.GPSLatitudeRef' );
					setField( 'gps', 'latitude', 'tags.GPSLatitude' );
					setField( 'gps', 'longitudeRef', 'tags.GPSLongitudeRef' );
					setField( 'gps', 'longitude', 'tags.GPSLongitude' );
					setField( 'gps', 'altitudeRef', 'tags.GPSAltitudeRef' );
					setField( 'gps', 'altitude', 'tags.GPSAltitude' );
				}
			}
			return info;
		},function() {
			//	Ignore exif data
			return info;
		});
}

function Delegate(id,name,root) {
	var self = {
		id: id,
		name: name,
		root: root,
		icon: path.join(__dirname,'imageIcon.png'),
		logo: path.join(__dirname,'imageLogo.png')
	};

	self.supportedExtensions = function() {
		return ['.png', '.gif', '.jpg', '.svg', '.bmp'];
	};

	self.map = function(oper) {
		if (oper.stats.isFile()) {
			oper.info.type = 'image';
			oper.info.url = 'file://' + oper.info.id;
			if (oper.opts.fields) {
				if (oper.opts.fields.indexOf('size') !== -1) {
					oper.info.size = oper.stats.size;
				}

				//	Exif only on jpg files
				if (oper.extname === '.jpg' && _.intersection(oper.opts.fields,exifFields).length > 0) {
					return mapExif( oper.opts, oper.info );
				}
				else {
					return oper.info;
				}
			}
			else {
				return oper.info;
			}
		}
		else {
			oper.info.type = 'album';
			return oper.info;
		}
	};

	return self;
}

module.exports = Delegate;

