'use strict';

var fs = require('fs');
var mockfs = require('mock-fs');
var mod = require('../src/logos');
var bPromise = require('bluebird');
var util = require('util');
var _ = require('lodash');
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;
var path = require('path');
var tvdutil = require('tvdutil');

var logosContent = '{"canales":[{"nombre_imagen":"http:\/\/bocetos.minplan.gob.ar\/grilla\/public\/images\/productoras\/3_82.svg","provincia":null,"id_provincia":null,"id_productora":"3","nombre":"Encuentro","solo_en":"","enlace":"http:\/\/www.encuentro.gov.ar\/","alt_img":"Encuentro","title_img":"Encuentro","frequency":"521143","fecha_modificacion_tac":"2015-04-20 16:20:23","canales_virtuales":[{"type":"MV","service_id":"0","numero":"2231","ts_id":"0","on_id":"0"},{"type":"SD","service_id":"59168","numero":"2201","ts_id":"1849","on_id":"1849"}]},{"nombre_imagen":"http:\/\/bocetos.minplan.gob.ar\/grilla\/public\/images\/productoras\/_nacional-22.02-paka-paka.svg","provincia":null,"id_provincia":null,"id_productora":"4","nombre":"Paka Paka","solo_en":"","enlace":"http:\/\/www.pakapaka.gov.ar\/","alt_img":"Paka Paka","title_img":"Paka Paka","frequency":"521143","fecha_modificacion_tac":"2015-04-20 16:20:23","canales_virtuales":[{"type":"SD","service_id":"59169","numero":"2202","ts_id":"1849","on_id":"1849"}]},{"nombre_imagen":"http:\/\/bocetos.minplan.gob.ar\/grilla\/public\/images\/productoras\/ta-te-ti.svg","provincia":null,"id_provincia":null,"id_productora":"5","nombre":"TaTeTi","solo_en":"","enlace":"http:\/\/tatetitv.tv\/","alt_img":"TaTeTi","title_img":"TaTeTi","frequency":"521143","fecha_modificacion_tac":"2015-04-20 16:20:23","canales_virtuales":[{"type":"SD","service_id":"59170","numero":"2203","ts_id":"1849","on_id":"1849"}]}],"ultima_fecha_modificacion":"2015-04-20 16:20:23"}';

function setUpGlobals( logLevel ) {
	//	Register log
	tvdutil.setupLog( logLevel );
}

describe('logos', function() {
	beforeEach( function() {
		setUpGlobals('silly');
	});

	describe('dbName', function() {
		it('dbName must exist', function () {
			assert( mod.dbName !== undefined );
		});

		it('dbName must return the database path', function () {
			assert( mod.dbName('/pepe') === '/pepe/channels.json' );
		});
	});

	describe('imageName', function() {
		it('imageName must exist', function () {
			assert( mod.imageName !== undefined );
		});

		it('imageName must return image path', function () {
			assert( mod.imageName('/pepe','name.img') === '/pepe/images/name.img' );
		});
	});

	describe('fetchLogos', function() {
		var oldFetch = null;
		var defaultFs = { '/': {} };
		var defaultOpts = {
			user: 'pepe',
			password: 'otro',
			url: 'http://pepe.com/logos/no_existe',
			basePath: '/base',
			lastChanged: 'alguno'
		};

		beforeEach( function() {
			mockfs(defaultFs);
			fs.mkdirSync( defaultOpts.basePath );
			assert( tvdutil.isDirectory( defaultOpts.basePath ) );
			oldFetch = tvdutil.fetchURLAsync;
		});

		afterEach(function() {
			mockfs.restore();
			tvdutil.fetchURLAsync = oldFetch;
		});

		it('fetchLogos must exist', function () {
			assert( mod.fetchLogos !== undefined );
		});

		it('fetchLogos must work ok!', function (done) {
			tvdutil.fetchURLAsync = function( opts ) {
				fs.writeFileSync( opts.fileOutput, logosContent );
				return bPromise.resolve();
			};
			var prom = mod.fetchLogos( _.cloneDeep(defaultOpts) )
				.then(function(obj) {
					assert( obj !== undefined );
					assert.isObject(obj);
					assert( obj.logos.length === 4 );
					assert.isString( obj.path );
					assert.isString( obj.lastChanged );
					done();
				},function(err) {
					assert( err === undefined );
				});
			assert( prom !== undefined );
		});

		it('fetchLogos must work ok and, if again, fail!', function (done) {
			tvdutil.fetchURLAsync = function( opts ) {
				fs.writeFileSync( opts.fileOutput, logosContent );
				return bPromise.resolve();
			};
			var prom = mod.fetchLogos( _.cloneDeep(defaultOpts) )
				.then(function(obj) {
					var opts = _.cloneDeep(defaultOpts);
					opts.lastChanged = obj.lastChanged;
					mod.fetchLogos( opts )
						.then(function(obj) {
						},function(err) {
							assert( err !== undefined );
							assert( err.message === 'No changes in DB' );
							done();
						});
				},function(err) {
					assert( err === undefined );
				});
			assert( prom !== undefined );
		});

		it('fetchLogos must handle invalid json db!', function (done) {
			tvdutil.fetchURLAsync = function( opts, cb ) {
				fs.writeFileSync( opts.fileOutput, '' );
				return bPromise.resolve();
			};
			var prom = mod.fetchLogos( _.cloneDeep(defaultOpts) )
				.then(function(obj) {
					assert.isNull( ob );
				},function(err) {
					assert( err !== undefined );
					var filesInTemp = fs.readdirSync(defaultOpts.basePath);
					assert.isTrue( filesInTemp.length === 0 );
					done();
				});
			assert( prom !== undefined );
		});

		it('fetchLogos must handle invalid json db 2!', function (done) {
			tvdutil.fetchURLAsync = function( opts, cb ) {
				fs.writeFileSync( opts.fileOutput, '{}' );
				return bPromise.resolve();
			};
			var prom = mod.fetchLogos( _.cloneDeep(defaultOpts) )
				.then(function(obj) {
					assert.isNull( ob );
				},function(err) {
					assert( err !== undefined );
					var filesInTemp = fs.readdirSync(defaultOpts.basePath);
					assert.isTrue( filesInTemp.length === 0 );
					done();
				});
			assert( prom !== undefined );
		});

		it('fetchLogos must handle valid but empty db!', function (done) {
			tvdutil.fetchURLAsync = function( opts, cb ) {
				fs.writeFileSync( opts.fileOutput, '{"canales":[]}' );
				return bPromise.resolve();
			};
			var prom = mod.fetchLogos( _.cloneDeep(defaultOpts) )
				.then(function(obj) {
					assert.isNull(obj);
				},function(err) {
					assert( err !== undefined );
					assert( err.message === 'Empty DB' );
					var filesInTemp = fs.readdirSync(defaultOpts.basePath);
					assert.isTrue( filesInTemp.length === 0 );
					done();
				});
			assert( prom !== undefined );
		});

		it('fetchLogos must handle invalid canales_virtuales field!', function (done) {
			tvdutil.fetchURLAsync = function( opts, cb ) {
				var data = JSON.parse( logosContent );
				delete data[0].canales_virtuales;
				fs.writeFileSync( opts.fileOutput, JSON.stringify(data, null, 4) );
				return bPromise.resolve();
			};
			var prom = mod.fetchLogos( _.cloneDeep(defaultOpts) )
				.then(function(obj) {
					assert( obj === undefined );
				},function(err) {
					assert( err !== undefined );
					done();
				});
			assert( prom !== undefined );
		});

		it('fetchLogos must handle invalid numero field!', function (done) {
			tvdutil.fetchURLAsync = function( opts, cb ) {
				var data = JSON.parse( logosContent );
				data[0][0].numero = 'pepe';
				fs.writeFileSync( opts.fileOutput, JSON.stringify(data, null, 4) );
				return bPromise.resolve();
			};
			var prom = mod.fetchLogos( _.cloneDeep(defaultOpts) )
				.then(function(obj) {
					assert( obj === undefined );
				},function(err) {
					assert( err !== undefined );
					done();
				});
			assert( prom !== undefined );
		});

		it('fetchLogos must download all images present!', function (done) {
			tvdutil.fetchURLAsync = function( opts, cb ) {
				if (opts.url === 'pepe') {
					return bPromise.reject( new Error( 'Pepe no existe' ) );
				}
				else {
					var data = JSON.parse( logosContent );
					data.canales[0].nombre_imagen = 'pepe';
					fs.writeFileSync( opts.fileOutput, JSON.stringify(data, null, 4) );
					return bPromise.resolve();
				}
			};
			var prom = mod.fetchLogos( _.cloneDeep(defaultOpts) )
				.then(function(obj) {
					assert( obj === undefined );
				},function(err) {
					assert( err !== undefined );
					assert( err.message === 'Pepe no existe' );
					var filesInTemp = fs.readdirSync(defaultOpts.basePath);
					assert.isTrue( filesInTemp.length === 0 );
					done();
				});
			assert( prom !== undefined );
		});

		it('fetchLogos must try fetch json from url and handle error', function (done) {
			tvdutil.fetchURLAsync = function( opts ) {
				return bPromise.reject(new Error('Alguno'));
			};
			var prom = mod.fetchLogos( _.cloneDeep(defaultOpts) )
				.then(function(obj) {
					assert( obj === undefined );
				},function(err) {
					assert( err !== undefined );
					assert( err.message === 'Alguno' );
					var filesInTemp = fs.readdirSync(defaultOpts.basePath);
					assert.isTrue( filesInTemp.length === 0 );
					done();
				});
			assert( prom !== undefined );
		});
	});
});
