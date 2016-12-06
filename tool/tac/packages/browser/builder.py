#!/usr/bin/env python3

import glob
import imp
import os

module = imp.load_source('builder', os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'build', 'generic_builder.py'))

class Builder( module.Builder ):
#public:
	def __init__(self, package, platform, deployDir, util):
		super().__init__(package, platform, deployDir, util)
		self.remoteDir = os.path.join( self.rootDir, 'node_modules', 'htmlshellremote' )

#protected:
	#override
	def cleanImpl(self):
		self.util.mkdir( self.remoteDir, True )

	#override
	def buildImpl(self, outDir, outSymsDir, isHost):
		# Create deploy directory structure
		self.util.mkdir( os.path.join( outDir, 'bin' ) )
		self.util.mkdir( os.path.join( outDir, 'lib' ) )
		self.util.mkdir( os.path.join( outDir, 'node_modules' ) )

		# Deploy htmlshell and htmlshellremote
		self.deployHtmlShell( outDir, outSymsDir )
		self.deployHtmlShellRemote( outDir )

	#override
	def installImpl(self, isHost):
		sandbox = os.path.join(self.fqnOriginalDir(), 'bin', 'chrome-sandbox')
		if os.path.exists(sandbox):
			self.log('Fix chrome-sandbox permissions')
			self.util.run(['sudo', 'chown', 'root:root', sandbox])
			self.util.run(['sudo', 'chmod', '4755', sandbox])

	#override
	def hasSymbols(self):
		return True

	# override
	def isMultiPlatform(self):
		return False

	#override
	def postProcessImage(self, rootfsDir, fRootEnv):
		self.log('Fix chrome-sandbox permissions inside fake rootfs')
		self.util.fakerootRun( fRootEnv, ['chmod','4755', os.path.join( rootfsDir, self.util.device.middlewareInstallDir, 'packages', 'ar.edu.unlp.info.lifia.tvd.browser', 'bin', 'chrome-sandbox' )] )

#private:
	def deployHtmlShellRemote(self, outDir):
		nodeModulesDir = os.path.join( outDir, 'node_modules' )

		#           extract         ,  from  , to
		extra = [ ('htmlshellremote', 'share', nodeModulesDir ) ]
		self.util.deployLibrary( 'htmlshellremote', self.platform, os.path.join( outDir, 'lib' ), extra )

		# Copy htmlshellremote in node_modules for running test
		self.util.cpDirContent( 'htmlshellremote', nodeModulesDir, self.remoteDir )

	def deployHtmlShell(self, outDir, outSymsDir):
		# Get dependency
		filePath = self.util.getDependency({
			'name': 'htmlshell',
			'extension': 'tar.gz',
			'platform': self.platform,
			'url_sufix': 'LATEST-%s.tar.gz' % self.util.getDepVersion('htmlshell')
		}, buildHtmlShell)

		outBinDir = os.path.join( outDir, 'bin' )
		self.util.extractFile( filePath, outBinDir )

		# Move htmlshell symbols to symbols output directory
		for f in ['htmlshell.syms.d', 'minidump_stackwalk', 'dmp2minidump.py']:
			self.util.cp( f, outBinDir, outSymsDir )
			self.util.rm( os.path.join( outBinDir, f ) )
		# Remove unnecesary files/dirs
		self.util.rm( os.path.join( outBinDir, 'run.sh' ) )
		self.util.rm( os.path.join( outBinDir, 'profiles' ) )

# Hook for building htmlshell
def buildHtmlShell(util, depInfo):
	platform = depInfo['platform']
	util.vlog('Building dependency htmlshell for platform %s' % platform)

	currDir = os.getcwd()
	tmpBuildDir = util.addTmpDir('htmlshell', False)
	os.chdir( tmpBuildDir )

	htmlshellBuild = os.path.join( os.environ['DEPOT'], 'tool', 'htmlshell', 'build.py' )
	args = [htmlshellBuild, '-s', platform, '-b', '-i', '-p']

	if (not 'USE_CCACHE' in os.environ) or os.environ['USE_CCACHE'] == '0':
		args += '-c'

	if platform != 'linux' and util.options.depot:
		args += ['--depot', util.options.depot]

	# Build htmlshell
	util.run(args)

	os.chdir(currDir)

	return glob.glob( os.path.join( tmpBuildDir, 'rootfs', 'htmlshell-*.tar.gz' ) )[0]
