'use strict';

var fs = require('fs');
var path = require('path');
var assert = require('assert');
var util = require('util');
var _ = require('lodash');
var bPromise = require('bluebird');
var Mocks = require('mocks');
var mockfs = require('mock-fs');
var tvdutil = require('tvdutil');
var LogosModule = require('../src/main');

function FakeTvd() {
	var self = this || {};
	var _providers = [];

	self.addLogoProvider = function( p ) {
		_providers.push( p );
	}

	self.removeLogoProvider = function( p ) {
		var index = _providers.indexOf(p);
		if (index >= 0) {
			_providers.splice(index,1);
		}
	}

	self.updateLogos = function() {
	};

	self.find = function(ch) {
		var best = null;
		_providers.forEach(function(p) {
			var result = p( ch );
			if (result && ((best && best.priority < result.priority) || !best)) {
				best = result;
			}
		});
		return best;
	}

	return self;
}

var logosContent = '{\
		"canales":[\
			{"nombre_imagen":"http:\/\/bocetos.minplan.gob.ar\/grilla\/public\/images\/productoras\/3_82.svg","provincia":null,"id_provincia":null,"id_productora":"3","nombre":"Encuentro","solo_en":"","enlace":"http:\/\/www.encuentro.gov.ar\/","alt_img":"Encuentro","title_img":"Encuentro","frequency":"521143","fecha_modificacion_tac":"2015-04-20 16:20:23","canales_virtuales":[{"type":"MV","service_id":"0","numero":"2231","ts_id":"0","on_id":"0"},{"type":"SD","service_id":"59168","numero":"2201","ts_id":"1849","on_id":"1849"}]},\
			{"nombre_imagen":"http:\/\/bocetos.minplan.gob.ar\/grilla\/public\/images\/productoras\/_nacional-22.02-paka-paka.svg","provincia":null,"id_provincia":null,"id_productora":"4","nombre":"Paka Paka","solo_en":"","enlace":"http:\/\/www.pakapaka.gov.ar\/","alt_img":"Paka Paka","title_img":"Paka Paka","frequency":"521143","fecha_modificacion_tac":"2015-04-20 16:20:23","canales_virtuales":[{"type":"SD","service_id":"59169","numero":"2202","ts_id":"1849","on_id":"1849"}]},\
			{"nombre_imagen":"http:\/\/bocetos.minplan.gob.ar\/grilla\/public\/images\/productoras\/ta-te-ti.svg","provincia":null,"id_provincia":null,"id_productora":"5","nombre":"TaTeTi","solo_en":"","enlace":"http:\/\/tatetitv.tv\/","alt_img":"TaTeTi","title_img":"TaTeTi","frequency":"521143","fecha_modificacion_tac":"2015-04-20 16:20:23","canales_virtuales":[{"type":"SD","service_id":"59170","numero":"2203","ts_id":"1849","on_id":"1849"}]},\
			{"nombre_imagen":"http:\/\/bocetos.minplan.gob.ar\/grilla\/public\/images\/productoras\/ta-te-ti.svg","provincia":null,"id_provincia":null,"id_productora":"5","nombre":"TaTeTi","solo_en":"","enlace":"http:\/\/tatetitv.tv\/","alt_img":"TaTeTi","title_img":"TaTeTi","frequency":null,"fecha_modificacion_tac":"2015-04-20 16:20:23","canales_virtuales":[{"type":"SD","service_id":"59170","numero":"2203","ts_id":"1849","on_id":"1849"}]}\
		],\
		"ultima_fecha_modificacion":"2015-04-20 16:20:23"\
	}';

describe('tdalogos', function() {
	var _reg = null;
	var _module = null;
	var _adapter = null;
	var dbPath = '/channels.json';
	var defaultDB = [
		{
			"name": "Tecnopolis",
			"frequency": "503",
			"nitID": "100",
			"tsID": "101",
			"srvID": "102",
			"channelID": "22.05",
			"logo": "tecnopolis.jpg"
		},
		{
			"name": "DeporTV",
			"frequency": "-1",
			"nitID": "-1",
			"tsID": "-1",
			"srvID": "-1",
			"channelID": "24.01",
			"logo": "deportv.jpg"
		}
	];

	beforeEach( function() {
		_reg = Mocks.init();

		var defaultFs = { '/': {} };
		mockfs(defaultFs);

		_reg.put( 'tvd', new FakeTvd() );

		fs.writeFileSync( dbPath, JSON.stringify(defaultDB, null, 4) );
		_adapter = new Mocks.ServiceAdapter(_reg);
		_adapter.getInstallPath = function() {
			return '/';
		}

		_module = new LogosModule(_adapter);
		assert( _module );
	});

	afterEach(function() {
		mockfs.restore();

		Mocks.fin();
		_module = null;
		_adapter = null;
		_reg = null;
	});

	it('should load default DB if corrupted', function(done) {
		_adapter.load = function( file, defaultsValues ) {
			var cfg = _.cloneDeep(defaultsValues);
			cfg.dbBase = '/tmp';
			return cfg;
		};
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert(api);
			_module.onStop(done);
		});
	});

	it('should start/stop', function(done) {
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert(api);
			_module.onStop(done);
		});
	});

	it('should fail if cannot load logos configuration', function(done) {
		fs.writeFileSync( '/channels.json', "{'pepe'}" );
		_module.onStart(function(err,api) {
			assert.equal(err, 'Error: Cannot load logos configuration' );
			done();
		});
	});

	it('should start/stop/start/stop', function(done) {
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert(api);
			_module.onStop(function() {
				_module.onStart(function(err2,api2) {
					assert.equal(err2);
					assert(api2);
					assert( api !== api2 );
					_module.onStop(done);
				});
			});
		});
	});

	describe('update', function() {
		var oldFetch = null;

		beforeEach( function() {
			oldFetch = tvdutil.fetchURLAsync;

			_adapter.load = function( file, defaultsValues ) {
				var cfg = _.cloneDeep(defaultsValues);
				cfg.url = 'http://algun.com/pepe';
				cfg.firstCheck = 1;
				return cfg;
			};
			_adapter.getDataPath = function() {
				return '/tmp';
			};
		});

		afterEach( function() {
			tvdutil.fetchURLAsync = oldFetch;
		});

		it('check sucessfully', function(done) {
			tvdutil.fetchURLAsync = function(opts) {
				fs.writeFileSync( opts.fileOutput, logosContent );
				return bPromise.resolve();
			};

			_module.onStart(function(err,api) {
				assert.equal(err);
				assert(api);
				setTimeout(function() {
					_module.onStop(done)
				}, 4);
			});
		});

		it('check sucessfully and remove old', function(done) {
			_adapter.load = function( file, defaultsValues ) {
				var cfg = _.cloneDeep(defaultsValues);
				cfg.url = 'http://algun.com/pepe';
				cfg.dbBase = '/tmp/old-data';
				cfg.firstCheck = 1;
				return cfg;
			};

			tvdutil.fetchURLAsync = function(opts) {
				fs.writeFileSync( opts.fileOutput, logosContent );
				return bPromise.resolve();
			};

			_module.onStart(function(err,api) {
				assert.equal(err);
				assert(api);
				setTimeout(function() {
					_module.onStop(done)
				}, 4);
			});
		});

		it('should check with error for updates on start', function(done) {
			tvdutil.fetchURLAsync = function(opts) {
				return bPromise.reject( new Error('Alguno') );
			};
			_module.onStart(function(err,api) {
				assert.equal(err);
				assert(api);
				setTimeout(function() {
					_module.onStop(done)
				}, 4);
			});
		});

		it('should check for errors when update on start and skip if no changes', function(done) {
			tvdutil.fetchURLAsync = function(opts) {
				return bPromise.reject( new Error('No changes in DB') );
			};
			_module.onStart(function(err,api) {
				assert.equal(err);
				assert(api);
				setTimeout(function() {
					_module.onStop(done)
				}, 4);
			});
		});
	});

	describe('methods', function() {
		var api = null;
		var tvd = null;

		beforeEach( function(done) {
			_module.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert(api);
				tvd = _reg.get( 'tvd' );
				done();
			});
		});

		afterEach( function (done) {
			_module.onStop(done);
			api = null;
		});

		it('findLogo fail', function () {
			var ch = {"id":1,"channel":"02","name":"TV Publica HD","info":{"type":"dvb","frequency":"503","nitID":1850,"tsID":1850,"srvID":59201}};
			var result=tvd.find( ch );
			assert( !result );
		});

		it('findLogo by id ok', function () {
			var ch = {"id":2,"channel":"99.99","name":"--","info":{"type":"dvb","frequency":"503","nitID":100,"tsID":101,"srvID":102}};
			var result=tvd.find( ch );
			assert( result );
			assert.equal( result.logo, '/images/tecnopolis.jpg' );
			assert( result.priority === 10 );
		});

		it('findLogo by name ok', function () {
			var ch = {"id":2,"channel":"23.03","name":"Tecnopolis","info":{"type":"dvb","frequency":"503","nitID":1850,"tsID":1850,"srvID":59202}};
			var result=tvd.find( ch );
			assert( result );
			assert.equal( result.logo, '/images/tecnopolis.jpg' );
			assert( result.priority === 5 );
		});

		it('findLogo by number ok', function () {
			var ch = {"id":2,"channel":"22.05","name":"pepe","info":{"type":"dvb","frequency":"503","nitID":1850,"tsID":1850,"srvID":59202}};
			var result=tvd.find( ch );
			assert( result );
			assert.equal( result.logo, '/images/tecnopolis.jpg' );
			assert( result.priority === 5 );
		});
	});
});
