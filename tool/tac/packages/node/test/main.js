'use strict';

var assert = require('assert');

describe('Node', function() {

	it('check node_modules installation', function(done) {
		// generic node modules in $DEPOT/tool/tac/node_modules
		assert( require('istanbul') );
		assert( require('bluebird') );
		assert( require('chai') );
		assert( require('jshint') );
		assert( require('lodash') );
		assert( require('mktemp') );
		assert( require('mocha') );
		assert( require('mock-fs') );
		assert( require('nock') );
		assert( require('npmlog') );

		// tvd node modules in $DEPOT/tool/tac/packages/node_modules
		assert( require('mocks') );
		assert( require('tvdutil') );
		done();
	});
});
