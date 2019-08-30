#include "noteshrinkdialog.h"
#include "ui_noteshrinkdialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QFile>
#include <QPushButton>

NoteshrinkDialog::NoteshrinkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoteshrinkDialog),
    m_preview_image_tmp_path("/tmp/noteshrink-qt-tmp0000.png"), // :fixme: temporary path
    m_preview_image_pp_tmp_path("/tmp/noteshrink-qt-tmp0000_post.png") // :fixme: temporary path, also use '-e' for noteshrink
{
    ui->setupUi(this);
    m_preview_files_model = new QStringListModel();
    ui->m_preview_files->setModel(m_preview_files_model);

    // setup buttons
    QPushButton *button = ui->m_params_button_box->button(QDialogButtonBox::Apply);
    button->setText("Preview");
    button = ui->m_params_button_box->button(QDialogButtonBox::Ok);
    button->setText("Run");

    // put all input controls in a vector
    // :fixme: get rid of the old-style cast
    for (QWidget *w : {(QWidget*)ui->m_params_button_box, (QWidget*)ui->m_bkg_value_thres,
                       (QWidget*)ui->m_pixels_sample, (QWidget*)ui->m_num_colors }) {
        m_inputs.push_back(w);
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


bool NoteshrinkDialog::run_noteshrink_cmd()
{
    bool rc = false;
    bool postprocess = false;
    QString cmd = "noteshrink.py ";

    disable_inputs();

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
        postprocess = true;
    }

    if (ui->m_use_pngcrush->isChecked()) {
        cmd += " -C ";
        postprocess = true;
    }

    if (ui->m_use_pngquant->isChecked()) {
        cmd += " -Q ";
        postprocess = true;
    }
    cmd += " -c \"/bin/true\" ";

    cmd += m_preview_image_src_path;

    //QMessageBox::information(nullptr, "Command", cmd);
    ui->m_log_window->appendPlainText("Running command:");
    ui->m_log_window->appendPlainText(cmd);

    QCoreApplication::processEvents();
    if (QProcess::execute(cmd) == 0) {
        update_preview_image();
        ui->m_log_window->appendPlainText("Done");

        // :fixme: check results
        if (postprocess) {
            ui->m_log_window->appendPlainText("Renaming postproc file");
            bool cc = true;
            if (!QFile::remove(m_preview_image_tmp_path)) {
                ui->m_log_window->appendPlainText("Remove error");
                cc = false;
            }
            if (cc && QFile::rename(m_preview_image_pp_tmp_path, m_preview_image_tmp_path)) {
                ui->m_log_window->appendPlainText("Rename OK");
            } else {
                ui->m_log_window->appendPlainText("Rename error");
                cc = false;
            }
        }
        QFile output_file(m_preview_image_tmp_path);
        qint64 size = output_file.size() / 1024;
        ui->m_log_window->appendPlainText(QString("Output file size: ") + QString::number(size) + " K");
        rc = true;
    } else {
        QMessageBox::critical(nullptr, "Error", "Error executing noteshrink");
    }
    ui->m_log_window->appendPlainText("");
    enable_inputs();
    return rc;
}


void NoteshrinkDialog::on_m_params_button_box_clicked(QAbstractButton *button)
{
    // this is the Preview button
    if((QPushButton*)button == ui->m_params_button_box->button(QDialogButtonBox::Apply)) {
        if (run_noteshrink_cmd()) {
            update_preview_image();
        } else {
            QMessageBox::critical(nullptr, "Error", "noteshrink.py error");
        }
    }
    if((QPushButton*)button == ui->m_params_button_box->button(QDialogButtonBox::Open) ) {
       //QMessageBox::information(nullptr, "Opening preview image", "Opening preview image");
       m_preview_image_src_path = QFileDialog::getOpenFileName(nullptr, "Select image");
       if (QFile::exists(m_preview_image_tmp_path)) {
           QFile::remove(m_preview_image_tmp_path);
       }

       if (!m_preview_image_src_path.isNull()) {
           // copy src to tmp for initial preview
           if (QFile::copy(m_preview_image_src_path, m_preview_image_tmp_path)) {
               update_preview_image();

               // populate file list
               QStringList list;
               list << m_preview_image_src_path;
               m_preview_files_model->setStringList(list);
           } else {
               QMessageBox::critical(nullptr, "Error", "Cannot create temporary file");
           }
       }
    }
}

void NoteshrinkDialog::on_m_bkg_value_thres_valueChanged(int value)
{
    ui->m_bkg_value_thres_label->setText(QString("Background value threshold %: ") +
                                         QString::number(value));
}

void NoteshrinkDialog::on_m_pixels_sample_valueChanged(int value)
{
    ui->m_pixels_sample_label->setText(QString("% of pixels to sample: ") +
                                       QString::number(value));
}


void NoteshrinkDialog::enable_inputs()
{
    for_each(m_inputs.begin(), m_inputs.end(),
             [] (QWidget *w) { w->setEnabled(true); });
}


void NoteshrinkDialog::disable_inputs()
{
    for_each(m_inputs.begin(), m_inputs.end(),
             [] (QWidget *w) { w->setEnabled(false); });
}
