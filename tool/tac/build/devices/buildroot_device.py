#!/usr/bin/env python3

import imp
import os

module = imp.load_source('generic_device', os.path.join(os.path.dirname(__file__), 'generic_device.py'))

HASH_FILENAME = 'check.md5'

class BuildrootDevice( module.GenericDevice ):

	def __init__(self, util, device):
		super().__init__( util, device )
		self.toolchainPrefix = 'unknown'

	# override
	def arch(self):
		return self.toolchainPrefix.split('-')[0] if self.toolchainPrefix else super().arch()

	# override
	def platformName(self):
		if 'platform_name' in self.manifest:
			return self.manifest['platform_name'].upper()
		else:
			self.util.fail( 'Buildroot platform_name not found in manifest.json' )

	# override
	def clean(self):
		super().clean()

		if self.util.options.depot:
			self.manifestFile = self.util.options.depot
			self.depotDir = os.path.dirname( self.manifestFile )
		else:
			# Look for depot file in --deps-dir or download from tvd-git
			self.depotDir = self.util.addTmpDir( 'depot', False )
			self.manifestFile = os.path.join( self.depotDir, 'manifest.json' )
			self.util.options.depot = self.manifestFile
			depotFilePath = self.util.downloadedFilePath( os.path.basename( self.defaultDepot ) )
			if not os.path.exists( depotFilePath ):
				depotUrl = 'https://tvd-git.dmz.tecnologia.lifia.info.unlp.edu.ar/buildbot/toolchains/linux' + self.defaultDepot
				self.util.downloadFile( depotUrl, depotFilePath, True )
			self.util.extractFile( depotFilePath, self.depotDir, False )

		if not os.path.exists( self.manifestFile ):
			self.util.fail( 'Depot manifest file not found: %s' % self.manifestFile )
		self.manifest = self.util.loadJSON( self.manifestFile )

		if not 'toolchain_prefix' in self.manifest:
			self.util.fail( 'Toolchain prefix not found in depot manifest.json' )
		self.toolchainPrefix = self.manifest['toolchain_prefix']

		self.util.vlog( 'Using depot:' )
		self.util.indent()
		self.util.vlog( 'SYSROOT: %s' % self.getDepotPath('staging') )
		self.util.vlog( 'TOOLCHAIN_PATH: %s' % self.getDepotPath('toolchain') )
		self.util.vlog( 'TOOLCHAIN_PREFIX: %s' % self.toolchainPrefix )
		self.util.unindent()

	def exportTool(self,var,tool):
		toolPath = '%s%s' % (os.path.join(self.getDepotPath('toolchain'), self.toolchainPrefix), tool)
		if not os.path.exists(toolPath):
			self.util.fail('Toolchain tool not found: tool=%s, path=%s' % (var, toolPath))
		os.environ[var] = toolPath

	# override
	def exportsImpl(self):
		os.environ['BR_NO_CCACHE'] = '1'
		self.exportTool( 'AR', 'ar' )
		self.exportTool( 'CC', 'gcc' )
		self.exportTool( 'CXX', 'g++' )
		self.exportTool( 'LINK', 'g++' )

	# virtual
	def installTacImageFiles(self, destDir):
		files = super().installTacImageFiles(destDir)

		self.util.vlog('Deploying depot files to image')
		if 'files' in self.manifest:
			for f in self.manifest['files']:
				fPath = f if os.path.isabs( f ) else os.path.join( self.depotDir, f )
				srcDir = os.path.dirname( fPath )
				fName = os.path.basename( fPath )
				self.util.cp( fName, srcDir, destDir )
				files.append( fName )

		return files

	# override
	def packageDepot(self, sufix):
		depotFile = os.path.join( self.util.depsCache, 'depot-%s%s.tgz' % (self.platformName().lower(), '-%s' % sufix if sufix else '') )
		self.util.indent( 'Packaging buildroot depot to: %s' % depotFile )

		pkgDir = self.util.addTmpDir( 'pkg_depot' )
		newManifest = {
			'toolchain': 'toolchain',
			'staging': 'staging',
			'toolchain_prefix': self.toolchainPrefix,
			'rootfs': os.path.basename( self.getDepotPath('rootfs') ),
			'files': []
		}
		self.util.cp( '', self.getDepotPath('toolchain'), os.path.join( pkgDir, 'toolchain' ) )
		self.util.cp( '', self.getDepotPath('staging'), os.path.join( pkgDir, 'staging' ) )
		self.util.cp( '', self.getDepotPath('rootfs'), pkgDir )
		if 'files' in self.manifest:
			for f in self.manifest['files']:
				fPath = f if os.path.isabs( f ) else os.path.join( self.depotDir, f )
				self.util.cp( '', fPath, pkgDir )
				newManifest['files'].append( os.path.basename(fPath) )

		self.util.saveJSON( os.path.join(pkgDir, 'manifest.json'), newManifest )
		self.util.createTarFile( depotFile, pkgDir )

		self.util.unindent()

	# override
	def genHashes(self, baseDir, files, out=''):
		if not out:
			out = HASH_FILENAME
		self.util.genHashes(baseDir, files, out)

#private:
	def getDepotPath(self, key):
		if (key in self.manifest):
			if os.path.isabs( self.manifest[key] ):
				return self.manifest[key]
			else:
				return os.path.join( self.depotDir, self.manifest[key] )
		else:
			self.util.fail( 'Key %s not found in depot manifest: %s' % (key, self.manifestFile) )
