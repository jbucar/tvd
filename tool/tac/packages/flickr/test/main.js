'use strict';

var chai = require("chai");
var assert = chai.assert;
chai.config.includeStack = true;
var Mocks = require('mocks');
var _ = require('lodash');
var EventEmitter = require('events').EventEmitter;

function HttpsMock() {
	var self = this;

	self.requests = [];
	self.events = new EventEmitter();

	self.get = function(url, cb) {
		log.info( 'HttpsMock', 'get url=%s', url );
		var signals = {};
		var res = {
			on: function(name, cb) {
				signals[name] = cb;
			}
		};
		self.requests.push( {
			url: url,
			signals: signals
		});
		cb(res);
		self.events.emit( 'request' );
		return res;
	};

	return self;
}

describe('FlickrProvider', function() {
	var reg;
	var adapter;
	var flickr;
	var httpsMock;

	beforeEach( function() {
		reg = Mocks.init('silly');
		adapter = new Mocks.ServiceAdapter(reg);
		reg.put( 'ar.edu.unlp.info.lifia.tvd.media.image', new Mocks.Provider() );
		httpsMock = new HttpsMock();

		var FlickrProvider = Mocks.mockRequire( require, '../src/main', {https: httpsMock});
		flickr = new FlickrProvider(adapter);
		assert.isObject( flickr );
	});

	afterEach( function() {
		flickr = undefined;
		adapter = undefined;
		reg = undefined;
		httpsMock = undefined;
		Mocks.fin();
	});

	describe('start/stop', function() {
		it('should start/stop flickr provider successfuly', function(done) {
			flickr.onStart(function(err, api) {
				assert.isUndefined( err );
				assert.isObject( api );
				assert.isFunction( api.getWebAPI );
				assert.deepEqual( api.getWebAPI(), {
					'name': 'flickr',
					'public': [],
					'private': ['setUserName', 'getUserName']
				});
				assert.isTrue( reg.get('ar.edu.unlp.info.lifia.tvd.media.image').hasProvider('flickr') );
				flickr.onStop(function() {
					assert.isFalse( reg.get('ar.edu.unlp.info.lifia.tvd.media.image').hasProvider('flickr') );
					done();
				});
			});
		});

		it('should start/stop/start/stop flickr provider successfuly', function(done) {
			flickr.onStart(function(err, api) {
				assert.isUndefined( err );
				assert.isObject( api );
				flickr.onStop(function() {
					flickr.onStart(function(err, api) {
						assert.isUndefined( err );
						assert.isObject( api );
						flickr.onStop(done);
					});
				});
			});
		});

		it('should call to convert old config', function(done) {
			adapter.load = function( file, defaultsValues, convert ) {
				assert.isFunction( convert );
				var cfg = _.cloneDeep(defaultsValues);
				var newCfg = convert(cfg);
				assert.deepEqual( cfg, newCfg );
				return cfg;
			};

			flickr.onStart(function(err, api) {
				assert.isUndefined( err );
				assert.isObject( api );
				flickr.onStop(done);
			});
		});
	});

	describe('methods', function() {
		var fApi;
		var img;

		beforeEach(function(done) {
			img = reg.get('ar.edu.unlp.info.lifia.tvd.media.image');
			flickr.onStart(function(err, api) {
				assert.isUndefined( err );
				assert.isObject( api );
				fApi = api;
				done();
			});
		});

		afterEach(function(done) {
			img = undefined;
			fApi = undefined;
			flickr.onStop(done);
		});

		describe('cfg user name', function() {
			it('should return empty string if no username setted', function() {
				assert.equal( '', fApi.getUserName() );
			});

			it('should not set username if user not exist', function(done) {
				fApi.setUserName('pepe', function(err) {
					assert.instanceOf( err, Error, 'User does not exist' );
					assert.equal( 'Query to flickr return error! stat=fail, message="User does not exist"', err.message );
					assert.equal( '', fApi.getUserName() );
					done();
				});

				assert.equal( httpsMock.requests.length, 1 );
				var req = httpsMock.requests[0];
				assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.people.findByUsername&username=pepe' );
				req.signals.data('{"stat":"fail","message":"User does not exist"}');
				req.signals.end();
			});

			it('should set username if user exist', function(done) {
				fApi.setUserName('pepe', function(err) {
					assert.isUndefined( err );
					assert.equal( 'pepe', fApi.getUserName() );
					done();
				});

				assert.equal( httpsMock.requests.length, 1 );
				var req = httpsMock.requests[0];
				assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.people.findByUsername&username=pepe' );
				assert.isFunction( req.signals.data );
				assert.isFunction( req.signals.error );
				assert.isFunction( req.signals.end );
				req.signals.data('{"stat":"ok","user":{"nsid":"1111"}}');
				req.signals.end();
			});

			it('should clear username if empty', function(done) {
				fApi.setUserName('', function(err) {
					assert.isUndefined( err );
					assert.equal( '', fApi.getUserName() );
					done();
				});
			});
		});

		describe('get', function() {
			it('should return recent images if no user setted', function(done) {
				var opts = {
					provider: 'flickr'
				};
				img.get(opts, function(err, result) {
					assert.isUndefined(err);
					assert.equal( result.length, 1 );
					assert.deepEqual( result[0], {
						provider: 'flickr',
						id: '111',
						name: 'Test Image',
						url: 'https://farm1.staticflickr.com/123/111_456_b.jpg',
						type: 'image'
					});
					done();
				});

				assert.equal( httpsMock.requests.length, 1 );
				var req = httpsMock.requests[0];
				assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.interestingness.getList' );
				assert.isFunction( req.signals.data );
				assert.isFunction( req.signals.error );
				assert.isFunction( req.signals.end );
				req.signals.data('{"stat":"ok",');
				req.signals.data('"photos":{"photo":[');
				req.signals.data('{"id":"111","title":"Test Image","farm":"1","server":"123","secret":"456"}');
				req.signals.data(']}}');
				req.signals.end();
			});

			it('should return error if flickr response with stat != ok', function(done) {
				var opts = {
					provider: 'flickr'
				};
				img.get(opts, function(err, result) {
					assert.isUndefined( result );
					assert.instanceOf( err, Error, "Fail to get photos" );
					assert.equal( 'Query to flickr return error! stat=fail, message="Fail to get photos"', err.message );
					done();
				});

				assert.equal( httpsMock.requests.length, 1 );
				var req = httpsMock.requests[0];
				assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.interestingness.getList' );
				assert.isFunction( req.signals.data );
				assert.isFunction( req.signals.error );
				assert.isFunction( req.signals.end );
				req.signals.data('{"stat":"fail","message":"Fail to get photos"}');
				req.signals.end();
			});

			it('should return error if https request fail', function(done) {
				var opts = {
					provider: 'flickr'
				};
				img.get(opts, function(err, result) {
					assert.isUndefined( result );
					assert.instanceOf( err, Error, "Connection lost" );
					assert.equal( err.message, 'Fail to query flickr api! err="Connection lost"' );
					done();
				});

				assert.equal( httpsMock.requests.length, 1 );
				var req = httpsMock.requests[0];
				assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.interestingness.getList' );
				assert.isFunction( req.signals.data );
				assert.isFunction( req.signals.error );
				assert.isFunction( req.signals.end );
				req.signals.data('{"stat":"ok",');
				req.signals.error( new Error('Connection lost') );
			});

			it('should return images of user if setted', function(done) {
				var opts = {
					provider: 'flickr'
				};

				fApi.setUserName('pepe', function(err) {
					assert.isUndefined( err );
					assert.equal( 'pepe', fApi.getUserName() );

					img.get(opts, function(err, result) {
						assert.isUndefined(err);
						assert.equal( result.length, 1 );
						assert.deepEqual( result[0], {
							provider: 'flickr',
							id: '111',
							name: 'Test Image',
							url: 'https://farm1.staticflickr.com/123/111_456_b.jpg',
							type: 'image'
						});
						done();
					});

					assert.equal( httpsMock.requests.length, 2 );
					let req = httpsMock.requests[1];
					assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.photos.search&user_id=1111' );
					assert.isFunction( req.signals.data );
					assert.isFunction( req.signals.error );
					assert.isFunction( req.signals.end );
					req.signals.data('{"stat":"ok",');
					req.signals.data('"photos":{"photo":[');
					req.signals.data('{"id":"111","title":"Test Image","farm":"1","server":"123","secret":"456"}');
					req.signals.data(']}}');
					req.signals.end();
				});

				assert.equal( httpsMock.requests.length, 1 );
				let req = httpsMock.requests[0];
				assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.people.findByUsername&username=pepe' );
				assert.isFunction( req.signals.data );
				assert.isFunction( req.signals.error );
				assert.isFunction( req.signals.end );
				req.signals.data('{"stat":"ok","user":{"nsid":"1111"}}');
				req.signals.end();
			});

			it('should return recent images paginated', function(done) {
				var opts = {
					provider: 'flickr',
					page: 0,
					perPage: 2
				};
				img.get(opts, function(err, result) {
					assert.isUndefined(err);
					assert.equal( result.length, 2 );
					assert.deepEqual( result[0], {
						provider: 'flickr',
						id: '111',
						name: 'Test Image 1',
						url: 'https://farm1.staticflickr.com/123/111_456_b.jpg',
						type: 'image'
					});
					assert.deepEqual( result[1], {
						provider: 'flickr',
						id: '112',
						name: 'Test Image 2',
						url: 'https://farm2.staticflickr.com/124/112_457_b.jpg',
						type: 'image'
					});
					done();
				});

				assert.equal( httpsMock.requests.length, 1 );
				var req = httpsMock.requests[0];
				assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.interestingness.getList&page=1&per_page=2' );
				assert.isFunction( req.signals.data );
				assert.isFunction( req.signals.error );
				assert.isFunction( req.signals.end );
				req.signals.data('{"stat":"ok",');
				req.signals.data('"photos":{"photo":[');
				req.signals.data('{"id":"111","title":"Test Image 1","farm":"1","server":"123","secret":"456"},');
				req.signals.data('{"id":"112","title":"Test Image 2","farm":"2","server":"124","secret":"457"}');
				req.signals.data(']}}');
				req.signals.end();
			});

			it('should return image by id', function(done) {
				var opts = {
					provider: 'flickr',
					id: '1',
					fields: ['description','camera','width','height','date','exposureTime','apertureValue','exposureCompensation','flash','orientation','gps']
				};

				var counter=0;
				httpsMock.events.on( 'request', function() {
					if (counter === 1) {
						setTimeout(function() {
							assert.equal( httpsMock.requests.length, 2 );
							let req = httpsMock.requests[1];
							assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.photos.getExif&photo_id=1' );
							assert.isFunction( req.signals.data );
							assert.isFunction( req.signals.error );
							assert.isFunction( req.signals.end );
							req.signals.data('{"stat":"ok",');
							req.signals.data('"photo":{');
							req.signals.data('"id": "1",');
							req.signals.data('"exif":[');
							req.signals.data('{"tag":"ImageDescription","raw":{"_content":"descripcion de la foto"}},');
							req.signals.data('{"tag":"Make","raw":{"_content":"Cam maker"}},');
							req.signals.data('{"tag":"Model","raw":{"_content":"Cam model"}},');
							req.signals.data('{"tag":"ImageHeight","raw":{"_content":"400"},"clean":{"_content":"4a00"}},');
							req.signals.data('{"tag":"ImageWidth","raw":{"_content":"600"},"clean":{"_content":"600"}},');
							req.signals.data('{"tag":"ModifyDate","raw":{"_content":"2015:10:25 15:39:58"}},');
							req.signals.data('{"tag":"CreateDate","raw":{"_content":"2015:10:25 15:39:57"}},');
							req.signals.data('{"tag":"ExposureTime","raw":{"_content":"0.006 sec (1/160)"}},');
							req.signals.data('{"tag":"FNumber","raw":{"_content":"f/4.5"}},');
							req.signals.data('{"tag":"ISO","raw":{"_content":"250"}},');
							req.signals.data('{"tag":"ExposureCompensation","raw":{"_content":0}},');
							req.signals.data('{"tag":"Flash","raw":{"_content":"Off, Did not fire"}},');
							req.signals.data('{"tag":"Orientation","raw":{"_content":"landscape"}},');
							req.signals.data('{"tag":"GPSLatitudeRef","raw":{"_content":"-1"}},');
							req.signals.data('{"tag":"GPSLatitude","raw":{"_content":"12"}},');
							req.signals.data('{"tag":"GPSLongitudeRef","raw":{"_content":"-2"}},');
							req.signals.data('{"tag":"GPSLongitude","raw":{"_content":"14"}},');
							req.signals.data('{"tag":"GPSAltitudeRef","raw":{"_content":"3"}},');
							req.signals.data('{"tag":"GPSAltitude","raw":{"_content":"10"}}');
							req.signals.data(']}}');
							req.signals.end();
						}, 1 );
					}
					counter++;
				});

				img.get(opts, function(err, result) {
					assert.isUndefined(err);
					assert.isObject( result );
					var wait = {
						provider: 'flickr',
						id: '1',
						name: 'Test Image',
						url: 'https://farm1.staticflickr.com/569/1_d58465b70c_b.jpg',
						type: 'image',
						camera: {
							model: 'Cam model',
							make: 'Cam maker'
						},
						gps: {},
						description: 'descripcion de la foto',
						width: 600,
						height: 400,
						date: '2015:10:25 15:39:57',
						exposureCompensation: 0
					};
					assert.equal( result.provider, wait.provider );
					assert.deepEqual( result, wait );
					assert.equal( httpsMock.requests.length, 2 );
					done();
				});

				{
					assert.equal( httpsMock.requests.length, 1 );
					let req = httpsMock.requests[0];
					assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.photos.getInfo&photo_id=1' );
					assert.isFunction( req.signals.data );
					assert.isFunction( req.signals.error );
					assert.isFunction( req.signals.end );
					req.signals.data('{"stat":"ok",');
					req.signals.data('"photo": {');
					req.signals.data('"id": "1",');
					req.signals.data('"secret": "d58465b70c",');
					req.signals.data('"server": "569",');
					req.signals.data('"farm": 1,');
					req.signals.data('"title": { "_content": "Test Image" }');
					req.signals.data('}}');
					req.signals.end();
				}


			});

			it('should parse fractions in exif tags', function(done) {
				var opts = {
					provider: 'flickr',
					id: '1',
					fields: ['exposureTime','exposureCompensation']
				};

				var counter=0;
				httpsMock.events.on( 'request', function() {
					if (counter === 1) {
						setTimeout(function() {
							assert.equal( httpsMock.requests.length, 2 );
							let req = httpsMock.requests[1];
							assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.photos.getExif&photo_id=1' );
							assert.isFunction( req.signals.data );
							assert.isFunction( req.signals.error );
							assert.isFunction( req.signals.end );
							req.signals.data('{"stat":"ok",');
							req.signals.data('"photo":{');
							req.signals.data('"id": "1",');
							req.signals.data('"exif":[');
							req.signals.data('{"tag":"ExposureTime","raw":{"_content":"1/160"}},');
							req.signals.data('{"tag":"ExposureCompensation","raw":{"_content":"1/160/10"}}');
							req.signals.data(']}}');
							req.signals.end();
						}, 1 );
					}
					counter++;
				});

				img.get(opts, function(err, result) {
					assert.isUndefined(err);
					assert.isObject( result );
					var wait = {
						provider: 'flickr',
						id: '1',
						name: 'Test Image',
						url: 'https://farm1.staticflickr.com/569/1_d58465b70c_b.jpg',
						type: 'image',
						camera: {},
						gps: {},
						exposureTime: 0.00625
					};
					assert.equal( result.provider, wait.provider );
					assert.deepEqual( result, wait );
					assert.equal( httpsMock.requests.length, 2 );
					done();
				});

				assert.equal( httpsMock.requests.length, 1 );
				let req = httpsMock.requests[0];
				assert.equal( req.url, 'https://api.flickr.com/services/rest?api_key=b0f0512c18133f50dfa87e458dbaf0a1&format=json&nojsoncallback=1&method=flickr.photos.getInfo&photo_id=1' );
				assert.isFunction( req.signals.data );
				assert.isFunction( req.signals.error );
				assert.isFunction( req.signals.end );
				req.signals.data('{"stat":"ok",');
				req.signals.data('"photo": {');
				req.signals.data('"id": "1",');
				req.signals.data('"secret": "d58465b70c",');
				req.signals.data('"server": "569",');
				req.signals.data('"farm": 1,');
				req.signals.data('"title": { "_content": "Test Image" }');
				req.signals.data('}}');
				req.signals.end();
			});
		});
	});
});
