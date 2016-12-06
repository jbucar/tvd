#!/usr/bin/env python3

import imp
import os
import sys

# Prevent python from generating .pyc
sys.dont_write_bytecode = True

# TAC root directory
TAC_ROOT = os.path.dirname( os.path.abspath(__file__) )
NODE_HOST_DIR = os.path.join( os.getcwd(), '.build', 'node' )
NODE_BIN_DIR = os.path.join( NODE_HOST_DIR, 'bin' )
PACKAGES_DIR = os.path.join( TAC_ROOT, 'packages' )
TAC_MODULES_DIR = os.path.join( PACKAGES_DIR, 'node_modules' )

class Options:
	def __init__(self):
		self.verbose = 2

module = imp.load_source('utils', os.path.join(TAC_ROOT, 'build', 'buildutils.py'))
build_utils = module.BuildUtils(TAC_ROOT, Options())

def loadBuilder(pkgName):
	# Try to find custom builder for package, if not found load generic builder
	builderFile = os.path.join(PACKAGES_DIR, pkgName, 'builder.py')
	if not os.path.exists(builderFile):
		builderFile = os.path.join(TAC_ROOT, 'build', 'generic_builder.py')
		if not os.path.exists(builderFile):
			print('[FAIL] Cannot found builder for package: %s' % pkgName)
			sys.exit(1)

	module = imp.load_source('pkgbuilder', builderFile)
	return module.Builder(pkgName, 'linux', '/tmp', build_utils)

def getPkgBaseDir(pkgName):
	if pkgName in os.listdir(TAC_MODULES_DIR):
		return os.path.join(TAC_MODULES_DIR, pkgName)
	else:
		return os.path.join(PACKAGES_DIR, pkgName)

def report(successful, failed):
	print('\n\n')
	print('================================  Test results  ================================')
	print('- Passed tests: %d' % len(successful))
	for t in successful:
		print('- \t\x1b[32m%s\x1b[0m' % t)
	print('================================================================================')
	print('- Failed tests: %d' % len(failed))
	for test, msg in failed:
		print('- \t\x1b[31m%s\x1b[0m: %s' % (test, msg))
	print('================================================================================')

def main():
	# Setup node_path
	os.environ['NODE_PATH'] = os.path.join( PACKAGES_DIR, 'node', 'node_modules' ) + ':' + os.path.join( PACKAGES_DIR, 'node', 'node_modules_dev' )

	pkgsToTest = []
	if len(sys.argv) > 1:
		for i in range(1, len(sys.argv)):
			pkgsToTest.append(sys.argv[i])
	else:
		pkgsToTest += os.listdir(PACKAGES_DIR)
		pkgsToTest += os.listdir(TAC_MODULES_DIR)
	if 'node_modules' in pkgsToTest:
		pkgsToTest.remove('node_modules')

	successful = []
	failed = []

	for p in pkgsToTest:
		# Locate package in <TAC_ROOT>/packages/ or <TAC_ROOT>/packages/node_modules/
		pkgBaseDir = getPkgBaseDir(p)

		try:
			# Pass linter
			build_utils.lintPackage(p, pkgBaseDir)

			# Run tests and check coverage
			build_utils.testAndCheckCoverage(p, pkgBaseDir)

			# Succees!
			successful.append(p)

		except module.BuildError as e:
			# Failure!
			failed.append((p, str(e)))

	report(successful, failed)
	return len(failed)

# Entry point
sys.exit( main() )
