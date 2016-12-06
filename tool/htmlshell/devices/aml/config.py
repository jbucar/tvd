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
            'use_video_proxy': 'aml',
            'video_hole': 1,
        })
        return cfg

    def prepareSources(self):
        super().prepareSources()
        srcDir = os.path.join(os.environ['DEPOT'], 'lib', 'dtv-aml')
        dstDir = os.path.join(self._buildDir, 'ozone-fbdev')
        self.installDir( srcDir, dstDir, 'src' )
        self.installFile( srcDir, dstDir, 'aml.gypi' )
