'use strict';

var fs = require('fs');
var path = require('path');
var UrlParser = require('url');
var assert = require('assert');
var util = require('util');
var https = require('https');
var EventEmitter = require('events').EventEmitter;
var Mocks = require('mocks');
var tvdutil = require('tvdutil');
var Audiencia = require('../src/main.js');

function FakeTvd() {
	var self = this || {};
	var _events = new EventEmitter();
	var _current = -1;

	tvdutil.forwardEventEmitter(self,_events);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	self.change = function( chID ) {
		_current = chID;
		_events.emit( 'playerChanged', chID );
	};

	self.current = function() {
		return _current;
	};

	self.get = function(chID) {
		return {
			"id": chID,
			"channel": '10.10',
			"name": 'pepe'
		};
	};

	return self;
}

function CreateConnection(params,reqCallback,cb) {
	var self = {};
	var errorCB = null;
	var _data = null;

	self.on = function(sig,cb) {
		if (sig === 'error') {
			errorCB = cb;
		}
	};

	self.write = function(data) {
		_data = data;
	};

	self.end = function() {
		cb(self);
	};

	self.params = function() {
		params.search = _data;
		return params;
	};

	self.postError = function(err) {
		assert(errorCB);
		errorCB( err );
	};

	self.postResponse = function(st,data) {
		reqCallback({
			"statusCode": st,
			"data": data
		});
	};

	return self;
}

describe('audiencia', function() {
	var _reg = null;
	var _adapter = null;
	var _module = null;
	var _tvd = null;

	beforeEach(function() {
		_reg = Mocks.init('silly');
		_adapter = new Mocks.ServiceAdapter(_reg);
		_module = new Audiencia(_adapter);

		//	Setup registry
		_reg.put( 'screen', new Mocks.Screen() );
		_tvd = new FakeTvd();
		_reg.put( 'tvd', _tvd );
	});

	afterEach(function() {
		Mocks.fin();
		_tvd = null;
		_module = null;
		_adapter = null;
	});

	it('should start/stop', function(done) {
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert( api );
			assert( api.segments );
			_module.onStop(done);
		});
	});

	it('should request ID on start', function(done) {
		var oldRequest = tvdutil.doRequest;
		tvdutil.doRequest = function( opts, cb ) {
			var res = {
				stbID: 10,
				privateKey: "akasdfkasjdfkj"
			};
			cb( undefined, {
				"statusCode": 200,
				"data": res
			});
			_module.onStop(done);
			tvdutil.doRequest = oldRequest;
		};
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert( api );
			assert( api.segments );
		});
	});

	it('should request ID on start and validate', function(done) {
		var oldRequest = tvdutil.doRequest;
		tvdutil.doRequest = function( opts, cb ) {
			var res = {
				stbID: 10,
				pKey: "akasdfkasjdfkj"
			};
			cb( undefined, {
				"statusCode": 200,
				"data": res
			});
			_module.onStop(done);
			tvdutil.doRequest = oldRequest;
		};
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert( api );
			assert( api.segments );
		});
	});

	it('should request ID on start and validate for status 200', function(done) {
		var oldRequest = tvdutil.doRequest;
		tvdutil.doRequest = function( opts, cb ) {
			var res = {
				stbID: 10,
				pKey: "akasdfkasjdfkj"
			};
			cb( undefined, { "statusCode": 201 });
			_module.onStop(done);
			tvdutil.doRequest = oldRequest;
		};
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert( api );
			assert( api.segments );
		});
	});

	it('should request ID on start and check for errors', function(done) {
		var oldRequest = tvdutil.doRequest;
		tvdutil.doRequest = function( opts, cb ) {
			console.log( 'opts=%j', opts );
			var res = {
				stbID: 10,
				pKey: "akasdfkasjdfkj"
			};
			cb( new Error('pepe'), { "statusCode": 200 });
			_module.onStop(done);
			tvdutil.doRequest = oldRequest;
		};
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert( api );
			assert( api.segments );
		});
	});

	it('should start/stop/start/stop', function(done) {
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert( api );
			_module.onStop(function() {
				_module.onStart(function(err,api2) {
					assert.equal(err);
					assert( api !== api2 );
					_module.onStop(done);
				});
			});
		});
	});

	describe('segments', function() {
		var api = null;

		function defaultConfig() {
			return {
				"stbID": "PEPE",
				"privateKey": "-----BEGIN RSA PRIVATE KEY-----\nMIICXAIBAAKBgQCyIQU10TgHkTpVQ5Lw2a5jtKIWeUKBL1Tb0kRQEPmuUZ3var5f\n4BBCVJ1N7AULa4ou14v2X1eLfKMlML5aNmC4dIVmasT43Xiw5pWfAGAmmyyUqEGP\ni+KN/mfcSoeT7rWDY7mQDRMcV1MW/Y3bns1NRn7rBRqOPj5qLUnfj4OdRQIDAQAB\nAoGAYFYjbzu7xbvxhHOLYN8ZxarHjsf+A8gJaOmLMJI9tt1FQVrFoyyXHoiMmrvu\n7oI2taa1d4WQ30uFIHQAPuHx5Gi4dIq9QTfYS6TVbaRpNDLdxvUo//Hq8ANwdObn\ncUokC6MOo3YzULZFlPWvQs++d/sqT56ZoDIE/2SGwTb1DQECQQDoCwNwNMEIHccA\nmNjv/av/cblnWeP83rcDYaD3QCMFFXRR0BRcujwIdZtjVQGHyRSIHMB/QOEBPKdj\nhj9eLx1RAkEAxIUJxTB5jTuvwyM1kZAmkJ8jHLvzXBa/ywiZKq9qxr6cwZRhy/m3\n4OtxSzReyrUBaANE5Lfavl7dgEA38WDztQJABCgeWaY4RwLxi6sxR6tNp3fMEHPl\nx6Pdt2vKTGOTvQWTXdvSYY7cgHJK5E6jhmEtwvpw4FazMSh6/ydqfy9s4QJBAJLh\neJ74jzEfVkv8SfKFEj3vo1e7prXCjxEaWRiNSbpIB3OsCrhg6a1jbCcwelT5mpZ6\nMwjb3dOBpf5QgT+ZHi0CQFPHUFymnzz8SMdnp6M1Bl8oaDQi7FV84xLuxsnixKxl\nhr/OslmvP8TcgAtS1GkTrHDeXJsBDRu2ugFqUycBKew=\n-----END RSA PRIVATE KEY-----\n",
				"url": "https://localhost",
				"sendTimeout": 10*60*1000,
				"minElapsedTime": 1*60*1000
			};
		}

		function start( cfg, cb ) {
			_adapter.load = function( file, defaultsValues ) { return cfg; };
			_module.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert(api);
				cb();
			});
		}

		afterEach(function(done) {
			_module.onStop(done);
		});

		describe( 'sendData', function() {
			it('should send data', function(done) {
				var oldRequest = https.request;

				var cfg = defaultConfig();
				cfg.minElapsedTime = 5;
				cfg.sendTimeout = 1;

				var count=-1;
				function onCreateRequest(conn) {
					count++;
					if (count === 0) {
						conn.postError(new Error('pepe'));
					}
					else if (count === 1) {
						conn.postResponse(202);
					}
					else {
						var url = UrlParser.format( conn.params(), true );
						var oURL = UrlParser.parse(url,true);
						assert.equal( oURL.hostname, 'localhost' );
						assert.equal( oURL.pathname, '/add/' );

						assert.equal( oURL.query.stbID, 'PEPE' );
						assert( oURL.query.signature );
						assert( oURL.query.currentTime );
						assert( oURL.query.data.length > 0 );

						conn.postResponse(200);
						https.request = oldRequest;
						done();
					}
				}

				https.request = function( params, callback ) {
					var conn = new CreateConnection(params,callback,onCreateRequest);
					return conn;
				};

				start( cfg, function() {
					_reg.get( 'screen' ).setActive(true);

					_tvd.change( 10 );
					setTimeout(function() {
						_tvd.change( 11 );
					},10);
				});
			});
		});

		describe( 'channelChanged', function() {
			it('should add segments when change', function(done) {
				var cfg = defaultConfig();
				cfg.minElapsedTime = 1;
				start( cfg, function() {
					_reg.get( 'screen' ).setActive(true);


					_tvd.change( 10 );
					setTimeout(function() {
						_tvd.change( 11 );
						assert.equal(api.segments().length,1);
						assert.equal(api.segments()[0].chID,10);
						done();
					},5);
				});
			});

			it('should skip same channel', function(done) {
				start( defaultConfig(), function() {
					_reg.get( 'screen' ).setActive(true);

					_tvd.change( 10 );
					_tvd.change( 10 );
					assert.equal(api.segments().length,0);
					done();
				});
			});

			it('should skip zapping', function(done) {
				start( defaultConfig(), function() {
					_reg.get( 'screen' ).setActive(true);

					_tvd.change( 10 );
					_tvd.change( 11 );
					assert.equal(api.segments().length,0);
					done();
				});
			});
		});

		describe( 'screenChanged', function() {
			it('should add segments when screen change', function(done) {
				var cfg = defaultConfig();
				cfg.minElapsedTime = 1;
				start( cfg, function() {

					_tvd.change( 10 );
					assert.equal(api.segments().length,0);
					_reg.get( 'screen' ).setActive(true);
					assert.equal(api.segments().length,0);
					setTimeout(function() {
						_reg.get( 'screen' ).setActive(false);
						assert.equal(api.segments().length,1);
						assert.equal(api.segments()[0].chID,10);
						done();
					},5);
				});
			});

			it('should skip same channel', function(done) {
				start( defaultConfig(), function() {
					_tvd.change( 10 );
					_reg.get( 'screen' ).setActive(true);
					_reg.get( 'screen' ).setActive(true);
					assert.equal(api.segments().length,0);
					done();
				});
			});
		});
	});
});
