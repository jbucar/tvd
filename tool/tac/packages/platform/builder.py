#!/usr/bin/env python3

import imp
import os

module = imp.load_source('builder', os.path.join(os.path.dirname(__file__), '..', '..', 'build', 'generic_builder.py'))

class Builder( module.Builder ):
#public:
	def __init__(self, package, platform, deployDir, util):
		super().__init__(package, platform, deployDir, util)

#protected:
	#override
	def buildImpl(self, outDir, outSymsDir, isHost):
		# Put build.json in output directory
		self.util.cp( 'build_%s.json' % self.util.options.build_cfg, self.rootDir, os.path.join(outDir, 'build.json') )

		# Deploy configs to output directory
		for f in ['platform.js', 'platform.json']:
			if os.path.exists( os.path.join(self.util.device.deviceDir, 'js', f) ):
				self.util.cp( f, os.path.join(self.util.device.deviceDir, 'js'), outDir )
			else:
				self.util.cp( f, os.path.join(self.util.device.commonDir, 'js'), outDir )

	#override
	def installImpl(self, isHost):
		if self.util.cfgFile:
			buildJSON = os.path.join(self.fqnOriginalDir(), 'build.json')
			cfg = self.util.loadJSON( buildJSON )
			cfgName = self.util.loadJSON( self.util.tacAbsPath( 'build', self.util.cfgFile ) )['name']
			cfg['build'] = {
				'name': cfgName,
				'file': os.path.basename( self.util.cfgFile ),
				'type': self.util.options.build_type,
				'config': self.util.options.build_cfg
			}
			cfg['build'].update(self.util.keyValueArray2Dic(self.util.options.build_opts))
			self.util.saveJSON( buildJSON, cfg )
