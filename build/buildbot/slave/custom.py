# This file is part of Buildbot.  Buildbot is free software: you can
# redistribute it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, version 2.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import inspect
import os
import function

from buildslave.commands import base

class RunFunction(base.Command):

    header = "runfnc"

    def _make_args(self, args_names, log, success):
        kwargs = {}
        
        if self.args['fnc_args'] is not None:
            args = self.args['fnc_args']
        else:
            args = {}
        
        for arg in args.keys():
            if arg in args_names:
                kwargs[arg] = args[arg]
        
        if 'log' in args_names:
            kwargs['log'] = log

        if 'props' in args_names:
            kwargs['props'] = self.args['properties']

        if 'success' in args_names:
            kwargs['success'] = success
            
        return kwargs
    
    def start(self):
        assert self.args['fnc_code'] is not None
        assert self.args['fnc_name'] is not None
        
        log = []
        success = True
        
        symbols = dict()
        if self.args['fnc_symbols'] is not None:
            for symbol in self.args['fnc_symbols']:
                module = __import__(symbol, globals(), locals(), [], -1)
                symbols[symbol] = module
        
		import lxml.etree

        func = function.create(self.args['fnc_code'], self.args['fnc_name'], additional_symbols = symbols)
        argspec = inspect.getargspec(func)
        kwargs = self._make_args(argspec[0], log, success)
        res = func(**kwargs)
        
        if len(log) > 0:
            self.sendStatus({'fnclog': log})
        
        if success:
            self.sendStatus({'properties': self.args['properties']})
            self.sendStatus({'result': res})
            self.sendStatus({'rc': 0})
        else:
            self.sendStatus({'rc': 0})
        return
