'use strict';

var _ = require('lodash');
var tvdutil = require('tvdutil');
var fs = require('fs');
var chai = require('chai');
chai.config.includeStack = true;
var bPromise = require('bluebird');
var assert = chai.assert;
var Mocks = require('mocks');
var Video = require('../src/video');

describe('Video', function() {
	var reg = null;
	var adapter = null;

	function createVideo(name) {
		return {
			id: name,
			name: name,
			type: 'video',
			url: 'http://www.pepe.com/' + name,
		};
	};

	function DummyProvider(name,imgs) {
		var videos = {};
		for (var i=0; i<imgs.length; i++) {
			var key = imgs[i].name;
			videos[key] = imgs[i];
			videos[key].provider = name;
		}

		var self = {};
		self.id = name;
		self.name = name;
		self.icon = 'icon.png';
		self.logo = 'logo.png';
		self.get = function(opts) {
			return new bPromise(function(resolve) {
				if (opts.id) {
					resolve( videos[opts.id] );
				}
				else {
					var info = [];
					Object.keys(videos).forEach(function(key) {
						info.push( videos[key] );
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
		it('should construct basic Video', function() {
			new Video();
		});
	});

	describe('onStart/onStop', function() {
		var video = null;

		beforeEach(function() {
			video = new Video();
			assert(video);
		});

		afterEach(function() {
			video = null;
		});

		it('should check api from video', function(done) {
			video.onStart(function(err,api) {
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
				assert.equal( webApi.name, 'media.video' );
				assert.deepEqual( webApi.public, ['getProviders', 'hasProvider', 'get', 'on' ] );
				assert.deepEqual( webApi.private, ['addProvider', 'rmProvider'] );
				video.onStop(done);
			});
		});

		it('should start/stop/start', function(done) {
			video.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				video.onStop(function() {
					video.onStart(function(err1,api1) {
						assert.equal(err1);
						assert.isObject(api1);
						assert(api1 != api);
						video.onStop(done);
					});
				});
			});
		});
	});
});
