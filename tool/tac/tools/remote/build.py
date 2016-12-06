#!/usr/bin/env python3
from optparse import OptionParser
import locale
import os
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET

REMOTE_DIR=os.path.dirname(os.path.abspath(__file__))
DEPOT = os.path.dirname( os.path.dirname( os.path.dirname( os.path.dirname( REMOTE_DIR ) ) ) )
dependencies = [
	{ 'package': 'cordova', 'version': '6.0.0' }
]

class BuildError(Exception):
	# Custom exception for build errors
	pass

def setOptions(parser):
	parser.add_option('-b', '--buildType',    dest='build_type',   action='store', type='choice', default='debug',    help='Build Type: (release, debug)', choices=['debug','release'])
	parser.add_option('-d', '--install-deps', dest='install_deps', action='store_true',           default=False,      help='Install dependencies.')
	parser.add_option('-o', '--out-dir' ,     dest='out_dir' ,     action='store', type='string', default=REMOTE_DIR, help='Directory where the apk will be put.')

def log(msg):
	print(msg)
	sys.stdout.flush()

def run(args, out=subprocess.DEVNULL, err=subprocess.DEVNULL):
	try:
		subprocess.check_call(args, stdout=out, stderr=err)
	except:
		raise BuildError('While running: %s' % ' '.join(args)).with_traceback(sys.exc_info()[2])

def runAndGetOut(args):
        if not 'LC_ALL' in os.environ or os.environ['LC_ALL'] == 'C':
                os.environ['LC_ALL'] = 'en_US.UTF-8'
        return subprocess.check_output(args, stderr=subprocess.STDOUT).decode(locale.getdefaultlocale()[1])

def check_node():
	try:
		run(['node', '--version'])
	except:
		log("[ERROR] 'node' not installed!\n")
		return False

	return True

def check_android():
	if not 'ANDROID_SDK_ROOT' in os.environ:
		log("[ERROR] missing ANDROID_SDK_ROOT in PATH!\n")
		return False
	try:
		run(['android', 'list', 'avd'])
	except Exception as e:
		log("[ERROR] 'android' not installed! %s\n" % e)
		return False

	return True

def get_version():
	tree = ET.parse(os.path.join(REMOTE_DIR, 'config.xml'))
	xml_version = '.'.join(tree.getroot().attrib['version'].split('.')[:-1])
	git_version = runAndGetOut( ['git', '--git-dir=%s/.git' % DEPOT, 'describe', '--tags'] ).split('-')[1]
	return "%s.%s" % ( xml_version, git_version )

def install_deps():
	for dep in dependencies:
		package = "%s@%s" % (dep['package'], dep['version'])
		log("Installing %s" % package)
		run(['npm', 'install', '--global', '%s' % package ], out=None, err=None)

def build(options):
	log("Building TAC Remote app")
	command = ['cordova', 'build', 'android']
	if options.build_type == 'release':
		command.append('--release')
	os.chdir( REMOTE_DIR )
	run(command, out=None, err=None)

def install(options):
	log("Installing TAC Remote app")
	src_apk_file = os.path.join( REMOTE_DIR, 'platforms/android/build/outputs/apk/android-%s.apk' % options.build_type )
	filename = os.path.splitext(os.path.basename(src_apk_file))[0]
	extension = os.path.splitext(src_apk_file)[1][1:]
	dst_apk_file = os.path.join( options.out_dir, "%s-%s.%s" % (filename, get_version(), extension))
	shutil.copy(src_apk_file, dst_apk_file)

def main():
	global options

	parser = OptionParser()
	setOptions(parser)
	(options, args) = parser.parse_args()

	if not os.path.isabs(options.out_dir):
		options.out_dir = os.path.abspath(os.path.join(origWD, options.out_dir))

	log('\nCurrent config:')
	log('\tDEPOT: %s' % DEPOT)
	log('\toptions.build_type: %s' % options.build_type)
	log('\toptions.install_deps: %s\n' % options.install_deps)
	log('\toptions.out_dir: %s\n' % options.out_dir)

	if not check_node() or not check_android():
		sys.exit(1)

	if options.install_deps:
		install_deps()

	build(options)

	install(options)

	log("Done!")

origWD = os.getcwd()
# Run main
main()
os.chdir( origWD )
