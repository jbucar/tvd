#!/usr/bin/env python3

import json
import sys
import os
from optparse import OptionParser
import urllib.request
import tarfile
import subprocess
import imp
import locale

sys.dont_write_bytecode = True

htmlshell_version = '1.49'
chromium_version = '49.0.2623.87'
chromium_src_file = 'chromium-%s.tar.xz' % chromium_version
chromium_repo_url = 'http://gsdview.appspot.com/chromium-browser-official/' + chromium_src_file
chromium_valgrind_url = 'https://chromium.googlesource.com/chromium/deps/valgrind/binaries.git'

# methods
def getOptions():
    rootDir = os.path.dirname(os.path.abspath(__file__))
    platforms = []
    for root, dirs, files in os.walk(os.path.join(rootDir,'devices')):
        platforms.extend(dirs)
        break

    parser = OptionParser()
    parser.add_option("-s", "--setup", dest="setup", action="store", type="choice", default="", help="Target platform: (%s)" % " ".join(platforms), choices=platforms + [""])
    parser.add_option("-b", "--build", dest="build", action="store_true", default=False, help="Build chromium sources")
    parser.add_option("-i", "--install", dest="install", action="store_true", default=False, help="Install binary")
    parser.add_option("-m", "--memcheck", dest="memcheck", action="store_true", default=False, help="Setup to run memcheck")
    parser.add_option("-p", "--pack", dest="pack", action="store_true", default=False, help="Generate tar.gz")
    parser.add_option("-t", "--test", dest="test", action="store_true", default=False, help="Create and install htmlshelltest")
    parser.add_option("-g", "--git", dest="initgit", action="store_true", default=False, help="Init a git repository with chromium sources")
    parser.add_option("-d", "--no-strip", dest="strip", action="store_false", default=True, help="Prevent strip of htmlshell binary")
    parser.add_option("-c", "--clean-build", dest="use_ccache", action="store_false", default=True, help="Do not use ccache")
    parser.add_option("--install-dir", dest="installdir", action="store", type="string", default="", help="Install directory")
    parser.add_option("--dcheck-on", dest="enable_dchecks", action="store_true", default=False, help="Release build with debug checks enabled")
    parser.add_option("--depot", dest="depot", action="store", type="string", default="", help="Path to a JSON file describing the depot to use")
    (ops, _) = parser.parse_args()
    return ops

def downloadDir():
    return os.environ['TVD_PACKAGES'] if "TVD_PACKAGES" in os.environ else '/tmp/tvd_packages'

def getConfigFile(device):
    return os.path.join( os.environ['DEPOT'], 'tool', 'htmlshell', 'devices', device, 'config.py' )

def downloadFile():
    return os.path.join( downloadDir(), chromium_src_file )

def download():
    # Validate if source exists
    out = downloadFile()
    if not os.path.exists( out ):
        print( "Downloading chromium sources from " + chromium_repo_url + ' to ' + out )
        tgz = urllib.request.urlopen(chromium_repo_url)
        output = open(out,'wb')
        output.write(tgz.read())
        output.close()

def extract(device):
    if not os.path.exists( os.path.join(_buildDir, 'AUTHORS') ):
        f = downloadFile()
        print( "Extracting chromium sources from " + f + " in " + _buildDir)
        assert( os.path.exists( f ) )
        try:
            tgz = tarfile.TarFile( f, 'r' )
            tgz.extractAll()
            tgz.close()
        except:
            subprocess.check_call(['tar', 'xf', f, '--strip', '1'])

        cfg = getConfig( device )

        # Create .gitignore
        f = open( os.path.join(_buildDir,'.gitignore'), 'w')
        f.write( '/*\n' )
        for gitDir in cfg.getGitDirs():
            f.write( '!/%s\n' % gitDir )
        f.close()

def excludeFromPack( filename ):
    if filename.startswith( os.path.join(_installDir, 'test') ):
        print("%s excluded from pack" % filename)
        return True
    elif filename.startswith( os.path.join(_installDir, 'htmlshelltest') ):
        print("%s excluded from pack" % filename)
        return True
    elif filename.startswith( os.path.join(_installDir, 'htmlshell-') ):
        print("%s excluded from pack" % filename)
        return True
    else:
        print("%s included in pack" % filename)
        # Include in pack
        return False

def pack(tar_filename, arcname):
    print('Packaging output to %s' % tar_filename)
    tar = tarfile.open(tar_filename, 'w:gz')
    tar.add(_installDir, arcname=arcname, exclude=excludeFromPack)
    tar.close()

def setupChromiumValgrind():
    srcDir = os.path.join( downloadDir(), 'third_party', 'valgrind')
    dstDir = os.path.join( _buildDir, 'third_party', 'valgrind' )

    # Download chromium-valgrind binaries if necesary
    if not os.path.exists( srcDir ):
        subprocess.check_call(['git', 'clone', chromium_valgrind_url, srcDir])

    # Create symlink to chromium-valgrind binaries if necesary
    if not os.path.exists( dstDir ):
        os.symlink( srcDir, dstDir )

def prepareSources( opts ):
    print( 'Prepare source code' )
    device = opts.setup
    download()
    extract(device)
    if opts.memcheck:
        setupChromiumValgrind();

    cfg = getConfig( device )
    cfg.prepareSources()

    if opts.initgit:
        cfg.initGit()
        subprocess.check_call(['git', 'commit', '-m', 'Initial commit', '--quiet'])

    cfg.patch()

    if opts.initgit:
        subprocess.check_call(['git', 'commit', '-am', 'Patches applied', '--quiet'])

def setupBuild( opts ):
    os.chdir( _buildDir )
    device = opts.setup
    configureDevice( device, opts )
    saveConfig( device )

def getConfig( device ):
    global _config
    if _config is None:
        print( 'Loading config for device: %s' % device )
        module = imp.load_source( 'config', getConfigFile( device ) )
        _config = module.Config( _buildDir, _installDir )
    return _config

def configureDevice( device, opts ):
    # Load device
    cfg = getConfig( device )

    # Setup environment
    cfg.setupEnv(opts)

    # Execute custom device configuration
    cfg.customSetup()

    gypCfg = cfg.getGypConfig()
    if opts.memcheck:
        gypCfg['global']['build_for_tool'] = 'memcheck';
    if opts.enable_dchecks:
        gypCfg['global']['dcheck_always_on'] = 1;

    # Fix toolchain
    if 'sysroot' in gypCfg['global']:
        sysrootVar = gypCfg['global']['sysroot']
        if not os.path.exists( sysrootVar ):
            assert(sysrootVar in os.environ)
            gypCfg['global']['sysroot'] = os.environ[sysrootVar]

    # Prepare clang if used
    if 'clang' in gypCfg['global'] and gypCfg['global']['clang'] == 1:
        #mkdir -p third_party/llvm-build/Release+Asserts/bin
        dir = os.path.join( _buildDir, 'third_party', 'llvm-build', 'Release+Asserts', 'bin' )
        if not os.path.exists( dir ):
            os.makedirs( dir, 0o755 )
            # ln -s /usr/bin/clang third_party/llvm-build/Release+Asserts/bin/
            os.symlink( '/usr/bin/clang', os.path.join( dir, 'clang' ) )
            # ln -s /usr/bin/clang++ third_party/llvm-build/Release+Asserts/bin/
            os.symlink( '/usr/bin/clang++', os.path.join( dir, 'clang++' ) )
            # ln -s /usr/bin/llvm-symbolizer third_party/llvm-build/Release+Asserts/bin/
            os.symlink( '/usr/bin/llvm-symbolizer', os.path.join( dir, 'llvm-symbolizer' ))

    # Convert parameters
    system_flags = ''
    for key,value in gypCfg['system'].items():
        system_flags += ' -D' + key + '=' + str(value)

    global_flags = ''
    for key,value in gypCfg['global'].items():
        global_flags += ' -D' + key + '=' + str(value)

    # print( "system flags: " + system_flags )
    # print( "\n\nglobal flags: " + global_flags )
    all_flags = global_flags + ' ' + system_flags

    # Replace for system libraries
    print( 'Replace build to use system libraries' )
    args = ['python2', 'build/linux/unbundle/replace_gyp_files.py']
    args += system_flags.split()
    subprocess.check_call(args)

    # Configure
    print( 'Configure chromium sources' )
    args = ['python2', 'build/gyp_chromium', '--depth=.', 'htmlshell/htmlshell.gyp', 'htmlshell/htmlshelltest.gyp']
    args += all_flags.split()
    subprocess.check_call(args)

def setupFile():
    return os.path.join( _buildDir, '.setup.conf' )

def getDevice():
    cfgFile = setupFile()
    if not os.path.exists( cfgFile ):
        print( 'Config file not found!' )
        raise error( 'Config file not found!' )

    with open( cfgFile, 'r' ) as f:
        device = f.read()
        f.close()
    return device

def saveConfig( device ):
    cfgFile = setupFile()
    if not os.path.exists( cfgFile ):
        with open( cfgFile, 'a' ) as f:
            f.write( device )
            f.close()

def loadJSON(jsonFile):
    if os.path.exists(jsonFile):
        with open(jsonFile, 'r') as f:
            return json.load(f)

def getFullPath(manifest, key, rootDir):
    if (key in manifest):
        if os.path.isabs( manifest[key] ):
            return manifest[key]
        else:
            return os.path.join( rootDir, manifest[key] )
    return ""

def main(opts):
    # 0: Export buildroot depot if necesary
    if opts.depot:
        filename = os.path.abspath( opts.depot )
        print( 'Loading buildroot depot manifest: %s' % filename )
        manifest = loadJSON( filename )
        os.environ['TVD_DEPOT_TOOLCHAIN_PATH'] = getFullPath( manifest, 'toolchain', os.path.dirname(filename) )
        os.environ['TVD_DEPOT_SYSROOT'] = getFullPath( manifest, 'staging', os.path.dirname(filename) )
        if 'toolchain_prefix' in manifest:
            os.environ['TVD_DEPOT_TOOLCHAIN_PREFIX'] = manifest['toolchain_prefix']

    # 1: Setup if necesary (-s <arch>)
    if opts.setup:
        if not os.path.isdir(_installDir):
            os.mkdir(_installDir)
        os.chdir( _buildDir )
        # 1.1: Prepare sources (download, extract and apply generic patches)
        prepareSources( opts )
        # 1.2: Configure device and run gyp
        setupBuild( opts )

    cfg = getConfig( getDevice() )

    # 2: Make the build if necesary (-b)
    if opts.build:
        os.chdir(_buildDir)

        if not opts.setup:
            cfg.setupEnv(opts)

        for target in cfg.getTargets(opts):
            print( "Build out/Release/%s" % (target) )
            subprocess.check_call(['ninja', '-C', 'out/Release', target])

        cfg.postBuild(opts)

    # 3: Deploy built binaries to install directory if necesary (-i or -p)
    if opts.install or opts.pack:
        cfg.install( opts )

    # 4: Create htmlshell.tar.gz if necesary (-p)
    if opts.pack:
        os.chdir(_installDir)
        commit = '100'
        try:
            tmpVer = subprocess.check_output( ['git', '--git-dir=%s/.git' % os.environ['DEPOT'], 'describe', '--tags'] ).strip()
            commit = tmpVer.decode(locale.getdefaultlocale()[1]).split('-')[1]
        except:
            pass
        pack('htmlshell-%s.%s.tar.gz' % (htmlshell_version, commit), 'htmlshell')

# Init and get cmd options
oldEnv = os.environ
opts = getOptions()

# Setup global directories
_buildDir = os.getcwd()
_installDir = opts.installdir if opts.installdir else os.path.join(_buildDir, "rootfs")
_config = None

# Run main
main(opts)

# Restore environment
os.environ = oldEnv
