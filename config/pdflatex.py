#!/usr/bin/env python2
import sys
import os
import subprocess

if len(sys.argv) < 6 :
	print("Error: too few arguments")
	sys.exit(1)
else:
	COMPILER = sys.argv[1]
	INTERACTION_LEVEL = sys.argv[2]
	JOBNAME = sys.argv[3]
	OUTPUTDIR = sys.argv[4]
	LATEX_FILE = sys.argv[5]

	command = [COMPILER, "-interaction", INTERACTION_LEVEL, "-jobname", JOBNAME, "-output-directory", OUTPUTDIR, LATEX_FILE]
	result = subprocess.call(command)

	#First run to generate pdf
	if result == 0:
		sys.exit( subprocess.call(command))
	else:
		sys.exit(result)