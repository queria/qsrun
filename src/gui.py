import os
import sys
from PySide import QtCore, QtGui
import time
import shlex
import subprocess

from data import AppHinter

def resource(resfile):
    return os.path.join(
            os.path.dirname(__file__),
            '..',
            'res',
            resfile)

class RunBar(QtGui.QLineEdit):

    confirmed = QtCore.Signal(bool)

    def __init__(self, parent=None):
        super(RunBar, self).__init__(parent)

        self.setWindowFlags(QtCore.Qt.FramelessWindowHint)

        self._hinter = AppHinter()

        self._last_typed = None

        self.textEdited.connect(self._typed)
        self.returnPressed.connect(self._confirmed)

        self._completer = QtGui.QCompleter(self._hinter.applications(), self)
        #self._completer.setModelSorting(QtGui.QCompleter.CaseInsensitivelySortedModel)
        self._completer.setWidget(self)
        self._completer.highlighted[str].connect(self._completer_highlighted)

        self._icon = QtGui.QIcon(resource('qsrun_tango_system.png'))

        self._tray = QtGui.QSystemTrayIcon(self)
        self._tray.setIcon(self._icon)
        self._tray.activated.connect(self.toggle)
        self._tray.show()

        self.setMinimumSize(300,20)
        self.setWindowTitle('QSRun')
        self.setWindowIcon(self._icon)

    def _typed(self, typed):
        if not typed:
            self._completer.popup().hide()
            self._last_typed = typed

        if typed == self._last_typed:
            return

        self._last_typed = typed

        try:
            suggestion = self._hinter.from_history_or_apps( typed )
            self._set_line_string(typed, suggestion)
            self._completer.setCompletionPrefix(typed)
            self._completer.complete()
        except KeyError:
            pass

    def _completer_highlighted(self, highlighted):
        self._set_line_string( self._last_typed, highlighted )
    
    def _set_line_string(self, typed, string):
        self.setText( string )
        self.setSelection(len(typed), len(string) - len(typed))


    def _confirmed(self):
        if self.text()[0] == '#':
            print ['xterm', '-e', str(self.text()[1:])]
            p = subprocess.Popen([
                'xterm',
                '-e',
                str(self.text()[1:])
                ])
        else:
            print(str(self.text()))
            p = subprocess.Popen(shlex.split(str(self.text())), shell=True)
        print 'launched'
        time.sleep(0.1)
        p.poll()
        #print('returncode= '+str(p.returncode))
        if p.returncode != 127:
            self._hinter.add_to_history(self.text())
            self.clear()
            self.confirmed.emit(True)

    def event(self, ev):
        if ev.type() == QtCore.QEvent.Type.KeyRelease:
            if ev.key() == QtCore.Qt.Key_Escape:
                self.run_confirmed(True)
                return True
        return super(RunBar, self).event(ev)

    def run_confirmed(self, success):
        if success:
            self.hide()


    def toggle(self):
        self.setVisible( not self.isVisible() )

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    b = RunBar()
    b.show()
    sys.exit(app.exec_())

