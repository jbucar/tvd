'use strict';

var bUtils = require('../util');
var fs = require('fs');
var path = require('path');

module.exports = function(system) {
	var self = this;

	self.crashDirectory = '';

	self.init = function(dir) {
		try {
			self.crashDirectory = dir;
			if (fs.existsSync(dir)) {
				log.silly('CrashObserver', 'Cleaning up crash directory: %s', dir);
				self.cleanCrashDirectory();
			} else {
				log.silly('CrashObserver', 'Creating crash directory: %s', dir);
				fs.mkdirSync(dir);
			}
		} catch( error ) {
			log.warn('CrashObserver', 'Fail to create/clean crash directory, error=%s', error.message);
			self.crashDirectory = '';
			return false;
		}

		return true;
	};

	self.reportCrashes = function() {
		var nDumps = getMinidumps().length;
		if (nDumps > 0) {
			log.warn('CrashObserver', 'Found %d minidump/s in htmlshell crash directory', nDumps);
			system.sendError(new Error('One or more minidump/s found in htmlshell crash directory'));
		}
	};

	self.getEncodedMinidumps = function() {
		return getMinidumps().reduce(function(result, file) {
			result[file] = fs.readFileSync(file, {'encoding': 'base64'});
			return result;
		}, {});
	};

	self.cleanCrashDirectory = function() {
		bUtils.cleanDirectory(self.crashDirectory, false);
	};

	function getMinidumps() {
		var crashFiles = [];

		try {
			crashFiles = fs.readdirSync(self.crashDirectory);
		} catch(error) {
			log.warn('CrashObserver', 'Fail to read htmlshell crash directory, error=%s', error.message);
			crashFiles = [];
		}

		return crashFiles.reduce( function(minidumps, file) {
			if (file.match(/^.*-minidump-.*\.dmp$/)) {
				minidumps.push(path.join(self.crashDirectory, file));
			} else {
				log.warn('CrashObserver', 'Found invalid file in crash directory: %s', file);
			}
			return minidumps;
		}, []);
	}

	return self;
};
