from config import config
import repositories
import impl
import util

from buildbot.process.factory           import BuildFactory
from buildbot.process.properties        import Interpolate, WithProperties
from buildbot.steps.shell               import SetPropertyFromCommand, ShellCommand
from buildbot.steps.slave               import RemoveDirectory
from buildbot.steps.slave               import SetPropertiesFromEnv
from buildbot.steps.transfer            import FileUpload

depot = '%(prop:workdir)s/build'

def make_factory(platform):
	# Interpolate al env variables
	env = platform['env']
	for env_key, env_value in env.items():
		env[env_key] = Interpolate(env_value)

	factory = BuildFactory()

	factory.addStep(repositories.thirdparty)

	factory.addStep(SetPropertiesFromEnv(variables=['HOME']))

	factory.addStep(
		ShellCommand(
			name = 'Create workdir',
			command= ['mkdir', '-p', platform['workdir']],
			env = env,
			workdir = Interpolate(depot),
			logEnviron = True,
			haltOnFailure = True
		)
	)

	factory.addStep(
		ShellCommand(
			name = 'Create dl symlink',
			command= ['ln', '-s', WithProperties(platform['sources_dir']), 'dl'],
			env = env,
			workdir = Interpolate(platform['workdir']),
			logEnviron = True,
			haltOnFailure = True
		)
	)

	if platform['kernel']:
		factory.addStep(
			ShellCommand(
				name = 'Configure kernel',
				command= ['make', platform['kernel']['config']],
				env = env,
				workdir = Interpolate(platform['kernel']['workdir']),
				logEnviron = True,
				haltOnFailure = True
			)
		)

		factory.addStep(
			ShellCommand(
				name = 'Compiling kernel image',
				command= ['make',  '-j5', 'ARCH=arm', 'uImage' ],
				env = env,
				workdir = Interpolate(platform['kernel']['workdir']),
				logEnviron = True,
				haltOnFailure = True
			)
		)

		factory.addStep(
			ShellCommand(
				name = 'Compiling kernel modules',
				command= ['make',  '-j5', 'ARCH=arm', 'modules' ],
				env = env,
				workdir = Interpolate(platform['kernel']['workdir']),
				logEnviron = True,
				haltOnFailure = True
			)
		)

		factory.addStep(
			ShellCommand(
				name = 'Installing kernel modules',
				command= ['make',  '-j5', 'ARCH=arm', 'INSTALL_MOD_PATH=out', 'modules_install' ],
				env = env,
				workdir = Interpolate(platform['kernel']['workdir']),
				logEnviron = True,
				haltOnFailure = True
			)
		)

	factory.addStep(
		ShellCommand(
			name = 'Configure buildroot',
			command= platform['config_cmds'],
			env = env,
			workdir = Interpolate(platform['workdir']),
			logEnviron = True,
			haltOnFailure = True
		)
	)

	factory.addStep(
		ShellCommand(
			name = 'Compile buildroot',
			command= ['make'],
			env = env,
			workdir = Interpolate(platform['workdir']),
			logEnviron = True,
			haltOnFailure = True
		)
	)

	factory.addStep(
		SetPropertyFromCommand(
			name = 'Set commit number property',
			command = [ Interpolate(cmd) for cmd in platform['commit_cmd']],
			property = "commit"
		)
	)

	src_file = platform['src_filename']
	src = platform['src_path'] % (depot, src_file)
	dst_path = "%(prop:dst_path)s"
	dst = '%s/%s/%s' % (config["general"]["repository"], dst_path, src_file)
	url = '%s/%s/%s' % (config["general"]["repository_url"], dst_path, src_file)

	factory.addStep(
		FileUpload(
			name = 'Upload depot to %s' % util.ReInterpolate(Interpolate(dst_path)),
			slavesrc = util.ReInterpolate(Interpolate(src)),
			masterdest = util.ReInterpolate(Interpolate(dst)),
			url = util.ReInterpolate(Interpolate(url)),
			mode = 0644,
			doStepIf = impl.doStepIfHasDestPath
		)
	)


	factory.addStep(
		RemoveDirectory(
			name = 'Remove build directory',
			dir = Interpolate('%(prop:workdir)s')
		)
	)

	return factory
