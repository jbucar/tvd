'use strict';

var _ = require('lodash');
var tvdutil = require('tvdutil');
var fs = require('fs');
var chai = require('chai');
chai.config.includeStack = true;
var bPromise = require('bluebird');
var assert = chai.assert;
var Mocks = require('mocks');
var Image = require('../src/image');

describe('Image', function() {
	var reg = null;
	var adapter = null;

	function createImage(name) {
		return {
			id: name,
			name: name,
			type: 'image',
			url: 'http://www.pepe.com/' + name,
		};
	};

	function createAlbum(name) {
		return {
			name: name,
			type: 'image',
			date: new Date(2015, 08, 10, 10, 30, 0, 0),
			description: 'desc1'
		};
	};

	function DummyProvider(name,imgs) {
		var images = {};
		for (var i=0; i<imgs.length; i++) {
			var key = imgs[i].name;
			images[key] = imgs[i];
			images[key].provider = name;
		}

		var self = {};
		self.id = name;
		self.name = name;
		self.icon = 'icon.png';
		self.logo = 'logo.png';
		self.get = function(opts) {
			return new bPromise(function(resolve) {
				if (opts.id) {
					resolve( images[opts.id] );
				}
				else {
					var info = [];
					Object.keys(images).forEach(function(key) {
						info.push( images[key] );
					});
					resolve( info );
				}
			});
		};
		return self;
	};


	beforeEach(function() {
		reg = Mocks.init('silly');
		adapter = new Mocks.ServiceAdapter(reg);
	});

	afterEach(function() {
		Mocks.fin();
		adapter = null;
	});

	describe('constructor', function() {
		it('should construct basic Image', function() {
			new Image();
		});
	});

	describe('onStart/onStop', function() {
		var image = null;

		beforeEach(function() {
			image = new Image();
			assert(image);
		});

		afterEach(function() {
			image = null;
		});

		it('should check api from image', function(done) {
			image.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				assert.equal( Object.keys(api).length, 9 );
				assert.isFunction(api.getProviders);
				assert.isFunction(api.addProvider);
				assert.isFunction(api.rmProvider);
				assert.isFunction(api.get);
				assert.isFunction(api.on);
				assert.isFunction(api.once);
				assert.isFunction(api.removeListener);
				assert.isFunction( api.getWebAPI );
				var webApi = api.getWebAPI();
				assert.equal( webApi.name, 'media.image' );
				assert.deepEqual( webApi.public, ['getProviders', 'hasProvider', 'get', 'on' ] );
				assert.deepEqual( webApi.private, ['addProvider', 'rmProvider'] );
				image.onStop(done);
			});
		});

		it('should cant read schema', function(done) {
			var old = fs.readFile;
			fs.readFile = function(n,cb) {
				cb( new Error('pepe') );
			};

			image.onStart(function(err,api) {
				assert.equal( err.message, 'pepe' );
				fs.readFile = old;
				done();
			});
		});

		it('should cant parse schema', function(done) {
			var old = fs.readFile;
			fs.readFile = function(n,cb) {
				cb( undefined, '{pepe' );
			};

			image.onStart(function(err,api) {
				assert.equal( err.message, 'Unexpected token p in JSON at position 1' );
				fs.readFile = old;
				done();
			});
		});

		it('should cant parse schema', function(done) {
			var old = fs.readFile;
			fs.readFile = function(n,cb) {
				cb( undefined, '{pepe' );
			};

			image.onStart(function(err,api) {
				assert.equal( err.message, 'Unexpected token p in JSON at position 1' );
				fs.readFile = old;
				done();
			});
		});

		it('should start/stop/start', function(done) {
			image.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				image.onStop(function() {
					image.onStart(function(err1,api1) {
						assert.equal(err1);
						assert.isObject(api1);
						assert(api1 != api);
						image.onStop(done);
					});
				});
			});
		});
	});

	describe('api', function() {
		var image = null;
		var api = null;

		beforeEach(function(done) {
			image = new Image();
			image.onStart(function(err,impl) {
				assert.equal(err);
				assert.isObject(impl);
				api = impl;
				done();
			});
		});

		afterEach(function(done) {
			image.onStop(function() {
				api = null;
				image = null;
				done();
			});
		});

		it('should add/remove provider', function(done) {
			api.on('providerAdded', function(provID) {
				assert.equal( provID, 'pepe' );

				var prov = {id: 'pepe', name:'pepe', icon: 'icon.png', logo: 'logo.png', description: undefined};
				assert.deepEqual( api.getProviders(), [prov] );

				api.rmProvider( 'pepe' );
				assert.deepEqual( api.getProviders(), [] );
				done();
			});

			assert.deepEqual( api.getProviders(), [] );
			api.addProvider( new DummyProvider('pepe',[]) );
			var provs = api.getProviders();
		});

		it('should fail if provider already exists', function(done) {
			var count=-1;
			api.on('providerAdded', function(provID) {
				count++;
				if (count === 0) {
					assert.equal( provID, 'pepe' );

					var prov = { id: 'pepe', name:'pepe', icon: 'icon.png', logo: 'logo.png', description: undefined };
					assert.deepEqual( api.getProviders(), [prov] );

					api.addProvider( new DummyProvider('pepe',[]) );
					assert.deepEqual( api.getProviders(), [prov] );
					done();
				}
			});

			assert.deepEqual( api.getProviders(), [] );
			api.addProvider( new DummyProvider('pepe',[]) );
		});

		it('should fail if provider invalid', function() {
			assert.deepEqual( api.getProviders(), [] );
			api.addProvider( {} );
			assert.deepEqual( api.getProviders(), [] );

			api.addProvider( { name: 'pepe' } );
			assert.deepEqual( api.getProviders(), [] );

			api.addProvider( { name: 'pepe', get: 'pepe' } );
			assert.deepEqual( api.getProviders(), [] );
		});

		it('should not remove invalid provider', function() {
			api.rmProvider( 'pepe' );
		});

		describe('get/getAll', function() {
			it('should not work with a invalid provider', function() {
				var testImages = [ new createImage('img1'), new createImage('img2') ];
				api.addProvider( new DummyProvider('pepe', testImages) );
				api.get( { provider: 'pepe1' }, function(err,aInfo) {
					assert.equal( err.message, 'Provider not found: pepe1' );
					assert.equal( aInfo, undefined );
				});
			});

			it('should return all images from all providers if provider undefined', function() {
				var testImages = [ new createImage('img1'), new createImage('img2') ];
				api.addProvider( new DummyProvider('pepe', testImages) );

				var testImages2 = [ new createImage('img3'), new createImage('img4') ];
				api.addProvider( new DummyProvider('pepe2', testImages2) );

				api.get( {}, function(err,aInfo) {
					assert.equal( err );
					assert.equal( aInfo.length, 4 );
					assert.deepEqual( aInfo, [testImages[0], testImages[1], testImages2[0], testImages2[1]] );
				});
			});

			it('should work only with provider parameter', function() {
				var testImages = [ new createImage('img1'), new createImage('img2') ];
				api.addProvider( new DummyProvider('pepe', testImages) );

				var testImages2 = [ new createImage('img3'), new createImage('img4') ];
				api.addProvider( new DummyProvider('pepe2', testImages2) );

				api.get( {provider:'pepe'}, function(err,aInfo) {
					assert.equal( err );
					assert.equal( aInfo.length, 2 );
					assert.deepEqual( aInfo, testImages );
				});
			});
		});
	});
});
