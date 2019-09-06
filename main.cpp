#include "noteshrinkdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/resources/noteshrink-qt-icon.png"));
    NoteshrinkDialog w;
    w.show();

    return a.exec();
}
