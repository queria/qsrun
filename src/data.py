import os
import json
import math
import re

class AppHinter:
    def __init__(self):
        self._apps = []
        self._history = []

        self.reload()
        for internal_cmd in ['!history', '!reload']:
            if internal_cmd not in self._history:
                self.add_to_history(internal_cmd)


    def reload(self):
        self.load_applications()
        self.load_history()

    def load_applications(self):
        self._apps = []
        for d in os.getenv('PATH').split(':'):
            try:
                self._apps.extend(os.listdir(os.path.expanduser(d)))
            except OSError:
                pass
        self._apps.sort()

    def load_history(self):
        self._history = []
        try:
            with open(self.cache(), 'r') as cache:
                self._history = json.load(cache)
        except (IOError, ValueError):
            pass

    def add_to_history(self, executed):
        if not executed in self._history:
            self._history.append(executed)
            self._history.sort()
            try:
                with open(self.cache(), 'w') as cache:
                    json.dump(self._history, cache)
            except IOError:
                pass

    def available_commands(self):
        cmds = self._apps[:]
        for h in self._history:
            if not h in cmds:
                cmds.append(h)
        cmds.sort()
        return cmds

    def from_history_or_apps(self, typed):
        for happ in self._history:
            if happ.startswith(typed):
                return happ

        for app in self._apps:
            if app.startswith(typed):
                return app
        raise KeyError()
    
    def cache(self):
        # depends on OS - currently implemented for Linux
        cachedir = os.path.join(
                os.path.expanduser('~'),
                '.cache',
                'qsrun')
        if not os.path.isdir(cachedir):
            os.mkdir(cachedir)
        return os.path.join(cachedir, 'history.json')



class Calculator:
    """Thanks to http://www.peterbe.com/plog/calculator-in-python-for-dummies !!!"""
    
    def __init__(self):

        self.eval_globals = dict(__builtins__=None)
        self.eval_symbols = vars(math)

        self.integers_pattern = re.compile(r'\b[\d\.]+\b')
        self.validator = re.compile('^('+'|'.join(
            ['-', r'\+', '/', r'\\', r'\*', r'\^', r'\*\*', r'\(', r'\)', '\d+']
            # functions of math module (ex. __xxx__)
            + [f for f in dir(math) if f[:2] != '__']) + ')*$')

    def calc(self, expr):
        expr = expr.replace('^','**')
        expr = self.integers_pattern.sub(
            self._whole_number_to_float,
            expr)

        return self._safe_eval(expr)

    def _validate(self, expr):
        return self.validator.match(expr) != None

    def _safe_eval(self, expr):
        return eval( expr,
            self.eval_globals,
            self.eval_symbols)

    def _whole_number_to_float(self, int_match):
        group = int_match.group()
        if group.find('.') == -1:
            return group + '.0'
        return group

