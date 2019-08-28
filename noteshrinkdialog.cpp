#include "noteshrinkdialog.h"
#include "ui_noteshrinkdialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>

NoteshrinkDialog::NoteshrinkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoteshrinkDialog),
    m_preview_image_tmp_path("/tmp/noteshrink-qt-tmp0000.png") // :fixme: temporary path
{
    ui->setupUi(this);
    if (QFile::exists(m_preview_image_tmp_path)) {
        QFile::remove(m_preview_image_tmp_path);
    }
}

NoteshrinkDialog::~NoteshrinkDialog()
{
    delete ui;
}


void NoteshrinkDialog::update_preview_image()
{
    m_preview_image.load(m_preview_image_tmp_path);
    ui->m_preview_area->setPixmap(QPixmap::fromImage(m_preview_image));
    ui->m_preview_area->setScaledContents(true);
    ui->m_preview_area->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}


void NoteshrinkDialog::on_m_preview_button_box_clicked(QAbstractButton *button)
{
    if((QPushButton*)button == ui->m_preview_button_box->button(QDialogButtonBox::Open) ){
       //QMessageBox::information(nullptr, "Opening preview image", "Opening preview image");
       m_preview_image_src_path = QFileDialog::getOpenFileName(nullptr, "Select image");
       if (!m_preview_image_src_path.isNull()) {
           // copy src to tmp for initial preview
           if (QFile::copy(m_preview_image_src_path, m_preview_image_tmp_path)) {
               update_preview_image();
               ui->m_preview_image_label->setText(m_preview_image_src_path);
           } else {
               QMessageBox::critical(nullptr, "Error", "Cannot create temporary file");
           }
       }
    }
}


bool NoteshrinkDialog::run_noteshrink_cmd()
{
    bool rc = false;
    QString cmd = "noteshrink.py ";

    cmd += "-v ";
    int i = ui->m_bkg_value_thres->value();
    cmd += QString::number(i);

    // output
    cmd += " -b ";
    cmd += m_preview_image_tmp_path.left(m_preview_image_tmp_path.size() - 8);

    cmd += " -p ";
    cmd += QString::number(ui->m_pixels_sample->value());

    cmd += " -n ";
    cmd += QString::number(ui->m_num_colors->value());

    if (ui->m_bkg_white->isChecked()) {
        cmd += " -w ";
    }

    if (ui->m_global_palette->isChecked()) {
        cmd += " -g ";
    }

    if (ui->m_do_not_saturate->isChecked()) {
        cmd += " -S ";
    }

    cmd += " -c \"/bin/true\" ";

    cmd += m_preview_image_src_path;

    //QMessageBox::information(nullptr, "Command", cmd);
    ui->m_log_window->appendPlainText("Running command:");
    ui->m_log_window->appendPlainText(cmd);
    ui->m_log_window->appendPlainText("");

    if (QProcess::execute(cmd) == 0) {
        update_preview_image();
        rc = true;
    } else {
        QMessageBox::critical(nullptr, "Error", "Error executing noteshrink");
    }
    return rc;
}


void NoteshrinkDialog::on_m_params_button_box_clicked(QAbstractButton *button)
{
    if((QPushButton*)button == ui->m_params_button_box->button(QDialogButtonBox::Apply)) {
        if (run_noteshrink_cmd()) {
            update_preview_image();
        } else {
            QMessageBox::critical(nullptr, "Error", "noteshrink.py error");
        }
    }
}
