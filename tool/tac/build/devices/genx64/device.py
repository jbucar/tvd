#!/usr/bin/env python3

import imp
import os

EXYNOS_DEVICE_DIR = os.path.dirname(os.path.abspath(__file__))

module = imp.load_source('buildroot_device', os.path.join(EXYNOS_DEVICE_DIR, '..', 'buildroot_device.py'))

class Device( module.BuildrootDevice ):

	def __init__(self, util):
		super().__init__( util, os.path.basename(EXYNOS_DEVICE_DIR) )
		self.defaultDepot = '/X64/sdk-X64-569-gcc300f5.tgz'

	# override
	def arch(self):
		return 'x64'
