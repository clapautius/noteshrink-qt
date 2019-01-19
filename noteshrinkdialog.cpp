#include "noteshrinkdialog.h"
#include "ui_noteshrinkdialog.h"

NoteshrinkDialog::NoteshrinkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoteshrinkDialog)
{
    ui->setupUi(this);
}

NoteshrinkDialog::~NoteshrinkDialog()
{
    delete ui;
}
