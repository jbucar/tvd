#!/usr/bin/env python3

import imp
import os
import multiprocessing

builder_dir = os.path.dirname(os.path.abspath(__file__))
module = imp.load_source('builder', os.path.join(builder_dir, '..', '..', 'build', 'generic_builder.py'))

class Builder( module.Builder ):
#public:
	def __init__(self, package, platform, deployDir, util):
		super().__init__(package, platform, deployDir, util)

#protected:
	#override
	def buildImpl(self, outDir, outSymsDir, isHost):
		# Correct node_modules path
		modulesDir = os.path.join(outDir, 'lib', 'node_modules')
		self.util.mkdir(modulesDir)
		self.util.cpDirContent('node_modules', outDir, modulesDir)

