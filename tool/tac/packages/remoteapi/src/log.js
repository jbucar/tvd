'use strict';

var assert = require('assert');
var Url = require('url');
var util = require('util');

var headers = {
	error: '<font color=red>ERROR</font>',
	warn : '<font color=orange>WARNING</font>',
	http : '<font color=blue>http</font>',
	info : '<font color=green>Info</font>',
	silly : '<font color=grey>silly</font>',
	verbose : 'verbose'
};

function LogModule() {
	var self = {};
	var _enabled = true;
	var _maxWidth = null;

	function escapeText(txt){
		return txt.replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/'/, '&#39;').replace(/’/, '&#180;').replace(/’/, '&#8217;');
	}

	function formatter(msg) {
		var txt = _maxWidth ? msg.message.substr(0, _maxWidth) : msg.message;
		var message = util.format('<div> [<b> %s </b>] - <span style="color:purple"> <b>%s</b> </span> - %s </div>',
								  headers[msg.level], msg.prefix, escapeText(txt));
		return message;
	}

	//Filters logs from the log record, matching the parameters from an array
	function filterLog (logfilters) {
		var filteredRecord = log.record;
		var lvl = {
			error:0,
			warn:1,
			http:2,
			info:3,
			verbose:4,
			silly:5,
		};

		// Applies if filter level
		if (logfilters.level) {
			filteredRecord = filteredRecord.filter (function(msg) {
				return lvl[msg.level] <= lvl[logfilters.level];
			});
		}
		// Applies if filter prefix
		if (logfilters.prefix) {
			filteredRecord = filteredRecord.filter (function(msg) {
				return msg.prefix === logfilters.prefix;
			});
		}

		return filteredRecord;
	}

	function setConfig( cfg ) {
		log.level = cfg.level;
		log.maxRecordSize = cfg.maxHistorySize;
		_enabled = cfg.enabled;
		_maxWidth = cfg.maxWidth;
	}

	function onSystemConfigurationChanged(cfg) {
		log.verbose( 'log', 'Update configuration: cfg=%j', cfg );
		setConfig( cfg );
	}

	function onRequest( req, res ) {
		if (_enabled) {
			var logfilters = Url.parse(req.url, true).query; //Filters are obtained from url
			var filteredlog = filterLog(logfilters); // Filtered logs are obtained
			var data = filteredlog.map(function(msg) {
				return formatter(msg);
			}).join('\n');
			self.sendResult( res, 200, undefined, data );
		}
		else {
			self.sendResult( res, 404 );
		}
	}

	//	Module API
	self.start = function() {
		//	Get config from system && apply
		var sys = self.reg().get('system');
		assert(sys);
		setConfig( sys.getLogConfig() );

		sys.on( 'LogConfigChanged', onSystemConfigurationChanged );

		self.addHandler( 'GET', '/log', onRequest );
		return true;
	};

	self.stop = function() {
		var sys = self.reg().get('system');
		assert(sys);
		sys.removeListener( 'LogConfigChanged', onSystemConfigurationChanged );
	};

	//	Properties
	self.name = 'log';

	return self;
}

module.exports = LogModule;

