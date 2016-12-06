#!/usr/bin/env python3

import json
import os
import shutil
import stat
import subprocess

global_config = {
    'google_api_key': 'AIzaSyDwr302FpOSkGRpLlUpPThNTDPbXcIn_FM',
    'google_default_client_id': '413772536636.apps.googleusercontent.com',
    'google_default_client_secret': '0ZChLK6AxeA3Isu96MkwqDR4',
    'logging_like_official_build' : 1,
    'werror' : '',
    'python_ver' : '2.7',
    'disable_fatal_linker_warnings' : 1,
    'remove_webcore_debug_symbols' : 1,
    'disable_glibc' : 1,
    'disable_nacl' : 1,
    'disable_pnacl' : 1,
    'component' : 'static_library',
    'proprietary_codecs' : 1,
    'ffmpeg_branding' : 'Chrome',
    'chromeos' : 0,
    'use_allocator' : 'none',
    'use_video_proxy': '',
    'linux_dump_symbols': 1,
    'v8_use_external_startup_data': 0,
    'icu_use_data_file_flag' : 0,
    'enable_plugins' : 1,
    'enable_basic_printing' : 0,
    'enable_print_preview' : 0,
    'enable_app_list': 0,
    'linux_use_bundled_binutils' : 0,
    'linux_use_bundled_gold' : 0,
#ozone
    'use_aura' : 1,
    'use_ozone' : 1,
    'toolkit_views' : 0,
    'ozone_platform_caca' : 0,
    'ozone_platform_cast' : 0,
    'ozone_platform_egltest' : 0,
    'ozone_platform_fbdev' : 0,
    'ozone_platform_gbm' : 0,
    'ozone_platform_headless' : 0,
    'ozone_platform_ozonex' : 0,
# Widevine:
    'enable_pepper_cdms': 1,
    'enable_widevine': 1,
# libs
    'use_ash' : 0,
    'use_cairo' : 0,
    'use_glib' : 0,
    'use_gconf' : 0,
    'use_gio' : 0,
    'use_gnome_keyring' : 0,
    'use_pango' : 0,
    'use_clipboard_aurax11' : 0,
    'use_x11' : 0,
    'use_alsa': 1,
    'use_pulseaudio': 1,
    'linux_link_pulseaudio': 1
}

system_config = {
    'use_system_libevent' : 1,
    'use_system_libjpeg' : 1,
    'use_system_libpng' : 1,
    'use_system_ffmpeg' : 0,
    'use_system_harfbuzz' : 0,
    'use_system_libxml' : 0,
    'use_system_ssl' : 0,
    'use_system_snappy' : 0,
    'use_system_zlib' : 0,
}

def touch(fname):
    with open(fname, 'a'):
        os.utime(fname, None)

def files(path):
    l = []
    if os.path.exists( path ):
        for f in os.listdir(path):
            if os.path.isfile(os.path.join(path, f)):
                l.append( f )
        l.sort()
    return l

def dirs(path):
    d = []
    if os.path.exists( path ):
        for name in os.listdir(path):
            if os.path.isdir(os.path.join(path, name)):
                d.append(name)
    return d

def loadJSON(jsonFile):
    if os.path.exists(jsonFile):
        with open(jsonFile, 'r') as f:
            return json.load(f)

class GenericConfig():

    def __init__(self, buildDir, installDir, deviceDir):
        print( 'Generic config initialized:\n\tbuildDir: %s\n\tinstallDir: %s\n\tdeviceDir: %s' % (buildDir, installDir, deviceDir) )
        self._buildDir = buildDir
        self._installDir = installDir
        self._deviceDir = deviceDir
        self._commonDir = os.path.dirname( os.path.realpath(__file__) )
        self._patchesDir = os.path.join( os.path.dirname( self._commonDir ), 'patches' )
        self._deviceOverlays = dirs( os.path.join( self._deviceDir, 'overlays' ) )
        self._commonOverlays = dirs( os.path.join( self._commonDir, 'overlays' ) )

    def getGypConfig(self):
        return {'global': global_config, 'system': system_config}

    def setupEnv(self, opts):
        print('Runing generic setupEnv')
        buildWithCCache = (os.environ['USE_CCACHE'] == '1') if ('USE_CCACHE' in os.environ) else True
        if opts.use_ccache and buildWithCCache:
            if not 'CCACHE_BASEDIR' in os.environ:
                os.environ['CCACHE_BASEDIR'] = os.environ['HOME']
            os.environ['CC'] = 'ccache %s' % (os.environ['CC'] if 'CC' in os.environ else 'gcc')
            os.environ['CXX'] = 'ccache %s' % (os.environ['CXX'] if 'CXX' in os.environ else 'g++')
            os.environ['AR'] = 'ccache %s' % (os.environ['AR'] if 'AR' in os.environ else 'ar')
            os.environ['CC_host'] = 'ccache %s' % (os.environ['CC_host'] if 'CC_host' in os.environ else 'gcc')
            os.environ['CXX_host'] = 'ccache %s' % (os.environ['CXX_host'] if 'CXX_host' in os.environ else 'g++')

        print('Build environment:')
        print('\tPATH: %s' % os.environ['PATH'])
        print('\tCC: %s' % os.environ['CC'] if 'CC' in os.environ else '')
        print('\tCXX: %s' % os.environ['CXX'] if 'CXX' in os.environ else '')
        print('\tCC_host: %s' % os.environ['CC_host'] if 'CC_host' in os.environ else '')
        print('\tCXX_host: %s' % os.environ['CXX_host'] if 'CXX_host' in os.environ else '')

    def customSetup(self):
        pass

    def getTargets(self, opts):
        return ['chrome_sandbox', 'dump_syms', 'minidump_stackwalk', 'htmlshell']

    def getGitDirs(self):
        return ['base', 'content', 'media', 'ui'] + self._commonOverlays + self._deviceOverlays

    def postBuild(self, opts):
        if opts.strip:
            print( 'Generating debug symbols' )
            subprocess.check_call(['./components/crash/content/tools/generate_breakpad_symbols.py', '--build-dir=out/Release', '--binary=out/Release/htmlshell', '--symbols-dir=out/Release/htmlshell.syms.d', '--clear', '-j5'])
            self.strip()

    def strip(self):
        binary = '%s/out/Release/htmlshell' % self._buildDir
        print( 'Striping %s' % binary )
        subprocess.check_call(['strip', '--strip-unneeded', binary])

    # Copy overlay directories to chromium base
    def prepareSources(self):
        self.installOverlays( self._commonDir, self._commonOverlays )
        self.installOverlays( self._deviceDir, self._deviceOverlays )

    def initGit(self):
        print( 'Initializing git repository in ' + self._buildDir )
        if os.path.isdir( os.path.join(self._buildDir,'.git') ):
            print( 'Removing existing git repository in ' + self._buildDir )
            shutil.rmtree( os.path.join(self._buildDir,'.git') )

        subprocess.check_call(['git', 'init', '.'])
        subprocess.check_call(['git', 'add'] + self.getGitDirs())

    # Apply generic and device specific patches if necesary
    def patch(self):
        self.applyPatches( "common", os.path.join( self._commonDir, 'patches.json') )
        self.applyPatches( os.path.basename( self._deviceDir ), os.path.join( self._deviceDir, 'patches.json') )

    def install(self, opts):
        outDir = os.path.join( self._buildDir, 'out/Release' )

        # Install htmlshell and chrome-sandbox if not already installed
        self.installFile( self._deviceDir, self._installDir, 'run.sh' )
        self.installFile( outDir, self._installDir, 'htmlshell' )
        sandbox = os.path.join(self._installDir, 'chrome-sandbox')
        if not os.path.exists( sandbox ):
            self.installFile( outDir, self._installDir, 'chrome_sandbox', 'chrome-sandbox' )
            os.chmod( sandbox, stat.S_ISUID | stat.S_IRUSR | stat.S_IWUSR | stat.S_IXUSR | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH )

        # Install resources needed by user application (need to be placed next to exe)
        self.installFile( outDir, self._installDir, 'htmlshell.pak')

        # Install default system and user profile
        self.installDir( self._commonDir, self._installDir, 'profiles' )

        # Install htmlshell symbols and dump tools
        if os.path.exists( os.path.join(outDir, 'htmlshell.syms.d') ):
            self.installDir( outDir, self._installDir, 'htmlshell.syms.d')
        self.installFile( os.path.join( self._buildDir, 'components', 'crash', 'content', 'tools' ), self._installDir, 'dmp2minidump.py')
        self.installFile( outDir, self._installDir, 'minidump_stackwalk' )

#private:
    def applyPatches(self, name, jsonFile):
        patchAppliedFile = os.path.join( self._buildDir, '.patch_%s' % name )
        if not os.path.exists( patchAppliedFile ):
            print( 'Applying patch list: ' + jsonFile )
            patches = loadJSON(jsonFile)
            for f in patches:
                subprocess.check_call(['patch', '-p0', '-i', os.path.join(self._patchesDir, f)])
            touch( patchAppliedFile )
        else:
            print( 'Patches for %s already applied, skiping!' % name )

    def createDir(self, parent, new=''):
        directory = os.path.join( parent, new )
        if not os.path.exists( directory ):
            os.makedirs( directory, 0o755 )

    def makeInstallNames(self, srcDir, destDir, src, dst):
        if dst == '':
            dst = src
        src = os.path.join( srcDir, src )
        dst = os.path.join( destDir, dst )
        print( 'Install: src=%s, dst=%s' % (src,dst) )
        return { 'src':src, 'dst':dst }

    def installFile(self, srcDir, destDir, src, dst=''):
        self.createDir( destDir )
        names = self.makeInstallNames(srcDir, destDir, src, dst)
        shutil.copy( names['src'], names['dst'] )

    def installDir(self, srcDir, destDir, src, dst=''):
        names = self.makeInstallNames(srcDir, destDir, src, dst)
        self.cpDirContent(names['src'], names['dst'])

    def installOverlays(self, path, overlays):
        print( 'Installing overlays in %s' % path )
        for o in overlays:
            self.installDir( os.path.join(path, 'overlays'), self._buildDir, o )

    def reconfigureFFMPEG(self, arch):
        ffmpegCfgFile = os.path.join( self._buildDir, '.ffmpeg_configured' )
        if not os.path.exists( ffmpegCfgFile ):
            print("Reconfiguring chromium ffmpeg..")
            currDir = os.getcwd()
            os.chdir( os.path.join(self._buildDir, 'third_party', 'ffmpeg') )
            subprocess.check_call([os.path.join('chromium', 'scripts', 'build_ffmpeg.py'), 'linux', arch, '--branding', 'Chrome'])
            subprocess.check_call([os.path.join('chromium', 'scripts', 'copy_config.sh')])
            subprocess.check_call([os.path.join('chromium', 'scripts', 'generate_gyp.py')])
            ffmpeg_stubs = os.path.join(self._buildDir, 'out', 'Release', 'gen', 'ffmpeg', 'third_party', 'ffmpeg', 'ffmpeg_stubs.h')
            if os.path.exists( ffmpeg_stubs ):
                os.remove( ffmpeg_stubs )
            os.chdir( currDir )
            touch( ffmpegCfgFile )
        else:
            print( "FFMPEG already reconfigured, skiping!" )

    def cpDirContent(self, src, dest):
        if not os.path.exists( dest ):
            os.makedirs(dest, 0o755)
        for item in os.listdir(src):
            self.cp(item, src, dest)

    def cp(self, name, src, dst):
        srcPath = os.path.join(src, name) if name else src
        if os.path.exists(srcPath):
            if os.path.isfile(srcPath):
                shutil.copy(srcPath, dst)
            elif os.path.isdir(srcPath):
                destPath = os.path.join(dst, name)
                if os.path.exists(destPath):
                    self.cpDirContent(os.path.join(src, name), destPath)
                else:
                    shutil.copytree(srcPath, destPath, True)
