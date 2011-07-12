#!/usr/bin/python2
import sys
from PySide.QtGui import QApplication 
import platform
import gui
from singleapp import SingleApp, OtherInstanceFoundError

def main():
    if platform.system() != 'Linux':
        print 'Only linux system is supported at the moment. Sorry!'
        sys.exit(2)

    app = QApplication(sys.argv)

    sa = SingleApp()
    try:
        sa.start()
    except OtherInstanceFoundError:
        sys.exit(1)

    win = gui.MainWindow()
    sa.set_window(win)

    win.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()

