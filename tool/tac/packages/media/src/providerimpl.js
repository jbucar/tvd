"use strict";

var assert = require('assert');
var path = require('path');
var _ = require('lodash');
var tvdutil = require('tvdutil');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var naturalSort = require('naturalsort').compare;

function ProviderImpl(dlg) {
	assert(dlg);

	function filter( oper ) {
		if (tvdutil.isHidden(oper.name)) {
			//log.silly( 'DiskProvider', 'Ignring hidden file: name=%s', oper.name );
			return false;
		}

		//	Filter files or directories
		if (oper.stats.isFile()) {
			oper.extname = path.extname(oper.name).toLowerCase();
			if (dlg.supportedExtensions().indexOf(oper.extname) < 0) {
				return false;
			}
		}
		else if (!oper.stats.isDirectory()) {
			//	FIFO or devices must be ignored
			return false;
		}

		return true;
	}

	function map( oper ) {
		oper.info = {
			provider: dlg.id,
			id: oper.id,
			name: oper.name
		};

		return new bPromise(function(resolve) {
			return resolve( dlg.map( oper ) );
		});
	}

	function sort( entries ) {
		return entries.sort(function(a,b) {
			if (a.stats.isDirectory() === b.stats.isDirectory()) {
				return naturalSort(a.name,b.name);
			}
			else if (a.stats.isDirectory()) {
				//	a dir
				return -1;
			}
			else {
				//	b dir
				return 1;
			}
		});
	}

	function scanFile( root, opts ) {
		return fs.readdirAsync( root )
			.then(function(entries) {
				var index = _.findIndex( entries, function(entry) {
					var id = path.join(root,entry);
					return opts.id === id;
				});
				if (index < 0) {
					throw new Error('Not found');
				}

				return fs.statAsync(opts.id)
					.then(function(stats) {
						var oper = {
							id: opts.id,
							root: root,
							name: entries[index],
							stats: stats,
							opts: opts
						};

						if (!filter( oper )) {
							throw new Error( 'Invalid entry' );
						}

						return map( oper );
					});
			});
	}

	function scanFiles( root, opts ) {
		return fs.readdirAsync( root )
			.then(function(files) {
				return bPromise.settle(files.map(function(entry) {
					var absPath = path.join(root,entry);
					return fs.statAsync(absPath)
						.then(function(stats) {
							return {
								id: absPath,
								root: root,
								name: entry,
								stats:stats,
								opts: opts
							};
						})
						.error( function(error) {
							throw tvdutil.warnErr('DiskProvider', 'Cannot get stats from file:',absPath, error.message);
						});
					}))
					.filter(function(prom) {
						return prom.isFulfilled();
					})
					.map(function(prom) {
						return prom.value();
					});
			})
			.then(sort)
			.then(function(opers) {
				var cur = -1;
				var processed = 0;
				var result = [];

				for (var i=0; i<opers.length; i++) {
					var oper = opers[i];

					if (!filter(oper)) {
						continue;
					}

					//log.silly( 'DiskProvider', 'page=(%d,%d), cur=%d, processed=%d, id=%s', opts.page, opts.perPage, cur, processed, oper.id );

					if (opts.page !== undefined) {
						cur++;
						if (!((~~(cur / opts.perPage)) === opts.page && processed < opts.perPage)) {
							//console.log( 'break: cond1=%d, cond2=%d, cond2=%d', (cur / opts.perPage), ~~(cur / opts.perPage), processed < opts.perPage );
							continue;
						}
					}

					processed++;
					result.push( map( oper ) );
				}

				return result;
			});
	}

	//	Setup provider
	var self = {
		id: dlg.id,
		name: dlg.name,
		icon: dlg.icon,
		logo: dlg.logo,
		get: function(opts) {
			var root = opts.root ? path.join(dlg.root, opts.root) : dlg.root;
			log.silly( 'DiskProvider', 'scanFiles: opts=%j, root=%s', opts, root );
			if (opts.id) {
				return scanFile( root, opts );
			}
			else {
				return bPromise.all(scanFiles( root, opts ));
			}
		}
	};

	return self;
}

module.exports = ProviderImpl;

