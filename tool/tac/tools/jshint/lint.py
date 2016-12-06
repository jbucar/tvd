#!/usr/bin/env python3

from optparse import OptionParser
import os
import subprocess
import sys

def setOptions(parser):
	parser.add_option("-p", "--package", dest="package", action="store", type='string', default='', help="Package to lint" )
	parser.add_option("-c", "--config", dest="config", action="store", type='string', default='', help="Config to use" )
	parser.add_option("-f", "--file", dest="file", action="store", type='string', default='', help="File to lint" )

def main():
	parser = OptionParser()
	setOptions(parser)
	(options, args) = parser.parse_args()

	jshintDir = os.path.dirname(os.path.abspath(__file__))
	tacDir = os.path.dirname(os.path.dirname(jshintDir))
	pkgsDir = os.path.join( tacDir, 'packages' )
	jsHintBinDir = os.path.join( pkgsDir, 'node', 'node_modules_dev', 'jshint', 'bin')
	jsHint = os.path.join( jsHintBinDir, 'jshint' )
	os.environ['NODE_PATH'] = os.path.join( pkgsDir, 'node', 'node_modules' ) + ':' + os.path.join( pkgsDir, 'node', 'node_modules_dev' )

	src = '.'
	if (options.file != ''):
		src = options.file

	if (options.package != ''):
		src = os.path.join( tacDir, 'packages', options.package )

	if (options.config != ''):
		conf = options.config
	else:
		conf = os.path.join(os.path.dirname(src),'.jshintrc')
		if not os.path.exists(conf):
			conf = os.path.join(jshintDir, 'jshintconf.json')

	ignore = os.path.join(os.path.dirname(src),'.jshintignore')
	if not os.path.exists(ignore):
		ignore = os.path.join(jshintDir, '.jshintignore')

	try:
		subprocess.check_call([jsHint, '--config', conf, '--exclude-path', ignore, '--extra-ext', '.html', '--extract=auto', src])
	except:
		print('Linting of %s failed!' % src)
		sys.exit(1)

main()
