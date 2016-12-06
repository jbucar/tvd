'use strict';

var assert = require('assert');

var srvID = 'ar.edu.unlp.info.lifia.tvd.remoteapi';

function ApiModule() {
	var self = {};

	function onAPI(req,res) {
		res.setHeader('Access-Control-Allow-Origin', '*');
		function onResult(ret) {
			assert(!res.isSignal);

			var st = (ret.error) ? 404 : 200;
			var dataToReturn = (ret.data && ret.data.length > 0) ? ret.data[0] : {};
			var reply = JSON.stringify( dataToReturn, null, 4 );
			self.sendResult( res, st, 'application/json', reply );
		}

		if (req.data && req.data.serviceID && req.data.method) {
			var evt = {
				serviceID: req.data.serviceID,
				apiName: req.data.apiName,
				method: req.data.method,
				params: req.data.params,
				resultCB: onResult,
				appID: srvID,
				failOnSignal: true
			};

			//	Call API
			self.reg().runAPI( evt );
		}
		else {
			self.sendResult( res, 404, 'text/html' );
		}
	}

	self.start = function() {
		//	Add api service
		var handler = self.addHandler( 'POST', '/api', onAPI );
		handler.useJSON = true;
	};

	//	Properties
	self.name = 'api';

	return self;
}

// Export module hooks
module.exports = ApiModule;

