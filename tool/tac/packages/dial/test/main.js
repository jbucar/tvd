'use strict';

var http = require('http');
var Url = require ('url');
var tvdutil = require('tvdutil');
var util = require('util');

var Mocks = require('mocks');
var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;

var bPromise = require('bluebird');
var Dial = require('../src/main');

describe('DialService', function() {
	var _reg = null;
	var _adapter = null;

	function RemoteMock() {
		var self = {};
		var modules = {};
		self.addModule = function(mod) {
			modules[mod.name] = mod;
			return bPromise.resolve();
		};
		self.rmModule = function(name) {
			delete modules[name];
			return bPromise.resolve();
		};
		return self;
	}

	beforeEach(function() {
		_reg = Mocks.init('silly');
		_reg.put( 'remoteapi', new RemoteMock() );
		_adapter = new Mocks.ServiceAdapter( _reg );
	});

	afterEach(function() {
		_adapter = null;
		_reg = null;
	});

	describe('Service', function() {
		it('should start/stop/start', function(done) {
			var srv = new Dial(_adapter);

			srv.onStart(function(err,api) {
				assert.equal( err );
				assert.isObject( api );

				srv.onStop(function() {
					srv.onStart(function(err,api) {
						assert.equal( err );
						assert.isObject( api );
						srv.onStop(done);
					});
				});
			});
		});
	});
});

