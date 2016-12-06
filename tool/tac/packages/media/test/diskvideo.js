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
var aux = require('./aux');
var Mocks = require('mocks');
var DiskVideo = require('../src/disk/video');

var pulpMeta = JSON.parse(fs.readFileSync(path.join(__dirname,'pulp.json')));

describe('DiskVideo', function() {
	beforeEach(function() {
		Mocks.init('silly');
	});

	afterEach(function() {
		Mocks.fin();
	});

	describe('constructor', function() {
		it( 'should construct basic disk video delegate', function() {
			var del = new DiskVideo( 'video1', 'DiskVideo1', 'mount', 'movie' );
			assert.equal( del.id, 'video1' );
			assert.equal( del.name, 'DiskVideo1' );
			assert( del.icon );
			assert( del.logo );
			assert.equal( del.root, 'mount' );
		});
	});

	describe('methods', function() {
		var del;

		beforeEach(function() {
			var sfs = {
				'/mnt': {}
			};
			mockfs(sfs);
			del = new DiskVideo( 'video1', 'DiskVideo1', '/mnt', 'video' );
		});

		afterEach(function() {
			del = null;
			mockfs.restore();
		});

		describe('filter', function() {
			it('should validate videos', function() {
				assert.deepEqual( del.supportedExtensions(), ['.avi', '.mp4', '.mkv', '.url'] );
			});
		});

		describe('map', function() {
			it('should map a video without opts to obligatory fields', function() {
				var info = aux.createInfo(del.root,'video.avi');
				var stats = aux.createStats(true);
				var opts = {};
				var res = del.map(aux.createOper(info,del.root,stats,opts));
				assert.equal( Object.keys(res).length, 5 );
				assert.equal( res.provider, 'disk1' );
				assert.equal( res.id, path.join('/mnt','video.avi') );
				assert.equal( res.name, 'video.avi' );
				assert.equal( res.type, 'video' );
				assert.equal( res.url, 'file://' + path.join('/mnt','video.avi') );
			});

			it('should map a video with opts + obligatories fields', function(done) {
				var info = aux.createInfo( del.root, 'video.avi');
				var stats = aux.createStats(true);
				var opts = {fields:['size']};
				aux.callMap(del,opts,info,stats)
					.then(function(res) {
						assert.equal( Object.keys(res).length, 6 );
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join('/mnt','video.avi') );
						assert.equal( res.name, 'video.avi' );
						assert.equal( res.type, 'video' );
						assert.equal( res.url, 'file://' + path.join('/mnt','video.avi') );
						assert.equal( res.size, 15 );
						done();
					});
			});

			it('should map a video with opts + meta info', function(done) {
				var info = aux.createInfo( del.root, 'video.avi');
				var stats = aux.createStats(true);
				var opts = {fields:['title','year','runtime','genre', 'director', 'writer', 'actors', 'plot', 'language', 'country', 'poster']};
				fs.writeFileSync( path.join('/mnt','meta.json'), JSON.stringify(pulpMeta, null, 4) );
				aux.callMap(del,opts,info,stats)
					.then(function(res) {
						assert.equal( Object.keys(res).length, 17 );
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join('/mnt','video.avi') );
						assert.equal( res.name, 'video.avi' );
						assert.equal( res.type, 'video' );
						assert.equal( res.url, 'file://' + path.join('/mnt','video.avi') );
						assert.equal( res.title, pulpMeta.title );
						assert.equal( res.year, pulpMeta.year );
						assert.equal( res.runtime, pulpMeta.runtime );
						assert.deepEqual( res.subtitle, pulpMeta.subtitle );
						assert.deepEqual( res.genre, pulpMeta.genre );
						assert.deepEqual( res.director, pulpMeta.director );
						assert.deepEqual( res.writer, pulpMeta.writer );
						assert.deepEqual( res.actor, pulpMeta.actor );
						assert.equal( res.plot, pulpMeta.plot );
						assert.deepEqual( res.language, pulpMeta.language );
						assert.equal( res.country, pulpMeta.country );
						assert.equal( res.poster, pulpMeta.poster );
						done();
					});
			});

			it('should map a video with opts + obligatories fields + meta info', function(done) {
				var info = aux.createInfo( del.root, 'video.avi');
				var stats = aux.createStats(true);
				var opts = {fields:['size','title','year','runtime','genre', 'director', 'writer', 'actors', 'plot', 'language', 'country', 'poster']};
				fs.writeFileSync( path.join('/mnt','meta.json'), JSON.stringify(pulpMeta, null, 4) );
				aux.callMap(del,opts,info,stats)
					.then(function(res) {
						assert.equal( Object.keys(res).length, 18 );
						assert.equal( res.provider, 'disk1' );
						assert.equal( res.id, path.join('/mnt','video.avi') );
						assert.equal( res.name, 'video.avi' );
						assert.equal( res.type, 'video' );
						assert.equal( res.url, 'file://' + path.join('/mnt','video.avi') );
						assert.equal( res.size, 15 );
						assert.equal( res.title, pulpMeta.title );
						assert.equal( res.year, pulpMeta.year );
						assert.deepEqual( res.subtitle, pulpMeta.subtitle );
						assert.equal( res.runtime, pulpMeta.runtime );
						assert.deepEqual( res.genre, pulpMeta.genre );
						assert.deepEqual( res.director, pulpMeta.director );
						assert.deepEqual( res.writer, pulpMeta.writer );
						assert.deepEqual( res.actor, pulpMeta.actor );
						assert.equal( res.plot, pulpMeta.plot );
						assert.deepEqual( res.language, pulpMeta.language );
						assert.equal( res.country, pulpMeta.country );
						assert.equal( res.poster, pulpMeta.poster );
						done();
					});
			});
		});
	});
});

