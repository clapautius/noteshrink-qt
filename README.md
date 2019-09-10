Synopsis
=========

GUI for noteshrink.py ( https://github.com/mzucker/noteshrink ).

Features:
 * preview
 * pre-processing (crop, resize)

Screenshot
==========

![Screenshot](https://github.com/clapautius/noteshrink-qt/blob/master/doc/noteshrink-qt-screenshot.png)

Download
========

 * [noteshrink-qt-v3-linux-x86_64.AppImage](https://github.com/clapautius/noteshrink-qt/releases/download/v3/noteshrink-qt-v3-x86_64.AppImage)
    * tested on: (k)ubuntu 16.04 _xenial_, (k)ubuntu 19.04 _disco_, debian 10 _buster_, openSUSE _Leap_ 15

Dev. notes
==========

Build, install
--------------

### Build AppImage

Use `build-appimage.sh` script (usage: `build-appimage.sh <src-dir> <build-dir>`).

Recommended host for building: (k)ubuntu 16.04 _xenial_.

### (K)ubuntu 18.04 notes

Install `qt5-default` (`qmake` will go to `qmake-qt5`)
