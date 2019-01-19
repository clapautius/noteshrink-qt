#ifndef NOTESHRINKDIALOG_H
#define NOTESHRINKDIALOG_H

#include <QDialog>

namespace Ui {
class NoteshrinkDialog;
}

class NoteshrinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoteshrinkDialog(QWidget *parent = 0);
    ~NoteshrinkDialog();

private:
    Ui::NoteshrinkDialog *ui;
};

#endif // NOTESHRINKDIALOG_H
