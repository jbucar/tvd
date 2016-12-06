#!/usr/bin/env python2
# -*- coding: utf8 -*-

import sys, re, os, shutil, subprocess

patterns = [
	['Namespace', 'módulo', None],
	['namespaces}', 'módulos}', None],
	['luatype', '', None],
	['@version@', '', None],
	['\\\\-::', '.', None],
	['\\\\-\\\\_\\\\-\\\\-Lua\\\\-Param\\\\-\\\\_\\\\-', '  ', None]
]

def compilePatterns():
	for pattern in patterns:
		print '[DOC2LUA]: compiling pattern: ', pattern[0]
		pattern[2] = re.compile( pattern[0] )

def foundPattern( pattern, line ):
	return re.sub( pattern[2], pattern[1], line)

def parseFile( filename ):
	fin = open(filename, "r")
	fout = open(filename + '.tmp', "w")
	print '[DOC2LUA]: Parsing file: ', fin.name
	for line in fin:
		tmp = line
		for pattern in patterns:
			tmp = foundPattern( pattern, tmp )
		fout.write( tmp )
	fin.close()
	fout.close()
	shutil.move(filename + '.tmp', filename)

outputDir = "out/latex/"

# Get version from parameter, if not use git tag
if len(sys.argv) > 1:
	patterns[3][1] = sys.argv[1]
else:
	#Read version from git
	proc = subprocess.Popen(["git", "describe", "--tags"], stdout=subprocess.PIPE)
	patterns[3][1] = proc.stdout.readline()

compilePatterns()
filenames = []

subprocess.call(["doxygen", "Doxyfile"])

print '[DOC2LUA]: Parsing directory ', outputDir
i = 0
for root,dirs,files in os.walk(outputDir):
	for f in files:
		if f.endswith(".tex"):
			filenames.append( os.path.join(root,f) )
			print '[DOC2LUA]: Found file: ' , filenames[i]
			i+=1;

for f in filenames:
	parseFile( f )

os.chdir(outputDir)
subprocess.call(["pdflatex", "refman.tex"])
subprocess.call(["pdflatex", "refman.tex"])

shutil.copyfile("refman.pdf", "../../lua_api.pdf")

os.chdir("../..")
#shutil.rmtree("out")
