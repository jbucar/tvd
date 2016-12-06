import util
from config import config

import json
import urllib2

import custom_steps.slave
import custom_steps.master

from buildbot.schedulers.forcesched     import ChoiceStringParameter, ForceScheduler, StringParameter, UserNameParameter, FixedParameter
from buildbot.scheduler                 import Nightly
from buildbot.process.properties        import Interpolate
from buildbot.steps.transfer            import FileUpload

from twisted.python import log

import os

def doStepAlways(step):
	return True

def doStepIfHasDestPath(step):
	return step.getProperty("dst_path") and (len(step.getProperty("dst_path")) > 0)


#TODO: Replace upload for a triggrable scheduler ?
def upload(factory, src, dst, url, base_dir, link_name, max_history=None, doStep=doStepAlways):
	factory.addStep(
		FileUpload(
			name = 'Upload build',
			slavesrc = Interpolate(src),
			masterdest = util.ReInterpolate(Interpolate(dst)),
			url = Interpolate(url),
			mode = 0755,
			doStepIf = doStep
		)
	)

	if url:
		factory.addStep(
			custom_steps.slave.PythonFunction(
				name = 'Set url property',
				fnc = util.set,
				args = {'to_set': util.ReInterpolate(Interpolate(url)) },
				property = 'url',
				doStepIf = doStep
			)
		)

	if max_history:
		factory.addStep (
			custom_steps.master.PythonFunction(
				name = 'Delete old builds, keeping the last %d' % max_history,
				fnc = util.remove_files,
				args = {'dir': base_dir, 'amount': max_history},
				doStepIf = doStep
			)
		)

	factory.addStep(
		custom_steps.master.PythonFunction(
			name = 'Make %s link' % link_name,
			fnc = util.make_symbolic_link,
			args = {
				'src': util.ReInterpolate(Interpolate(os.path.split(dst)[1])),
				'dst': util.ReInterpolate(Interpolate('%s/%s' % (base_dir, link_name)))
			},
			doStepIf = doStep
		)
	)

	linkNameParts = link_name.split('.')
	factory.addStep(
		custom_steps.master.PythonFunction(
			name = 'Make versioned link',
			fnc = util.make_versioned_link,
			args = {
				'src': util.ReInterpolate(Interpolate(os.path.split(dst)[1])),
				'dst': util.ReInterpolate(Interpolate(base_dir)),
				'linkName': linkNameParts[0],
				'linkExt': '.'.join(linkNameParts[1:]),
				'version': Interpolate('%(prop:toolversion)s')
			},
			doStepIf = doStep
		)
	)

	return

def get_default_branch():
	prop = config["general"]["default_branch"]
	if config["general"]["buildbot_global_branch"] in os.environ:
		prop = os.environ[config["general"]["buildbot_global_branch"]]
	return prop

def getDefaultProperties(properties):
	defaults = {}
	for p in properties:
		defaults[p.name] = p.default
	return defaults

def force_schedule(
			target,
			builders,
			branch,
			hide_branch=False,
			properties=[],
			name='',
			dayOfWeek='*',
			username=UserNameParameter(),
			reason=StringParameter(name="reason", default="force build", length=20),
			revision=None,
			repository=None,
			project=None
		):
	builder_names = map(lambda builder: getattr(builder, "name"), list(builders))
	branch_param = StringParameter(name="branch", default=branch)
	if hide_branch:
		branch_param = FixedParameter(name="branch", default=branch)

	if name == '':
		name = "Force build for %s" % target.upper()

	return [
		ForceScheduler(
			name = name,
			builderNames = builder_names,
			branch = branch_param,
			properties = properties,
			username = username,
			reason = reason,
			revision = revision,
			repository = repository,
			project = project
		)
	]

def nightly_schedule(target,
			builders,
			branch,
			hide_branch=False,
			properties=[],
			name='',
			dayOfWeek='*',
			username=UserNameParameter(),
			reason=StringParameter(name="reason", default="force build", length=20),
			revision=None,
			repository=None,
			project=None
		):

	schedulers = []
	builder_names = map(lambda builder: getattr(builder, "name"), list(builders))

	if name == '':
		name = "Automatic %s build (from %s branch)" % (target.capitalize(), branch)

	schedulers.append(
		Nightly(
			name = name,
			builderNames = builder_names,
			branch = branch,
			properties = getDefaultProperties(properties),
			dayOfWeek = dayOfWeek,
			hour = config["general"]["nightly_scheduler_start"]['hour'],
			minute = config["general"]["nightly_scheduler_start"]['minute']
		)
	)

	return schedulers

def emptyParameter(name):
	return FixedParameter(name=name, default='')

class DownloadedJSONParameter(ChoiceStringParameter):
	# buildbot token is used to download the list of patches
	url = config["general"]["patches_url"]

	def getFromKwargs(self, kwargs):
		args = kwargs.get(self.fullName, [])
		if (len(args) == 0) and self.required:
			raise ValidationError("'%s' needs to be specified" % (self.label))
		if self.regex:
			for arg in args:
				if not self.regex.match(arg):
					raise ValidationError("%s:'%s' does not match pattern '%s'"
							% (self.label, arg, self.regex.pattern))

		try:
			arg = self.parse_from_args(args)
		except Exception, e:
			if self.debug:
				traceback.print_exc()
			raise e
		if arg is None:
			raise ValidationError("need %s: no default provided by config"
					% (self.fullName,))
		return arg

	def getChoices(self, master, scheduler, buildername):
		self.choices = []
		html = urllib2.urlopen(self.url).read()
		for item in json.loads(html):
			self.choices.append(item['name'].encode('latin_1'))

		log.msg("Got the following patchlist: %s" % ','.join(self.choices))
		return self.choices
