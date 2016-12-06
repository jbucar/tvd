import slaves
import impl
import repositories
from config import config
import util
import os

import custom_steps.slave

from buildbot.process.factory           import BuildFactory
from buildbot.config                    import BuilderConfig
from buildbot.process.properties        import Interpolate, Property
from buildbot.steps.shell               import SetPropertyFromCommand
from buildbot.steps.shell               import ShellCommand
from buildbot.steps.slave               import RemoveDirectory
from buildbot.steps.master              import MasterShellCommand
from buildbot.schedulers.forcesched     import BooleanParameter, FixedParameter

from twisted.python import log

import os

# Usage of tool/tac/build.py
#	-P,--platform   : Target platform
#	-p,--packages   : Packages to build
#	-i,--image      : Create image
#	-t,--test       : Run tests
#	-v,--verbose    : Verbose
#	-e,--extra      : Extra tpks directory
#	-b,--buildType  : Build type (release, debug)
#	  ,--buildCfg   : Build config (dev, beta, stable)
#     ,--build-deps : Build dependencies instead of downloading
#     ,--deps-dir   : Directory where dependencies are looked for
#     ,--clean-data : Clean data directory in deploy dir

def uploadPackages(depot, platformName, baseDir, destFolder, factory, sufixes):
	max_history = config["general"]["build_max_history"]

	for sufix in sufixes:
		src = '%s/tool/tac/.build/packages/image-%s-%s.%s' % (depot, platformName, '%(prop:toolversion)s', sufix)
		dst_file = 'image-%s-%s.%s' % ('%(prop:date)s', '%(prop:toolversion)s', sufix)
		url = '%s/%s/%s' % (config["general"]["repository_url"], destFolder, dst_file)
		dst = '%s/%s/%s' % (config["general"]["repository"], destFolder, dst_file)
		impl.upload(factory, src, dst, url, baseDir, 'LATEST.%s' % sufix, max_history * 2, impl.doStepIfHasDestPath)

def exportPropertySet(step):
	return step.getProperty("export") and impl.doStepIfHasDestPath(step)

def backupFilesPropertySet(step):
	return step.getProperty("backup_files") and impl.doStepIfHasDestPath(step)

def hasPatches(step):
	return len(step.getProperty("patches")) > 0

class DeterminePatches(object):
	def getRenderingFor(self, props):
		return ','.join(props['patches'])

def make_factory(platform):
	factory = BuildFactory()

	factory.addStep(repositories.middleware)

	depot = '%(prop:workdir)s/build'

	env = {
		'DEPOT': Interpolate(depot),
		'USE_CCACHE': Interpolate('%(prop:ccache:#?|1|0)s'),
	}
	env.update(config['slaves']['slaves'][platform["slave"]]['env'])

	factory.addStep(
		ShellCommand(
			name = 'Applying patches',
			command= ['./build/check_backed_branch.py', '-p', DeterminePatches() ],
			env = env,
			workdir = 'build',
			logEnviron = True,
			haltOnFailure = True,
			doStepIf = hasPatches
		)
	)

	command = [ './build.py', '-P', platform['platform'], '-i', '-p', 'build/release.conf', '-v', '2', '--clean-data' ]
	command +=[ Interpolate('--buildType=%(prop:build_type)s') ]
	command += [ Interpolate('--buildCfg=%(prop:build_cfg)s') ]
	command += [ Interpolate('%(prop:build_deps:#?|--build-deps|)s') ]
	command += ['-e', Interpolate('/home/buildslave/tpk_extras_%(prop:build_type)s') ]
	if platform['run_tests']:
		command += ['-t']

	factory.addStep(
		ShellCommand(
			name = 'Generating image for platform %s' % platform['platform'],
			command= command,
			env = env,
			workdir = 'build/tool/tac',
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

	dst_path = "%(prop:dst_path)s"
	base_dir = '%s/%s' % (config["general"]["repository"], dst_path)

	factory.addStep(
		custom_steps.slave.PythonFunction(
			name = "Get tac image version",
			fnc = util.get_tac_img_version,
			args = {
				'location': Interpolate(depot + '/tool/tac/.build/packages'),
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

	uploadPackages(depot, platform['platform'], base_dir, dst_path, factory, ['tac','tac.sym','sdk'])

	cfg_name = '%(prop:workdir)s/build/image.json'
	factory.addStep(
		custom_steps.slave.PythonFunction(
			name = 'Make join configuration file',
			fnc = util.create_join_config,
			args = {
				'filename': Interpolate(cfg_name),
				'model_id': platform['model_id'],
				'src': Interpolate('%s/%s/image-%s-%s.tac' % (config["general"]["repository_url"], dst_path, '%(prop:date)s', '%(prop:toolversion)s')),
				'version': Interpolate('%(prop:commit)s'),
				'local_src': Interpolate('%s/tool/tac/.build/packages/image-%s-%s.tac' % (depot, platform['platform'], '%(prop:toolversion)s')),
				'output_format':'json',
				'mandatory': Interpolate('%(prop:mandatory)s'),
			},
			doStepIf = impl.doStepIfHasDestPath
		)
	)


	cfg_dst = '%s/image-%s.json' % (base_dir, '%(prop:date)s-%(prop:toolversion)s')
	impl.upload(factory, cfg_name, cfg_dst, '', base_dir, 'LATEST.json', config["general"]["build_max_history"] * 2, impl.doStepIfHasDestPath)

	dst_file_base = 'image-%s-%s' % ('%(prop:date)s', '%(prop:toolversion)s')
	factory.addStep(
		MasterShellCommand(
			name='Upload files to sol.info.unlp.edu.ar',
			command=[os.path.join(os.getcwd(), 'tac_image_ftp_upload.sh'), platform['repo'], platform['platform'], Interpolate(dst_file_base)],
			doStepIf=exportPropertySet
		)
	)

	factory.addStep(
		MasterShellCommand(
			name='Backup files',
			command=[os.path.join(os.getcwd(), 'tac_image_backup.sh'), platform['repo'], platform['codename'], Interpolate('%(prop:toolversion)s'), Interpolate('%(prop:build_cfg)s'), Interpolate(dst_file_base)],
			doStepIf=backupFilesPropertySet
		)
	)

	factory.addStep(
		RemoveDirectory(
			name = 'Remove build directory',
			dir = Interpolate('%(prop:workdir)s')
		)
	)

	return factory
