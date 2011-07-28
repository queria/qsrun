import os
import json

class AppHinter:
    def __init__(self):
        self._apps = []
        self._history = []

        self.load_applications()
        self.load_history()

    def load_applications(self):
        self._apps = []
        for d in os.getenv('PATH').split(':'):
            self._apps.extend(os.listdir(os.path.expanduser(d)))
        self._apps.sort()

    def load_history(self):
        self._history = []
        try:
            with open(self.__cache__(), 'r') as cache:
                self._history = json.load(cache)
        except (IOError, ValueError):
            pass

    def add_to_history(self, executed):
        if not executed in self._history:
            self._history.append(executed)
            try:
                with open(self.__cache__(), 'w') as cache:
                    json.dump(self._history, cache)
            except IOError:
                pass

    def applications(self):
        return self._apps

    def from_history_or_apps(self, typed):
        for happ in self._history:
            if happ.startswith(typed):
                return happ

        for app in self._apps:
            if app.startswith(typed):
                return app
        raise KeyError()
    
    def __cache__(self):
        # depends on OS - currently implemented for Linux
        cachedir = os.path.join(
                os.path.expanduser('~'),
                '.cache',
                'qsrun')
        if not os.path.isdir(cachedir):
            os.mkdir(cachedir)
        return os.path.join(cachedir, 'history.json')

