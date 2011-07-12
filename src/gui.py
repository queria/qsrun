import sys
from PySide import QtCore, QtGui
import os
import json
import time
import shlex
import subprocess

def resource(resfile):
    return os.path.join(
            os.path.dirname(__file__),
            '..',
            'res',
            resfile)


class Completer:
    def __init__(self):
        self._apps = []
        self._history = []

        self.load_applications()
        self.load_history()

    def load_applications(self):
        self._apps = []
        for d in os.getenv('PATH').split(':'):
            self._apps.extend(os.listdir(os.path.expanduser(d)))

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


class RunBar(QtGui.QWidget):

    confirmed = QtCore.Signal(bool)

    def __init__(self, parent=None):
        super(RunBar, self).__init__(parent)

        self._cmpl = Completer()

        self._last_typed = None

        self._line = QtGui.QLineEdit(self)
        #self._settings = QtGui.QPushButton(self.tr('Settings'), self)
        
        l = QtGui.QHBoxLayout(self)
        l.addWidget(self._line)
        #l.addWidget(self._settings)

        self._line.setCompleter(QtGui.QCompleter(self._cmpl.applications()))
        self._line.textEdited.connect(self._typed)
        self._line.returnPressed.connect(self._confirmed)

    def _typed(self, typed):
        if not typed or typed == self._last_typed:
            return

        self._last_typed = typed

        try:
            suggestion = self._cmpl.from_history_or_apps( typed )
            self._line.setText( suggestion )
            self._line.setSelection(len(typed), len(suggestion) - len(typed))
        except KeyError:
            pass

    def _confirmed(self):
        p = subprocess.Popen(shlex.split(str(self._line.text())), shell=True)
        time.sleep(0.1)
        p.poll()
        print('returncode= '+str(p.returncode))
        if p.returncode != 127:
            self._cmpl.add_to_history(self._line.text())
            self._line.clear()
            self.confirmed.emit(True)

class MainWindow(QtGui.QMainWindow):
    def __init__(self, parent=None):
        super(MainWindow,self).__init__(parent, QtCore.Qt.FramelessWindowHint)

        self._icon = QtGui.QIcon(resource('qsrun_tango_system.png'))

        self._tray = QtGui.QSystemTrayIcon(self)
        self._tray.setIcon(self._icon)
        self._tray.activated.connect(self.toggle)
        self._tray.show()

        self._run = RunBar(self)
        self._run.confirmed.connect(self.run_confirmed)
        
        cwidget = QtGui.QWidget(self)
        l = QtGui.QVBoxLayout(cwidget)
        l.addWidget(self._run)
        
        self.setCentralWidget(cwidget)
        self.setMinimumSize(400,20)
        self.setSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Minimum)
        self.setWindowTitle('QSRun')
        self.setWindowIcon(self._icon)

    def event(self, ev):
        if ev.type() == QtCore.QEvent.Type.KeyRelease:
            if ev.key() == QtCore.Qt.Key_Escape:
                self.run_confirmed(True)
                return True
        return super(MainWindow, self).event(ev)

    def run_confirmed(self, success):
        if success:
            self.hide()


    def toggle(self):
        self.setVisible( not self.isVisible() )

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    win = MainWindow()
    win.show()
    sys.exit(app.exec_())

