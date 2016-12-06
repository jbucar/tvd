#!/usr/bin/env python3

import imp
import os

module = imp.load_source('builder', os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'build', 'generic_builder.py'))

class Builder( module.Builder ):
#public:
	def __init__(self, package, platform, deployDir, util):
		super().__init__( package, platform, deployDir, util )
		self.tvdModuleDir = os.path.join( self.rootDir, 'node_modules', 'tvdmodule' )

#protected:
	# override
	def isMultiPlatform(self):
		return False

	#override
	def cleanImpl(self):
		self.util.mkdir( self.tvdModuleDir, True )

	#override
	def buildImpl(self, outDir, outSymsDir, isHost):
		# Put tvdmodule in outDir for target
		dest = os.path.join( outDir, 'node_modules', 'tvdmodule', 'index.node' )
		self.util.deployLibrary( 'tvdmodule', self.platform, dest )

		# Put ginga in outDir for target
		destDir = os.path.join( outDir, 'plugins' )
		self.util.mkdir( destDir )
		ppapiPlat = 'ppapi' if (self.platform == 'linux') else '%s_ppapi' % self.platform
		self.util.deployLibrary( 'ginga', self.platform, destDir, urlPlat=ppapiPlat )

		# Put tvdmodule for host in src node_modules directory (for running tests)
		if self.platform == 'linux':
			self.util.cp( 'index.node', os.path.join( outDir, 'node_modules', 'tvdmodule' ), self.tvdModuleDir )
		else:
			self.util.deployLibrary( 'tvdmodule', 'linux', os.path.join( self.tvdModuleDir, 'index.node' ) )

