#!/usr/bin/env python3

from argparse import ArgumentParser
import os
import shutil
import sys

sys.dont_write_bytecode = True

import git_utils

DEPOT = os.path.dirname(os.path.dirname(os.path.abspath( __file__ )))

def fail(msg):
	raise Exception(msg)

def main():
	parser = ArgumentParser(description="Back-up a branch storing its commits as diffs using git format-patch")

	parser.add_argument("branch", metavar="BRANCH_NAME", help="Specify the branch to backup.")
	parser.add_argument("-b", "--base-commit", dest="base_commit", default='', help="Specify the base commit id from branch master.")
	args = parser.parse_args()

	origBranch = git_utils.currentBranch()
	failed = None

	try:
		if not git_utils.existsLocalBranch(args.branch):
			fail('Local branch \'%s\' does not exist' % args.branch)

		if origBranch != args.branch:
			git_utils.checkoutBranch(args.branch)

		patchesDir = os.path.join(DEPOT, 'patches', args.branch)
		if os.path.exists(patchesDir):
			print(' [info] Cleaning directory %s' % patchesDir)
			shutil.rmtree(patchesDir)
		os.makedirs(patchesDir, 0o755)
		os.chdir(patchesDir)

		ciID = git_utils.getMasterCommitID()

		if not git_utils.branchHasCommit(args.branch, ciID):
			if args.base_commit:
				if git_utils.branchHasCommit('master', args.base_commit):
					ciID = args.base_commit
				else:
					fail('You specify a base commit id that is not present in master')
			else:
				fail('Branch is not up to date with master! You need to specify the base commit id from the master')

		git_utils.generatePatchFiles()
		with open('base_id.txt', 'w') as f:
			f.write(ciID)

		print(' [info] Back-up of %s branch generated successfully in %s' % (args.branch, patchesDir))

	except:
		failed = sys.exc_info()[1]

	finally:
		if origBranch != git_utils.currentBranch():
			git_utils.checkoutBranch(origBranch)

	if failed:
		print(' [FAILURE] %s' % failed)
		sys.exit(1)

origWD = os.getcwd()
main()
os.chdir(origWD)
