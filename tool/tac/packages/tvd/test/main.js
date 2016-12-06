'use strict';

var tvdutil = require('tvdutil');
var _ = require('lodash');
var assert = require('chai').assert;
var EventEmitter = require('events').EventEmitter;
var Mocks = require('mocks');

function Channel() {
	var self = {};
	self.toggleFavorite = function() {
	};
	self.toggleBlocked = function() {
		return true;
	};
	self.getShow = function() {
		return {};
	};
	self.getShowsBetween = function() {
		return [];
	};

	self.id = 'pepe';
	self.channel = '12.01';
	self.name = 'pepe';
	self.logo = 'logo';
	self.isMobile = false;
	self.category = 'pepe';
	self.info = 'info';
	self.isProtected = function() {
		return false;
	};

	self.isFavorite = function() {
		return true;
	};

	self.isBlocked = function() {
		return false;
	};

	self.parentalAge = function() {
		return 10;
	};

	self.parentalContent = function() {
		return 'ATP';
	};

	return self;
};

function Channels() {
	var self = {};
	self.count = function() {
		return 0;
	};

	self.getAll = function() {
		return [];
	};

	self.get = function(id) {
		if (id === 10) {
			return new Channel();
		}
		return null;
	};

	self.remove = function() {
	};

	return self;
};

function Session() {
	var self = {};

	self.enable = function() {
	};

	self.isEnabled = function() {
	};

	self.check = function() {
	};

	self.isBlocked = function() {
	};

	self.expire = function() {
	};

	self.getTimeExpiration = function() {
	};

	self.setTimeExpiration = function() {
	};

	self.restrictSex = function() {
	};

	self.isSexRestricted = function() {
	};

	self.restrictViolence = function() {
	};

	self.isViolenceRestricted = function() {
	};

	self.restrictDrugs = function() {
	};

	self.isDrugsRestricted = function() {
	};

	self.restrictAge = function() {
	};

	self.ageRestricted = function() {
	};

	return self;
}

function Player() {
	var self = {};
	self.current = function() {
	};
	self.isProtected = function() {
	};
	self.currentNetworkName = function() {
	};
	self.status = function() {
	};
	self.change = function() {
	};
	self.nextChannel = function() {
	};
	self.nextFavorite = function() {
	};
	self.showMobile = function() {
	};
	self.nextVideo = function() {
	};
	self.videoInfo = function() {
	};
	self.nextAudio = function() {
	};
	self.audioInfo = function() {
	};
	self.nextSubtitle = function() {
	};
	self.subtitleInfo = function() {
	};
	return self;
}

function TvdMock() {
	var self = {};
	var _channels = new Channels();
	var _player = new Player();
	var _session = new Session();
	var _events = new EventEmitter();

	self.load = function(cfg) {
		return true;
	};

	self.start = function() {
		return true;
	};

	self.stop = function() {
	};

	self.defaultPlayer = function() {
		return _player;
	};

	self.channels = function() {
		return _channels;
	};

	self.session = function() {
		return _session;
	};

	tvdutil.forwardEventEmitter(self,_events);

	//	Middleware
	self.runApplication	= function(appID) {
	};

	self.enableMiddleware = function(st) {
	};

	self.disableApplications = function(st) {
	};

	//	Scan
	self.isScanning = function() {
		return false;
	};

	self.startScan = function() {
		_events.emit( 'scanChanged', 'begin' );
		_events.emit( 'scanChanged', 'network', '152244', 0 );
		_events.emit( 'scanChanged', 'end' );
	};

	self.cancelScan = function() {
	};

	self.addLogoProvider = function(p) {
	};

	//console.log( 'setupTvd=%s', setupTvd );
	if (setupTvd) {
		setupTvd(self);
	}

	return self;
}

var setupTvd = null;

describe('TvdModule', function() {
	var _reg = null;
	var _adapter = null;
	var _module = null;

	var _cfg = {
		data_path: '/tmp/tvdmodule/data',
		install_path: '/tmp/tvdmodule/install_path',
		temporary_path: '/tmp/tvdmodule/tmp',
		provider: "dummy"
	};

	beforeEach( function() {
		_reg = Mocks.init('silly');
		_adapter = new Mocks.ServiceAdapter(_reg);
		_adapter.load = function( file, defaults ) {
			var tmp = _.cloneDeep(_cfg);
			_.defaults(tmp, defaults);
			return tmp;
		};
		_reg.put( 'screen', new Mocks.Screen() );
		assert(_adapter);
		_module = Mocks.mockRequire( require, '../src/main', { './tvd': TvdMock } )(_adapter);
		assert( _module );
		setupTvd = null;
	});

	afterEach( function () {
		_module = null;
		Mocks.fin();
		_adapter = null;
		_reg = null;
	});

	it('should start/stop basic', function(done) {
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert.isObject(api);
			_reg.put( 'tvd', api );
			_module.onStop(done);
		});
	});

	it('should convert old config', function(done) {
		_adapter.load = function( file, defaultsValues, convert ) {
			assert( convert );
			var cfg = _.cloneDeep(defaultsValues);
			var newCfg = convert(cfg);
			assert.deepEqual( cfg, newCfg );
			return cfg;
		};

		_module.onStart(function(err,api) {
			assert.equal(err);
			assert.isObject(api);
			_module.onStop(done);
		});
	});

	it('should check load error', function(done) {
		var lib = null;
		setupTvd = function(tvd) {
			tvd.load = function() {
				return false;
			};
			lib = tvd;
		}
		_module.onStart(function(err,api) {
			assert(lib);
			assert.equal(err.message,'Cannot load tvd configuration');
			done();
		});
	});

	it('should check start error', function(done) {
		var lib = null;
		setupTvd = function(tvd) {
			tvd.start = function() {
				return false;
			};
			lib = tvd;
		}
		_module.onStart(function(err,api) {
			assert(lib);
			assert.equal(err.message,'Cannot start tvd module');
			done();
		});
	});

	it('should stop current channel on stop', function(done) {
		var cur = -1;
		setupTvd = function(tvd) {
			var p = tvd.defaultPlayer();
			p.change = function(id) {
				cur = id;
			};
		}

		_module.onStart(function(err,api) {
			assert.equal(err);
			api.player.change( 10 );
			assert.equal( cur, 10 );
			_module.onStop(function(err) {
				assert.equal(err);
				assert.equal(cur,-1);
				done();
			});
		});
	});

	it('should reset current when screen active change', function(done) {
		var cur = -1;
		setupTvd = function(tvd) {
			var p = tvd.defaultPlayer();
			p.change = function(id) {
				cur = id;
			};
		}
		_reg.get( 'screen' ).setActive(true);

		_module.onStart(function(err,api) {
			assert.equal(err);
			api.player.change( 10 );
			assert.equal( cur, 10 );
			_reg.get( 'screen' ).setActive(false);
			assert.equal( cur, -1 );
			_reg.get( 'screen' ).setActive(true);
			assert.equal( cur, -1 );
			_module.onStop(function(err) {
				assert.equal(err);
				assert.equal(cur,-1);
				done();
			});
		});
	});

	it('should add provider and get logos from it', function(done) {
		var fncProvider = null;
		setupTvd = function(tvd) {
			tvd.addLogoProvider = function(fnc) {
				fncProvider = fnc;

				//	Test before api
				fncProvider( {} );
			};
		}
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert.isObject(api);

			var findLogo = function(ch) {
				return { logo: 'pepe.img', priority: 10 };
			};
			api.addLogoProvider(findLogo);

			//	Find empty channel
			var res = fncProvider( {} );
			assert.isObject( res );
			assert.isTrue( res.logo === 'pepe.img' );
			assert.isTrue( res.priority === 10 );

			api.removeLogoProvider(findLogo);

			//	Find empty channel again
			var res = fncProvider( {} );
			assert.isNull( res );

			_module.onStop(done);
		});
	});

	describe('channels signals', function() {
		var api = null;
		var channels = null;

		beforeEach(function(done) {
			setupTvd = function(tvd) {
				channels = tvd.channels();
			}
			_module.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert.isObject(api);
				assert(channels);
				assert(channels.emit);
				done();
			});
		});

		afterEach(function (done) {
			_module.onStop(done);
		});

		it('should test channelAdded signal', function() {
			var signalCalled = -1;
			api.on( 'channelAdded', function(chID) {
				signalCalled++;
				if (signalCalled === 0) {
					assert.equal( chID, 10 );
				}
				else if (signalCalled === 1) {
					assert.equal( chID, 11 );
				}
				else {
					assert(false);
				}
			});

			//	test channelAdded( chID )
			channels.emit( 'channelAdded', 10 );
			channels.emit( 'channelAdded', 11 );
			assert.equal( signalCalled, 1 );
		});

		it('should test channedlRemoved signal', function() {
			var signalCalled = -1;
			api.on( 'channedlRemoved', function(chID) {
				signalCalled++;
				if (signalCalled === 0) {
					assert.equal( chID, 10 );
				}
				else if (signalCalled === 1) {
					assert.equal( chID, 11 );
				}
				else {
					assert(false);
				}
			});

			//	test channedlRemoved( chID )
			channels.emit( 'channedlRemoved', 10 );
			channels.emit( 'channedlRemoved', 11 );
			assert.equal( signalCalled, 1 );
		});
	});

	describe('middleware signals', function() {
		var api = null;
		var tvd = null;

		function AppMgrMock() {
			var self = {};
			self.addInstance = function() {
				var app = {};
				app.info = {};
				app.info.user = false;
				return app;
			};
			return self;
		}

		beforeEach(function(done) {
			setupTvd = function(impl) {
				tvd = impl;
			}

			_reg.put( 'appmgr', new AppMgrMock() );
			_module.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert.isObject(api);
				assert(tvd);
				assert(tvd.emit);
				done();
			});
		});

		afterEach(function (done) {
			_module.onStop(done);
		});

		it('should test onMiddleware type=added signal', function() {
			_reg.get( 'appmgr' ).addInstance = function(id,extID) {
				assert.equal(id,'ar.edu.unlp.info.lifia.tvd.ginga');
				assert.equal(extID, 'ar.edu.unlp.info.lifia.tvd.ginga.pepeID');
				var app = {};
				app.info = {};
				app.info.user = false;
				return app;
			};

			var signalCalled = -1;
			api.on( 'middleware', function(evt) {
				signalCalled++;
				assert(evt);
				assert.equal(evt.type,'added');
				if (signalCalled === 0) {
					assert.equal( evt.app.name, 'pepe' );
					assert.equal( evt.app.autoStart, true );
					assert.equal( evt.app.language, 'ARG' );
				}
				else {
					assert(false);
				}
			});

			//	test added( isEnabled )
			var evt = {
				type: 'added',
				id: 'pepeID',
				app: {
					name: 'pepe',
					autoStart: true,
					language: 'ARG'
				}
			};
			tvd.emit( 'onMiddleware', evt  );
			assert.equal( signalCalled, 0 );
		});

		it('should test onMiddleware type=removed signal', function() {
			var rmInstanceCalled=false;
			_reg.get( 'appmgr' ).rmInstance = function(id,cb) {
				assert.equal( id, 'ar.edu.unlp.info.lifia.tvd.ginga.pepeID' );
				rmInstanceCalled=true;
				cb();
			};

			var signalCalled = -1;
			api.on( 'middleware', function(evt) {
				signalCalled++;
				assert(evt);
				assert.equal(evt.type,'removed');
				if (signalCalled === 0) {
					assert.equal( evt.id, 'pepeID' );
				}
				else {
					assert(false);
				}
			});

			//	test added( isEnabled )
			var evt = {
				type: 'removed',
				id: 'pepeID'
			};
			tvd.emit( 'onMiddleware', evt  );
			assert.equal( signalCalled, 0 );
			assert(rmInstanceCalled);
		});

		it('should test onMiddleware type=download signal', function() {
			var signalCalled = -1;
			api.on( 'middleware', function(evt) {
				signalCalled++;
				assert(evt);
				assert.equal(evt.type,'download');
				if (signalCalled === 0) {
					assert.equal( evt.id, 'pepeID' );
					assert.equal( evt.progress.step, 10 );
					assert.equal( evt.progress.total, 100 );
				}
				else {
					assert(false);
				}
			});

			var evt = {
				type: 'download',
				id: 'pepeID',
				progress: {
					step: 10,
					total: 100
				}
			};
			tvd.emit( 'onMiddleware', evt  );
			assert.equal( signalCalled, 0 );
		});

		it('should test onMiddleware type=start signal', function() {
			var methodCalled=false;
			_reg.get( 'appmgr' ).runAsync = function(opts,cb) {
				assert.equal( opts.id, 'ar.edu.unlp.info.lifia.tvd.ginga.pepeID' );
				assert.equal( opts.params, 'param1=10&param2=pepe' );
				cb();
				methodCalled=true;
			};

			var signalCalled = -1;
			api.on( 'middleware', function(evt) {
				signalCalled++;
				assert(evt);
				assert.equal(evt.type,'start');
				if (signalCalled === 0) {
					assert.equal( evt.id, 'pepeID' );
					assert.equal( evt.parameters.length, 2 );
				}
				else {
					assert(false);
				}
			});

			var evt = {
				type: 'start',
				id: 'pepeID',
				parameters: ['param1=10','param2=pepe']
			};
			tvd.emit( 'onMiddleware', evt  );
			assert.equal( signalCalled, 0 );
			assert.equal( methodCalled, true );
		});

		it('should test onMiddleware type=stop signal', function() {
			var methodCalled=false;
			_reg.get( 'appmgr' ).stop = function(id,cb) {
				assert(cb);
				assert.equal( id, 'ar.edu.unlp.info.lifia.tvd.ginga.pepeID' );
				cb();
				methodCalled=true;
			};

			var signalCalled = -1;
			api.on( 'middleware', function(evt) {
				signalCalled++;
				assert(evt);
				assert.equal(evt.type,'stop');
				if (signalCalled === 0) {
					assert.equal( evt.id, 'pepeID' );
					assert.equal( evt.kill, true );
				}
				else {
					assert(false);
				}
			});

			var evt = {
				type: 'stop',
				id: 'pepeID',
				kill: true
			};
			tvd.emit( 'onMiddleware', evt  );
			assert.equal( signalCalled, 0 );
			assert.equal( methodCalled, true );
		});
	});

	describe('session signals', function() {
		var api = null;
		var session = null;

		beforeEach(function(done) {
			setupTvd = function(tvd) {
				session = tvd.session();
			}
			_module.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert.isObject(api);
				assert(session);
				assert(session.emit);
				done();
			});
		});

		afterEach(function (done) {
			_module.onStop(done);
		});

		it('should test sessionEnabled signal', function() {
			var signalCalled = -1;
			api.on( 'sessionEnabled', function(isEnabled) {
				signalCalled++;
				if (signalCalled === 0) {
					assert.equal( isEnabled, true );
				}
				else if (signalCalled === 1) {
					assert.equal( isEnabled, false );
				}
				else {
					assert(false);
				}
			});

			//	test sessionEnabled( isEnabled )
			session.emit( 'sessionEnabled', true );
			session.emit( 'sessionEnabled', false );
			assert.equal( signalCalled, 1 );
		});

		it('should test sessionChanged signal', function() {
			var signalCalled = -1;
			api.on( 'sessionChanged', function(isActive) {
				signalCalled++;
				if (signalCalled === 0) {
					assert.equal( isActive, true );
				}
				else if (signalCalled === 1) {
					assert.equal( isActive, false );
				}
				else {
					assert(false);
				}
			});

			//	test sessionChanged( isActive )
			session.emit( 'sessionChanged', true );
			session.emit( 'sessionChanged', false );
			assert.equal( signalCalled, 1 );
		});
	});

	describe('player signals', function() {
		var api = null;
		var player = null;

		beforeEach(function(done) {
			setupTvd = function(tvd) {
				player = tvd.defaultPlayer();
			}
			_module.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert.isObject(api);
				assert(player);
				done();
			});
		});

		afterEach(function (done) {
			_module.onStop(done);
		});

		it('should test playerMediaChanged signal from player', function() {
			var signalCalled = -1;
			api.on( 'playerMediaChanged', function(url, state) {
				signalCalled++;
				if (signalCalled === 0) {
					assert.equal( url, 'url1' );
					assert.equal( state, true );
				}
				else if (signalCalled === 1) {
					assert.equal( url, 'url2' );
					assert.equal( state, false );
				}
				else {
					assert(false);
				}
			});

			assert(player);
			assert(player.emit);

			//	test playerMediaChanged (url,state)
			player.emit( 'playerMediaChanged', 'url1', true );
			player.emit( 'playerMediaChanged', 'url2', false );
		});

		it('should test playerProtectedChanged signal from player', function() {
			var signalCalled = -1;
			api.on( 'playerProtectedChanged', function(isProtected) {
				signalCalled++;
				if (signalCalled === 0) {
					assert.equal( isProtected, true );
				}
				else if (signalCalled === 1) {
					assert.equal( isProtected, false );
				}
				else {
					assert(false);
				}
			});

			assert(player);
			assert(player.emit);

			//	test playerProtectedChanged (isProtected)
			player.emit( 'playerProtectedChanged', true );
			player.emit( 'playerProtectedChanged', false );
		});

		it('should test playerChanged signal from player', function() {
			var signalCalled = -1;
			api.on( 'playerChanged', function(chID) {
				signalCalled++;
				if (signalCalled === 0) {
					assert.equal( chID, 10 );
				}
				else if (signalCalled === 1) {
					assert.equal( chID, 11 );
				}
				else {
					assert(false);
				}
			});

			assert(player);
			assert(player.emit);

			//	test playerChanged (chID)
	 		player.emit( 'playerChanged', 10 );
	 		player.emit( 'playerChanged', 11 );
		});

		it('should test currentShowChanged signal from player', function() {
			var signalCalled = -1;
			api.on( 'currentShowChanged', function() {
				signalCalled++;
			});

			assert(player);
			assert(player.emit);

			//	test currentShowChanged
	 		player.emit( 'currentShowChanged' );
			assert.equal( signalCalled, 0 );
		});
	});

	it('should return tvd api on start', function(done) {
		_module.onStart(function(err,api) {
			assert.equal(err);
			assert.isObject(api);

			{	//	main methods
				assert.isFunction( api.getWebAPI );
				assert.isObject( api.getWebAPI() );
			}

			{	// Tuner methods:
				assert.isFunction( api.tuner.isScanning );
				assert.isFunction( api.tuner.startScan );
				assert.isFunction( api.tuner.cancelScan );
				assert.isFunction( api.tuner.getWebAPI );
				assert.isObject( api.tuner.getWebAPI() );
			}

			{	// DB methods
				assert.isFunction( api.db.count );
				assert.isFunction( api.db.getAll );
				assert.isFunction( api.db.remove );
				assert.isFunction( api.db.getWebAPI );
				assert.isObject( api.db.getWebAPI() );
			}

			{	//	Channel
				assert.isFunction( api.channel.get );
				assert.isFunction( api.channel.toggleFavorite );
				assert.isFunction( api.channel.toggleBlocked );
				assert.isFunction( api.channel.getShow );
				assert.isFunction( api.channel.getShowsBetween );
				assert.isFunction( api.channel.getWebAPI );
				assert.isObject( api.channel.getWebAPI() );
			}

			{	//	Session
				assert.isFunction( api.session.getWebAPI );
				assert.isObject( api.session.getWebAPI() );
			}

			{	//	Player: status
				assert.isFunction( api.player.getWebAPI );
				assert.isObject( api.player.getWebAPI() );

				assert.isFunction( api.player.current );
				assert.isFunction( api.player.isProtected );
				assert.isFunction( api.player.status );

				//	Player: change
				assert.isFunction( api.player.change );
				assert.isFunction( api.player.nextChannel );
				assert.isFunction( api.player.nextFavorite );
				assert.isFunction( api.player.showMobile );

				//	Player: media
				assert.isFunction( api.player.nextVideo );
				assert.isFunction( api.player.videoInfo );
				assert.isFunction( api.player.nextAudio );
				assert.isFunction( api.player.audioInfo );
				assert.isFunction( api.player.nextSubtitle );
				assert.isFunction( api.player.subtitleInfo );
			}

			{	//	Middleware
				assert.isFunction( api.middleware.getWebAPI );
				assert.isFunction( api.middleware.enableMiddleware );
				assert.isFunction( api.middleware.disableApplications );
				assert.isObject( api.middleware.getWebAPI() );
			}

			_module.onStop(done);
		});
	});

	describe('methods', function() {
		var api = null;

		beforeEach( function(done) {
			_module.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert(api);
				done();
			});
		});

		afterEach( function (done) {
			_module.onStop(done);
		});

		it('scan', function(done) {
			function onScanChanged(evt) {
				if (evt.state === 'end') {
					api.removeListener('scanChanged', onScanChanged);
					done();
				}
			}
			api.on('scanChanged', onScanChanged);
			api.tuner.startScan();
		});

		it('get', function() {
			api.channel.get(10);
			api.channel.get(20);
		});

		it('toggleFavorite', function() {
			api.channel.toggleFavorite(10);
			api.channel.toggleFavorite(20);
		});

		it('toggleBlocked', function() {
			api.channel.toggleBlocked(10);
			api.channel.toggleBlocked(20);
		});

		it('getShow', function() {
			api.channel.getShow(10);
			api.channel.getShow(20);
		});

		it('getShowsBetween', function() {
			api.channel.getShowsBetween(10);
			api.channel.getShowsBetween(20);
			api.channel.getShowsBetween(10,'asdfasdf', 'asdfasdf');
		});

		it('dummy', function() {
			_reg.get( 'screen' ).setActive(false);
			api.channel.getShow(10);
			api.updateLogos();
		});

		describe('middleware', function() {
			it( 'others', function() {
				api.middleware.isMiddlewareEnabled();
				api.middleware.runApplication('0.0');
			});

			it('enable', function() {
				api.middleware.enableMiddleware( true );
				api.middleware.enableMiddleware( false );
			});

			it('disableApps', function() {
				api.middleware.disableApplications( true );
				api.middleware.disableApplications( false );
			});
		});

		describe('logos', function() {
			it('should add provider', function() {
				var findLogo = function(ch) {
					return { logo: 'pepe.img', priority: 10 };
				};
				api.addLogoProvider(findLogo);
				var res = api.getLogo( {} );
				assert.isObject( res );
				assert.isTrue( res.logo === 'pepe.img' );
				assert.isTrue( res.priority === 10 );

				api.removeLogoProvider(findLogo);
				var res = api.getLogo( {} );
				assert.isNull( res );
			});

			it('should add/rm/add/rm provider', function() {
				var obj = function(ch) {};
				api.addLogoProvider(obj);
				api.removeLogoProvider(obj);
				api.addLogoProvider(obj);
				api.removeLogoProvider(obj);
				var res = api.getLogo( {} );
				assert.isNull( res );
			});

			it('remove invalid provider', function() {
				var obj = function(ch) {};
				api.removeLogoProvider(obj);
			});

			it('findLogo return null', function() {
				var findLogo10 = function(ch) {
					return { logo: 'pepe10.img', priority: 10 };
				};
				var findLogo1 = function(ch) {
					return { logo: 'pepe1.img', priority: 1 };
				};
				api.addLogoProvider(findLogo10);
				api.addLogoProvider(findLogo1);
				var res = api.getLogo( {} );
				assert( res );
				assert.equal( res.logo, 'pepe10.img' );
			});

			it('findLogo return null', function() {
				var findLogo = function(ch) {
					return null;
				};
				api.addLogoProvider(findLogo);
				var res = api.getLogo( {} );
			});

			it('findLogo throw error', function() {
				var findLogo = function(ch) {
					throw new Error('Algo');
				};
				api.addLogoProvider(findLogo);
				var res = api.getLogo( {} );
			});
		});
	});
});
