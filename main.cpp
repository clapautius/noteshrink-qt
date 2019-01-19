#include "noteshrinkdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NoteshrinkDialog w;
    w.show();

    return a.exec();
}
