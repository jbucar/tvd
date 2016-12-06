#!/usr/bin/env python2

import json
import sys
import os
from optparse import OptionParser

def setParam(option, opt, value, parser):
	setattr(parser.values, option.dest, value)

COMPILE=""
def setTool(option, opt, value, parser):
	global COMPILE
	COMPILE="TOOL"
	setParam(option, opt, value, parser)

def setLib(option, opt, value, parser):
	global COMPILE
	COMPILE="LIB"
	setParam(option, opt, value, parser)

def setConfig(option, opt, value, parser):
	if value != "":
		os.putenv("USER_CONFIG_FILE", value)

def setOption(option, opt, value, parser):
	if value != "":
		if value.count('=') != 1:
			print("WARNING :: Malformed option '%s'. Not processed." % value)
			return
		var_name, var_value = value.split('=')
		os.putenv("TVD_%s" % var_name, var_value)

def checkBuildType(parser):
	if (parser.values.BUILD == "") or (parser.values.BUILD == "Debug") or (parser.values.BUILD == "Release") or (parser.values.BUILD == "RelWithDebInfo") or (parser.values.BUILD == "MinSizeRel"):
		return True
	else:
		return False

def checkParams(parser):
	if bool(parser.values.TOOL) ^ bool(parser.values.LIB):
		return checkBuildType(parser)
	else:
		print("ERROR :: You must set lib OR tool param to compile.")
		return False

#-- main()

def runCmd( command ):
	print(command)

	# Unset MAKEFLAGS to prevent make parallell processing conflicts
	os.environ["MAKEFLAGS"] = ""

	result = os.system(command)
	if not result == 0:
		sys.exit(1)

def setOptions(parser):
	parser.add_option("-t", "--tool", dest="TOOL", action="callback", callback=setTool, type="string", help="Specify the TOOL to compile.", metavar="TOOL")
	parser.add_option("-l", "--lib", dest="LIB", action="callback", callback=setLib, type="string", help="Specify the LIB to compile.", metavar="LIB")
	parser.add_option("-c", "--config", dest="CONFIG", action="callback", callback=setConfig, type="string", help="Specify the USER_CONFIG_FILE to use.", default="", metavar="CONFIG")
	parser.add_option("-s", "--set", dest="SET", action="callback", callback=setOption, type="string", help="Sets a cmake variable.", default="", metavar="SET")
	parser.add_option("-o", "--options", dest="SHOW_OPTIONS", action="store_true", default=False, help="Get the options defined in the LIB/TOOL. Also is needed to set -l or -t param.", metavar="SHOW_OPTIONS")
	parser.add_option("-f", "--files", dest="SHOW_FILES", action="store_true", default=False, help="Get the files needed to build the LIB/TOOL. Also is needed to set -l or -t param.", metavar="SHOW_FILES")
	parser.add_option("-d", "--deps", dest="SHOW_DEPS", action="store_true", default=False, help="Get target dependencies. Also is needed to set -l or -t param.", metavar="SHOW_DEPS")
	parser.add_option("-p", "--platform", dest="PLATFORM", action="callback", callback=setParam, type="string", help="Set the PLATFORM to compile.", default="", metavar="PLATFORM")
	parser.add_option("-k", "--pack", dest="PACKAGE", action="store_true", help="Pack the project?", default=False, metavar="PACKAGE")
	parser.add_option("-b", "--build", dest="BUILD", action="callback", callback=setParam, type="string", default="", help="Set the build type for make based generators.")
	parser.add_option("-v", "--verbose", dest="VERBOSE", action="store", type="int", default=0, help="Put cmake debug mode")
	parser.add_option("-g", "--generator", dest="GENERATOR", action="store", type="string", default="", help="Generate project for IDEs (Xcode, eclipse, KDevelop3)")
	parser.add_option("", "--depot", dest='BUILDROOT_DEPOT', action='store', type='string', default="", help='Path to a JSON file describing the buildroot depot to use')

def loadJSON(jsonFile):
	with open(jsonFile, 'r') as f:
		return json.load(f)

def getFullPath(manifest, key, rootDir):
	if (key in manifest):
		if os.path.isabs( manifest[key] ):
			return manifest[key]
		else:
			return os.path.join( rootDir, manifest[key] )
	return ""

def setupDepot(filename):
	if os.path.exists( filename ):
		print('Loading buildroot depot manifest: %s' % filename)
		manifest = loadJSON( filename )
		if 'name' in manifest:
			os.putenv("TVD_DEPOT_NAME", manifest['name'])
		os.putenv("TVD_DEPOT_TOOLCHAIN_PATH", getFullPath(manifest, 'toolchain', os.path.dirname(filename)))
		os.putenv("TVD_DEPOT_SYSROOT", getFullPath(manifest, 'staging', os.path.dirname(filename)))
		if 'toolchain_prefix' in manifest:
			os.putenv("TVD_DEPOT_TOOLCHAIN_PREFIX", manifest['toolchain_prefix'])
	else:
		print( 'Depot file not exist!: filename=%s' % filename )

def setupConfigure(parser):
	DEPOT = os.path.dirname(os.path.dirname(os.path.abspath( __file__ )))

	if parser.values.BUILDROOT_DEPOT:
		setupDepot( os.path.abspath( parser.values.BUILDROOT_DEPOT ) )

	if os.getenv("DEPOT") == None:
		os.putenv("DEPOT",DEPOT)
	else:
		DEPOT = os.getenv("DEPOT")

	if parser.values.PLATFORM == "":
		PLATFORM=""
	else:
		PLATFORM="-DCMAKE_TOOLCHAIN_FILE=\"%s/config/target/Platform_%s.cmake\"" % (DEPOT, parser.values.PLATFORM)

	if not parser.values.BUILD == "":
		BUILD="-DCMAKE_BUILD_TYPE=\""+parser.values.BUILD+"\""
	else:
		BUILD=""

	if not parser.values.VERBOSE == 0:
		VERBOSITY="-DVERBOSITY=%d" % parser.values.VERBOSE
	else:
		VERBOSITY=""

	if COMPILE == "TOOL":
		IS_TOOL="-DIS_TOOL=1"
		COMPILE_PROJECT="-DCOMPILE_PROJECT=\""+parser.values.TOOL+"\""
	else:
		IS_TOOL="-DIS_TOOL=0"
		COMPILE_PROJECT="-DCOMPILE_PROJECT=\""+parser.values.LIB+"\""

	if not parser.values.GENERATOR == "":
		GENERATOR="-G %s" % parser.values.GENERATOR
	else:
		GENERATOR=""

	return "cmake %s %s %s %s %s %s %s/config" % (VERBOSITY, BUILD, IS_TOOL, COMPILE_PROJECT, PLATFORM, GENERATOR, DEPOT)


parser = OptionParser()
setOptions(parser)

parser.parse_args()

if checkParams(parser):
	# Configure the project
	runCmd( setupConfigure(parser) )

	if parser.values.SHOW_FILES:
		runCmd( "cmake --build . --target files" )
	elif parser.values.SHOW_OPTIONS:
		runCmd( "cmake --build . --target options" )
	elif parser.values.SHOW_DEPS:
		runCmd( "cmake --build . --target deps" )
	else:
		if not parser.values.BUILD == "":
			cfg = "--config %s " % parser.values.BUILD
		else:
			cfg = ""

		# build them
		if parser.values.GENERATOR == "":
			runCmd( "cmake --build . %s" % cfg )

			if(parser.values.PACKAGE):
				runCmd( "cmake --build . --target pack %s" % cfg )

else:
	parser.print_help()

