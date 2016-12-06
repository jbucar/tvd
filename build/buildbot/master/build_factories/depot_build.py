from buildbot.config                    import BuilderConfig
from buildbot.process.factory		    import BuildFactory
from buildbot.process.properties        import Interpolate
from buildbot.steps.master              import SetProperty
from buildbot.steps.trigger             import Trigger
from buildbot.steps.slave               import RemoveDirectory
from buildbot.steps.shell               import Compile, ShellCommand

from twisted.python import log

import custom_steps.slave
import custom_steps.master
import impl
import os
import util
import repositories
from config import config

class DetermineConfig(object):
	def __init__(self, test=True, include=''):
		self.test = test
		self.include = include

	def on_if(self, var, value, extra=True):
			return ('ON' if ((var == value) and extra) else 'OFF')

	def getRenderingFor(self, props):
		build_type = props['build_type']
		build_method = props['build_method']
		test = self.test and (props['build_method'] == 'static')

		config =  'SET(BUILD_SHARED %s)\n' % self.on_if(build_method, 'shared')
		config += 'SET(CMAKE_BUILD_TYPE %s)\n' % build_type.capitalize()
		config += 'SET(COMPILE_TESTS %s)\n' % self.on_if(build_type, 'debug', test)
		config += 'SET(RUN_TESTS %s)\n' % self.on_if(build_type, 'debug', test)
		config += 'SET(TEST_GINGAPLAYER_TCP %s)\n' % self.on_if(build_type, 'debug', test)
		config += 'SET(MAKE_DOCUMENTATION %s)\n' % self.on_if(build_type, 'release')
		config += 'SET(MAKE_DOXYGEN %s)\n' % self.on_if(build_type, 'release')
		if self.include:
			config += 'INCLUDE(%s/build/%s)\n' % (props['workdir'], self.include)

		return config

def make_factory(platform):
	factory = BuildFactory()
	toolchain = config["toolchains"][platform["toolchain"]]
	slave = config["slaves"]["slaves"][platform["slave"]]
	product = platform["product"]

	factory.addStep(
		RemoveDirectory(
			name = 'Remove build directory',
			dir = Interpolate('%(prop:workdir)s')
		)
	)

	factory.addStep(repositories.middleware)

	factory.addStep(
		custom_steps.slave.PythonFunction(
			name = "Writte configuration",
			fnc = util.temporary_file,
			args = {
				'location': config["general"]["build_configuration_location"],
				'content': DetermineConfig(toolchain["test"], platform["config"])
			},
			property = 'configfile'
		)
	)

	env = toolchain['env'].copy()
	env.update(slave['env'])
	env.update({
		'USER_CONFIG_FILE': Interpolate('%(prop:configfile)s'),
		'DEPOT': Interpolate('%(prop:workdir)s/build')
	})

	p_value = toolchain.get('platform_name', toolchain['name'])
	args = '%s %s' % ('-t' if platform["is_tool"] else '-l', product)
	args += ' -p%s' % p_value if toolchain['set_platform_flag'] else ''
	args += ' -k'
	if 'platform_name' in toolchain:
		args += ' --depot=/opt/%s/manifest.json' % toolchain['platform_name']

	factory.addStep(
		ShellCommand(
			name = 'Print cmake options (-o output)',
			command= 'python build/build.py -v 1 %s -o' % args,
			env = env,
			workdir = None,
			logEnviron = True
		)
	)

	factory.addStep(
		Compile(
			name = 'Compile ' + product.capitalize(),
			command = 'python build/build.py -v 1 %s' % args,
			env = env,
			workdir = None,
			logEnviron = True,
			flunkOnWarnings = toolchain['flunk_on_warnings'],
			timeout = config["general"]["compilation_timeout"],
			warningPattern='.*warning:.*'
		)
	)

	factory.addStep(
		custom_steps.slave.PythonFunction(
			name = 'Get tool version',
			fnc = util.find_in_file,
			args = {
				'src': Interpolate('%s/build/%s/generated/config.h' % ('%(prop:workdir)s/', product)),
				'to_find': "%s_VERSION \"" % (product.upper().replace("-","_"))
			},
			property = 'toolversion',
			doStepIf = impl.doStepIfHasDestPath
		)
	)

	factory.addStep(
		custom_steps.slave.PythonFunction(
			name = 'Set date property',
			fnc = util.now,
			property = 'date',
			doStepIf = impl.doStepIfHasDestPath
		)
	)
	dst_path = "%(prop:dst_path)s"
	max_history = config["general"]["build_max_history"]
	src = '%s/%s-%s.%s' % (product, product, '%(prop:toolversion)s', toolchain['pack_extension'])
	dst_file = '%s.%s' % ('%(prop:date)s-%(prop:toolversion)s', toolchain['pack_extension'])
	url = '%s/%s/%s' % (config["general"]["repository_url"], dst_path, dst_file)
	dst = '%s/%s/%s' % (config["general"]["repository"], dst_path, dst_file)
	base_dir = '%s/%s' % (config["general"]["repository"], dst_path)
	impl.upload(factory, src, dst, url, base_dir, 'LATEST.%s' % toolchain['pack_extension'], max_history, impl.doStepIfHasDestPath)

	factory.addStep(
		RemoveDirectory(
			name = 'Remove build directory',
			dir = Interpolate('%(prop:workdir)s')
		)
	)

	return factory
