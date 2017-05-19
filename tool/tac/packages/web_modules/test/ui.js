'use strict';

var path = require('path');
var Component = require('component');

var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;

describe('ui.js', function() {
	var comp = new Component( path.join(__dirname,'../src/components/util/ui.js'), 'UI' );
	var utilComp = new Component( path.join(__dirname,'../src/components/util/util.js'), '$' );
	var UI;

	beforeEach(function(done) {
		global.assert = assert;
		global.window = {
			location: {
				pathname: '/root/index.html'
			}
		};
		utilComp.create()
			.then(function(partial) {
				assert(typeof partial === 'function');
				global.$ = partial;
			})
			.then(() => comp.create())
			.then(function(ctor) {
				assert(ctor);
				UI = ctor;
				done();
			});
	});

	afterEach(function() {
		delete global.assert;
		delete global.window;
		UI = null;
		utilComp.destroy();
		comp.destroy();
	});

	function createResources() {
		return {
			functions: {},
			data: {
				_parent: {},
				_styles: {
					items: 'item_style'
				},
				components: {
					_parent: {},
					comp1: {
						_parent: {},
						_templates: {
							tpl1: 'comp1_tpl1',
							tpl2: 'comp1_tpl2'
						},
						_styles: {
							css1: 'comp1_css1',
							css2: 'comp1_css2'
						},
						_images: {
							img1: 'components/comp1/comp1_img1.png',
							img2: 'components/comp1/comp1_img2.png'
						},
						_strings: {
							'id1': 'comp1_txt1_id1',
							'id2': 'comp1_txt1_id2',
							'id3': 'comp1_txt1_id3',
							'id4': 'comp1_txt1_id4'
						}
					},
					comp2: {
						_parent: {},
						_templates: {
							tpl1: 'comp2_tpl1'
						},
						_styles: {
							css1: 'comp2_css1'
						},
						_images: {
							img1: 'components/comp2/comp2_img1'
						},
						_strings: {
							'id1': 'comp1_txt1_id1',
							'id2': 'comp1_txt1_id2'
						}
					}
				}
			}
		};
	}

	it('should construct', function() {
		var ui = new UI( createResources() );
		assert(ui);
		assert.equal( Object.keys(ui).length, 14 );
	});

	describe( 'getResources', function() {
		it( 'basic', function() {
			var ui = new UI( createResources() );
			assert(ui);
			let res = ui.getResources('components/comp1');
			assert( res );

			//	Templates
			assert.equal( res.tpl('tpl1'), 'comp1_tpl1' );
			assert.equal( res.tpl('tpl2'), 'comp1_tpl2' );
			assert.throw( () => res.tpl('tpl3'), Error, 'Cannot get resource: folder=components/comp1, type=_templates, id=tpl3' );

			//	Styles
			assert.equal( res.style('css1'), 'comp1_css1' );
			assert.equal( res.style('css2'), 'comp1_css2' );
			assert.throw( () => res.style('css3'), Error, 'Cannot get resource: folder=components/comp1, type=_styles, id=css3' );
			assert.equal( res.styles('css1', '../../items'), 'comp1_css1\nitem_style' );

			//	Strings
			assert.equal( res.txt('id1'), 'comp1_txt1_id1' );
			assert.equal( res.txt('id2'), 'comp1_txt1_id2' );
			assert.throw( () => res.txt('txt3'), Error, 'Cannot get resource: folder=components/comp1, type=_strings, id=txt3' );

			//	Images
			assert.equal( res.img('img1'), 'components/comp1/comp1_img1.png' );
			assert.equal( res.img('img2'), 'components/comp1/comp1_img2.png' );
			assert.throw( () => res.img('img3'), Error, 'Cannot get resource: folder=components/comp1, type=_images, id=img3' );
			assert.equal( res.img('img1',true), '/root/components/comp1/comp1_img1.png' );
		});
	});
});
