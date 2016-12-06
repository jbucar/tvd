#!/usr/bin/env python3

import imp
import json
import os
import subprocess

module = imp.load_source( 'common_config', os.path.join( os.path.dirname(__file__), '..', 'common', 'config.py' ) )

class BuildrootConfig( module.GenericConfig ):

	def __init__(self, buildDir, installDir, deviceDir):
		super().__init__( buildDir, installDir, deviceDir )

	def getGypConfig(self):
		gCfg = {
			'clang' : 0,
			'host_clang': 0,
			'target_arch' : self.toolchainPrefix.split('-')[0],
			'use_sysroot': 1,
			'sysroot' : self.stagingDir,
			'release_optimize' : 1,
			'linux_strip_binary' : 1,
			# Embedded
			'embedded' : 1,
			'use_udev': 1,
			'use_ozone_evdev': 1,
			'use_video_proxy': 'dummy',
		}
		if self.toolchainPrefix.split('-')[0] == 'arm':
			gCfg['arm_float_abi'] = 'hard'
		if self.useOzoneGbm():
			gCfg.update({
				'ozone_platform_gbm' : 1,
				'use_system_minigbm' : 1,
				'use_drm_atomic': 1,
			})
		else:
			gCfg['ozone_platform_fbdev'] = 1

		cfg = super().getGypConfig()
		cfg['global'].update(gCfg)
		cfg['system'].update({
			'use_system_bzip2' : 0,
			'use_system_flac' : 0,
			'use_system_xdg_utils' : 0,
			'use_system_yasm' : 0,
		})
		return cfg

	def setupEnv(self, opts):
		print("Runing setup environment for buildroot")

		if not ('TVD_DEPOT_TOOLCHAIN_PATH' in os.environ and 'TVD_DEPOT_SYSROOT' in os.environ):
			raise Exception('TVD_DEPOT_TOOLCHAIN_PATH or TVD_DEPOT_SYSROOT not present in environment')

		self.toolchainDir = os.environ['TVD_DEPOT_TOOLCHAIN_PATH']
		self.stagingDir = os.environ['TVD_DEPOT_SYSROOT']
		self.toolchainPrefix = os.environ['TVD_DEPOT_TOOLCHAIN_PREFIX'] if ('TVD_DEPOT_TOOLCHAIN_PREFIX' in os.environ) else 'arm-linux-gnueabihf'

		toolchainPath = os.path.join( self.toolchainDir, self.toolchainPrefix )
		os.environ['BUILDROOT_TOOLCHAIN_PREFIX'] = toolchainPath
		os.environ['BR_NO_CCACHE'] = '1'
		os.environ['AR'] = '%sar' % toolchainPath
		os.environ['CC'] = '%sgcc' % toolchainPath
		os.environ['CXX'] = '%sg++' % toolchainPath
		os.environ['CC_host'] = 'gcc'
		os.environ['CXX_host'] = 'g++'
		super().setupEnv(opts)

	def strip(self):
		binary = '%s/out/Release/htmlshell' % self._buildDir
		print( 'Striping %s' % binary )
		subprocess.check_call( ['%sstrip' % os.path.join(self.toolchainDir, self.toolchainPrefix), '--strip-unneeded', binary] )

	def prepareSources(self):
		super().prepareSources()

	def useOzoneGbm(self):
		return False
