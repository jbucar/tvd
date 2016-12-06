#!/usr/bin/env python3

import glob
import imp
import os
from optparse import OptionParser, OptionGroup
import re
import sys

# Prevent python from generating .pyc
sys.dont_write_bytecode = True

default_build_opts = [
	'shell=ar.edu.unlp.info.lifia.tvd.shell'
]
default_web_opts = [
]

def parseOptions():
	global util
	tacRoot = os.path.dirname(os.path.abspath(__file__))
	parser = OptionParser()

	platforms = []
	for root, dirs, files in os.walk(os.path.join(tacRoot,'build','devices')):
		platforms.extend(dirs)
		break
	if 'common' in platforms:
		platforms.remove('common')

	groups = []
	group = OptionGroup(parser, "Build configuration")
	group.add_option('-P', '--platform'      ,dest='platform'      ,action='store'      ,type='choice' ,default='linux'  ,help='Target platform: (%s)' % ' '.join(platforms), choices=platforms)
	group.add_option('-p', '--packages'      ,dest='packages'      ,action='store'      ,type='string' ,default=''       ,help='Comma separated list of packages or .conf file' )
	group.add_option('-m', '--modules'       ,dest='modules'       ,action='store'      ,type='string' ,default=''       ,help='Comma separated list of modules' )
	group.add_option('-b', '--buildType'     ,dest='build_type'    ,action='store'      ,type='choice' ,default='debug'  ,help='Build Type: (release, debug)', choices=['release', 'debug'])
	group.add_option(      '--buildCfg'      ,dest='build_cfg'     ,action='store'      ,type='choice' ,default='dev'    ,help='Build Config: (dev, beta, stable)', choices=['dev', 'beta', 'stable'])
	group.add_option('-i', '--image'         ,dest='image'         ,action='store_true'                ,default=False    ,help='Create tac image' )
	group.add_option('-o', '--deploy-dir'    ,dest='deploy_dir'    ,action='store'      ,type='string' ,default=''       ,help='Directory where the deploy is generated (tpk files are extracted and scripts are copied)')
	group.add_option('-t', '--test'          ,dest='test'          ,action='store_true'                ,default=False    ,help='Run tests before build')
	group.add_option('-v', '--verbose'       ,dest='verbose'       ,action='store'      ,type='int'    ,default=0        ,help='Verbose logging')
	group.add_option('-e', '--extra'         ,dest='extra_tpks'    ,action='store'      ,type='string' ,default=''       ,help='Extra tpk directory')
	group.add_option(      '--clean-data'    ,dest='keep_data'     ,action='store_false'               ,default=True     ,help='Clean deployed data directory')
	group.add_option(      '--use-cache'     ,dest='use_cache'     ,action='store_true'                ,default=False    ,help='Use cached packages tpks instead of re-building')
	group.add_option(      '--version'       ,dest='version'       ,action='store'      ,type='string' ,default=''       ,help='Provide version instead of getting from "git describe --tags"')
	group.add_option(      '--build-deps'    ,dest='build_deps'    ,action='store_true'                ,default=False    ,help='Build tac dependencies if not found in <deps-dir>')
	group.add_option(      '--set-build'     ,dest='build_opts'    ,action='append'     ,type='string' ,default=default_build_opts ,help='Adds key=value to ar.xxxx.platform/build.json')
	group.add_option(      '--set-web'       ,dest='web_opts'      ,action='append'     ,type='string' ,default=default_web_opts   ,help='Adds key=value to web build script')
	parser.add_option_group(group)
	groups.append(group)

	group = OptionGroup(parser, "Buildroot depot")
	group.add_option(      '--depot'      ,dest='depot'      ,action='store'      ,type='string' ,default=''    ,help='Path to a JSON file describing the depot to use')
	group.add_option(      '--pkg-depot'  ,dest='pkg_depot'  ,action='store_true'                ,default=False ,help='Generate a packaged depot file (with the contents provided by --depot=manifest.json)')
	group.add_option(      '--depot-sufix',dest='depot_sufix',action='store'      ,type='string' ,default=''    ,help='Sufix appended to the filename of the packaged depot (depot-<sufix>.tgz)')
	parser.add_option_group(group)
	groups.append(group)

	(opts, args) = parser.parse_args()

	module = imp.load_source('utils', os.path.join(tacRoot, 'build', 'buildutils.py'))
	util = module.BuildUtils(tacRoot, opts)
	global BuildError
	BuildError = module.BuildError

	makePathsAbsolute( util )

	checkBuildConfig()

	pkgs = ','.join(getPkgList())
	util.log('Current options:', True)
	for g in groups:
		for option in g.option_list:
			if not option.dest: continue
			if option.dest == 'packages':
				util.log('\t%s: %s' % (option.dest, pkgs))
			else:
				util.log('\t%s: %s' % (option.dest, eval('opts.' + option.dest)))

	return util

def makePathsAbsolute(util):
	if util.options.image:
		# Create tmp directory for deploying the tac image
		util.options.deploy_dir = util.addTmpDir('image')
	elif util.options.deploy_dir:
		util.options.deploy_dir = os.path.abspath(util.options.deploy_dir)
	else:
		util.options.deploy_dir = os.getcwd()
		if util.options.deploy_dir == util.tacRoot:
			util.fail('You are trying to build in-source. Use -o or build in a diferent directory!')
	util.originalDir = os.path.join( util.options.deploy_dir, 'original' )

	if util.options.packages and not re.match("^(\w,?)+$", util.options.packages):
		util.options.packages = os.path.abspath( util.options.packages )
	if util.options.extra_tpks:
		util.options.extra_tpks = os.path.abspath( util.options.extra_tpks )
	if util.options.depot:
		util.options.depot = os.path.abspath( util.options.depot )

def getPkgId( pkg ):
	manifest = util.loadJSON( util.tacAbsPath('packages', pkg, 'manifest.json') )
	return manifest['id']

def cleanOriginalDir( pkgsNames ):
	origDir = os.path.join( util.originalDir, 'packages' )
	allowedPkgs = [getPkgId(pkg) for pkg in pkgsNames]
	for p in os.listdir( origDir ):
		if not p in allowedPkgs:
			util.rm( os.path.join( origDir, p ) )

def checkPkgList( pkgsNames ):
	for p in getPkgList():
		if not p in pkgsNames:
			util.fail( 'Fail to build package %s! Not present in current build config: %s' % (p, util.cfgFile) )

def checkBuildConfig():
	if re.match("^(\w,?)+$", util.options.packages):
		# Individual packages build
		buildFile = os.path.join( util.originalDir, 'packages', getPkgId('platform'), 'build.json' )
		if os.path.exists(buildFile):
			util.cfgFile = util.tacAbsPath( 'build', util.loadJSON( buildFile )['build']['file'] )
			pkgsNames = util.loadJSON( util.cfgFile )['packages']
			checkPkgList( pkgsNames )
			cleanOriginalDir( pkgsNames );
		else:
			util.fail( 'Fail to build individual packages. You need to make a full build first!' )
	elif util.options.packages:
		# Full build
		util.cfgFile = util.options.packages
		cfgJSON = util.loadJSON( util.cfgFile )
		util.options.packages = ','.join( cfgJSON['packages'] )
		util.modules += cfgJSON['modules']
		util.mkdir( os.path.join(util.originalDir, 'packages'), True )

	if re.match("^(\w,?)+$", util.options.modules):
		extraModules = util.options.modules.split(',')
		for m in extraModules:
			if not m in util.modules:
				util.modules.append(m)

def validateOptions():
	if not util.options.platform:
		util.fail('Platform not specified')

def loadDevice():
	device = None
	deviceFile = util.tacAbsPath( 'build', 'devices', util.options.platform, 'device.py' )
	if os.path.exists( deviceFile ):
		util.log( 'Loading %s device: %s' % (util.options.platform, deviceFile), True )
		module = imp.load_source( 'config', deviceFile )
		device = module.Device( util )
	else:
		util.log( 'Loading generic device for platform: %s' % util.options.platform, True )
		module = imp.load_source( 'config', util.tacAbsPath('build', 'devices', 'generic_device.py') )
		device = module.GenericDevice( util, util.options.platform )

	if device:
		util.device = device
	else:
		util.fail( 'Fail to load device for platform: %s' % util.options.platform )

def getPkgList():
	# Parse packages list
	return [x for x in util.options.packages.split(',') if x]

def installNodeModules():
	destDir = os.path.join(util.originalDir, 'node_modules')
	util.mkdir(destDir)
	util.indent('Installing node_modules in: %s' % destDir)

	# Process tac/packages/node_modules
	for module in os.listdir( util.tacAbsPath('packages', 'node_modules') ):
		if module in util.modules:
			builder = getBuilder( os.path.join('packages', 'node_modules'), module, util.options.platform, destDir, 'module_builder.py' )
			builder.install()
			builder.clean()
			builder.build()
			if util.options.test:
				builder.test()

	util.unindent()

def installExtraTpks():
	util.indent('Installing extra tpks')

	for tpk in glob.glob(os.path.join(util.options.extra_tpks, '*.tpk')):
		extraTmpDir = util.addTmpDir('extra_pkg')
		util.extractFile(tpk, extraTmpDir, False)

		# Read id from manifest.json
		manifest = util.loadJSON(os.path.join(extraTmpDir, 'manifest.json'))
		fqn = manifest['id']
		util.vlog("Installing extra package %s from %s" % (fqn, tpk))
		tpkDestDir = os.path.join(util.originalDir, 'packages', fqn)
		util.rm(tpkDestDir)
		util.cpDirContent('', extraTmpDir, tpkDestDir)

	util.unindent()

def installInitScripts():
	util.indent( 'Installing init scripts' )
	for f in util.device.tacScripts():
		util.cp( '', f, util.originalDir )
	util.unindent()

def getBuilder(baseDir, name, platform, destDir, genericBuilder):
	# Try to find custom builder for name, if not found load generic builder
	builderFile = util.tacAbsPath(baseDir, name, 'builder.py')
	if not os.path.exists(builderFile):
		builderFile = util.tacAbsPath('build', genericBuilder)
		if not os.path.exists(builderFile):
			util.fail('Cannot found builder for: %s' % name)

	util.vlog('Loading builder for %s, platform=%s, builder=%s' % (name, platform, builderFile))
	module = imp.load_source('builder', builderFile)

	return module.Builder(name, platform, destDir, util)

def processPackage(pkgName, platform, destDir, symsDir, buildForHost=False):
	util.vlog('')
	util.vlog('================================================================================')
	util.log('%sProcessing package %s for %s platform: %s' % ('             ' if util.options.verbose else '', pkgName, 'host' if buildForHost else 'target', platform), True)
	util.vlog('================================================================================')
	util.indent()

	# builder = getPkgBuilder(pkgName, platform, destDir)
	builder = getBuilder('packages', pkgName, platform, destDir, 'generic_builder.py')

	# Delete from cache if necesary (not --use-cache)
	if not util.options.use_cache:
		if pkgName != 'node':
			util.rmAll( builder.tpkCacheFiles() )
			util.rmAll( builder.symsCacheFiles() )

	# Cleanup deployed files
	builder.clean()

	# Build if not in cache
	if not builder.isCached():
		builder.build(buildForHost)

	# Install in deploy dir (-o)
	builder.install(symsDir, buildForHost)

	# Run linter on package
	builder.lint()

	# Run tests (-t)
	if util.options.test:
		builder.test()

	util.unindent()
	util.vlog('================================================================================')
	util.vlog('')

	return builder

def tacVersion():
	return '%s.%s' % (util.TAC_VERSION, '.'.join(util.getVersion().split('-')[1:2]))

def createTacImage(packages):
	util.indent('Packaging image')

	# Create tmp directory structure (tmp_tac_image_####)
	destDir = util.addTmpDir('tac_image')

	fnamelst = util.device.installTacImageFiles(destDir)

	# pack middleware
	fnamelst += util.device.packMiddleware(destDir, packages)

	# Generate md5 hashes and create image file
	util.device.genHashes(destDir, fnamelst)
	util.createTarInCache( destDir, 'image-%s-%s.tac' % (util.options.platform, tacVersion()) )

	util.unindent()

def createTacSyms(symDir):
	util.indent('Packaging symbols')

	# Generate md5 hashes and create deps file
	tmpDepsDir = util.addTmpDir('deps')
	depsDir = os.path.join(tmpDepsDir, '.build', 'deps')
	util.cpDirContent( '', util.depsCache, depsDir )
	util.device.genHashes(depsDir, os.listdir(depsDir), 'deps.md5')
	util.createTarInCache( tmpDepsDir, 'image-%s-%s.sdk' % (util.options.platform, tacVersion()) )

	# Generate md5 hashes and create symbols file
	util.device.genHashes(symDir, os.listdir(symDir), 'symbols.md5')
	util.createTarInCache( symDir, 'image-%s-%s.tac.sym' % (util.options.platform, tacVersion()) )

	util.unindent()

def checkPackagesDeps():
	cfgFile = '/tmp/tac-packages.json'
	util.saveJSON(cfgFile, {'packages': getPkgList()})
	util.log('Checking packages dependencies with %s' % cfgFile, True)
	util.run([util.tacAbsPath('tools', 'check_deps.js'), cfgFile])
	util.rm(cfgFile)

def needBuildNodeForHost():
	nodeHostBuild = os.path.join( util.buildCache, 'node', 'bin', 'node' )
	nodeHostDep = os.path.join( util.buildCache, 'deps', 'node-%s-%s.tar.gz' % (util.device.hostPlatform(), util.getDepVersion('node')) )
	return not os.path.exists(nodeHostBuild) or not os.path.exists(nodeHostDep) or util.options.build_deps

def setupNodeHost():
	destDir = os.path.join(util.buildCache, 'node')

	baseDir = os.path.join( os.path.os.path.dirname(__file__), 'packages', 'node' )
	nodeDirs = os.path.join( baseDir, 'node_modules' ) + ':' + os.path.join( baseDir, 'node_modules_dev' )
	util.log('Node modules: %s' % nodeDirs )
	os.environ['NODE_PATH'] = nodeDirs

def build():
	# Clean deploy directory
	util.device.clean()

	# Create tmp directory for symbols
	symsDir = util.addTmpDir('syms')

	setupNodeHost()

	checkPackagesDeps()

	# Build and deploy/install packages
	packages = []
	for p in getPkgList():
		util.device.exports(False)
		packages.append( processPackage(p, util.options.platform, util.options.deploy_dir, symsDir) )

	# Install node_modules
	if util.modules:
		installNodeModules()

	# Install extra tpks (-e)
	if util.options.extra_tpks:
		installExtraTpks()

	# Install tac init scripts
	installInitScripts()

	# Create data dir if not exists
	util.mkdir(os.path.join(util.options.deploy_dir, 'data'))

	# Create tac image (-i)
	if util.options.image:
		createTacImage(packages)
		createTacSyms(symsDir)

	# Package depot if necesary
	if util.options.depot and util.options.pkg_depot:
		util.device.packageDepot( util.options.depot_sufix )

def checkHostMachine():
	(host_os, host_node, host_release, host_version, host_machine) = os.uname()
	if not (host_os=='Linux' and host_machine=='x86_64'):
		util.fail('You need a 64-bit Linux distribution to build TAC-TV')

def main():
	try:
		# Parse command line arguments and create BuildUtils
		parseOptions()

		# Check we are building in a valid host
		checkHostMachine()

		# Validate command line arguments
		validateOptions()

		# Load build implementation for platform
		loadDevice()

		# Perform the build
		build()

	except SystemExit as exit:
		# Catch -h/--help SystemExit
		raise exit
	except BuildError as err:
		util.terminate(err, sys.exc_info()[2])
	except:
		util.terminate('Unexpected error: %s' % sys.exc_info()[0], sys.exc_info()[2])

# Setup locale
os.environ['LC_ALL'] = 'en_US.UTF-8'

util = None

# Run main
main()

# Fin build successfully
util.cleanUp()

