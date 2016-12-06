from config import config
import impl
import repositories
import slaves
import util

import custom_steps.slave

from buildbot.process.factory           import BuildFactory
from buildbot.config                    import BuilderConfig
from buildbot.process.properties        import Interpolate
from buildbot.steps.shell               import SetPropertyFromCommand
from buildbot.steps.shell               import ShellCommand
from buildbot.steps.slave               import RemoveDirectory
from buildbot.schedulers.forcesched     import BooleanParameter, ChoiceStringParameter

import os

APK_DIR='/tool/tac/tools/remote'
NODE_ROOTFS_PATH='tool/tac/.build/node'
slave = config["slaves"]["slaves"]["ArchI7"]

def make_factory():
	factory = BuildFactory()

	factory.addStep(repositories.middleware)

	depot = '%(prop:workdir)s/build'

	env = {
		'DEPOT': Interpolate(depot),
		'NODE_PATH': Interpolate(os.path.join(depot, NODE_ROOTFS_PATH, 'lib/node_modules')),
		'PATH':  Interpolate( "%s:%s" % (
				os.path.join(depot, NODE_ROOTFS_PATH, 'bin'),
				'${PATH}'
				)
			)
	}
	env.update(slave['env'])

	node_command = './build.py -P linux -p build/node.conf -v 2 -o %s/builds' % depot

	android_command = './build.py'
	android_command += ' --buildType=%(prop:build_type)s'
	android_command += '%(prop:install_deps:#?| --install-deps|)s'

	factory.addStep(
		ShellCommand(
			name = 'Build node',
			command= Interpolate(node_command),
			env = env,
			workdir = 'build/tool/tac',
			logEnviron = True,
			haltOnFailure = True,
		)
	)

	factory.addStep(
		ShellCommand(
			name = 'Building android app',
			command= Interpolate(android_command),
			env = env,
			workdir = 'build/tool/tac/tools/remote',
			logEnviron = True,
			haltOnFailure = True,
		)
	)

	factory.addStep(
		custom_steps.slave.PythonFunction(
			name = 'Set date property',
			fnc = util.now,
			property = 'date'
		)
	)

	dst_folder = os.path.join('ANDROID', 'tac_remote')
	base_dir = '%s/%s' % (config["general"]["repository"], dst_folder)

	factory.addStep(
		custom_steps.slave.PythonFunction(
			name = "Get tac image version",
			fnc = util.get_apk_version,
			args = {
				'location': Interpolate(depot + APK_DIR),
			},
			property = 'toolversion'
		)
	)

	factory.addStep(
		SetPropertyFromCommand(
			name = 'Set commit number property',
			command = ['git', 'describe', '--tags'],
			property = "commit"
		)
	)

	factory.addStep(
		custom_steps.slave.PythonFunction(
			name = 'Format commit number',
			fnc = util.format_commit_number
		)
	)

	# Upload android-*.apk
	src = Interpolate('%s/%s/android-%s-%s.apk' % (depot, APK_DIR, '%(prop:build_type)s', '%(prop:toolversion)s'))
	dst_file = 'android-%(prop:build_type)s-%(prop:date)s-%(prop:toolversion)s.apk'
	url = Interpolate('%s/%s/%s' % (config["general"]["repository_url"], dst_folder, dst_file))
	dst = '%s/%s/%s' % (config["general"]["repository"], dst_folder, dst_file)
	impl.upload(factory, src, dst, url, base_dir, 'LATEST.apk', config["general"]["release_build_history"])

	factory.addStep(
		RemoveDirectory(
			name = 'Remove build directory',
			dir = Interpolate('%(prop:workdir)s')
		)
	)

	return factory

def make_builder(build_lock):
	return BuilderConfig(
		name = 'TAC_REMOTE',
		factory = make_factory(),
		slavename = slave['name'],
		locks = [build_lock.access('counting')]
	)

def configure(buildmasterConfig, build_lock):
	builders = []

	builders.append(make_builder(build_lock))

	extraProps = [
		BooleanParameter(name="install_deps", label="Install dependencies?", default=True),
		ChoiceStringParameter(name="build_type", label="Build type:", choices=["release", "debug"], default="release"),
	]

	buildmasterConfig['builders'].extend(builders)
	buildmasterConfig['schedulers'].extend(impl.nightly_schedule('tac_remote', builders, properties=extraProps))
