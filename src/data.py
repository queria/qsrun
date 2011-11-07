import os
import json

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

