#include "noteshrinkdialog.h"
#include <QApplication>
#include <QMessageBox>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/resources/noteshrink-qt-icon.png"));
    QCoreApplication::setOrganizationName("noteshrink-qt");
    QCoreApplication::setOrganizationDomain("https://github.com/clapautius/noteshrink-qt");
    QCoreApplication::setApplicationName("noteshrink-qt");
    NoteshrinkDialog w;
    if (!w.init_ok()) {
        return 1;
    }
    w.show();

    QObject::connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(aboutToQuit()));

    QStringList params = a.arguments();
    params.removeAt(0);
    if (params.size() > 0) {
        // check if files exist
        for (auto &f : params) {
            if (!QFile::exists(f)) {
                QMessageBox::critical(nullptr, "Invalid parameret", QString("No such file: ") + f);
                return 1;
            }
        }
        w.set_input_files(params);
    }

    return a.exec();
}
