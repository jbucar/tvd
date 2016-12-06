#!/usr/bin/env python3

import imp
import os

depotPath = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
module = imp.load_source('builder', os.path.join(depotPath, 'build', 'generic_builder.py'))

class Builder( module.Builder ):
	#public:
	def __init__(self, package, platform, deployDir, util):
		super().__init__(package, platform, deployDir, util)

	#override
	def deploySrcDir(self):
		return False

	#override
	def deployResources(self):
		return False

	#override
	def buildImpl(self, outDir, outSymsDir, isHost):
		self.deployWeb( outDir, 'shell.json' )
		self.deployWeb( outDir, 'notification.json' )


