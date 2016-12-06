#!/usr/bin/env python3

import imp
import os

absPath = os.path.dirname( os.path.realpath(__file__) )
module = imp.load_source( 'buildroot_config', os.path.abspath( os.path.join( absPath, '..', 'buildroot_config.py' ) ) )

class Config( module.BuildrootConfig ):

	def __init__(self, buildDir, installDir):
		super().__init__(buildDir, installDir, absPath)

	def getGypConfig(self):
		cfg = super().getGypConfig()
		cfg['global'].update({
			'target_arch' : 'x64',
			'linux_use_gold_flags' : 0,
		})
		return cfg

	def customSetup(self):
		self.reconfigureFFMPEG('x64')

	def useOzoneGbm(self):
		return True
