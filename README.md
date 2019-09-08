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

[noteshrink-qt-v1-linux-x86_64.AppImage](https://github.com/clapautius/noteshrink-qt/releases/download/v1/noteshrink-qt-v1-x86_64.AppImage)

[noteshrink-qt-v2-linux-x86_64.AppImage](https://github.com/clapautius/noteshrink-qt/releases/download/v2/noteshrink-qt-v2-x86_64.AppImage)

Dev. notes
==========

Build, install
--------------

(K)ubuntu 18.04: install qt5-default (qmake will go to qmake-qt5)

Build AppImage (on (k)ubuntu-16.04.6 xenial)
---------------------------------------------

This is work in progress !!

    git clone https://github.com/clapautius/noteshrink-qt.git
    cd noteshrink-qt/
    mkdir -p build
    cd build/
    qmake ..
    make CXXFLAGS="-std=c++11 -fPIC "
    mkdir -p AppDir
    make install INSTALL_ROOT=AppDir
    wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
    chmod +x ./linuxdeployqt-continuous-x86_64.AppImage
    /linuxdeployqt-continuous-x86_64.AppImage AppDir/share/applications/noteshrink-qt.desktop -no-translations -appimage -no-strip
