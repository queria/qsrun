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

    runbar = gui.RunBar()
    sa.set_window(runbar)

    runbar.show()

    result = app.exec_()
    sys.exit(result)

if __name__ == '__main__':
    main()

