from buildbot.buildslave                import BuildSlave
from buildbot                           import locks

from config import config
import slave_group_lock

def configure(buildmasterConfig):
	#   Configure default common properties
	for slave_name,slave in config["slaves"]["slaves"].items():
		for k, v in config["slaves"]["defaults"].items():
			if k not in slave:
				slave[k] = v

		#   Add slave !
		buildmasterConfig['slaves'].append(
			BuildSlave(
				slave_name,
				slave['accesskey'],
				notify_on_missing = slave["admin"]
			)
		)

	build_lock=slave_group_lock.SlavegroupLock("slave_builds", maxCount=config["general"]["max_parallel_builds"], slaveToSlavegroup=config["slaves"]["slaveToSlavegroup"])

	return build_lock
