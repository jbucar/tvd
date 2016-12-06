#!/usr/bin/env python3

import imp
import os

absPath = os.path.dirname( os.path.realpath(__file__) )
module = imp.load_source( 'config', os.path.abspath( os.path.join( absPath, '..', '..', 'common', 'config.py' ) ) )

class Config( module.GenericConfig ):

    def __init__(self, buildDir, installDir):
        super().__init__(buildDir, installDir, absPath)

    def getGypConfig(self):
        cfg = super().getGypConfig()
        cfg['global'].update({
            'clang' : 0,
            'host_clang' : 0,
            'target_arch' : 'x64',
            'use_sysroot' : 0,
            'clang_use_chrome_plugins' : 0,
            'linux_link_gsettings' : 0,
            'linux_link_libpci' : 1,
            'linux_link_libspeechd' : 1,
            'linux_link_pulseaudio' : 0,
            'linux_strip_binary' : 1,
            'linux_use_gold_flags' : 0,
            'libspeechd_h_prefix' : 'speech-dispatcher/',
            'system_libdir' : 'lib',
            'usb_ids_path' : '/usr/share/hwdata/usb.ids',
            'linux_dump_symbols': 1,
            'use_video_proxy': 'dummy',
            # Ozone
            'ozone_platform_egltest' : 1,
            'ozone_platform_headless' : 1,
            'ozone_platform_ozonex' : 1,

            # GCC 6 crash workarround:
            'release_extra_cflags': '-fno-delete-null-pointer-checks',
        })
        cfg['system'].update({
            'use_system_bzip2' : 1,
            'use_system_flac' : 1,
            'use_system_xdg_utils' : 1,
            'use_system_yasm' : 1,
            'use_system_snappy' : 1,
            'use_system_harfbuzz' : 1,
        })
        return cfg

    def getTargets(self, opts):
        targets = super().getTargets(opts)
        if opts.test or opts.memcheck:
            targets += ['htmlshelltest']
        targets += ['eglplatform_shim_x11']
        return targets

    def customSetup(self):
        self.reconfigureFFMPEG('x64')

    def install(self, opts):
        super().install( opts )
        outDir = os.path.join( self._buildDir, 'out/Release' )
        scriptsSrcDir = os.path.join( self._commonDir, 'scripts' )

        self.installFile( outDir, self._installDir, 'libeglplatform_shim.so.1' )
        if opts.test or opts.memcheck:
            self.installFile( outDir, self._installDir, 'htmlshelltest' )
            testDir = os.path.join( self._installDir, 'test')
            testRsrcDir = os.path.join( self._commonDir, 'overlays', 'htmlshell', 'test', 'resources' )
            self.installDir( testRsrcDir, testDir, 'launcher' )
            self.installDir( testRsrcDir, testDir, 'apps' )
            self.installFile( outDir, os.path.join( testDir, 'launcher', 'libs'), 'libtestinteraction.so' )
            self.installFile( outDir, os.path.join( testDir, 'launcher', 'libs'), 'libtestinvalid.so' )
            self.installFile( outDir, os.path.join( testDir, 'launcher', 'libs'), 'libtestinitfail.so' )
            self.installFile( outDir, os.path.join( testDir, 'launcher', 'libs'), 'libtestinitsucceed.so' )
            self.installFile( outDir, os.path.join( testDir, 'launcher', 'libs'), 'libtestwebapi.so' )
            self.installFile( outDir, os.path.join( testDir, 'launcher', 'libs'), 'libtestapis.so' )
            self.createDir( testDir, os.path.join( 'profiles', 'system') )
            self.createDir( testDir, os.path.join( 'profiles', 'user') )
            self.installFile( scriptsSrcDir, self._installDir, 'test.sh' )
            if opts.memcheck:
                self.installFile( scriptsSrcDir, self._installDir, 'memcheck.sh' )
