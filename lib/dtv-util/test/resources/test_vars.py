#!/usr/bin/python

import os
import sys

VAR = os.getenv("VAR")
VAR2 = os.getenv("VAR2")

if VAR == "/home/path" and VAR2 == "/home/path2:/etc/path3":
	fileHandle = open('ok.env', 'w') 
	fileHandle.close()

	if len(sys.argv)==3:
		if sys.argv[1]=="parameter1" and sys.argv[2]=="parameter2":
			fileHandle = open('ok.vars', 'w') 
			fileHandle.close()
