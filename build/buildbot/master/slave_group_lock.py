from buildbot.process.slavebuilder import SlaveBuilder
from buildbot.locks import BaseLockId, BaseLock, SlaveLock

# This code lets us enforce a count across a graph of buildslaves.
class RealSlavegroupLock:
    def __init__(self, lockid):
        self.name = lockid.name
        self.maxCount = lockid.maxCount
        self.maxCountForSlavegroup = lockid.maxCountForSlavegroup
        self.slaveToSlavegroup = lockid.slaveToSlavegroup
        self.description = "<SlavegroupLock(%s, %s, %s, %s)>" % (self.name,
                                                                 self.maxCount,
                                                                 self.maxCountForSlavegroup,
                                                                 self.slaveToSlavegroup)
        self.locks = {}
    def __repr__(self):
        return self.description
    def getLock(self, slave):
        if isinstance(slave, SlaveBuilder):
            slavename = slave.slave.slavename
        else:
            slavename = slave.slavename
        # slavegroup defaults to slavename
        slavegroup = self.slaveToSlavegroup.get(slavename, slavename)
        if not self.locks.has_key(slavegroup):
            maxCount = self.maxCountForSlavegroup.get(slavegroup,
                                                      self.maxCount)
            lock = self.locks[slavegroup] = BaseLock(self.name, maxCount)
            desc = "<SlavegroupLock(%s, %s)[%s] %d>" % (self.name, maxCount,
                                                        slavegroup, id(lock))
            lock.description = desc
            self.locks[slavegroup] = lock
        return self.locks[slavegroup]

# Note: this only inherits from SlaveLock to get past an assert. Ideally the
# buildbot code would be changed to allow other lock types.
class SlavegroupLock(SlaveLock):
    """I am a semaphore that limits simultaneous actions on each group of
    buildslaves.

    Builds and BuildSteps can declare that they wish to claim me as they run.
    Only a limited number of such builds or steps will be able to run
    simultaneously on any given group of buildslave. By default this number is one,
    but my maxCount parameter can be raised to allow two or three or more
    operations to happen across a group of buildslaves at the same time.

    Use this to protect a resource that is shared among all the builds taking
    place on a group of slaves that share resources, for example to limit CPU
    or memory load on an underpowered machine that runs multiple buildslaves.

    Each buildslave can be assigned to a group using the dictionary
    slaveToSlavegroup; buildslaves that do not appear in this dictionary are
    placed in the slavegroup with a name equal to the name of the buildslave.

    Each group of buildslaves will get an independent copy of this semaphore. By
    default each copy will use the same owner count (set with maxCount), but
    you can provide maxCountForSlavegroup with a dictionary that maps
    slavegroup to owner count, to allow some slavegroups more parallelism than
    others.
    """

    compare_attrs = ['name', 'maxCount', '_maxCountForSlavegroupList',
                     '_slaveToSlavegroupList']
    lockClass = RealSlavegroupLock
    def __init__(self, name, maxCount=1, maxCountForSlavegroup={}, slaveToSlavegroup={}):
        self.name = name
        self.maxCount = maxCount
        self.maxCountForSlavegroup = maxCountForSlavegroup
        self.slaveToSlavegroup = slaveToSlavegroup
        # for comparison purposes, turn this dictionary into a stably-sorted
        # list of tuples
        self._maxCountForSlavegroupList = self.maxCountForSlavegroup.items()
        self._maxCountForSlavegroupList.sort()
        self._maxCountForSlavegroupList = tuple(self._maxCountForSlavegroupList)
        self._slaveToSlavegroupList = self.slaveToSlavegroup.items()
        self._slaveToSlavegroupList.sort()
        self._slaveToSlavegroupList = tuple(self._slaveToSlavegroupList)
