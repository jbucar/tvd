'use strict';

var assert = require('assert');
var path = require('path');
var bPromise = require('bluebird');
var https = require('https');
var tvdUtil = require('tvdutil');
var util = require('util');

function defaultConfig() {
	return {username: '', userid: ''};
}

function convertConfig( oldCfg ) {
	return oldCfg;
}

function parseNumber( numStr ) {
	var value = Number(numStr);
	if (isNaN(value)) {
		// Some numbers in flickr came in fractions like "-1/20"
		var parts = numStr.split('/');
		if (parts.length == 2) {
			var num = Number(parts[0]);
			var den = Number(parts[1]);
			if (!(isNaN(num) || isNaN(den))) {
				value = num / den;
			}
		}
	}
	return value;
}

function FlickrProviderImpl(adapter) {
	var self = this;
	self.id = 'flickr';
	self.name = 'Flickr';
	self.description = 'Proveedor de images de Flickr';
	self.icon = path.join(__dirname,'flickrImageIcon.png');
	self.logo = path.join(__dirname,'flickrImageLogo.png');
	self.cfg = adapter.load( 'flickr.json', defaultConfig(), convertConfig );

	function queryFlickr( api ) {
		return new bPromise(function(fullfill, reject) {
			var rawData = '';
			var url = 'https://api.flickr.com/services/rest' +
			          '?api_key=b0f0512c18133f50dfa87e458dbaf0a1' +
			          '&format=json' +
			          '&nojsoncallback=1' +
			          '&method=flickr.' + api;

			log.verbose( 'FlickrProviderImpl', 'Sending request to flickr! url=%s', url);
			https.get( url, function(res) {
				res.on('data', function(data) {
					rawData += data;
				});
				res.on('end', function() {
					var result = JSON.parse(rawData);
					if (result.stat === 'ok') {
						fullfill( result );
					} else {
						reject( tvdUtil.warnErr('FlickrProviderImpl', 'Query to flickr return error! stat=%s, message="%s"', result.stat, result.message) );
					}
				});
			}).on('error', function(e) {
				reject( tvdUtil.warnErr('FlickrProviderImpl', 'Fail to query flickr api! err="%s"', e.message) );
			});
		});
	}

	function parseExif(fields, exif, info) {
		log.verbose( 'FlickrProviderImpl', 'Got photo data: fields=%j photo_exif=%j', fields, exif);

		function setField(data, field, subfield, convert) {
			if (fields.indexOf(field)<0) return;
			var value = data.clean ? data.clean._content : data.raw._content;
			if (convert) {
				value = parseNumber(data.raw._content);
				if (isNaN(value)) {
					log.warn('FlickProviderImpl', 'Cannot convert to number: field=%s, data=%j', field, data );
					return;
				}
			}
			if (subfield) {
				info[field][subfield] = value;
			} else {
				info[field] = value;
			}
		}

		exif.forEach(function(e) {
			switch (e.tag) {
				case 'ImageDescription':     setField( e, 'description'); break;
				case 'Make':                 setField( e, 'camera', 'make'); break;
				case 'Model':                setField( e, 'camera', 'model'); break;
				case 'ImageWidth':           setField( e, 'width', undefined, true); break;
				case 'ImageHeight':          setField( e, 'height', undefined, true); break;
				case 'ModifyDate':           setField( e, 'date'); break;
				case 'CreateDate':           setField( e, 'date'); break;
				case 'ExposureTime':         setField( e, 'exposureTime', undefined, true); break;
				case 'FNumber':              setField( e, 'apertureValue', undefined, true); break;
				case 'ISO':                  setField( e, 'iso', undefined, true); break;
				case 'ExposureCompensation': setField( e, 'exposureCompensation', undefined, true); break;
				// case 'Flash':                setField( e, 'flash'); break;
				// case 'Orientation':          setField( e, 'orientation'); break;
				// case 'GPSLatitudeRef':       setField( e, 'gps', 'latitudeRef'); break;
				// case 'GPSLatitude':          setField( e, 'gps', 'latitude'); break;
				// case 'GPSLongitudeRef':      setField( e, 'gps', 'longitudeRef'); break;
				// case 'GPSLongitude':         setField( e, 'gps', 'longitude'); break;
				// case 'GPSAltitudeRef':       setField( e, 'gps', 'altitudeRef'); break;
				// case 'GPSAltitude':          setField( e, 'gps', 'altitude'); break;
			}
		});
	}

	function getAllImages(opts) {
		var query = self.cfg.userid ? 'photos.search&user_id=' + self.cfg.userid : 'interestingness.getList';
		if (opts.page !== undefined) {
			// Flikr's API begins at 1 instead of 0
			query += '&page=' + (opts.page + 1);
		}
		if (opts.perPage !== undefined) {
			query += '&per_page=' + opts.perPage;
		}

		return queryFlickr(query)
			.then( function(result) {
				log.verbose('FlickrProviderImpl', 'Got images from flickr');
				return result.photos.photo.map(function(photo) {
					return {
						provider: self.id,
						id: photo.id,
						name: photo.title,
						url: util.format('https://farm%s.staticflickr.com/%s/%s_%s_b.jpg', photo.farm, photo.server, photo.id, photo.secret),
						type: 'image'
					};
				});
			});
	}


	function getImageInfo(opts) {

		function getInfo() {
			return queryFlickr( 'photos.getInfo&photo_id=' + opts.id )
				.then( function( resp ) {
					assert( opts.id === resp.photo.id );
					var info = {
						provider: self.id,
						id: opts.id,
						name: resp.photo.title._content,
						url: util.format('https://farm%s.staticflickr.com/%s/%s_%s_b.jpg', resp.photo.farm, resp.photo.server, resp.photo.id, resp.photo.secret),
						type: 'image',
						camera: {},
						gps: {}
					};
					return info;
				});
		}

		function getExif(info) {
			return queryFlickr( 'photos.getExif&photo_id=' + opts.id )
				.then( function(resp) {
					assert( opts.id === resp.photo.id );
					parseExif(opts.fields, resp.photo.exif, info);
					return info;
				});
		}

		return getInfo().then( getExif );
	}

	function saveUser(name, id, cb) {
		self.cfg.username = name;
		self.cfg.userid = id;
		adapter.save( 'flickr.json', self.cfg );
		cb();
	}

	self.get = function(opts) {
		log.info( 'FlickrProvider', 'Get opts=%j', opts );
		return opts.id ?
			getImageInfo(opts):
			getAllImages(opts);
	};

	self.getUserName = function() {
		return self.cfg.username;
	};

	self.setUserName = function(name, cb) {
		if (name) {
			queryFlickr('people.findByUsername&username=' + name)
				.then( function(result){
					saveUser(name, result.user.nsid, cb);
				}, cb);
		} else {
			saveUser('', '', cb);
		}
	};

	return self;
}

function FlickrApi(impl) {
	var self = {};

	self.setUserName = function(usrname, cb) {
		impl.setUserName(usrname, cb);
	};
	self.setUserName.isAsync = true;

	self.getUserName = function() {
		return impl.getUserName();
	};

	self.getWebAPI = function() {
		return {
			'name': 'flickr',
			'public': [],
			'private': ['setUserName', 'getUserName']
		};
	};

	return Object.freeze(self);
}

function FlickrProvider(adapter) {
	var self = {};
	var _impl;
	var _image;

	self.onStart = function(cb) {
		log.info( 'FlickrProvider', 'Start' );

		_image = adapter.registry().get('ar.edu.unlp.info.lifia.tvd.media.image');
		assert( _image );
		assert( !_image.hasProvider('flickr') );
		_impl = new FlickrProviderImpl( adapter );
		_image.addProvider( _impl );

		cb(undefined, new FlickrApi(_impl));
	};

	self.onStop = function(cb) {
		log.info( 'FlickrProvider', 'Stop' );
		_image.rmProvider( _impl.id );
		_impl = undefined;
		_image = undefined;
		cb();
	};

	return Object.freeze(self);
}

module.exports = FlickrProvider;
