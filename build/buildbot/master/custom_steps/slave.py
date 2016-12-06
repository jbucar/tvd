import stat
import inspect
from buildbot.process import buildstep
from buildbot.status.results import SUCCESS, FAILURE
from buildbot.interfaces import BuildSlaveTooOldError

class PythonFunction(buildstep.BuildStep):
    """
    Run a python function on the slave.
    """
    name = "PythonFunction"
    renderables = [ 'fnc', 'property', 'fncargs' ] 
    description=['run']
    desciprtionDone=['runned']
    haltOnFailure = True
    flunkOnFailure = True

    def __init__(self, fnc, property = None, args = None, **kwargs):
        buildstep.BuildStep.__init__(self, **kwargs)
        self.fnc = fnc
        self.property = property
        self.fncargs = args

    def start(self):
        slavever = self.slaveVersion('runfnc')
        if not slavever:
            raise BuildSlaveTooOldError("slave is too old, does not know "
                                        "about runfnc")
        
        if self.fncargs is not None:
            log = []
            for arg in self.fncargs.keys():
                log.append("%s = %s" % (arg, self.fncargs[arg]))
            self.addCompleteLog('Function args', '\n'.join(log))
        
        lines = inspect.getsourcelines(self.fnc)[0]
        code = ""
        for line in lines:
            code += line
        
        self.addCompleteLog('Function code', code)
        
        symbols = []
        for k, v in self.fnc.func_globals.items():
            if inspect.ismodule(v) and not k == '__builtins__':
                symbols.append(k)

        self.addCompleteLog('Function modules used', '\n'.join(symbols))

        props = {}
        for key, value, source in self.getProperties().asList():
            props[key] = value

        args = {
            'fnc_code': code,
            'fnc_name': self.fnc.func_name,
            'fnc_args': self.fncargs,
            'fnc_symbols': symbols,
            'properties': props,
        }

        cmd = buildstep.RemoteCommand('runfnc', args)
        d = self.runCommand(cmd)
        d.addCallback(lambda res: self.commandComplete(cmd))
        d.addErrback(self.failed)

    def commandComplete(self, cmd):
        status = SUCCESS
        
        logtxt = 'No log'
        if 'fnclog' in cmd.updates:
            log = cmd.updates['fnclog'][-1]
            if (len(log) > 0):
                logtxt = '\n'.join(log)
        self.addCompleteLog('Function log', logtxt)
        
        if cmd.didFail():
            self.step_status.setText(['Function run failed.'])
            status = FAILURE
        else:
            self.step_status.setText(['Function run success.'])

            props = cmd.updates['properties'][-1]
            propsSetsLog = []
            for key, value in props.items():
                if self.getProperties().hasProperty(key):
                    old = self.getProperties().getProperty(key)
                    if old != value:
                        self.setProperty(key, value, 'PythonFunction Step')
                        propsSetsLog.append('%s = %s' % (key, value))
                else:
                    propsSetsLog.append('%s = %s' % (key, value))
                    self.setProperty(key, value, 'PythonFunction Step')
            if propsSetsLog:
                self.addCompleteLog('Function set properties', '\n'.join(propsSetsLog))

            result = cmd.updates['result'][-1]
            
            if result is not None and self.property:
                self.setProperty(self.property, result, 'PythonFunction Step')
                self.addCompleteLog('Function return', 'Returned %s and it was stored in %s property' % (str(result), self.property))
            
        self.finished(status)
