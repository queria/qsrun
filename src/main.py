#!/usr/bin/python2
import sys
from PySide.QtGui import QApplication 
import platform
import gui
from singleapp import SingleApp, OtherInstanceFoundError
from ctypes import cdll, c_int

# we prepare MCL_FUTURE constant
# see mlock(2) and /usr/include/asm-generic/mman.h
# for your value
C_MCL_CURRENT = c_int(1)
C_MCL_FUTURE = c_int(2)

def main():
    if platform.system() != 'Linux':
        print 'Only linux system is supported at the moment. Sorry!'
        sys.exit(2)

    libc = cdll.LoadLibrary('libc.so.6')

    app = QApplication(sys.argv)

    sa = SingleApp()
    try:
        sa.start()
    except OtherInstanceFoundError:
        sys.exit(1)

    runbar = gui.RunBar()
    sa.set_window(runbar)

    runbar.show()

    libc.mlockall(C_MCL_CURRENT) # always stay in RAM
    result = app.exec_()
    libc.munlockall();
    sys.exit(result)

if __name__ == '__main__':
    main()

