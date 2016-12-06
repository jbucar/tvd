#!/usr/bin/env python2
import sys
import os
import subprocess
from argparse import ArgumentParser


DEPOT = os.path.dirname(os.path.dirname(os.path.abspath( __file__ )))
RELEASE_OUTPUT="output.tar"

def setOptions(parser):
	parser.add_argument("-t", "--tool", dest="TOOL", action="append", help="Specify a TOOL to compile.", default=[])
	parser.add_argument("-l", "--lib", dest="LIB", action="append", help="Specify a LIB to compile.", default=[])
	parser.add_argument("-p", "--platform", dest="PLATFORM", action="append", help="Specify a PLATFORM to use.", )
	parser.add_argument("-s", "--system", dest="SYSTEM", action="append", help="Specify a SYSTEM to use.", )

parser = ArgumentParser(epilog="You can also add extra build.py parameters")
setOptions(parser)

args, unknown = parser.parse_known_args()

if not ( args.LIB or args.TOOL ):
	parser.print_help()
	print "\nYou must specify at least a tool or a lib."
	sys.exit(-1)

if not ( args.PLATFORM and args.SYSTEM ):
	parser.print_help()
	print "\nYou must specify at least one platform and one system."
	sys.exit(-1)

RELEASE_EXTRA_PARAM=" ".join(unknown)

print
print "RELEASE_OUTPUT: %s" % RELEASE_OUTPUT
print "RELEASE_PLATFORMS: %s" % (" ".join(args.PLATFORM))
print "RELEASE_SYSTEMS: %s" % (" ".join(args.SYSTEM))
print "RELEASE_LIBS: %s" % (" ".join(args.LIB))
print "RELEASE_TOOLS: %s" % (" ".join(args.TOOL))
print "RELEASE_EXTRA_PARAM: %s" % RELEASE_EXTRA_PARAM
print


GENERIC_PROJECT_FILENAMES=['build/build.py', 'config/CMakeLists.txt', 'config/Desktop.cmake', 'config/doxygen/', 
			   'config/Impl.cmake', 'config/modules/', 'config/pdflatex.py', 'config/Tvd.cmake', 
			   'config/Utils.cmake', 'config/target/gnu.cmake', 'config/target/Platform_@PLATFORM@.cmake', 
			   'config/target/SetupGlobal.cmake', 'config/target/SetupGlobal.PLATFORM_@PLATFORM@.cmake', 
			   'config/target/SetupGlobal.SYSTEM_@CMAKE_SYSTEM_NAME@.cmake', 'config/target/st_@ST_GENERIC_PLATFORM_LOWER@.cmake', 
			   'config/package/COPYING', 'config/package/COPYING.LESSER', 'config/package/CPackConfig.cmake', 
			   'config/package/CPackConfig.PLATFORM_@PLATFORM@.cmake', 'config/package/CPackConfig.SYSTEM_@CMAKE_SYSTEM_NAME@.cmake', 
			   'config/package/NSIS.template.in', 'doc/', 'lib/dtv-sample/', 'tool/sample-tool/']
for system in args.SYSTEM:
	GENERIC_PROJECT_FILENAMES.append("config/package/CPackConfig.SYSTEM_%s.cmake" % system)

for platform in args.PLATFORM:
	GENERIC_PROJECT_FILENAMES.append("config/package/CPackConfig.PLATFORM_%s.cmake" % platform)
	
GENERIC_PRODUCT_FILENAMES=['apps/', 'config/', 'data/', 'debian/', 'doc/', 'examples/', 'fonts/', 'images/', 'imgs/', 
			   'lua/', 'media/', 'qml/', 'res/', 'test/', 'test1/', 'AUTHORS', 'ChangeLog', 'CMakeLists.txt', 
			   'config.h.cin', 'COPYING', 'COPYING.LESSER', 'deps.cmake', 'lirc_input.conf', '@name@.desktop.cin', 
			   '@name@.lgpl.txt', 'sources.cmake']
CUSTOM_FILENAMES=['deps', 'sources', 'install', 'config']
FILES=[]


for release_platform in args.PLATFORM:
	platform_param = ""
	if not (release_platform in [ 'UNIX', 'VS' ]):
		platform_param="-p %s" % release_platform

	if release_platform == "ANDROID":
		GENERIC_PROJECT_FILENAMES.extend(["config/package/AndroidManifest.xml.in", "config/package/Android.mk.in", "config/package/Apk.cmake" ])

	env = os.environ.copy()
	env['GENERIC_PROJECT_FILENAMES'] = " ".join(GENERIC_PROJECT_FILENAMES)
	env['GENERIC_PRODUCT_FILENAMES'] = " ".join(GENERIC_PRODUCT_FILENAMES)
	env['CUSTOM_FILENAMES'] = " ".join(CUSTOM_FILENAMES)


	for lib in args.LIB:
		command = "%s/build/build.py -f -l %s %s %s" % ( DEPOT, lib, platform_param, RELEASE_EXTRA_PARAM)
		print "Getting sources from: %s" % command

		(files, err) = subprocess.Popen(command+" | grep ^deploy | cut -f 2- -d :", stdout=subprocess.PIPE, shell=True, env=env).communicate()
		FILES.extend([x.strip() for x in files.split("\n")])

	for tool in args.TOOL:
		command = "%s/build/build.py -f -t %s %s %s" % ( DEPOT, tool, platform_param, RELEASE_EXTRA_PARAM)
		print "Getting sources from: %s" % command

		(files, err) = subprocess.Popen(command+" | grep ^deploy | cut -f 2- -d :", stdout=subprocess.PIPE, shell=True, env=env).communicate()
		FILES.extend([x.strip() for x in files.split("\n")])

UNIQ_FILES = set(FILES)

EXCLUDED_DIRS=[]
# create exludings.
EXCLUDE_PRODUCTS=[ 'dtv-canvas', 'dtv-gingaplayer', 'ncl30-presenter' ]
EXCLUDE_PLATFORMS=[ 'cairo', 'directfb', 'gl', 'qt', 'sdl', 'skia' ]
for prod in EXCLUDE_PRODUCTS:
	for plat in EXCLUDE_PLATFORMS:
		EXCLUDED_DIRS.append("--exclude=lib/%s/test/images/expected/%s" % (prod, plat) )

print
print "EXCLUDED_DIRS: %s" % EXCLUDED_DIRS
print

print "Packaging files..."
subprocess.call("tar -C %s -cf %s %s %s" % (DEPOT, RELEASE_OUTPUT,
					    " ".join(EXCLUDED_DIRS),
					    " ".join(UNIQ_FILES) ), shell=True )
