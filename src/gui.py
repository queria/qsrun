import os
import sys
from PySide import QtCore, QtGui
import time
import shlex
import subprocess

from data import AppHinter, Calculator

CONF_EDITOR = 'gvim' # no console editors supported yet
CONF_TERM = 'urxvtc' # has to support "term -e command"

def resource(resfile):
    return os.path.join(
            os.path.dirname(__file__),
            '..',
            'res',
            resfile)

class RunBar(QtGui.QLineEdit):

    def __init__(self, parent=None):
        super(RunBar, self).__init__(parent)

        self.setWindowFlags(QtCore.Qt.FramelessWindowHint)

        self._hinter = AppHinter()
        self._calculator = Calculator()

        self._last_typed = ''

        self.textEdited.connect(self._typed)
        self.returnPressed.connect(self._confirmed)

        self._completer = None
        self.reload(True)

        self._icon = QtGui.QIcon(resource('qsrun_tango_system.png'))

        self._tray = QtGui.QSystemTrayIcon(self)
        self._tray.setIcon(self._icon)
        self._tray.activated.connect(self.toggle)
        self._tray.show()
        self._tray_menu = QtGui.QMenu('QSRun')
        self._tray_menu.addAction('Toggle', self.toggle)
        self._tray_menu.addAction('Refresh App DB', self.reload)
        self._tray_menu.addAction('Edit history', self.edit_history)
        self._tray_menu.addAction('Quit', QtGui.qApp.exit)
        self._tray.setContextMenu(self._tray_menu)

        self.setMinimumSize(300,20)
        self.setWindowTitle('QSRun')
        self.setWindowIcon(self._icon)

    def _typed(self, typed, test_last=True):
        if typed[0] == '=':
            self.calculate(typed)
            return
        last = self._last_typed
        self._last_typed = typed

        if not typed:
            self._completer.popup().hide()
            self._last_typed = typed

        if test_last:
            if typed == last:
                return

            if last.startswith(typed): # user uses backspace or something like that
                return

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
        if self.text() == '!history':
            self.edit_history()
            return
        if self.text() == '!reload':
            self.reload()
            return

        if self.text()[0] == '=':
            return # skip calculation inputs

        if self.text()[0] == '#':
            self._launch_term()
        else:
            self._launch_plain()
        print 'launched'

    def edit_history(self):
        p = subprocess.Popen([CONF_EDITOR,
            self._hinter.cache()
            ])
        QtGui.qApp.exit()

    def reload(self, startup=False):
        if not startup:
            self._hinter.reload()

        if self._completer:
            self._completer.highlighted[str].disconnect()

        self._completer = QtGui.QCompleter(self._hinter.available_commands(), self)
        self._completer.setWidget(self)
        self._completer.highlighted[str].connect(self._completer_highlighted)

    def _launch_plain(self):
        self._verify_launch(
                subprocess.Popen(
                    shlex.split(str(self.text())),
                    shell=True))

    def _launch_term(self):
        self._verify_launch(
                subprocess.Popen([
                    CONF_TERM,
                    '-e',
                    str(self.text()[1:])
                    ]))

    def _verify_launch(self, proc):
        time.sleep(0.1)
        proc.poll()
        if proc.returncode != 127:
            self._hinter.add_to_history(self.text())
            self.clear()
            self.run_confirmed(True)

    def event(self, ev):
        if ev.type() == QtCore.QEvent.Type.KeyRelease:
            if ev.key() == QtCore.Qt.Key_Escape:
                self.run_confirmed(True)
                return True
        return super(RunBar, self).event(ev)

    def run_confirmed(self, success):
        if success:
            self.hide()

    def toggle(self,reason=None):
        if reason == QtGui.QSystemTrayIcon.Context:
            return
        self.setVisible( not self.isVisible() )

    def hide(self):
        QtGui.QLineEdit.hide(self)
        self.setText('') # clear after hiding

    def show(self):
        QtGui.QLineEdit.show(self)
        QtGui.qApp.setActiveWindow(self)

    def calculate(self, typed):
        curpos = self.cursorPosition()
        result = '?'
        expr = ''
        
        delim = ' => '
        parts = typed.split(delim)
        if len(parts) > 1:
            expr = parts[0][1:]
            try:
                result = self._calculator.calc(expr)
            except SyntaxError:
                result = '?'
        else:
            expr = ' '
            curpos += 1

        self.setText('=' + expr + delim + str(result))
        self.setCursorPosition(curpos)

if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    b = RunBar()
    b.show()
    sys.exit(app.exec_())

