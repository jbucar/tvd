'use strict';

var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;
var Mocks = require('mocks');
var _ = require("lodash");
var tvdutil = require('tvdutil');
var RequestInterval = require('../src/requestinterval').create;

function postResponse(cb,data,st,err) {
	if (!st) {
		if (!data) {
			st = 404;
		}
		else {
			st = 200;
		}
	}
	cb( err, {
		"statusCode": st,
		"data": data
	});
}

function basicOptions() {
	return {
		"url": "http://pepe.com",
		"retryTimeout": 10,
		"checkTimeout": 10,
		"method": "POST"
	};
}

describe('RequestInterval', function() {
	var oldRequest = null;

	beforeEach(function() {
		Mocks.init('silly');
		oldRequest = tvdutil.doRequest;
	});

	afterEach(function() {
		tvdutil.doRequest = oldRequest;
		Mocks.fin();
	});

	it('should create request interval using new', function() {
		var ri = new RequestInterval();
		assert.isObject( ri );
	});

	it('should create request interval withouy using new', function() {
		var ri = RequestInterval();
		assert.isObject( ri );
	});

	it('should do basic request interval', function(done) {
		var ri = new RequestInterval();
		assert.isObject( ri );
		var options = _.cloneDeep(basicOptions());

		tvdutil.doRequest = function( params, callback ) {
			assert.isTrue( params.url === options.url );
			assert.isTrue( params.data['pepe'] === "otro pepe" );
			assert.isFunction( callback );
			postResponse(callback,{},200);
		}

		var getRequest = function() {
			return { "pepe": "otro pepe" };
		}

		var onResponse = function(err,data) {
			assert.isObject( data );
			ri.stop();
			done();
		}

		ri.start( options, getRequest, onResponse );
	});

	it('should do multiple requests interval', function(done) {
		var ri = new RequestInterval();
		assert.isObject( ri );
		var options = _.cloneDeep(basicOptions());
		var count = 0;

		tvdutil.doRequest = function( params, callback ) {
			assert.isTrue( params.url === options.url );
			assert.isTrue( params.data['pepe'] === "otro pepe" );
			assert.isFunction( callback );
			postResponse(callback,{},200);
		}

		var getRequest = function() {
			return { "pepe": "otro pepe" };
		}

		var onResponse = function(err,data) {
			assert.isObject( data );
			count++;
			if (count === 5) {
				ri.stop();
				done();
			} else {
				return true;
			}
		}

		ri.start( options, getRequest, onResponse );
	});

	it('should do multiple requests retries', function(done) {
		var ri = new RequestInterval();
		assert.isObject( ri );
		var options = _.cloneDeep(basicOptions());
		var count = 0;

		tvdutil.doRequest = function( params, callback ) {
			assert.equal( params.url, options.url );
			assert.isTrue( params.data['pepe'] === "otro pepe" );
			assert.isFunction( callback );
			count++;
			var code = 404;
			if (count === 3) {
				code = 200;
			}
			postResponse(callback,{},code);
		}

		var getRequest = function() {
			return { "pepe": "otro pepe" };
		}

		var onResponse = function(err,data) {
			if (err === undefined) {
				assert.isObject( data );
				ri.stop();
				done();
			}
		}

		ri.start( options, getRequest, onResponse );
	});

	it('should do multiple requests retries with errors', function(done) {
		var ri = new RequestInterval();
		assert.isObject( ri );
		var options = _.cloneDeep(basicOptions());
		var count = 0;

		tvdutil.doRequest = function( params, callback ) {
			assert.isTrue( params.url === options.url );
			assert.isTrue( params.data['pepe'] === "otro pepe" );
			assert.isFunction( callback );
			count++;
			if (count === 3) {
				postResponse(callback,{},200);
			} else {
				postResponse(callback,{},200, new Error( "An error" ));
			}
		}

		var getRequest = function() {
			return { "pepe": "otro pepe" };
		}

		var onResponse = function(err,data) {
			if (err === undefined) {
				assert.isObject( data );
				ri.stop();
				done();
			}
		}

		ri.start( options, getRequest, onResponse );
	});

	it('should cancel request when stop', function(done) {
		var ri = new RequestInterval();
		assert.isObject( ri );
		var options = _.cloneDeep(basicOptions());
		var count = 0;

		tvdutil.doRequest = function( params, callback ) {
			count++;
			assert.equal( count, 1, "Shouldn't reach this point more than once" );
			assert.isTrue( params.url === options.url );
			assert.isTrue( params.data['pepe'] === "otro pepe" );
			assert.isFunction( callback );
			postResponse(callback,{},200);
		}

		var getRequest = function() {
			return { "pepe": "otro pepe" };
		}

		var onResponse = function(err,data) {
			assert.isObject( data );
			ri.stop();
			return true;
		}

		ri.start( options, getRequest, onResponse );

		setTimeout(function() {
			done();
		},100);
	});

	it('shouldnt restart when config after stop', function(done) {
		var ri = new RequestInterval();
		assert.isObject( ri );
		var options = _.cloneDeep(basicOptions());
		var count = 0;

		tvdutil.doRequest = function( params, callback ) {
			count++;
			assert.equal( count, 1, "Shouldn't reach this point more than once" );
			assert.isTrue( params.url === options.url );
			assert.isTrue( params.data['pepe'] === "otro pepe" );
			assert.isFunction( callback );
			postResponse(callback,{},200);
		}

		var getRequest = function() {
			return { "pepe": "otro pepe" };
		}

		var onResponse = function(err,data) {
			assert.isObject( data );
			ri.stop();
			ri.config( options );
		}

		ri.start( options, getRequest, onResponse );

		setTimeout(function() {
			done();
		},100);
	});

	it('should ignore server response if stopped', function(done) {
		var ri = new RequestInterval();
		assert.isObject( ri );
		var options = _.cloneDeep(basicOptions());
		var count = 0;

		tvdutil.doRequest = function( params, callback ) {
			count++;
			assert.equal( count, 1, "Shouldn't reach this point more than once" );
			assert.isTrue( params.url === options.url );
			assert.isTrue( params.data['pepe'] === "otro pepe" );
			assert.isFunction( callback );
			ri.stop();
			postResponse(callback,{},200);
		}

		var getRequest = function() {
			return { "pepe": "otro pepe" };
		}

		var responses=0;
		var onResponse = function(err,data) {
			responses++;
		}

		ri.start( options, getRequest, onResponse );

		setTimeout(function() {
			assert.equal( responses, 0 );
			done();
		},100);
	});

	it('shouldnt stop when config changed url', function() {
		var ri = new RequestInterval();
		assert.isObject( ri );
		ri.stop();
	});

	it('should update when reconfig', function(done) {
		var ri = new RequestInterval();
		assert.isObject( ri );
		var options = _.cloneDeep(basicOptions());
		var count = 0;

		tvdutil.doRequest = function( params, callback ) {
			assert.isTrue( params.url === options.url );
			assert.isTrue( params.data['pepe'] === "otro pepe" );
			assert.isFunction( callback );
			postResponse(callback,{},200);
		}

		var getRequest = function() {
			return { "pepe": "otro pepe" };
		}

		var onResponse = function(err,data) {
			assert.isObject( data );
			count++;
			if (count === 1) {
				options.url = "http://pepe2.com";
				options.retryTimeout = undefined;
				options.checkTimeout = undefined;
				ri.config( options );
				return true;
			} else {
				ri.stop();
				done();
			}
		}

		ri.start( options, getRequest, onResponse );
	});

	it('should request and handle error', function(done) {
		var ri = new RequestInterval();
		assert.isObject( ri );
		var options = _.cloneDeep(basicOptions());

		tvdutil.doRequest = function( params, callback ) {
			assert.isTrue( params.url === options.url );
			assert.isTrue( params.data['pepe'] === "otro pepe" );
			assert.isFunction( callback );
			postResponse(callback);
		}

		var getRequest = function() { return { "pepe": "otro pepe" }; }
		var onResponse = function(err,data) {}
		ri.start( options, getRequest, onResponse );

		setTimeout(function() {
			ri.stop();
			done();
		},10);
	});
});
