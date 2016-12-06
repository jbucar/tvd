import impl
import repositories
from config import config
import util

import custom_steps.slave
import custom_steps.master

from buildbot.process.factory           import BuildFactory
from buildbot.config                    import BuilderConfig
from buildbot.process.properties        import Interpolate
from buildbot.steps.shell               import ShellCommand

import os

tool_name = 'htmlshell'

# Builder vars
rootfs = 'build/install.HTMLSHELL/rootfs'
depot = '%(prop:workdir)s/build'

# Usage of tool/htmlshell/build.py
#	 -s : Configure build for (linux or aml)
#	 -b : Do the build
#	 -i : Install files (by default in rootfs)
#	 -t : Build htmlshelltest
#	 -m : Setup to run memcheck
#	 -p : Generate tar.gz
#	 -d : Make a debug build
#	 -g : Init a git repository with chromium sources in the build dir
#	 --install-dir=INSTALLDIR : Install directory (defaults to <build-dir>/rootfs)
#	 --dcheck-on : Release build with debug checks enabled

def make_factory(platform):
	factory = BuildFactory()
	extra = platform.get("extra", None)
	slave = config["slaves"]["slaves"][platform["slave"]]

	factory.addStep(repositories.middleware)

	env = {
		'USE_CCACHE': Interpolate('%(prop:ccache:#?|1|0)s'),
		'DEPOT': Interpolate(depot)
	}
	env.update(config['slaves']['slaves'][platform["slave"]]['env'])

	command = '../tool/htmlshell/build.py -s %s -b -i -p %s' % (platform['name'], extra['option'] if extra else '')
	command += ' %(prop:enable_dcheck:#?|--dcheck-on|)s'
	if platform['name'] != 'linux':
		command += ' --depot=/opt/%s/manifest.json' % platform['name'].upper()

	factory.addStep(
		ShellCommand(
			name = 'Compile %s for %s' % (tool_name, platform['name']),
			command = Interpolate(command),
			env = env,
			workdir = 'build/install.HTMLSHELL',
			logEnviron = True,
			haltOnFailure = True,
		)
	)

	if extra:
		factory.addStep(
			ShellCommand(
				name = '%s %s' % (extra['name'], tool_name),
				command= extra['command'],
				env = env,
				workdir = rootfs,
				logEnviron = True,
				haltOnFailure = True,
			)
		)

	factory.addStep(
		custom_steps.slave.PythonFunction(
			name = "Search for upload file",
			fnc = util.get_file,
			args = {
				'location': Interpolate(depot + '/install.HTMLSHELL/rootfs'),
			},
			property = 'htmlshelltar'
		)
	)

	max_history = config["general"]["build_max_history"]
	src = '%s/install.HTMLSHELL/rootfs/%s' % (depot, '%(prop:htmlshelltar)s')
	dst_file = '%(prop:htmlshelltar)s'
	dst_path = "%(prop:dst_path)s"

	factory.addStep(
		custom_steps.slave.PythonFunction(
			name = 'Get htmlshell version',
			fnc = util.get_htmlshell_version,
			args = { 'src': Interpolate(src) },
			property = 'toolversion',
			doStepIf = impl.doStepIfHasDestPath
		)
	)

	url = '%s/%s/%s' % (config["general"]["repository_url"], dst_path, dst_file)
	dst = '%s/%s/%s' % (config["general"]["repository"], dst_path, dst_file)
	base_dir = '%s/%s' % (config["general"]["repository"], dst_path)
	impl.upload(factory, src, dst, url, base_dir, 'LATEST.tar.gz', max_history, impl.doStepIfHasDestPath)

	return factory
