"use strict";

var fs = require('fs');

function writeRom( file ) {
	let data = fs.readFileSync( file );
	let js = 'var Rom = "' + data.toString('base64') + '";\n';
	js += 'function getRom() {\n'
	js += '     return atob(Rom)';
	js += '};\n';
	fs.writeFileSync( 'src/rom.js', js );
}

writeRom( process.argv[2] );

