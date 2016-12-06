#!/usr/bin/env python3

import glob
import imp
import os

# This class is a generic tac package builder.
# Override in packages that need custom build steps
# Implementation must be located in: tac/packages/<pkg_name>/builder.py

class Builder():
#public:
	def __init__(self, package, platform, deployDir, util):
		self.package = package
		self.platform = platform
		self.deployDir = deployDir
		self.util = util
		self.rootDir = util.tacAbsPath('packages', package)
		self.manifest = util.loadJSON(os.path.join(self.rootDir, 'manifest.json'))

	# Deletes package in deploy dir
	def clean(self):
		self.util.indent('Cleaning')
		self.util.rm(self.fqnOriginalDir())
		if not self.util.options.keep_data:
			self.util.rm(self.fqnDataDir())
			self.util.rm(self.fqnConfigDir())
		self.cleanImpl()
		self.util.unindent()

	# Return True if package tpk is cached for current platform
	def isCached(self):
		return os.path.exists(self.tpkCacheFile())

	# Perform the build, generate <package>.tpk and possibly <package>.syms and save it in pkg_cache
	def build(self, isHost):
		self.util.indent('Building')

		tmp_build_dir = self.util.addTmpDir('build_%s' % self.package)
		syms_dir = None
		if self.hasSymbols():
			syms_dir = self.util.addTmpDir('syms_%s' % self.package)

		# Copy src files and dirs
		self.util.cp('manifest.json', self.rootDir, tmp_build_dir)
		self.util.cp('node_modules', self.rootDir, tmp_build_dir)

		if self.deployResources():
			self.util.cp('resources', self.rootDir, tmp_build_dir)

		if self.deploySrcDir():
			self.util.cpDirContent('src', self.rootDir, tmp_build_dir)

		if self.isWebBuild():
			self.deployWeb(tmp_build_dir)
		else:
			self.buildImpl(tmp_build_dir, syms_dir, isHost)

		# Fix manifest version,platform and validate
		manifest = os.path.join(tmp_build_dir, 'manifest.json')
		self.log('Fixing version numbers in manifest file: %s' % manifest)
		self.fixManifest(manifest, isHost)

		# Create package tar file
		self.util.createTarFile(self.tpkCacheFile(), tmp_build_dir)

		# If package install some symbols create syms file
		if syms_dir and len(os.listdir(syms_dir)) > 0:
			self.util.createTarFile(self.symsCacheFile(), syms_dir)

		self.util.unindent()

	# Run JavaScript linter
	def lint(self):
		self.util.lintPackage(self.package, self.rootDir)

	# Run mocha
	def test(self):
		self.util.testAndCheckCoverage(self.package, self.rootDir)

	# Install package in self.deployDir and symbols in symsDir
	def install(self, symsDir, isHost):
		# Deploy package from cache
		dest = self.fqnOriginalDir()
		self.log('Installing in: %s' % dest)
		self.util.indent()
		self.util.extractFile(self.tpkCacheFile(), dest)

		# Deploy package symbols from cache
		syms = self.symsCacheFile()
		if os.path.exists(syms):
			self.util.log('Installing symbols in %s' % symsDir)
			self.util.cp('', syms, symsDir)

		# Perform custom installation step
		self.installImpl(isHost)

		# Validate manifest
		manifest = os.path.join(dest, 'manifest.json')
		self.log('Validating manifest file: %s' % manifest)
		self.util.run(['node', self.util.tacAbsPath('tools', 'manifest_validator', 'validatemanifest.js'), manifest])

		self.util.unindent()

#protected:
	#virtual
	def cleanImpl(self):
		pass

	#virtual
	def buildImpl(self, outDir, symsOutDir, isHost):
		pass

	#virtual
	def isWebBuild(self):
		return os.path.exists(os.path.join(self.rootDir,'webbuild.json'))

	#virtual
	def deployWeb(self, outDir, cfgFile='webbuild.json'):
		parser = self.util.tacAbsPath('tools', 'webbuild', 'build.js')
		webOpts = self.util.keyValueArray2Dic(self.util.options.web_opts)
		opts = ['node', parser, '--config', os.path.join(self.rootDir,cfgFile), '--outPath', outDir]
		for k,v in webOpts.items():
			opts.append( '--' + k )
			opts.append( v )
		self.util.run( opts )

	#virtual
	def installImpl(self, isHost):
		pass

	#virtual
	def hasSymbols(self):
		return False

	# virtual
	def isMultiPlatform(self):
		return True

	#virtual
	def deploySrcDir(self):
		if self.isWebBuild():
			return False
		else:
			return True

	#virtual
	def deployResources(self):
		if self.isWebBuild():
			return False
		else:
			return True

	#virtual
	def postProcessImage(self, realRootFs, fRootEnv):
		pass

	def log(self, msg):
		self.util.vlog(msg)

	# Cached tpk file full-path
	def tpkCacheFile(self):
		last = None
		lastMtime = None
		for p in self.tpkCacheFiles():
			currMtime = os.path.getmtime(p)
			if (not last) or (currMtime > lastMtime):
				last = p
				lastMtime = currMtime
		return last if last else os.path.join(self.util.pkgCache, '%s-%s-%s.tpk' % (self.package, self.fixVersion(self.manifest['version']), self.platform))

	# All versions of cached tpk for current platform
	def tpkCacheFiles(self):
		return glob.glob(os.path.join(self.util.pkgCache, '%s-*-%s.tpk' % (self.package, self.platform)))

	# Cached symbols file full-path
	def symsCacheFile(self):
		return os.path.join(self.util.pkgCache, '%s-%s-%s.syms' % (self.package, self.fixVersion(self.manifest['version']), self.platform))

	# All versions of cached symbols file for current platform
	def symsCacheFiles(self):
		return glob.glob(os.path.join(self.util.pkgCache, '%s-*-%s.syms' % (self.package, self.platform)))

	# Fully cualified name of package in deploy directory
	def fqnOriginalDir(self):
		return os.path.join(self.deployDir, 'original', 'packages', self.manifest['id'])

	def fqnDataDir(self):
		return os.path.join(self.deployDir, 'data', 'packages', self.manifest['id'])

	def fqnConfigDir(self):
		return os.path.join(self.deployDir, 'data', 'config', self.manifest['id'])

#private:
	def fixManifest(self, manifestJson, isHost):
		manifest = self.util.loadJSON(manifestJson)

		# Fix package and components version
		manifest['version'] = self.fixVersion(manifest['version'])
		for component in manifest['components']:
			component['version'] = self.fixVersion(component['version'])

		# Fix package platform
		if 'platform' not in manifest:
			manifest['platform'] = 'UNKNOWN'

		if not self.isMultiPlatform() and not isHost:
			manifest['platform'] = self.util.device.platformName()

		self.util.saveJSON(manifestJson, manifest)

	def fixVersion(self, ver):
		git_version = self.util.getVersion()
		return '.'.join(ver.split('.')[:2] + git_version.split('-')[1:])
