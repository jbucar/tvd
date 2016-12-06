#!/usr/bin/env node

var validator = require('../../packages/system/src/jsonvalidator');
var fs = require('fs');
var path = require('path');

var fileName = process.argv[2];
var data = fs.readFileSync( fileName ).toString();

validator.isValid( JSON.parse( data ), function( error ) {
	if ( !error ) {
		console.log( "Se ha validado correctamente el archivo " + fileName );
	} else {
		console.log( "Se produjeron los siguientes errores: ");
		console.log( error.message );
		process.exit(1);
	}
});
