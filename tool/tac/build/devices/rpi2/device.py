#!/usr/bin/env python3

import imp
import os

RPI2_DEVICE_DIR = os.path.dirname(os.path.abspath(__file__))

module = imp.load_source('buildroot_device', os.path.join(RPI2_DEVICE_DIR, '..', 'buildroot_device.py'))

class Device( module.BuildrootDevice ):

	def __init__(self, util):
		super().__init__( util, os.path.basename(RPI2_DEVICE_DIR) )
		self.defaultDepot = '/RPI2/sdk-RPI2-569-gcc300f5.tgz'
