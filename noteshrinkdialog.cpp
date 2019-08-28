#include "noteshrinkdialog.h"
#include "ui_noteshrinkdialog.h"

#include <QMessageBox>
#include <QFileDialog>

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


void NoteshrinkDialog::on_m_preview_button_box_clicked(QAbstractButton *button)
{
    if((QPushButton*)button == ui->m_preview_button_box->button(QDialogButtonBox::Open) ){
       //QMessageBox::information(nullptr, "Opening preview image", "Opening preview image");
       m_preview_image_path = QFileDialog::getOpenFileName(nullptr, "Select image");
       if (!m_preview_image_path.isNull()) {
           m_preview_image.load(m_preview_image_path);
           ui->m_preview_area->setPixmap(QPixmap::fromImage(m_preview_image));
           ui->m_preview_area->setScaledContents(true);
           ui->m_preview_area->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
       }
    }
}
