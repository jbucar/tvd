#!/usr/bin/env python3

import imp
import os
import string

module = imp.load_source('builder', os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'build', 'generic_builder.py'))

ENYO_URL = 'https://github.com/enyojs/bootplate-moonstone.git'
ENYO_VERSION = '2.5.1.1'
ENYO_SRC_FILE = 'enyo-%s.tar.gz' % ENYO_VERSION

class Builder( module.Builder ):
#public:
	def __init__(self, package, platform, deployDir, util):
		super().__init__(package, platform, deployDir, util)
		self.srcDir = os.path.join(self.rootDir, 'src')
		self.tplDir = os.path.join(self.rootDir, 'templates')
		self.buildDir = os.path.join(self.rootDir, 'build')
		self.enyoDir = os.path.join(self.buildDir, 'enyo')

#protected:
	#override
	def buildImpl(self, outDir, outSymsDir, isHost):
		# Download enyo if necesary
		enyoSrc = self.util.downloadedFilePath(ENYO_SRC_FILE)
		if not os.path.exists(enyoSrc):
			inTvdPkgs = os.path.join(os.environ['TVD_PACKAGES'], ENYO_SRC_FILE) if ('TVD_PACKAGES' in os.environ) else ''
			if inTvdPkgs and os.path.exists(inTvdPkgs):
				self.util.cp('', inTvdPkgs, enyoSrc)
			else:
				self.downloadAndPackageEnyo(enyoSrc)

		# Extract enyo in build directory and apply patches
		self.util.rm(self.buildDir)
		self.util.extractFile(enyoSrc, self.buildDir)
		self.util.applyPatchesInDir(os.path.join(self.rootDir, 'patches'), self.buildDir)

		self.buildShell(outDir)

	#override
	def deploySrcDir(self):
		return False

#private:
	def downloadAndPackageEnyo(self, enyoSrc):
		self.log('Packing enyo sources to: %s' % enyoSrc)

		# Download enyo src from github and pack in a tar file
		currentDir = os.getcwd()
		tmpDir = self.util.addTmpDir('enyo')
		self.util.run(['git', 'clone', '--recursive', ENYO_URL, tmpDir])
		os.chdir(tmpDir)
		self.util.run(['git', 'checkout', ENYO_VERSION])
		self.util.run(['git', 'submodule', 'foreach', 'git', 'checkout', ENYO_VERSION])

		# Remove .git directories
		for dirname, subdirs, files in os.walk(tmpDir):
			if os.path.basename(dirname) == '.git':
				self.util.rm(dirname)
			if '.git' in files:
				self.util.rm(os.path.join(dirname, '.git'))

		self.util.createTarFile(enyoSrc, tmpDir)
		os.chdir(currentDir)

	def buildShell(self, outDir):
		index = self.util.readFile(os.path.join(self.tplDir, 'index-tpl.html'))
		assetsPath = 'images'
		if self.util.options.shell_dbg:
			assetsPath = os.path.join(self.srcDir, 'images')
			dbg_tpl = self.util.readFile(os.path.join(self.tplDir, 'dbg-scripts-tpl.html'))
			scripts = string.Template(dbg_tpl).safe_substitute(mockSrc=os.path.join(self.srcDir, 'mock'))
			styles = ''
		else:
			self.deploy(outDir)
			scripts = self.util.readFile(os.path.join(self.tplDir, 'scripts-tpl.html'))
			styles = self.util.readFile(os.path.join(self.tplDir, 'styles-tpl.html'))

		tpl = string.Template(index).safe_substitute(scripts=scripts, stylesheet=styles, assetsPath=assetsPath)
		with open(os.path.join(outDir, 'index.html'), 'w+') as f:
			f.write(string.Template(tpl).safe_substitute(package_path=self.srcDir, enyoSrc=self.buildDir, target='TAC'))

	def deploy(self, outDir):
		currDir = os.getcwd()
		os.chdir(self.srcDir);
		self.minify(outDir, 'enyo', os.path.join(self.enyoDir, 'minify', 'package.js'))
		for t in ['third-party', 'components', 'tacmenu', 'notification']:
			self.minify(outDir, t, os.path.join(self.srcDir, t, 'package.js'))
		self.deployAssets(self.srcDir, outDir)
		os.chdir(currDir);

	def minify(self, dest, target, package):
		self.log('Minifying %s' % target)
		cmd = [
			'node',
			os.path.join(self.enyoDir, 'tools', 'minifier', 'minify.js'),
			'-enyo', self.enyoDir,
			'-destdir', os.path.join(dest, 'build'),
			'-output', target,
			'-no-less',
			package
		]
		# Only beautify if building for pc
		if self.platform == 'linux':
			cmd.append('--beautify')
		self.util.run(cmd)

	def deployAssets(self, subDir, destdir):
		deploy = self.util.loadJSON(os.path.join(subDir, 'deploy.json'))
		if deploy:
			self.log('Deploying %s ...' % subDir);
			if ('assets' in deploy) and isinstance(deploy['assets'], list):
				for asset in deploy['assets']:
					self.util.cp(asset, subDir, destdir)
