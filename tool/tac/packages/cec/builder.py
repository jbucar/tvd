#!/usr/bin/env python3

import imp
import os

module = imp.load_source('builder', os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'build', 'generic_builder.py'))

class Builder( module.Builder ):
#public:
	def __init__(self, package, platform, deployDir, util):
		super().__init__( package, platform, deployDir, util )

#protected:
	#override
	def buildImpl(self, outDir, outSymsDir, isHost):
		# Put cecmodule for target in outDir
		dest = os.path.join( outDir, 'node_modules', 'cecmodule', 'index.node' )
		self.util.deployLibrary( 'libcecnode', self.platform, dest )

	# override
	def isMultiPlatform(self):
		return False
