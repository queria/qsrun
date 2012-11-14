QSRun
=====

QSRun is simple python + qt app for launching other applications.
It is similiar to other *runners* like xf4run, fbrun and other *Run ...* dialogs.

**WARNING: QSRun is being ported to C++ (see cpp branch) and there are still some missing features. Because of this python branch (master) is not maintained anymore.**

Motivation/Features
-------------------
I required few features from QSRun:

- be simple to tweak (using source code)
- **complete app names** from $PATH
- launch apps not only from system but also from $PATH modified by user (added ~/bin etc) - (so respects .bash{rc,_profile} like when run from xterm)
- **NOT** launch non-existant apps (like xf4run opens unknown commands in browser - *hate it*)
- should remember **history of apps launched by you** and prefer them before others in typing-completion
- remembered history should be **easy to modify**

All this is fullfilled at the moment ;)
And as addition qsrun:

- **hides to tray** after successfull launch of app or after pressing Escape
- uses dbus to work as **single-instance app**

Requirements
-------------------
Most of them are probably already installed in your linux distribution.
Rest can be (*maybe?*) installed using package manager (aptitude/emerge/pacman...).

* Python_ (~2.6)
* Working DBus_
* Qt_
* dbus-python_
* PySide_

Usage
-------------------
Launch qsrun/src/main.py.

::

  $ cd ~/apps/
  $ git clone ``git@github.com:queria/QSRun.git`` qsrun
  $ chmod +x qsrun/src/main.py
  $ qsrun/src/main.py # <- launch

You can add hotkey bindings for example to Alt+F2:

Fluxbox example

::

  $ grep qsrun ~/.fluxbox/keys
  Mod1 F2 :Exec ~/apps/qsrun/src/main.py

In xfce you can use xfce4-keyboard-settings.

History file is located in ``~/.config/qsrun/`` and it uses JSON_ format.

Author(s)
-------------------
`Queria Sa-Tas`_

Licensing
-------------------
Copyright (C) 2011 by Queria Sa-Tas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

.. _Python: http://www.python.org
.. _DBus: http://dbus.freedesktop.org
.. _Qt: http://qt.nokia.com
.. _dbus-python: http://www.freedesktop.org/wiki/Software/DBusBindings#Python
.. _PySide: http://www.pyside.org
.. _JSON: http://json.org/example.html
.. _Queria Sa-Tas: http://sa-tas.net/

