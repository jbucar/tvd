'use strict';

var path = require('path');
var bPromise = require('bluebird');
var fs = bPromise.promisifyAll( require('fs') );
var validator = require("tv4");
var util = require('util');

var ValidationError = function (message) {
	Error.call(this);
	Error.captureStackTrace(this, this.constructor);
	this.name = 'ValidationError';
	this.message = message;
};
util.inherits(ValidationError, Error);

function isValid( manifest, cb ) {
	fs.readFileAsync( path.join(__dirname, "schemamanifest.json") )
		.then(JSON.parse)
		.then(function( schema ) {
			var valid = validator.validate( manifest, schema );
			if (!valid) {
				var error = validator.error;
				var errorMsj = "";
				if (error.code === validator.errorCodes.ONE_OF_MISSING) {
					errorMsj = error.subErrors[0].message + " in component: " + error.subErrors[0].dataPath;
				} else {
					errorMsj = error.message + " in component: ";
					errorMsj += error.dataPath === '' ? '/' : error.dataPath;
				}
				cb( new ValidationError( errorMsj ) );
			}
			else {
				cb( undefined );
			}
		});
}

module.exports.isValid = isValid;
