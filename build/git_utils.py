import glob
import locale
import os
import subprocess
import sys

sys.dont_write_bytecode = True

class GitException(Exception):
	pass

def __gitRun(args):
	cmd = ['git'] + args
	print(' [running] %s' % ' '.join(cmd))
	if subprocess.call(cmd, stdout=open(os.devnull, 'wb')) != 0:
		raise GitException('Git command fail: %s' % ' '.join(cmd))

def __gitGet(args):
	cmd = ['git'] + args
	print(' [running] %s' % ' '.join(cmd))
	if not 'LC_ALL' in os.environ or os.environ['LC_ALL'] == 'C':
		os.environ['LC_ALL'] = 'en_US.UTF-8'
	return subprocess.check_output(cmd).decode(locale.getdefaultlocale()[1]).strip()

# returns true if exists a local branch with the given name
def existsLocalBranch(name):
	try:
		__gitRun(['rev-parse', '--verify', name])
	except GitException:
		return False
	return True

def createBranch(name):
	__gitRun( ['checkout', '-b', name] )

# try to checkout to the given branch
def createBranchFromCommit(name, ciID):
	__gitRun(['checkout', '-b', name, ciID])

# try to checkout to the given branch
def checkoutBranch(name):
	__gitRun(['checkout', name])

# return the name of the current active branch
def currentBranch():
	return __gitGet(['rev-parse', '--abbrev-ref', 'HEAD'])

# generate a set of patches files for each commit that exist in the current branch and not in master
def generatePatchFiles():
	__gitRun(['format-patch', 'origin/master'])

# return the hash-id of the last commit on master
def getMasterCommitID():
	return __gitGet(['rev-parse', '--verify', 'origin/master'])

# return true if the given branch has the commit identified by ciID
def branchHasCommit(name, ciID):
	return name in __gitGet(['branch', name, '--contains', ciID])

# delete the local branch with the given name
def rmLocalBranch(name):
	__gitRun(['branch', '-D', name])

# apply patches that match patchesExpr to the current branch using git am command
def applyPatches(patchesExpr):
	try:
		__gitRun(['am', '--keep-cr'] + sorted(glob.glob(patchesExpr)))
	except GitException:
		try:
			__gitRun(['am', '--abort'])
		except:
			pass
		raise GitException('Fail to apply patches from: %s' % patchesExpr)

# try to rebase master in current branch
def rebaseMaster( abortOnFailure ):
	try:
		__gitRun(['rebase', 'origin/master'])
	except GitException:
		if abortOnFailure:
			__gitRun(['rebase', '--abort'])
		raise Exception('Branch %s cannot be rebased with origin/master. Please fix conflicts and regenerate patches with $DEPOT/build/backup_branch.py' % currentBranch())
