#include "noteshrinkdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("noteshrink-qt-icon.png")); // :fixme: add this as resource
    NoteshrinkDialog w;
    w.show();

    return a.exec();
}
