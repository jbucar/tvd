'use strict';

var _ = require('lodash');
var fs = require('fs');
var path = require('path');
var tvdutil = require('tvdutil');
var chai = require('chai');
chai.config.includeStack = true;
var bPromise = require('bluebird');
var assert = chai.assert;
var mockfs = require('mock-fs');
var Mocks = require('mocks');
var aux = require('./aux');
var DiskImage = require('../src/disk/image');
var ExifParser = require('../src/node_modules/exif-parser');

var exif_easyshare_m863 = {
    "startMarker": {
        "offset": 0
    },
    "tags": {
        "Make": "EASTMAN KODAK COMPANY",
        "Model": "KODAK EASYSHARE M863 DIGITAL CAMERA",
        "Orientation": 1,
        "XResolution": 72,
        "YResolution": 72,
        "ResolutionUnit": 2,
        "YCbCrPositioning": 2,
        "Rating": 0,
        "RatingPercent": 0,
        "XPTitle": [
            0,
            0
        ],
        "ExposureTime": 0.003218,
        "FNumber": 2.8,
        "ExposureProgram": 2,
        "ISO": 64,
        "CreateDate": 1286606275,
        "ShutterSpeedValue": 8.279,
        "ApertureValue": 2.97,
        "ExposureCompensation": 0,
        "MaxApertureValue": 2.97,
        "MeteringMode": 5,
        "LightSource": 0,
        "Flash": 16,
        "FocalLength": 5.7,
        "ColorSpace": 1,
        "ExifImageWidth": 3280,
        "ExifImageHeight": 2460,
        "ExposureIndex": 64,
        "SensingMethod": 2,
        "CustomRendered": 0,
        "ExposureMode": 0,
        "WhiteBalance": 0,
        "DigitalZoomRatio": 0,
        "FocalLengthIn35mmFormat": 34,
        "SceneCaptureType": 0,
        "GainControl": 0,
        "Contrast": 0,
        "Saturation": 0,
        "Sharpness": 0,
        "SubjectDistanceRange": 0,
        "OffsetSchema": 1932,
        "InteropIndex": "R98"
    },
    "imageSize": {
        "height": 2460,
        "width": 3280
    },
    "thumbnailOffset": 17502,
    "thumbnailLength": 1892,
    "thumbnailType": 6,
    "app1Offset": 24
};

var exif_motorola_xt919 = {
    "startMarker": {
        "offset": 0
    },
    "tags": {
        "ImageDescription": "                               ",
        "Make": "MOTOROLA                       ",
        "Model": "XT919                          ",
        "Orientation": 1,
        "XResolution": 72,
        "YResolution": 72,
        "ResolutionUnit": 2,
        "Software": "Android",
        "ModifyDate": "2014:01:28 20:09:20",
        "YCbCrPositioning": 2,
        "GPSVersionID": [
            2,
            2,
            0,
            0
        ],
        "GPSLatitudeRef": "S",
        "GPSLatitude": -38.02047347222222,
        "GPSLongitudeRef": "W",
        "GPSLongitude": -57.526618944444444,
        "GPSAltitudeRef": 0,
        "GPSAltitude": 6,
        "GPSTimeStamp": [
            23,
            9,
            25
        ],
        "GPSDateStamp": "2014:01:28",
        "ExposureTime": 0.03,
        "FNumber": 2.4,
        "ExposureProgram": 2,
        "ISO": 226,
        "DateTimeOriginal": 1390939760,
        "CreateDate": 1390939760,
        "ExposureCompensation": 0,
        "MeteringMode": 2,
        "LightSource": 255,
        "Flash": 0,
        "FocalLength": 4.36,
        "ColorSpace": 1,
        "ExifImageWidth": 3264,
        "ExifImageHeight": 2448,
        "ExposureMode": 0,
        "WhiteBalance": 0,
        "DigitalZoomRatio": 1,
        "SceneCaptureType": 0,
        "InteropIndex": "R98"
    },
    "imageSize": {
        "height": 2448,
        "width": 3264
    },
    "thumbnailOffset": 1101,
    "thumbnailLength": 5312,
    "thumbnailType": 6,
    "app1Offset": 6
};

describe('DiskImage', function() {
	beforeEach(function() {
		Mocks.init('silly');
	});

	afterEach(function() {
		Mocks.fin();
	});

	describe('constructor', function() {
		it( 'should construct basic disk image delegate', function() {
			var del = new DiskImage( 'image1', 'DiskImage1', 'mount' );
			assert.equal( del.id, 'image1' );
			assert.equal( del.name, 'DiskImage1' );
			assert( del.icon );
			assert( del.logo );
			assert.equal( del.root, 'mount' );
		});
	});

	describe('methods', function() {
		var del;

		beforeEach(function() {
			del = new DiskImage( 'image1', 'DiskImage1', __dirname );
		});

		afterEach(function() {
			del = null;
		});

		describe('filter', function() {
			it('should validate images (jpg, png, svg, gif, bmp', function() {
				assert.deepEqual( del.supportedExtensions(), ['.png', '.gif', '.jpg', '.svg', '.bmp'] );
			});
		});

		describe('map', function() {
			it('should map a directory to album', function() {
				var info = aux.createInfo( del.root, 'album1');
				var stats = aux.createStats(false,true);
				var opts = {};
				var res = del.map( aux.createOper(info,del.root,stats,stats));
				assert.equal( Object.keys(res).length, 4 );
				assert.equal( res.provider, 'disk1' );
				assert.equal( res.id, path.join(__dirname,'album1') );
				assert.equal( res.name, 'album1' );
				assert.equal( res.type, 'album' );
			});

			it('should map image without opts to obligatory fields', function() {
				var info = aux.createInfo( del.root, 'image.jpg');
				var stats = aux.createStats(true);
				var opts = {};
				var res = del.map(aux.createOper(info,del.root,stats,opts));
				assert.equal( Object.keys(res).length, 5 );
				assert.equal( res.provider, 'disk1' );
				assert.equal( res.id, path.join(__dirname,'image.jpg') );
				assert.equal( res.name, 'image.jpg' );
				assert.equal( res.type, 'image' );
				assert.equal( res.url, 'file://' + path.join(__dirname,'image.jpg') );
			});

			it('should map image with opts + obligatories fields', function() {
				var info = aux.createInfo( del.root, 'image.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['size']};
				var res = del.map(aux.createOper(info,del.root,stats,opts));
				assert.equal( Object.keys(res).length, 6 );
				assert.equal( res.provider, 'disk1' );
				assert.equal( res.id, path.join(__dirname,'image.jpg') );
				assert.equal( res.name, 'image.jpg' );
				assert.equal( res.type, 'image' );
				assert.equal( res.url, 'file://' + path.join(__dirname,'image.jpg') );
				assert.equal( res.size, 15 );
			});

			it('should ignore invalid fields options + obligatories fields', function() {
				var info = aux.createInfo( del.root, 'image.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['pepe']};
				var res = del.map(aux.createOper(info,del.root,stats,opts));
				assert.equal( Object.keys(res).length, 5 );
				assert.equal( res.provider, 'disk1' );
				assert.equal( res.id, path.join(__dirname,'image.jpg') );
				assert.equal( res.name, 'image.jpg' );
				assert.equal( res.type, 'image' );
				assert.equal( res.url, 'file://' + path.join(__dirname,'image.jpg') );
			});

			it('should return exif data + obligatories fields', function(done) {
				var info = aux.createInfo( del.root, 'Stonehenge.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['size','width','height','camera','flash','date']};

				aux.callMap(del,opts,info,stats)
					.then(function(res) {
						console.log( res );
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.name, 'Stonehenge.jpg' );
						assert.equal( res.type, 'image' );
						assert.equal( res.url, 'file://' + path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.size, 15 );
						assert.equal( res.width, 709 );
						assert.equal( res.height, 472 );
						assert.equal( res.date, "2005:10:13 15:32:07" );

						assert.equal( Object.keys(res).length, 10 );

						done();
					},function(err) {
						console.log('error=%s', err);
					});
			});

			it('should return exif data + obligatories fields2', function(done) {
				var info = aux.createInfo( del.root, 'Stonehenge.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['size','camera']};

				aux.callMap( del,opts,info,stats)
					.then(function(res) {
						assert.equal( Object.keys(res).length, 7 );
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.name, 'Stonehenge.jpg' );
						assert.equal( res.type, 'image' );
						assert.equal( res.url, 'file://' + path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.size, 15 );

						done();
					},function(err) {
					});
			});

			it('should handle error on exif parser', function(done) {
				var oldCreate = ExifParser.create;
				ExifParser.create = function() {
					var self = {};
					self.parse = function() {
						throw new Error('pepe');
					};
					return self;
				};
				var info = aux.createInfo( del.root, 'Stonehenge.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['size','camera']};

				aux.callMap( del,opts,info,stats)
					.then(function(res) {
						assert.equal( Object.keys(res).length, 6 );
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.name, 'Stonehenge.jpg' );
						assert.equal( res.type, 'image' );
						assert.equal( res.url, 'file://' + path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.size, 15 );

						ExifParser.create = oldCreate;
						done();
					},function(err) {
					});
			});

			it('should handle error when no exist file', function(done) {
				var info = aux.createInfo( del.root, 'pepe.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['size','camera']};

				aux.callMap( del,opts,info,stats)
					.then(function(res) {
						assert.equal( Object.keys(res).length, 6 );
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join(__dirname,'pepe.jpg') );
						assert.equal( res.name, 'pepe.jpg' );
						assert.equal( res.type, 'image' );
						assert.equal( res.url, 'file://' + path.join(__dirname,'pepe.jpg') );
						assert.equal( res.size, 15 );
						done();
					});
			});

			it('should handle exif data from easyshare camera', function(done) {
				var oldCreate = ExifParser.create;
				ExifParser.create = function() {
					var self = {};
					self.parse = function() {
						return exif_easyshare_m863;
					};
					return self;
				};
				var info = aux.createInfo( del.root, 'Stonehenge.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['size','width','height','camera','orientation','exposureTime','exposureCompensation','apertureValue','iso','flash','date']};

				aux.callMap( del,opts,info,stats)
					.then(function(res) {
						assert.equal( Object.keys(res).length, 5+Object.keys(opts.fields).length );
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.name, 'Stonehenge.jpg' );
						assert.equal( res.type, 'image' );
						assert.equal( res.url, 'file://' + path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.size, 15 );
						assert.equal( res.width, 3280 );
						assert.equal( res.height, 2460 );
						assert.equal( res.date, "1286606275" );
						assert.equal( res.camera.make, "EASTMAN KODAK COMPANY" );
						assert.equal( res.camera.model, "KODAK EASYSHARE M863 DIGITAL CAMERA" );
						assert.equal( res.orientation, 1 );
						assert.equal( res.exposureTime, 0.003218 );
						assert.equal( res.exposureCompensation, 0 );
						assert.equal( res.apertureValue, 2.97 );
						assert.equal( res.iso, 64 );
						assert.equal( res.flash, 16 );
						ExifParser.create = oldCreate;
						done();
					},function(err) {
					});
			});

			it('should handle exif without camera field', function(done) {
				var oldCreate = ExifParser.create;
				ExifParser.create = function() {
					var self = {};
					self.parse = function() {
						return exif_easyshare_m863;
					};
					return self;
				};
				var info = aux.createInfo( del.root, 'Stonehenge.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['size','width','height','orientation','exposureTime','exposureCompensation','apertureValue','iso','flash','date']};

				aux.callMap( del,opts,info,stats)
					.then(function(res) {
						assert.equal( Object.keys(res).length, 5+Object.keys(opts.fields).length );
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.name, 'Stonehenge.jpg' );
						assert.equal( res.type, 'image' );
						assert.equal( res.url, 'file://' + path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.size, 15 );
						assert.equal( res.width, 3280 );
						assert.equal( res.height, 2460 );
						assert.equal( res.date, "1286606275" );
						assert.equal( res.orientation, 1 );
						assert.equal( res.exposureTime, 0.003218 );
						assert.equal( res.exposureCompensation, 0 );
						assert.equal( res.apertureValue, 2.97 );
						assert.equal( res.iso, 64 );
						assert.equal( res.flash, 16 );
						ExifParser.create = oldCreate;
						done();
					},function(err) {
					});
			});

			it('should handle exif data without tags', function(done) {
				var oldCreate = ExifParser.create;
				ExifParser.create = function() {
					var self = {};
					self.parse = function() {
						var data = _.clone(exif_easyshare_m863);
						data.tags = undefined;
						return data;
					};
					return self;
				};
				var info = aux.createInfo( del.root, 'Stonehenge.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['size','width','height','camera','orientation','exposureTime','exposureCompensation','apertureValue','iso','flash','date']};

				aux.callMap( del,opts,info,stats)
					.then(function(res) {
						assert.equal( Object.keys(res).length, 8 );
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.name, 'Stonehenge.jpg' );
						assert.equal( res.type, 'image' );
						assert.equal( res.url, 'file://' + path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.size, 15 );
						assert.equal( res.width, 3280 );
						assert.equal( res.height, 2460 );
						ExifParser.create = oldCreate;
						done();
					},function(err) {
					});
			});

			it('should handle exif data from motorola', function(done) {
				var oldCreate = ExifParser.create;
				ExifParser.create = function() {
					var self = {};
					self.parse = function() {
						return exif_motorola_xt919;
					};
					return self;
				};
				var info = aux.createInfo( del.root, 'Stonehenge.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['size','width','height','camera','orientation','exposureTime','exposureCompensation','apertureValue','iso','flash','gps','date']};

				aux.callMap( del,opts,info,stats)
					.then(function(res) {
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.name, 'Stonehenge.jpg' );
						assert.equal( res.type, 'image' );
						assert.equal( res.url, 'file://' + path.join(__dirname,'Stonehenge.jpg') );
						assert.equal( res.size, 15 );
						assert.equal( res.width, 3264 );
						assert.equal( res.height, 2448 );
						assert.equal( res.date, "2014:01:28 20:09:20" );
						assert.equal( res.camera.make, "MOTOROLA" );
						assert.equal( res.camera.model, "XT919" );
						assert.equal( res.orientation, 1 );
						assert.equal( res.exposureTime, 0.03 );
						assert.equal( res.exposureCompensation, 0 );
						//assert.equal( res.apertureValue, 2.97 );
						assert.equal( res.iso, 226 );
						assert.equal( res.flash, 0 );
						assert.equal( res.gps.latitudeRef, 'S' );
						assert.equal( res.gps.latitude, -38.02047347222222 );
						assert.equal( res.gps.longitudeRef, 'W' );
						assert.equal( res.gps.longitude, -57.526618944444444 );
						assert.equal( res.gps.altitudeRef, 0 );
						assert.equal( res.gps.altitude, 6 );

						assert.equal( Object.keys(res).length, 5+Object.keys(opts.fields).length-1 );

						ExifParser.create = oldCreate;
						done();
					},function(err) {
					});
			});

			xit('should return exif data + obligatories fields', function() {
				var info = aux.createInfo( del.root, 'image_without_exif.jpg');
				var stats = aux.createStats(true);
				var opts = {fields:['size','width','height','camera','flash','date']};
				var res = del.map(opts,info,stats);
				assert.equal( Object.keys(res).length, 5+opts.fields.length );
				assert.equal( res.provider, 'disk1' );
				assert.equal( res.id, path.join(__dirname,'image_without_exif.jpg') );
				assert.equal( res.name, 'image_without_exif.jpg' );
				assert.equal( res.type, 'image' );
				assert.equal( res.url, 'file://' + path.join(__dirname,'image_without_exif.jpg') );
				assert.equal( res.size, 15 );
			});
		});
	});
});

