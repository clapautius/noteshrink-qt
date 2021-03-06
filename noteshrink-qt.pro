#-------------------------------------------------
#
# Project created by QtCreator 2019-01-18T12:59:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = noteshrink-qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        noteshrinkdialog.cpp \
    noteshrink_utils.cpp

HEADERS += \
        noteshrinkdialog.h \
    noteshrink_utils.h

FORMS += \
        noteshrinkdialog.ui

unix {
    target.path = $$PREFIX/bin

    shortcutfiles.files = resources/noteshrink-qt.desktop
    shortcutfiles.path = $$PREFIX/share/applications/
    data.files += resources/noteshrink-qt-icon.png
    data.path = $$PREFIX/share/pixmaps/

    INSTALLS += shortcutfiles
    INSTALLS += data
}

INSTALLS += target

RESOURCES += \
    noteshrink-qt.qrc

DISTFILES += \
    README.md
