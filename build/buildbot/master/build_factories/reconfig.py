import repositories
import util
from config import config
import slaves
import impl
import custom_steps.slave
import custom_steps.master

from buildbot.process.factory           import BuildFactory
from buildbot.steps.transfer            import DirectoryUpload
from buildbot.process.properties        import Interpolate
from buildbot.steps.master              import MasterShellCommand
from buildbot.config                    import BuilderConfig
from buildbot.schedulers.forcesched     import ForceScheduler,FixedParameter,StringParameter

import os

def make_factory():
	master_dir = os.getcwd()

	f = BuildFactory()

	f.addStep(repositories.middleware)

	f.addStep(
		DirectoryUpload(
			slavesrc = 'build/buildbot/master',
			masterdest = master_dir,
		)
	)

	f.addStep(
		custom_steps.master.PythonFunction(
			name = "Reloading custom modules",
			fnc = util.reload_all,
			args = {'dir': master_dir}
		)
	)

	def set_configuration(props, log):
		log.append(props['branch'])
		os.environ[config["general"]["buildbot_global_branch"]] = props['branch'].encode("utf-8")

	f.addStep(
		custom_steps.master.PythonFunction(
			name = "Setting branch configuration",
			fnc = set_configuration
		)
	)

	f.addStep(
		MasterShellCommand(
			command=['buildbot', 'reconfig']
		)
	)

	return f


def configure(buildmasterConfig):
	builder_name = 'Reconfig Master'
	buildmasterConfig['schedulers'].append(
		ForceScheduler(
			name = 'Force Reconfig Master',
			builderNames = [builder_name],
			branch = StringParameter(name='branch', default=impl.get_default_branch()),
		)
	)

	buildmasterConfig['builders'].append(
		BuilderConfig(
			name = builder_name,
			factory = make_factory(),
			slavename = "linux32"
		)
	)

