#!/usr/bin/env python3

import glob
import os

class GenericDevice():

	def __init__(self, util, device):
		self.util = util
		self.device = device
		self.middlewareFilename = 'middleware'
		self.middlewareInstallDir = ''
		self.commonDir = util.tacAbsPath('build', 'devices', 'common')
		self.deviceDir = util.tacAbsPath('build', 'devices', device)

	def hostPlatform(self):
		return 'linux'

	def exports(self, buildForHost):
		# Called before build to export necesary environment variables (eg: CC and CXX)
		if buildForHost:
			os.environ['CC'] = 'gcc'
			os.environ['CXX'] = 'g++'
			os.environ['AR'] = 'ar'
		else:
			self.exportsImpl()

		if ('USE_CCACHE' in os.environ) and os.environ['USE_CCACHE'] == '1':
			os.environ['CCACHE_BASEDIR'] = os.environ['HOME']
			self.cCacheSet('CC',  'gcc')
			self.cCacheSet('CXX', 'g++')
			self.cCacheSet('AR',  'ar')

	def tacScripts(self):
		return [
			self.getPlatformFile('scripts', 'bootstrap.js'),
			self.getPlatformFile('scripts', 'run.sh'),
			self.getPlatformFile('scripts', 'impl.sh')
		]

	def patchFiles(self):
		wildcard = '%s-*.patch' % self.util.options.build_type
		patches = glob.glob(os.path.join(self.commonDir, 'patches', wildcard))
		patches += glob.glob(os.path.join(self.deviceDir, 'patches', wildcard))
		return patches

	# virtual
	def clean(self):
		excludeDirs = ['.build', 'original', 'node_modules'] + (['data'] if self.util.options.keep_data else [])
		if os.path.exists(self.util.options.deploy_dir):
			for item in os.listdir(self.util.options.deploy_dir):
				if not item in excludeDirs:
					self.util.rm(os.path.join(self.util.options.deploy_dir, item))

	# virtual
	def arch(self):
		return 'x64'

	# virtual
	def exportsImpl(self):
		pass

	# virtual
	def installTacImageFiles(self, destDir):
		return []

	# virtual
	def customPackMiddlewareStep(self, middlewareDir, middlewareEnv):
		pass

	# virtual
	def packMiddleware(self, destDir, packages):
		middlewareDir = os.path.join(destDir, 'fake_middleware')
		realMiddlewareDir = os.path.join(middlewareDir, 'middleware')
		self.util.mkdir(realMiddlewareDir)

		fMiddlewareEnv = os.path.join(middlewareDir, '%s.env' % self.middlewareFilename )
		open(fMiddlewareEnv, 'a').close()

		self.util.vlog('Copying middleware to tmpDir')
		self.util.cpDirContent('original', self.util.options.deploy_dir, os.path.join(realMiddlewareDir, self.middlewareInstallDir))

		self.customPackMiddlewareStep(realMiddlewareDir, fMiddlewareEnv)

		self.util.vlog('Setting middleware permissions')
		self.util.fakerootRun( fMiddlewareEnv, ['chmod','-R', 'go=u', realMiddlewareDir] )
		self.util.fakerootRun( fMiddlewareEnv, ['chmod','-R', 'go-w', realMiddlewareDir] )
		self.util.fakerootRun( fMiddlewareEnv, ['chown','-R', 'root:root', realMiddlewareDir])

		for p in packages:
			p.postProcessImage( realMiddlewareDir, fMiddlewareEnv )

		self.util.vlog('Packaging middleware')
		middlewareSrc = os.path.join(destDir, self.middlewareFilename)
		self.util.fakerootRun(fMiddlewareEnv, ['tar','--transform', 's#%s#.#' % realMiddlewareDir[1:],'-cf', middlewareSrc, realMiddlewareDir])
		self.util.rm(middlewareDir)

		self.util.vlog('Compressing middleware')
		self.util.gzipDir(middlewareSrc, os.path.join(destDir, '%s.tgz' % self.middlewareFilename ))
		self.util.rm(middlewareSrc)

		return [ '%s.tgz' % self.middlewareFilename ]

	# virtual
	def packageDepot(self, sufix):
		pass

	# virtual
	def genHashes(self, baseDir, files, out=''):
		pass

	# virtual
	def platformName(self):
		return 'UNKNOWN'

	def getPlatformFile(self, directory, filename):
		devDir = os.path.join( self.deviceDir, directory, filename )
		comDir = os.path.join( self.commonDir, directory, filename )
		return devDir if os.path.exists( devDir ) else comDir

	# private
	def cCacheSet(self, var, default):
		original_value = os.environ.get(var, default)
		if 'ccache' not in original_value:
			os.environ[var] = 'ccache %s' % original_value

