import os, types, re, inspect

from twisted.python import runtime
from twisted.internet import reactor
from buildbot.process.buildstep import BuildStep
from buildbot.process.buildstep import SUCCESS, FAILURE
from twisted.internet import error
from twisted.internet.protocol import ProcessProtocol


class PythonFunction(BuildStep):
    """
    Run a python function on the master.
    """
    name = "MasterPythonFunction"
    renderables = [ 'fnc', 'property', 'fncargs' ] 
    haltOnFailure = True
    flunkOnFailure = True

    def __init__(self, fnc, property = None, args = None, **kwargs):
        BuildStep.__init__(self, **kwargs)
        self.fnc = fnc
        self.property = property
        self.fncargs = args

    def _make_args(self, args_names, log, props, success):
        kwargs = {}
        
        if self.fncargs is not None:
            args = self.fncargs
        else:
            args = {}
        
        for arg in args.keys():
            if arg in args_names:
                kwargs[arg] = args[arg]
        
        if 'log' in args_names:
            kwargs['log'] = log

        if 'props' in args_names:
            kwargs['props'] = props

        if 'success' in args_names:
            kwargs['success'] = success
            
        return kwargs
        
    def start(self):        
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
        
        log = []
        success = True

        props = {}
        properties = self.getProperties()
        for key, value, source in properties.asList():
            props[key] = value

        argspec = inspect.getargspec(self.fnc)
        kwargs = self._make_args(argspec[0], log, props, success)
        result = self.fnc(**kwargs)
        
        logtxt = 'No log'
        if (len(log) > 0):
                logtxt = '\n'.join(log)
        self.addCompleteLog('Function log', logtxt)
        
        if success:
            propsSetsLog = []
            for key, value in props.items():
                if properties.hasProperty(key):
                    old = properties.getProperty(key)
                    if old != value:
                        self.setProperty(key, value, 'PythonFunction Step')
                        propsSetsLog.append('%s = %s' % (key, value))
                else:
                    propsSetsLog.append('%s = %s' % (key, value))
                    self.setProperty(key, value, 'PythonFunction Step')
            if propsSetsLog:
                self.addCompleteLog('Function set properties', '\n'.join(propsSetsLog))
            
            if result is not None and self.property:
                self.setProperty(self.property, result, 'PythonFunction Step')
                self.addCompleteLog('Function return', 'Returned %s and it was stored in %s property' % (str(result), self.property))

            self.step_status.setText(['Function run success.'])
            self.finished(SUCCESS)
        else:
            self.step_status.setText(['Function run failed.'])
            self.finished(FAILURE)