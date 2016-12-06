'use strict';


var path = require('path');
var fs = require('fs');
var util = require('util');
var _ = require('lodash');
var bPromise = require('bluebird');
var chai = require('chai');
chai.config.includeStack = true;
var assert = chai.assert;
var mockfs = require('mock-fs');
var tvdutil = require('tvdutil');
var Mocks = require('mocks');
var ProviderImpl = require('../src/providerimpl');

describe('ProviderImpl', function() {
	var reg = null;

	beforeEach(function() {
		reg = Mocks.init('silly');
	});

	afterEach(function() {
		Mocks.fin();
	});


	function createDlg( id, name, root ) {
		if (!id) {
			id = 'dlg1';
		}
		if (!name) {
			name = 'name1';
		}
		if (!root) {
			root = '/mnt';
		}
		return {
			id: id,
			name: name,
			root: root,
			icon: path.join(__dirname,'imageIcon.png'),
			logo: path.join(__dirname,'imageLogo.png'),
			supportedExtensions: function() {
				return ['.jpg'];
			},
			supportDirectories: function() {
				return true;
			},
			map: function(oper) {
				if (oper.stats.isFile()) {
					oper.info.type = 'image';
					oper.info.url = 'file://' + oper.info.id;
				}
				else {
					oper.info.type = 'album';
				}

				return oper.info;
			}
		};
	}

	describe('constructor', function() {
		it('should construct basic DiskProvider', function() {
			var p = new ProviderImpl( createDlg('dlgID','nameDlg', '/mnt/pepe') );
			assert.equal( p.id, 'dlgID' );
			assert.equal( p.name, 'nameDlg' );
			assert.equal( p.icon, path.join(__dirname,'imageIcon.png') );
			assert.equal( p.logo, path.join(__dirname,'imageLogo.png') );
			assert(p.get);
		});
	});

	describe('get', function() {
		var prov = null;

		function start(dlg) {
			prov = new ProviderImpl( dlg );
		}

		beforeEach(function() {
			var sfs = {
				'/mnt': {}
			};
			mockfs(sfs);
			prov = null;
		});

		afterEach(function() {
			mockfs.restore();
			prov = null;
		});

		it('should return empty', function(done) {
			var dlg = createDlg();
			start( dlg );
			prov.get( {} ).then(function(aValues) {
				assert.equal( aValues.length, 0 );
				done();
			});
		});

		it('should return image1', function(done) {
			fs.writeFileSync( '/mnt/image1.jpg', {} );
			fs.mkdirSync( '/mnt/pepe' );

			start( createDlg() );

			prov.get( {id:'/mnt/image1.jpg'} ).then(function(image) {
				assert.deepEqual( image, {
					provider:'dlg1',
					id: '/mnt/image1.jpg',
					name: 'image1.jpg',
					type: 'image',
					url: 'file:///mnt/image1.jpg'
				});

				done();
			});
		});

		it('should ignore file.wav', function(done) {
			var oldStat = fs.stat;
			fs.stat = function(name,cb) {
				oldStat( name, function(err,stats) {
					if (name === '/mnt/fifo') {
						stats.isDirectory = function() {
							return false;
						};
					}
					cb(err,stats);
				});
			};

			fs.writeFileSync( '/mnt/file.wav', {} );

			start( createDlg() );

			prov.get( {id:'/mnt/file.wav'} ).catch(function(err) {
				assert.equal(err.message,'Invalid entry');
				fs.stat = oldStat;
				done();
			});
		});

		it('should ignore file.wav', function(done) {
			var oldStat = fs.stat;
			fs.stat = function(name,cb) {
				cb( new Error('Algo') );
			};

			fs.writeFileSync( '/mnt/file.wav', {} );

			start( createDlg() );

			prov.get( {} ).then(function(aImages) {
				assert.equal( aImages.length, 0 );
				fs.stat = oldStat;
				done();
			});
		});

		it('should no found image1', function(done) {
			fs.writeFileSync( '/mnt/image2.jpg', {} );
			fs.mkdirSync( '/mnt/pepe' );

			start( createDlg() );

			prov.get( {id:'/mnt/image1.jpg'} ).catch(function(err) {
				assert(err);
				assert.equal(err.message,'Not found');
				done();
			});
		});

		it('should return two images (sorted)', function(done) {
			fs.writeFileSync( '/mnt/image2.jpg', {} );
			fs.writeFileSync( '/mnt/image1.jpg', {} );

			start( createDlg() );

			prov.get( {} ).then(function(aImages) {
				assert.equal( aImages.length, 2 );

				assert.deepEqual( aImages[0], {
					provider:'dlg1',
					id: '/mnt/image1.jpg',
					name: 'image1.jpg',
					type: 'image',
					url: 'file:///mnt/image1.jpg'
				});

				assert.deepEqual( aImages[1], {
					provider:'dlg1',
					id: '/mnt/image2.jpg',
					name: 'image2.jpg',
					type: 'image',
					url: 'file:///mnt/image2.jpg'
				});

				done();
			});
		});

		it('should return two images (sorted)', function(done) {
			fs.writeFileSync( '/mnt/image2.jpg', {} );
			fs.writeFileSync( '/mnt/image1.jpg', {} );
			start( createDlg() );
			prov.get( {} ).then(function(aImages) {
				assert.equal( aImages.length, 2 );

				assert.deepEqual( aImages[0], {
					provider:'dlg1',
					id: '/mnt/image1.jpg',
					name: 'image1.jpg',
					type: 'image',
					url: 'file:///mnt/image1.jpg'
				});

				assert.deepEqual( aImages[1], {
					provider:'dlg1',
					id: '/mnt/image2.jpg',
					name: 'image2.jpg',
					type: 'image',
					url: 'file:///mnt/image2.jpg'
				});

				done();
			});
		});

		it('should ignore hidden entries', function(done) {
			fs.writeFileSync( '/mnt/.image2.jpg', {} );
			fs.mkdirSync( '/mnt/.pepe' );
			start( createDlg() );
			prov.get( {} ).then(function(aImages) {
				assert.equal( aImages.length, 0 );
				done();
			});
		});

		it('should returns directories if necesary', function(done) {
			fs.writeFileSync( '/mnt/image1.jpg', {} );
			fs.mkdirSync( '/mnt/album_1' );
			fs.writeFileSync( '/mnt/album_1/image1.jpg', {} );
			fs.writeFileSync( '/mnt/album_1/image2.jpg', {} );
			fs.mkdirSync( '/mnt/album_1/album1_1' );

			start( createDlg() );

			prov.get( {} ).then(function(aImages) {
				assert.equal( aImages.length, 2 );

				assert.deepEqual( aImages[0], {
					provider:'dlg1',
					id: '/mnt/album_1',
					name: 'album_1',
					type: 'album'
				});

				assert.deepEqual( aImages[1], {
					provider:'dlg1',
					id: '/mnt/image1.jpg',
					name: 'image1.jpg',
					type: 'image',
					url: 'file:///mnt/image1.jpg'
				});

				prov.get( {root:'album_1'} ).then(function(aImages) {
					assert.equal( aImages.length, 3 );

					assert.deepEqual( aImages[0], {
						provider:'dlg1',
						id: '/mnt/album_1/album1_1',
						name: 'album1_1',
						type: 'album'
					});

					assert.deepEqual( aImages[1], {
						provider:'dlg1',
						id: '/mnt/album_1/image1.jpg',
						name: 'image1.jpg',
						type: 'image',
						url: 'file:///mnt/album_1/image1.jpg'
					});

					assert.deepEqual( aImages[2], {
						provider:'dlg1',
						id: '/mnt/album_1/image2.jpg',
						name: 'image2.jpg',
						type: 'image',
						url: 'file:///mnt/album_1/image2.jpg'
					});

					done();
				});
			});
		});

		it('should ignore unsuported file types', function(done) {
			var oldStat = fs.stat;
			fs.stat = function(name,cb) {
				oldStat( name, function(err,stats) {
					if (name === '/mnt/fifo') {
						stats.isDirectory = function() {
							return false;
						};
					}
					cb(err,stats);
				});
			};

			fs.writeFileSync( '/mnt/image1.jpg', {} );
			fs.mkdirSync( '/mnt/pepe' );
			fs.mkdirSync( '/mnt/fifo' );

			start( createDlg() );

			prov.get( {} ).then(function(aImages) {
				assert.equal( aImages.length, 2 );

				assert.deepEqual( aImages[0], {
					provider:'dlg1',
					id: '/mnt/pepe',
					name: 'pepe',
					type: 'album'
				});

				assert.deepEqual( aImages[1], {
					provider:'dlg1',
					id: '/mnt/image1.jpg',
					name: 'image1.jpg',
					type: 'image',
					url: 'file:///mnt/image1.jpg'
				});

				fs.stat = oldStat;
				done();
			});
		});

		it('should ignore unknown file type', function(done) {
			fs.writeFileSync( '/mnt/music', {} );
			fs.writeFileSync( '/mnt/music.wav', {} );
			fs.writeFileSync( '/mnt/image1.jpg', {} );
			fs.mkdirSync( '/mnt/pepe' );

			start( createDlg() );

			prov.get( {} ).then(function(aImages) {
				assert.equal( aImages.length, 2 );

				assert.deepEqual( aImages[1], {
					provider:'dlg1',
					id: '/mnt/image1.jpg',
					name: 'image1.jpg',
					type: 'image',
					url: 'file:///mnt/image1.jpg'
				});

				assert.deepEqual( aImages[0], {
					provider:'dlg1',
					id: '/mnt/pepe',
					name: 'pepe',
					type: 'album'
				});

				done();
			});
		});

		it('should test pagination', function(done) {
			function writeImage(p) {
				fs.writeFileSync( p, {} );
			}

			function imgName( x ) {
				return 'image' + x + '.jpg';
			}

			function cmpImage( img, x ) {
				var cmp = {
					provider:'dlg1',
					id: '/mnt/' + imgName(x),
					name: imgName(x),
					type: 'image',
					url: 'file:///mnt/' + imgName(x)
				};
				assert.deepEqual( img, cmp, util.format('cmpImage error: x=', x ) );
			}

			writeImage( '/mnt/otra.wav' );
			fs.mkdirSync( '/mnt/pepe1' );
			for (var i=0; i<25; i++) {
				writeImage( '/mnt/' + 'otra' + i + '.wav' );
				writeImage( '/mnt/.' + imgName(i) );
				writeImage( '/mnt/' + imgName(i) );
			}
			fs.mkdirSync( '/mnt/pepe2' );

			start( createDlg() );

			prov.get( {page:0,perPage:10} ).then(function(aImages) {
				assert.equal( aImages.length, 10 );

				assert.deepEqual( aImages[0], {
					provider:'dlg1',
					id: '/mnt/pepe1',
					name: 'pepe1',
					type: 'album'
				});

				assert.deepEqual( aImages[1], {
					provider:'dlg1',
					id: '/mnt/pepe2',
					name: 'pepe2',
					type: 'album'
				});

				cmpImage( aImages[2], 0 );
				cmpImage( aImages[3], 1 );
				cmpImage( aImages[4], 2 );
				cmpImage( aImages[5], 3 );
				cmpImage( aImages[6], 4 );
				cmpImage( aImages[7], 5 );
				cmpImage( aImages[8], 6 );
				cmpImage( aImages[9], 7 );

				prov.get( {page:1,perPage:10} ).then(function(aImages) {
					assert.equal( aImages.length, 10 );

					cmpImage( aImages[0], 8 );
					cmpImage( aImages[1], 9 );
					cmpImage( aImages[2], 10 );
					cmpImage( aImages[3], 11 );
					cmpImage( aImages[4], 12 );
					cmpImage( aImages[5], 13 );
					cmpImage( aImages[6], 14 );
					cmpImage( aImages[7], 15 );
					cmpImage( aImages[8], 16 );
					cmpImage( aImages[9], 17 );

					prov.get( {page:2,perPage:10} ).then(function(aImages) {
						assert.equal( aImages.length, 7 );

						cmpImage( aImages[0], 18 );
						cmpImage( aImages[1], 19 );
						cmpImage( aImages[2], 20 );
						cmpImage( aImages[3], 21 );
						cmpImage( aImages[4], 22 );
						cmpImage( aImages[5], 23 );
						cmpImage( aImages[6], 24 );

						done();
					});
				});
			});
		});
	});
});
