#!/usr/bin/env python3

import imp
import os

module = imp.load_source( 'buildroot_config', os.path.join( os.path.dirname(__file__), '..', 'buildroot_config.py' ) )

class Config( module.BuildrootConfig ):

    def __init__(self, buildDir, installDir):
        super().__init__( buildDir, installDir, os.path.dirname(__file__) )

    def getGypConfig(self):
        cfg = super().getGypConfig()
        cfg['global'].update({
            'use_v4lplugin': 1,
            'use_v4l2_codec': 1,
        })
        return cfg

    def customSetup(self):
        self.reconfigureFFMPEG('arm-neon')
