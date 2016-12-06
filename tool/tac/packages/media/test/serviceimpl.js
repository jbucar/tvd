'use strict';

var _ = require('lodash');
var tvdutil = require('tvdutil');
var fs = require('fs');
var path = require('path');
var chai = require('chai');
chai.config.includeStack = true;
var bPromise = require('bluebird');
var assert = chai.assert;
var Mocks = require('mocks');
var ServiceImpl = require('../src/serviceimpl').ServiceImpl;

function imageSchema() {
	var schemaPath = path.join(path.dirname(__dirname),'src','image.json');
	return JSON.parse(fs.readFileSync(schemaPath));
}

describe('ServiceImpl', function() {
	var reg = null;
	var adapter = null;

	function Image(name) {
		return {
			name: name,
			id: name,
			type: 'image',
			url: 'http://www.pepe.com/' + name,
		};
	};

	function Album(name) {
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
					var count;
					if (opts.perPage) {
						count = (opts.perPage < imgs.length) ? opts.perPage : imgs.length;
					}
					else {
						count = imgs.length;
					}
					for (var i=0; i<count; i++) {
						info.push( images[ imgs[i].name ] );
					}
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
		it('basic constructor ServiceImpl', function() {
			new ServiceImpl('pepe', '{}');
		});

		it('should check name parameter', function() {
			function fn() { new ServiceImpl(undefined,'{}'); }
			assert.throw(fn);
		});

		it('should check schema parameter', function() {
			function fn() { new ServiceImpl('name'); }
			assert.throw(fn);
		});
	});

	describe( 'Fields validation', function() {
		it('should check simple get result ok', function(done) {
			var provider = new ServiceImpl('pepe', imageSchema() );
			var testImages = [ new Image('img1'), new Image('img2') ];
			provider.addProvider( new DummyProvider('pepe', testImages) );
			provider.get( { provider: 'pepe', id: 'img1' }, function(err,aInfo) {
				assert.equal( err );
				assert( aInfo );
				done();
			});
		});

		it('should check simple get and fail', function(done) {
			var provider = new ServiceImpl('pepe', imageSchema() );
			var testImages = [ new Image('img1'), new Image('img2') ];
			delete testImages[0].id;
			provider.addProvider( new DummyProvider('pepe', testImages) );
			provider.get( { provider: 'pepe', id: 'img1' }, function(err,aInfo) {
				assert( err.message.indexOf('Not found') >= 0 );
				assert.equal( aInfo, undefined );
				done();
			});
		});

		it('should check simple get and fail becouse additional invalid field', function(done) {
			var provider = new ServiceImpl('pepe', imageSchema() );
			var testImages = [ new Image('img1'), new Image('img2') ];
			testImages[0].pepe = "value";
			provider.addProvider( new DummyProvider('pepe', testImages) );
			provider.get( { provider: 'pepe', id: 'img1' }, function(err,aInfo) {
				assert( err.message.indexOf('Not found') >= 0 );
				assert.equal( aInfo, undefined );
				done();
			});
		});

		it('should check get from multiple responses ok', function(done) {
			var provider = new ServiceImpl('pepe', imageSchema() );
			var testImages = [ new Image('img1'), new Image('img2') ];
			provider.addProvider( new DummyProvider('pepe', testImages) );
			provider.get( { provider: 'pepe' }, function(err,aInfo) {
				assert.equal( err );
				assert.equal( aInfo.length, 2 );
				done();
			});
		});

		it('should check get from multiple responses skip one', function(done) {
			var provider = new ServiceImpl('pepe', imageSchema() );
			var testImages = [ new Image('img1'), new Image('img2') ];
			delete testImages[0].id;
			provider.addProvider( new DummyProvider('pepe', testImages) );
			provider.get( { provider: 'pepe' }, function(err,aInfo) {
				assert.equal( err );
				assert.equal( aInfo.length, 1 );
				done();
			});
		});

		it('should check get from multiple responses skip all', function(done) {
			var provider = new ServiceImpl('pepe', imageSchema() );
			var testImages = [ new Image('img1'), new Image('img2') ];
			delete testImages[0].id;
			delete testImages[1].id;
			provider.addProvider( new DummyProvider('pepe', testImages) );
			provider.get( { provider: 'pepe' }, function(err,aInfo) {
				assert.equal( err );
				assert.equal( aInfo.length, 0 );
				done();
			});
		});
	});

	describe('api', function() {
		var provider = null;

		beforeEach(function() {
			provider = new ServiceImpl('pepe',imageSchema());
		});

		afterEach(function() {
			provider = null;
		});

		it('should add/remove provider', function() {
			var added=0, removed=0;

			provider.on('providerRemoved', function(provID) {
				removed++;
			});

			provider.on('providerAdded', function(provID) {
				added++;
				assert.equal( provID, 'pepe' );

				var prov = {id: 'pepe', name:'pepe', icon: 'icon.png', logo: 'logo.png', description: undefined};
				assert.equal( provider.hasProvider('pepe'), true );
				assert.deepEqual( provider.getProviders(), [prov] );

				provider.rmProvider( 'pepe' );
				assert.equal( provider.hasProvider('pepe'), false );
				assert.deepEqual( provider.getProviders(), [] );
			});

			assert.equal( provider.hasProvider('pepe'), false );
			assert.deepEqual( provider.getProviders(), [] );
			provider.addProvider( new DummyProvider('pepe',[]) );

			assert.equal( added, 1 );
			assert.equal( removed, 1 );
		});

		it('should fail if provider already exists', function(done) {
			var count=-1;
			provider.on('providerAdded', function(provID) {
				count++;
				if (count === 0) {
					assert.equal( provID, 'pepe' );

					var prov = { id: 'pepe', name:'pepe', icon: 'icon.png', logo: 'logo.png', description: undefined };
					assert.deepEqual( provider.getProviders(), [prov] );

					provider.addProvider( new DummyProvider('pepe',[]) );
					assert.deepEqual( provider.getProviders(), [prov] );
					done();
				}
			});

			assert.deepEqual( provider.getProviders(), [] );
			provider.addProvider( new DummyProvider('pepe',[]) );
		});

		it('should fail if provider invalid', function() {
			assert.deepEqual( provider.getProviders(), [] );
			provider.addProvider( {} );
			assert.deepEqual( provider.getProviders(), [] );

			provider.addProvider( { name: 'pepe' } );
			assert.deepEqual( provider.getProviders(), [] );

			provider.addProvider( { name: 'pepe', get: 'pepe' } );
			assert.deepEqual( provider.getProviders(), [] );
		});

		it('should not remove invalid provider', function() {
			provider.rmProvider( 'pepe' );
		});

		describe('get/getAll', function() {
			it('should not work with a invalid provider', function() {
				var testImages = [ new Image('img1'), new Image('img2') ];
				provider.addProvider( new DummyProvider('pepe', testImages) );
				provider.get( { provider: 'pepe1' }, function(err,aInfo) {
					assert.equal( err.message, 'Provider not found: pepe1' );
					assert.equal( aInfo, undefined );
				});
			});

			it('should return all images from all providers if provider undefined', function() {
				var testImages = [ new Image('img1'), new Image('img2') ];
				provider.addProvider( new DummyProvider('pepe', testImages) );

				var testImages2 = [ new Image('img3'), new Image('img4') ];
				provider.addProvider( new DummyProvider('pepe2', testImages2) );

				provider.get( {}, function(err,aInfo) {
					assert.equal( err );
					assert.equal( aInfo.length, 4 );
					assert.deepEqual( aInfo, [testImages[0], testImages[1], testImages2[0], testImages2[1]] );
				});
			});

			it('should work only with provider parameter', function() {
				var testImages = [ new Image('img1'), new Image('img2') ];
				provider.addProvider( new DummyProvider('pepe', testImages) );

				var testImages2 = [ new Image('img3'), new Image('img4') ];
				provider.addProvider( new DummyProvider('pepe2', testImages2) );

				provider.get( {provider:'pepe'}, function(err,aInfo) {
					assert.equal( err );
					assert.equal( aInfo.length, 2 );
					assert.deepEqual( aInfo, testImages );
				});
			});

			describe( 'paging', function() {
				it('should page with a provider', function(done) {
					var testImages = [ new Image('img1'), new Image('img2'), new Image('img3') ];
					provider.addProvider( new DummyProvider('pepe', testImages) );

					provider.get( {provider: 'pepe', page:0, perPage:2}, function(err,aInfo) {
						assert.equal( err );
						assert.equal( aInfo.length, 2 );
						assert.deepEqual( aInfo, [testImages[0], testImages[1]] );
						done();
					});
				});

				it('should check page exists if perPage exist', function(done) {
					var testImages = [ new Image('img1'), new Image('img2'), new Image('img3') ];
					provider.addProvider( new DummyProvider('pepe', testImages) );

					provider.get( {perPage:2}, function(err,aInfo) {
						assert.equal( err );
						assert.equal( aInfo.length, 3 );
						assert.deepEqual( aInfo, testImages );
						done();
					});
				});

				it('should check perPage exists if page exist', function(done) {
					var testImages = [ new Image('img1'), new Image('img2'), new Image('img3') ];
					provider.addProvider( new DummyProvider('pepe', testImages) );

					provider.get( {page:2}, function(err,aInfo) {
						assert.equal( err );
						assert.equal( aInfo.length, 3 );
						assert.deepEqual( aInfo, testImages );
						done();
					});
				});

				it('should check page is number', function(done) {
					var testImages = [ new Image('img1'), new Image('img2'), new Image('img3') ];
					provider.addProvider( new DummyProvider('pepe', testImages) );

					provider.get( {page:'0', perPage:2}, function(err,aInfo) {
						assert.equal( err );
						assert.equal( aInfo.length, 3 );
						assert.deepEqual( aInfo, testImages );
						done();
					});
				});

				it('should check perPage is number', function(done) {
					var testImages = [ new Image('img1'), new Image('img2'), new Image('img3') ];
					provider.addProvider( new DummyProvider('pepe', testImages) );

					provider.get( {page:0, perPage:'2'}, function(err,aInfo) {
						assert.equal( err );
						assert.equal( aInfo.length, 3 );
						assert.deepEqual( aInfo, testImages );
						done();
					});
				});

				it('should check page && perPage is number', function(done) {
					var testImages = [ new Image('img1'), new Image('img2'), new Image('img3') ];
					provider.addProvider( new DummyProvider('pepe', testImages) );

					provider.get( {page:'0', perPage:'2'}, function(err,aInfo) {
						assert.equal( err );
						assert.equal( aInfo.length, 3 );
						assert.deepEqual( aInfo, testImages );
						done();
					});
				});

				it('should check perPage is number greater than 0', function(done) {
					var testImages = [ new Image('img1'), new Image('img2'), new Image('img3') ];
					provider.addProvider( new DummyProvider('pepe', testImages) );

					provider.get( {page:'0', perPage:'0'}, function(err,aInfo) {
						assert.equal( err );
						assert.equal( aInfo.length, 3 );
						assert.deepEqual( aInfo, testImages );
						done();
					});
				});

				it('should page with multiple providers', function(done) {
					var testImages = [ new Image('img1_1'), new Image('img1_2'), new Image('img1_3') ];
					provider.addProvider( new DummyProvider('pepe', testImages) );

					var testImages2 = [ new Image('img2_1'), new Image('img2_2'), new Image('img2_3') ];
					provider.addProvider( new DummyProvider('pepe2', testImages2) );

					provider.get( {page:0,perPage:2}, function(err,aInfo) {
						assert.equal( err.message, 'Invalid options; Paging with multiple providers are not supported' );
						assert.equal( aInfo );
						done();
					});
				});
			});
		});
	});
});
