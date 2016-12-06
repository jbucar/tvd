"use strict";

var assert = require('assert');
var EventEmitter = require('events').EventEmitter;
var bPromise = require('bluebird');
var path = require('path');
var fs = bPromise.promisifyAll( require('fs') );
var tvdutil = require('tvdutil');
var validator = require("tv4");
var _ = require('lodash');

function ServiceApi(name,prov) {
	var self = {};

	//	Providers
	self.getProviders = prov.getProviders.bind(prov);
	self.addProvider = prov.addProvider.bind(prov);
	self.hasProvider = prov.hasProvider.bind(prov);
	self.rmProvider = prov.rmProvider.bind(prov);

	//	Get
	self.get = prov.get.bind(prov);
	self.get.isAsync = true;

	//	Events
	self.on = prov.on.bind(prov);
	self.on.isSignal = true;
	self.once = prov.once.bind(prov);
	self.removeListener = prov.removeListener.bind(prov);

	//	Web API
	self.getWebAPI = function() {
		return {
			'name': name,
			'public': ['getProviders', 'hasProvider', 'get', 'on' ],
			'private': ['addProvider', 'rmProvider']
		};
	};

	return self;
}

function ServiceImpl(pName,schema) {
	assert(pName);
	assert(schema);

	var _providers = {};
	var _events = new EventEmitter();

	var self = {};
	tvdutil.forwardEventEmitter(self, _events);

	function checkFields(res,cb) {
		if (res instanceof Array) {
			var valid=[];
			for (var i=0; i<res.length; i++) {
				if (validator.validate( res[i], schema )) {
					valid.push( res[i] );
				}
				else {
					log.warn( pName, 'Ignoring returned object: %s', validator.error.message );
				}
			}
			cb( undefined, valid );
		}
		else if (validator.validate( res, schema )) {
			cb( undefined, res );
		}
		else {
			var descs = [];
			validator.error.subErrors.forEach(function(err) {
				descs.push( err.message );
			});
			descs = _.uniq(descs);

			var err;
			if (descs.length === 1) {
				err = tvdutil.warnErr( pName, 'Not found: err=%s, data=%j', descs[0], res );
			}
			else {
				err = tvdutil.warnErr( pName, 'Not found: err=%s, data=%j', validator.error.subErrors[0].message, res );
			}
			cb( err );
		}
	}

	self.getProviders = function() {
		return Object.keys(_providers).map(function(p) {
			var prov = _providers[p];
			return {
				id: prov.id,
				name: prov.name,
				description: prov.description,
				icon: prov.icon,
				logo: prov.logo
			};
		});
	};

	self.hasProvider = function(pID) {
		return _providers[pID] ? true : false;
	};

	self.addProvider = function(p) {
		if (!p.id || !p.name || !_.isFunction(p.get)) {
			log.error( pName, 'Invalid provider: p=%j', p );
			return;
		}

		if (_providers[p.id]) {
			log.error( pName, 'Provider already exists: p=%s', p.id );
			return;
		}

		log.info( pName, 'Add provider: name=%s', p.name );

		_providers[p.id] = p;
		_events.emit( 'providerAdded', p.id );
	};

	self.rmProvider = function(pID) {
		var prov = _providers[pID];
		if (prov) {
			delete _providers[pID];
			_events.emit( 'providerRemoved', pID );
		}
		else {
			log.error( pName, 'Provider not found: p=%s', pID );
		}
	};

	self.getImpl = function( opts ) {
		if (opts.provider) {
			var p = _providers[opts.provider];
			if (!p) {
				return bPromise.reject(tvdutil.warnErr( pName, 'Provider not found: %s', opts.provider ));
			}

			return p.get(opts);
		}
		else if (opts.perPage && opts.provider === undefined) {
			return bPromise.reject(tvdutil.warnErr(pName, 'Invalid options; Paging with multiple providers are not supported'));
		}
		else {
			var aImages = [];
			for (var key in _providers) {
				var pOpts = _.cloneDeep(opts);
				pOpts.provider = key;
				aImages.push( self.getImpl( pOpts ) );
			}

			return bPromise.all(aImages).then(function(aResult) {
				var result = [];
				aResult.forEach(function(aInfo) {
					aInfo.forEach(function(info) {
						result.push( info );
					});
				});
				return result;
			});
		}
	};

	self.get = function( opts, cb ) {
		//	Validate page options
		if (opts.page || opts.perPage) {
			if (opts.page === undefined) {
				delete opts.perPage;
			}
			else if (!opts.perPage) {
				delete opts.page;
			}
			else if (typeof opts.page !== 'number') {
				delete opts.page;
				delete opts.perPage;
			}
			else if(typeof opts.perPage !== 'number' || opts.perPage <= 0) {
				delete opts.page;
				delete opts.perPage;
			}
		}

		log.silly( pName, 'get: opts=%j', opts );

		self.getImpl( opts ).then(function(res) {
			checkFields(res,cb);
		},function(err) {
			log.warn( pName, 'Error in getImpl: opts=%j', opts );
			cb(err);
		});
	};

	self.createApi = function() {
		return new ServiceApi(pName,self);
	};

	return self;
}

function createEntryPoint( srvName, schema ) {
	var self = {};
	var _impl = null;

	self.onStart = function(cb) {
		assert(cb);
		log.info( srvName, 'Start' );
		fs.readFileAsync( path.join(__dirname, schema) )
			.then(JSON.parse)
			.then(function(schema) {
				_impl = new ServiceImpl(srvName,schema);
				cb( undefined, _impl.createApi() );
			},function(err) {
				cb( err );
			});
	};

	self.onStop = function(cb) {
		log.info( srvName, 'Stop' );
		assert(_impl);
		_impl = null;
		cb();
	};

	return Object.freeze(self);
}


module.exports.ServiceImpl = ServiceImpl;
module.exports.createService = createEntryPoint;

