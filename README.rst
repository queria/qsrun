QSRun
=====

QSRun is simple Qt/C++ app for launching other applications.
It is similiar to other *runners* like xf4run, fbrun and other *Run ...* dialogs.

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

* Working DBus_
* Qt_

Build
-------------------
For tweaking and modification you can use QtCreator.

For simple usage:

::

  $ cd ~/apps/
  $ git clone ``git@github.com:queria/QSRun.git`` qsrun
  $ cd qsrun
  $ qmake
  $ make
  $ ./release/qsrun # <- launch

You can copy ./release/qsrun to (or make symlink) ~/bin/ or some
other directory which you have in your PATH.

You can add hotkey bindings for example to Alt+F2:

Fluxbox example

::

  $ echo "Mod1 F2 :Exec ~/apps/qsrun/release/qsrun" >> ~/.fluxbox/keys

Or yous your Desktop Environment hotkeys configuration utility (in xfce you can use xfce4-keyboard-settings and so on).

History file is located in ``~/.cache/qsrun/`` and it uses JSON_ format.
(Actually Qt/C++ version of qsrun uses only simple parsing of history file,
so be carefull ... only array/list of string is supported in this file,
like [ "item1", "item2" ] (newlines are skipped).

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

.. _Qt: http://qt.nokia.com
.. _DBus: http://dbus.freedesktop.org
.. _JSON: http://json.org/example.html
.. _Queria Sa-Tas: http://sa-tas.net/

