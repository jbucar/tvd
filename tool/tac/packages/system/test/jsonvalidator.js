'use strict';

var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;
var validator = require('../src/jsonvalidator');

describe('JSONValidator', function() {

	it('should validate example schema', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.VideoPlayer",
					"name": "Video Service",
					"vendor": "tvd",
					"type": "app",
					"version": "1.0.0",
					"main": "view/index.html",
				},
				{
					"id": "ar.edu.unl.info.lifia.videos.service",
					"name": "Video Service",
					"type": "service",
					"version": "1.0.0",
					"main": "src/main.js",
					"dependencies": [
						"ar.edu.unl.info.lifia.system.serviceWithProvider",
						"ar.edu.unl.info.lifia.videos.service",
						"ar.edu.unl.info.lifia.system.usb_reader"
					]
				},

				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider",
					"type": "service",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 app', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.VideoPlayer",
					"name": "Video Service",
					"vendor": "tvd",
					"type": "app",
					"version": "1.0.0",
					"main": "view/index.html",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 service', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider",
					"type": "service",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 service and deps', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider",
					"type": "service",
					"version": "1.0.0",
					"main": "src/provider.js",
					"dependencies": ["ar.edu.unl.info.lifia.system.serviceWithProvider"]
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 browser plugin', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.plugin",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "LINUX",
			"components": [
				{
					"id": "ar.edu.unlp.info.lifia.tvd.ginga",
					"name": "Ginga PlugIn",
					"type": "browser_plugin",
					"out_of_process": true,
					"sandboxed": false,
					"main": "plugins/libdtvginga.so",
					"version": "2.2",
					"mime_types": [{
						"type": "application/ginga",
						"extensions": "ncl",
						"description": "Ginga application (NCL/Lua)"
					}]
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 app and deps', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider",
					"type": "app",
					"vendor": "tvd",
					"version": "1.0.0",
					"main": "src/provider.js",
					"dependencies": ["ar.edu.unl.info.lifia.system.serviceWithProvider"]
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 app and no vendor', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 app and desc', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"description": "description for an app",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 app with desc', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"description": "description for an app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 app and no version', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 service and no version', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "service",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 service and desc', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"description": "description for a service",
					"type": "service",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});


	it('should validate schema with 1 app and icon', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
					"icon": "src/pepe.png",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 app and bg_transparent', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"bg_transparent": true,
					"version": "1.0.0",
					"main": "src/provider.js",
					"icon": "src/pepe.png",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 app and bg_transparent false', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"bg_transparent": false,
					"version": "1.0.0",
					"main": "src/provider.js",
					"icon": "src/pepe.png",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with system package and 1 app ', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"system": true,
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
					"icon": "src/pepe.png",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with non system package and 1 app ', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"system": false,
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
					"icon": "src/pepe.png",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 service and not vendor', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "service",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 service and runs', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "service",
					"version": "1.0.0",
					"main": "src/provider.js",
					"runs": "onDemand"
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate schema with 1 service and runs onStartup', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "service",
					"version": "1.0.0",
					"main": "src/provider.js",
					"runs": "onStartup"
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should fail schema with 1 service and runs invalid', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "service",
					"version": "1.0.0",
					"main": "src/provider.js",
					"runs": "invalid"
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isDefined( err );
			done();
		});
	});

	it('should validate schema with 1 app and not manifest-version', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should fail manifest with 1 app and no id', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"name": "Usb Video Service provider app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isDefined( err );
			assert.strictEqual( err.name, "ValidationError" );
			done();
		});
	});

	it('should fail manifest with 1 app and id invalid', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar",
					"name": "Usb Video Service provider app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isDefined( err );
			assert.strictEqual( err.name, "ValidationError" );
			done();
		});
	});

	it('should fail manifest with 1 service and no id', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"name": "Usb Video Service provider app",
					"type": "service",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isDefined( err );
			assert.strictEqual( err.name, "ValidationError" );
			done();
		});
	});

	it('should fail manifest with 1 service and bg_transparent', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"name": "Usb Video Service provider app",
					"type": "service",
					"version": "1.0.0",
					"bg_transparent": true,
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isDefined( err );
			assert.strictEqual( err.name, "ValidationError" );
			done();
		});
	});

	it('should fail manifest with 1 app and invalid bg_transparent', function(done) {
		var manifest = {
			"manifest-version": "1",
			"id": "ar.edu.unl.info.lifia.videos",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"bg_transparent": "invalid",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isDefined( err );
			assert.strictEqual( err.name, "ValidationError" );
			done();
		});
	});

	it('should fail manifest with 1 app and no main id', function(done) {
		var manifest = {
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isDefined( err );
			assert.strictEqual( err.name, "ValidationError" );
			done();
		});
	});

	it('should fail manifest with no components', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": []
		};

		validator.isValid( manifest, function(err) {
			assert.isDefined( err );
			assert.strictEqual( err.name, "ValidationError" );
			done();
		});
	});

	it('should fail manifest with invalid component', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos.usb",
			"manifest-version": "1",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "pepe",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isDefined( err );
			assert.strictEqual( err.name, "ValidationError" );
			done();
		});
	});

	it('should fail manifest with invalid attributes', function(done) {
		var manifest = {
			"id": "ar.edu.unl.info.lifia.videos.usb",
			"manifest-version": "1",
			"version": "1.0.0",
			"invalid-attribute": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "AML_ISDBT_1",
			"components": [
				{
					"id": "ar.edu.unl.info.lifia.videos.usb",
					"name": "Usb Video Service provider app",
					"type": "app",
					"version": "1.0.0",
					"main": "src/provider.js",
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isDefined( err );
			assert.strictEqual( err.name, "ValidationError" );
			done();
		});
	});

	it('should validate system manifest', function(done) {
		var manifest = {
			"id": "ar.edu.unlp.info.lifia.tvd.system",
			"version": "1.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "all",
			"components": [
				{
					"id": "ar.edu.unlp.info.lifia.tvd.system",
					"name": "System",
					"type": "service",
					"version": "1.0",
					"main": "system.js",
					"dependencies": ["ar.edu.unlp.info.lifia.tvd.appmgr"]
				},
				{
					"id": "ar.edu.unlp.info.lifia.tvd.appmgr",
					"name": "Application Manager",
					"type": "service",
					"version": "1.0",
					"main": "applicationmgr.js",
					"dependencies": ["ar.edu.unlp.info.lifia.tvd.browser"]
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate browser manifest', function(done) {
		var manifest = {
			"manifest-version": "1",
			"components": [
			    {
				"type": "service",
				"name": "Browser",
				"main": "main.js",
				"id": "ar.edu.unlp.info.lifia.tvd.browser",
				"version": "1.0.2635.g33b50bc"
			    }
			],
			"platform": "LINUX32",
			"id": "ar.edu.unlp.info.lifia.tvd.browser",
			"api-level": "1.0",
			"version": "1.0.2635.g33b50bc",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/"
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate node manifest', function(done) {
		var manifest = {
			"api-level": "1.0",
			"components": [
			    {
				"type": "binary",
				"name": "Node",
				"id": "ar.edu.unlp.info.lifia.tvd.node",
				"version": "1.0.2635.g33b50bc"
			    }
			],
			"platform": "LINUX32",
			"id": "ar.edu.unlp.info.lifia.tvd.node",
			"version": "1.0.2635.g33b50bc",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/"
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});

	it('should validate shell manifest', function(done) {
		var manifest = {
			"id": "ar.edu.unlp.info.lifia.tvd.shell",
			"version": "1.0.0",
			"vendor": "LIFIA",
			"vendor-url": "http://lifia.info.unlp.edu.ar/",
			"api-level": "1.0",
			"platform": "all",
			"components": [
				{
					"id": "ar.edu.unlp.info.lifia.tvd.shell",
					"name": "Shell Application",
					"type": "app",
					"version": "1.0.0",
					"main": "index.html"
				}
			]
		};

		validator.isValid( manifest, function(err) {
			assert.isUndefined( err );
			done();
		});
	});
});
