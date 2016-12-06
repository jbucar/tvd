#!/usr/bin/env python3

import glob
import gzip
import hashlib
import json
import locale
import os
import shutil
import subprocess
import sys
import tempfile
import traceback
import tarfile
import urllib.request
import zipfile

LOG_RED = '\x1b[31m%s\x1b[0m'
LOG_YELLOW = '\x1b[33m%s\x1b[0m'
LOG_CYAN = '\x1b[36m%s\x1b[0m'
if not os.isatty(0):
	LOG_RED = '%s'
	LOG_YELLOW = '%s'
	LOG_CYAN = '%s'

class BuildError(Exception):
	# Custom exception for build errors
	pass

# Generic functionality used in tac packages build scripts
class BuildUtils:
#public:
	def __init__(self, tacRoot, options):
		self.TAC_VERSION = '1.0'
		self.options = options
		self.tacRoot = tacRoot
		self.logIndent = 0
		self.device = None
		self.origWD = os.getcwd()
		self.origEnv = os.environ
		self.cfgFile = None
		self.caFile = os.path.join( tacRoot, 'build', 'ssl-cert-snakeoil.pem' )
		self.depsVersions = None
		self.repoBaseUrl = 'https://tvd-git.dmz.tecnologia.lifia.info.unlp.edu.ar/buildbot/builds'
		self.modules = []

		# Setup build directories
		self.buildCache = os.path.join( self.origWD, '.build' )
		self.pkgCache = os.path.join( self.buildCache, 'packages' )
		self.mkdir( self.pkgCache )
		self.tmpDir = os.path.join( self.buildCache, 'tmp' )
		self.mkdir( self.tmpDir, True )
		self.depsCache = os.path.join( self.buildCache, 'deps' )
		self.mkdir( self.depsCache )

	def fail(self, msg):
		raise BuildError( msg )

	def terminate(self, msg, trace=None):
		self.logNflush('\n * %s %s' % (LOG_RED % '[BUILD FAILURE]', msg))
		if trace:
			entries = traceback.extract_tb(trace)
			for (filename, lineNumber, funcName, text) in entries:
				if text and text.startswith('raise BuildError'):
					break
				self.logNflush(' *   %s %s:%d' % (LOG_RED % ('At %s() in' % funcName), filename, lineNumber))
		self.logNflush('\n')
		self.cleanUp(1)

	def cleanUp(self, exitCode=0):
		if not exitCode:
			# Build successfull => Delete tmp directories
			self.indent('Deleting temporary build directories')
			self.rm( self.tmpDir )
			self.unindent()

		# Restore original working directory and environment
		os.chdir( self.origWD )
		os.environ = self.origEnv

		self.log('')
		self.log('Build finished with exit code: %d' % exitCode, True)
		sys.exit(exitCode)

# linting and testing:
	def getPackageJson(self,baseDir):
		packageJson = None
		packageJsonFileName = os.path.join(baseDir, 'package.json')
		if os.path.exists(packageJsonFileName):
			packageJson = self.loadJSON( packageJsonFileName )

		return packageJson

	def coverageTresholds(self, baseDir):
		tresholds = [
			'--statements', '100',
			'--branches', '100',
			'--functions', '100',
			'--lines', '100'
		]
		packageJson = self.getPackageJson(baseDir)
		if packageJson and ('cov_tresholds' in packageJson):
			tresholds = packageJson['cov_tresholds']

		return tresholds

	def lintPackage(self, pkgName, baseDir):
		packageJson = self.getPackageJson(baseDir)
		if packageJson and packageJson.get('tac_disable_linter', False):
			return

		srcDir = os.path.join(baseDir, 'src')
		if os.path.exists(srcDir):
			self.indent('Linting package %s' % pkgName)
			try:
				self.run([self.tacAbsPath('tools', 'jshint', 'lint.py'), '-f', srcDir], 2)
			except BuildError:
				self.fail('Package %s do not pass linter' % pkgName)
			self.unindent()

	def testAndCheckCoverage(self, pkgName, baseDir):
		packageJson = self.getPackageJson(baseDir)
		if packageJson and packageJson.get('tac_disable_mocha', False):
			return

		srcDir = os.path.join(baseDir, 'src')
		testDir = os.path.join(baseDir, 'test')

		tresholds = self.coverageTresholds(baseDir)

		if os.path.exists(testDir):
			self.indent('Testing package %s' % pkgName)

			basePath = self.tacRoot
			istanbul = os.path.join( basePath, 'packages', 'node', 'node_modules_dev', 'istanbul', 'lib', 'cli.js')
			mocha = os.path.join( basePath, 'packages', 'node', 'node_modules_dev', 'mocha', 'bin', '_mocha')
			coverageDir = os.path.join(self.buildCache, 'coverage', pkgName)

			if os.path.exists(srcDir):
				# Package has sources: run tests with istanbul, and check coverage
				currDir = os.getcwd()
				os.chdir(self.tacRoot)

				try:
					self.run([istanbul, 'cover', '--report', 'html', '--root', srcDir, '--dir', coverageDir, mocha, '--', testDir])
				except BuildError:
					self.fail('Package %s do not pass tests' % pkgName)

				if os.path.exists(os.path.join(coverageDir, 'index.html')):
					try:
						self.run([istanbul, 'check-coverage'] + tresholds + [os.path.join(coverageDir, 'coverage.json')])
					except BuildError:
						self.fail('Package %s do not meet coverage treshold' % pkgName)

				os.chdir(currDir)
			else:
				# Package don't have sources: run tests with mocha
				try:
					self.run([mocha, testDir])
				except BuildError:
					self.fail('Package %s do not pass tests' % pkgName)

			self.unindent()

# subprocess:
	def run(self, args, verbose=None):
		if not verbose:
			verbose = self.options.verbose

		try:
			if verbose > 0:
				self.log( 'Run: %s' % ' '.join(args) )

			if verbose > 1:
				out = None
				err = None
			else:
				out = subprocess.DEVNULL
				err = subprocess.DEVNULL

			subprocess.check_call(args, stdout=out, stderr=err)
		except:
			self.fail('While running: %s' % ' '.join(args)).with_traceback(sys.exc_info()[2])

	def runAndGetOut(self, args):
		return subprocess.check_output(args, stderr=subprocess.STDOUT).decode(locale.getdefaultlocale()[1])

	def fakerootRun(self, env, args):
		self.run(['fakeroot', '-s', env, '-i', env, '--'] + args)

# logging:
	def log(self, msg, color=False):
		self.logNflush(' - %s%s' % (self.indentation(), (LOG_CYAN % msg) if color else msg))

	def vlog(self, msg):
		if self.options.verbose >= 1:
			self.log(msg)

	def indent(self, msg=None):
		if msg:
			self.log(msg, True)
		self.logIndent += 1

	def unindent(self, msg=None):
		if self.logIndent > 0:
			self.logIndent -= 1
		if msg:
			self.log(msg, True)

# file system:
	def tacAbsPath(self, *args):
		return os.path.join(self.tacRoot, *args)

	def cp(self, name, src, dst):
		srcPath = os.path.join(src, name) if name else src
		if os.path.exists(srcPath):
			self.vlog('Coping %s to: %s' % (os.path.basename(srcPath), dst))
			if os.path.isfile(srcPath):
				shutil.copy(srcPath, dst)
			elif os.path.isdir(srcPath):
				destPath = os.path.join(dst, name)
				if os.path.exists(destPath):
					self.cpDirContent(name, src, destPath)
				else:
					shutil.copytree(srcPath, destPath, True)

	def cpDirContent(self, dirname, src, dest):
		srcPath = os.path.join(src, dirname)
		if os.path.exists(srcPath):
			self.mkdir(dest)
			for item in os.listdir(srcPath):
				self.cp(item, srcPath, dest)

	def mkdir(self, directory, rmIfExist=False ):
		if rmIfExist:
			self.rm( directory )
		if not os.path.exists( directory ):
			os.makedirs(directory, 0o755)

	def symlink(self, name, source, dest):
		dst = os.path.join( dest, name )
		if os.path.exists( dst ):
			os.unlink( dst )
		os.symlink( os.path.join( source, name ), dst )

	def rm(self, path):
		if os.path.exists(path):
			if os.path.isfile(path):
				self.vlog('Deleting file: %s' % path)
				os.remove(path)
			elif os.path.isdir(path):
				self.vlog('Deleting directory: %s' % path)
				shutil.rmtree(path)

	def rmAll(self, paths):
		for p in paths:
			self.rm(p)

	def readFile(self, filename):
		if os.path.exists(filename):
			with open(filename, 'r') as f:
				return f.read()
		else:
			self.fail('File do not exist: %s' % filename)

	def loadJSON(self, jsonFile):
		if os.path.exists(jsonFile):
			self.vlog('Loading json file: %s' % jsonFile)
			f = open(jsonFile, 'r')
			data = json.load(f)
			f.close()
			return data
		else:
			self.fail('JSON file do not exist: %s' % jsonFile)

	def saveJSON(self, jsonFile, data):
		self.vlog( 'Writing json file: %s' % jsonFile )
		f = open(jsonFile, 'w')
		f.write(json.dumps(data, indent=4))
		f.close()

	def addTmpDir(self, name, withSufix=True):
		tmpdir = os.path.join( self.tmpDir, name )
		if withSufix:
			tmpdir = tempfile.mkdtemp( suffix='', prefix=('%s_' % name), dir=self.tmpDir )
		else:
			self.mkdir( tmpdir, True )
		return tmpdir

	def applyPatches(self, patchSubDir, baseDir, platform):
		self.applyPatchesInDir(os.path.join(self.tacRoot, 'build', 'devices', 'common', 'patches', patchSubDir), baseDir)
		self.applyPatchesInDir(os.path.join(self.tacRoot, 'build', 'devices', platform, 'patches', patchSubDir), baseDir)

	def applyPatchesInDir(self, patchesDir, baseDir):
		patches = glob.glob(os.path.join(patchesDir, '*.patch'))
		patches.sort()
		for p in patches:
			self.patch(baseDir, p)

	def patch(self, baseDir, patchFile):
		self.vlog('Applying patch %s' % patchFile)
		subprocess.check_call(['patch', '-p1', '-d', baseDir, '-i', patchFile, '--quiet'])

#downloads:
	def downloadedFilePath(self, name, platform=None, version=None, extension=None):
		tmp = os.path.join( self.depsCache, name )
		if platform:
			tmp += '-%s' % platform
		if version:
			tmp += '-%s' % version
		if extension:
			tmp += '.%s' % extension
		return tmp

	def downloadFile(self, url, filename, needCert=False, copyFromTvdPackages=True):
		inCache = self.downloadedFilePath(filename)
		inTvdPkgs = os.path.join(os.environ['TVD_PACKAGES'], filename) if ('TVD_PACKAGES' in os.environ) else False
		if os.path.exists(inCache):
			self.log('Found %s' % inCache)
		elif copyFromTvdPackages and inTvdPkgs and os.path.exists(inTvdPkgs):
			self.cp('', inTvdPkgs, inCache)
		else:
			self.log('Downloading %s to %s' % (url, inCache))
			output = None
			try:
				req = urllib.request.urlopen(url, cafile=(self.caFile if needCert else None))
				output = open(inCache, 'wb')
				output.write(req.read())
			except urllib.error.HTTPError as err:
				self.fail('Fail to download url=%s code=%d' % (url, err.code))
			finally:
				if output:
					output.close()

	# NOTE: urlPlat is a hack needed by ginga-ppapi because it url is not standard
	def deployLibrary(self, name, platform, dest, extra=[], urlPlat=None):
		depInfo = {
			'name': name,
			'platform': platform,
			'extension': 'tar.bz2',
			'dest': dest,
		}
		if urlPlat:
			depInfo['url_platform'] = urlPlat

		# Get dependency (from deps dir | download | build)
		tarball = self.getDependency( depInfo )
		filePath = os.path.join( 'lib', 'libdtv%s.so' % depInfo['name'] )
		wildcardPrefix = '%s-*/' % depInfo['name']
		self.vlog( 'Extracting %s%s from %s' % (wildcardPrefix, filePath, tarball) )
		destDir = os.path.join( os.path.dirname( depInfo['dest'] ), '.tmp' )
		self.mkdir( destDir )
		subprocess.check_call( ['tar', 'xf', tarball, '-C', destDir, '--strip', '1', '--wildcards', '%s%s' % (wildcardPrefix, filePath)] )
		self.cp( '', os.path.join( destDir, filePath ), depInfo['dest'] )
		for name,src,dest in extra:
			subprocess.check_call( ['tar', 'xf', tarball, '-C', destDir, '--strip', '1', '--wildcards', '%s%s' % (wildcardPrefix, src)] )
			self.cp( name, os.path.join( destDir, src ), dest )
		self.rm( destDir )

	# Return the absolute path to a dependency tarball in deps dir.
	#   if --build-deps:
	#     remove from cache if present and build
	#   else:
	#     download from tvdgit if necesary
	#
	# depInfo eg: {
	# 	name: 'tvdmodule',
	# 	platform: 'aml',
	# 	version: '1.0',
	# 	extension: 'tar.bz2',
	# 	builder: (<function>)(util, depsInfo)
	# }
	def getDependency(self, depInfo, builder=None, canDownload=True):
		depInfo['version'] = self.getDepVersion( depInfo['name'] )
		self.vlog( 'Get dependency: %s-%s-%s' % (depInfo['name'], depInfo['platform'], depInfo['version']) )

		cachedDep = self.downloadedFilePath( depInfo['name'], depInfo['platform'], depInfo['version'], depInfo['extension'] )

		if self.options.build_deps:
			self.rm( cachedDep )
			self.buildDep( depInfo, cachedDep, builder )
		else:
			if (not os.path.exists( cachedDep ) and canDownload):
				self.deploySdk(self.options.platform, self.TAC_VERSION, self.origWD)

			if not os.path.exists( cachedDep ):
				self.buildDep( depInfo, cachedDep, builder )
			else:
				self.log( 'Dependency found in deps-dir: %s' % cachedDep )

		return cachedDep

#packaging:
	def getVersion(self):
		version = self.options.version
		if not version:
			DEPOT = os.path.dirname( os.path.dirname( self.tacRoot ) )
			version = self.runAndGetOut( ['git', '--git-dir=%s/.git' % DEPOT, 'describe', '--tags'] ).strip()
		return version

	def gzipDir(self, src, dst):
		fSrc = open(src, 'rb')
		fDst = gzip.open(dst, 'wb')
		shutil.copyfileobj(fSrc, fDst)
		fSrc.close()
		fDst.close()

	def extractFile(self, filename, outDir, stripRoot=True):
		self.vlog('Extracting %s in %s' % (os.path.basename(filename), outDir))
		self.mkdir(outDir)
		if filename.endswith('.zip'):
			self.extractZip(filename, outDir)
		else:
			self.extractTar(filename, outDir, stripRoot)

	def createTarFile(self, filename, directory, mode='w:gz'):
		self.vlog('Creating tar file: %s' % filename)
		tar = tarfile.open(filename, mode)
		tar.add(directory, arcname='.')
		tar.close()

	def createTarInCache(self, baseDir, outName):
		self.vlog('Creating %s in %s' % (outName, self.pkgCache))
		destFile = os.path.join(self.pkgCache, outName)
		self.rm(destFile)
		self.createTarFile(destFile, baseDir, 'w:')

	def genHashes(self, baseDir, files, out=''):
		outFile = open(os.path.join(baseDir, out), 'a') if out else None
		foldersFiles = []
		for fName in files:
			fPath = os.path.join(baseDir, fName)
			if os.path.isdir(fPath):
				foldersFiles += [ os.path.join(fName, f) for f in os.listdir(fPath) ]
			else:
				self.vlog('Generating md5 hash for: %s' % fPath)
				with open(fPath, 'rb') as f:
					res = self.hashFile(f, hashlib.md5())
					if out:
						outFile.write('%s  %s\n' % (res, fName))
					else:
						with open(os.path.join(baseDir, '%s.md5' % fName), 'w') as md5file:
							md5file.write('%s  %s\n' % (res, fName))
		if out:
			outFile.close()

		if foldersFiles:
			self.genHashes(baseDir, foldersFiles, out)


	def hashFile(self, inFile, hasher, blocksize=65536):
		buf = inFile.read(blocksize)
		while len(buf) > 0:
			hasher.update(buf)
			buf = inFile.read(blocksize)
		return hasher.hexdigest()

#private:
	def indentation(self):
		current = ''
		for i in range(0,self.logIndent):
			current = '%s  ' % current
		return current

	def logNflush(self, msg):
		print(msg)
		sys.stdout.flush()

	def extractTar(self, filename, outDir, stripRoot):
		tgz = None
		currDir = os.getcwd()
		os.chdir(outDir)
		try:
			tgz = tarfile.TarFile(filename, 'r')
			tgz.extractAll()
		except:
			subprocess.check_call(['tar', 'xf', filename] + (['--strip', '1'] if stripRoot else []))
		finally:
			if tgz:
				tgz.close()
			os.chdir(currDir)

	def extractZip(self, filename, outDir):
		with zipfile.ZipFile(filename, 'r') as z:
			z.extractall(outDir)

	def buildDep(self, depInfo, dest, builder):
		# The setup of the compiler is made by the invoked tool (cmake, htmlshell/build.py, etc)
		if 'CC' in os.environ:
			del os.environ['CC']
		if 'CXX' in os.environ:
			del os.environ['CXX']
		if 'AR' in os.environ:
			del os.environ['AR']

		# Call builder if provided else use defaultBuilder
		outPut = builder( self, depInfo ) if builder else self.defaultBuilder( depInfo )

		# Check built version match the requested version
		outPutVersion = '.'.join( outPut.split('-')[1].split('.')[:2] )
		if outPutVersion != depInfo['version']:
			self.fail( 'Built dependency %s has version "%s" while requested version was "%s"' % (depInfo['name'], outPutVersion, depInfo['version']))

		self.cp( '', outPut, dest )

	def defaultBuilder(self, depInfo):
		name = depInfo['name']
		platform = depInfo['platform']

		if not 'DEPOT' in os.environ:
			self.fail('Cannot build dependency=%s, DEPOT not in environment' % name)

		cfgFile = os.path.join( self.tacRoot, 'build', 'devices', platform, 'deps', '%s.cmake' % name.upper() )
		if not os.path.exists( cfgFile ):
			cfgFile = os.path.join( self.tacRoot, 'build', 'devices', 'common', 'deps', '%s.cmake' % name.upper() )

		args = [
			os.path.join( os.environ['DEPOT'], 'build', 'build.py' ),
			'-t', name,
			'-c', cfgFile,
			'-k'
		]

		if platform != 'linux':
			args += ['-p', platform.upper()]
			if self.options.depot:
				args += ['--depot', self.options.depot]

		self.log( 'Building dependency %s for platform %s' % (name, platform) )

		currDir = os.getcwd()
		tmpBuildDir = self.addTmpDir( 'build_%s' % name, False )
		os.chdir( tmpBuildDir )

		self.run( args )

		os.chdir( currDir )

		return glob.glob( os.path.join( tmpBuildDir, name, '%s-*.tar.bz2' % name ) )[0]

	def getDepVersion(self, name):
		versionsFile = self.tacAbsPath( 'build', 'deps.conf' )

		if not self.depsVersions:
			self.depsVersions = self.loadJSON( versionsFile )

		if not name in self.depsVersions:
			self.fail( 'Version for dependency %s not found in %s' % (name, versionsFile) )

		return self.depsVersions[name]

	def downloadDep(self, depInfo, dest):
		urlSufix = 'RELEASE/STATIC/LATEST-%s.tar.bz2' % depInfo['version']
		if 'url_sufix' in depInfo:
			urlSufix = depInfo['url_sufix']
		urlPlat = depInfo['url_platform'] if ('url_platform' in depInfo) else depInfo['platform']
		if urlPlat == 'linux':
			urlPlat = 'CLANG_64'
		url = '%s/%s/%s/%s' % ( self.repoBaseUrl, urlPlat.upper(), depInfo['name'], urlSufix )
		self.downloadFile( url, os.path.basename( dest ), True, False )

	def deploySdk(self, platform, version, dest):
		filename = os.path.join( self.tmpDir, 'tac-%s.sdk' % version )
		platform2dir = {
			'linux': 'clang_64',
			'aml': 'aml_microtrol'
		}
		url = '%s/%s/tac/LATEST-%s.sdk' % ( self.repoBaseUrl, platform2dir.get(platform,platform).upper(), version )
		self.downloadFile( url, filename, True, False )
		self.extractFile(filename, dest)

	def keyValueArray2Dic(self, keyValueArray):
		result = {}
		for keyVal in keyValueArray:
			try:
				(key,val) = keyVal.split('=')
			except ValueError:
				self.fail('No key=value option')
			result[key] = val
		return result
