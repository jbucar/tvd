'use strict';

function IndexModule( proxy ) {
	var self = {};

	self.start = function() {
		//	Add api service
		self.addHandler( 'GET', '/', function(req,res) {
			let html = '<!DOCTYPE html>';
			html += '<html>';
			html += '<body>';
			proxy.currentHandlers().forEach(function(handler) {
				if (handler.method() === 'GET' && !handler.module().hidden) {
					html += '<a target="_self" href="' + handler.path() + '"></br>';
					html += handler.module().name;
					html += '</a>';
				}
			});
			html += '</body>';
			html += '</html>';

			self.sendResult( res, 200, 'text/html', html );
		});
	};

	//	Properties
	self.name = 'index';
	self.hidden = true;

	return self;
}

// Export module hooks
module.exports = IndexModule;

