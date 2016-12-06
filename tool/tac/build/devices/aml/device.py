#!/usr/bin/env python3

import imp
import os

AML_DEVICE_DIR = os.path.dirname(os.path.abspath(__file__))

module = imp.load_source('buildroot_device', os.path.join(AML_DEVICE_DIR, '..', 'buildroot_device.py'))

class Device( module.BuildrootDevice ):

	def __init__(self, util):
		super().__init__( util, os.path.basename(AML_DEVICE_DIR) )
		self.defaultDepot = '/AML/tac/microtrol/sdk-aml_isdbt_1-9cc6118818f209f4515ed1b4ab4100de15e63b2c.tgz'
		self.middlewareFilename = 'rootfs'
		self.middlewareInstallDir = 'system/original'

	# override
	def customPackMiddlewareStep(self, middlewareDir, middlewareEnv):
		# Should decompress rootfs into middlewareDir
		rootfs = self.getDepotPath('rootfs')
		self.util.vlog('Extracting %s in %s' % (rootfs, middlewareDir))
		self.util.fakerootRun(middlewareEnv, ['tar','--directory', middlewareDir, '-xf', rootfs])

		self.util.vlog('Applying patches to rootfs file')
		patches = self.patchFiles()
		patches.sort()
		for p in patches:
			self.util.fakerootRun(middlewareEnv, ['patch', '-p1', '-d', middlewareDir, '-i', p])

		self.util.vlog('Fixing rootfs ownership')
		self.util.fakerootRun( middlewareEnv, ['chmod','a+rx', middlewareDir])
		self.util.fakerootRun( middlewareEnv, ['chmod','go+rx', os.path.join( middlewareDir, 'root' )] )

	# override
	def genHashes(self, baseDir, files, out=''):
		self.util.genHashes(baseDir, files, out)

