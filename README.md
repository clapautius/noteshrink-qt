Synopsis
=========

GUI for noteshrink.py ( https://github.com/mzucker/noteshrink ).

Features:
 * preview
 * pre-processing (requires ImageMagick's **convert** app.)
   * crop
   * resize
   * normalize colors (increase contrast)

Screenshot
==========

![Screenshot](https://github.com/clapautius/noteshrink-qt/blob/master/doc/noteshrink-qt-screenshot.png)

Download
========

AppImage (linux)
----------------

**Latest release:** [noteshrink-qt-v10-linux-x86_64.AppImage](https://github.com/clapautius/noteshrink-qt/releases/download/v10/noteshrink-qt-v10-linux-x86_64.AppImage)

-----

Latest beta release: [noteshrink-qt-v11-beta-linux-x86_64.AppImage](https://github.com/clapautius/noteshrink-qt/releases/download/v11-beta/noteshrink-qt-v11-beta-linux-x86_64.AppImage)

-----

Previous releases:
 * [noteshrink-qt-v9-linux-x86_64.AppImage](https://github.com/clapautius/noteshrink-qt/releases/download/v9/noteshrink-qt-v9-linux-x86_64.AppImage)
 * [noteshrink-qt-v8-linux-x86_64.AppImage](https://github.com/clapautius/noteshrink-qt/releases/download/v8/noteshrink-qt-v8-linux-x86_64.AppImage)

### To run an AppImage, simply:

**make it executable:** `chmod a+x SomeApp.AppImage`

**and run:** `./SomeApp.AppImage`

### Details

Tested on: (k)ubuntu 16.04 _xenial_, (k)ubuntu 19.04 _disco_, debian 10 _buster_, openSUSE _Leap_ 15 .

Dev. notes
==========

Build, install
--------------

### Build AppImage

Use `build-appimage.sh` script (usage: `build-appimage.sh <src-dir> <build-dir>`).

Recommended host for building: (k)ubuntu 16.04 _xenial_.

### (K)ubuntu 18.04 notes

Install `qt5-default` (`qmake` will go to `qmake-qt5`)

### Icons

From 'White chips heavy' icon theme.
