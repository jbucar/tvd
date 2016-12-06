#!/usr/bin/env python3

from optparse import OptionParser, OptionGroup
import os
import random
import shutil
import string
import sys
import tempfile

sys.dont_write_bytecode = True

import git_utils

def LINFO(msg):
	print("\x1b[32m%s\x1b[0m" % msg)

def LERROR(msg):
	print("\x1b[31m%s\x1b[0m" % msg)

DEPOT = os.path.dirname(os.path.dirname(os.path.abspath( __file__ )))

def genTmpBranchName(backUpDir):
	while True:
		tmpName = "%s_%s" % (backUpDir, ''.join(random.choice(string.ascii_uppercase) for i in range(10)))
		if not git_utils.existsLocalBranch(tmpName):
			return tmpName

def checkBranch(tmpName, backUpDir, abortOnFailure):
	baseIdFile = os.path.join(backUpDir, 'base_id.txt')
	if not (os.path.exists(backUpDir) and os.path.isfile(baseIdFile)):
		raise Exception('Patches directory %s not found or base_id.txt file not exists' % backUpDir)

	tmpPatchesDir = os.path.join(tempfile.gettempdir(), tmpName)
	if os.path.exists(tmpPatchesDir):
		shutil.rmtree(tmpPatchesDir)
	shutil.copytree(backUpDir, tmpPatchesDir)

	with open(baseIdFile, 'r') as f:
		git_utils.createBranchFromCommit(tmpName, f.readline())

	git_utils.applyPatches( '%s/*.patch' % tmpPatchesDir )
	shutil.rmtree( tmpPatchesDir )

	git_utils.rebaseMaster(abortOnFailure)

def checkBackup( backup, keepBranch ):
	# Store current branch to checkout back later
	origBranch = git_utils.currentBranch()

	# Generate temporary names for branches
	branches = {}
	if backup:
		branches[genTmpBranchName(backup)] = os.path.join(DEPOT, 'patches', backup)
	else:
		for root, dirs, files in os.walk(os.path.join(DEPOT, 'patches')):
			for d in dirs:
				branches[genTmpBranchName(d)] = os.path.join( root, d )

	failedBranches = []
	succeesfullBranches = []
	for branch, path in branches.items():
		# Create branch, apply patches and try to rebase master
		# If rebase fail and keep_branch is false, abort rebase and go back to master
		try:
			checkBranch(branch, path, not keepBranch)
			succeesfullBranches.append(path)
		except:
			failedBranches.append(path)

		# Delete tmp branch and return to master if keep_branch was not passed
		if not keepBranch:
			if origBranch != git_utils.currentBranch():
				git_utils.checkoutBranch(origBranch)
			if git_utils.existsLocalBranch(branch):
				git_utils.rmLocalBranch(branch)

	# Log result
	if len(succeesfullBranches) > 0:
		LINFO(' [OK] The following branches succeed to rebase master:')
		for fb in succeesfullBranches:
			LINFO('\t%s' % fb)

	if len(failedBranches) > 0:
		LERROR(' [FAIL] The following branches fail to rebase master:')
		for fb in failedBranches:
			LERROR('\t%s' % fb)
		sys.exit(1)

def applyPatches( patches ):
	git_utils.createBranch( genTmpBranchName('TMP') )
	for patch in patches:
		patchDir = os.path.join( DEPOT, 'patches', patch )
		if os.path.exists(patchDir):
			git_utils.applyPatches( os.path.join( patchDir, '*.patch') )
		else:
			LERROR( 'Patch directory not found: %s' % patchDir )

# Entry point
parser = OptionParser()
group = OptionGroup(parser, "Branch backup")
group.add_option("-c", "--check-backup", dest="check_backup", default="",    action="store",      help="Create a new branch from base_id, apply the patches, and try to rebase master")
group.add_option("-k", "--keep-branch",  dest="keep_branch",  default=False, action="store_true", help="Do not delete the branch created")
parser.add_option_group(group)
group = OptionGroup(parser, "Apply patches")
group.add_option("-p", "--patches",      dest="patches",      default="",    action="store",      help="Create a new branch from the current index and apply the supplied patches (comma separated list)")
parser.add_option_group(group)
(args,_) = parser.parse_args()

if args.patches:
	applyPatches( args.patches.split(',') )
else:
	checkBackup( args.check_backup, args.keep_branch )
