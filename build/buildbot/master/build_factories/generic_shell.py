import impl

from buildbot.process.factory    import BuildFactory
from buildbot.process.properties import Interpolate
from buildbot.steps.shell        import ShellCommand
from buildbot.steps.slave        import RemoveDirectory

import repositories

import os

def make_factory(platform):
	factory = BuildFactory()

	factory.addStep(
		RemoveDirectory(
			name = 'Remove build directory',
			dir = Interpolate('%(prop:workdir)s')
		)
	)

	factory.addStep(repositories.middleware)

	for command in platform["commands"]:
		factory.addStep(
			ShellCommand(
				name = command["name"],
				command= command["command"],
				workdir = command["workdir"],
				logEnviron = True,
				haltOnFailure = True,
			)
		)

	return factory
