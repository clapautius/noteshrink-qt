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


bool NoteshrinkDialog::run_noteshrink_preview_cmd()
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

    ui->m_log_window->appendHtml("<div style=\"color: green;\">Running command:</div>");
    ui->m_log_window->appendHtml("<div style=\"color: blue;\">" + cmd + "</div>");

    QCoreApplication::processEvents();
    if (QProcess::execute(cmd) == 0) {
        update_preview_image();
        ui->m_log_window->appendHtml("<div style=\"color: green;\">Done</div>");

        // :fixme: check results
        if (postprocess) {
            bool cc = true;
            if (!QFile::remove(m_preview_image_tmp_path)) {
                ui->m_log_window->appendPlainText("Remove error");
                cc = false;
            }
            if (cc && QFile::rename(m_preview_image_pp_tmp_path, m_preview_image_tmp_path)) {
                ui->m_log_window->appendPlainText("Postproc file renamed");
            } else {
                ui->m_log_window->appendPlainText("Error renaming postproc file");
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


// :fixme: - remove duplicated code
bool NoteshrinkDialog::run_noteshrink_full_cmd()
{
    bool rc = false;
    QString cmd = "noteshrink.py ";

    disable_inputs();

    cmd += "-v ";
    int i = ui->m_bkg_value_thres->value();
    cmd += QString::number(i);

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

    if (ui->m_use_pngcrush->isChecked()) {
        cmd += " -C ";
    }

    if (ui->m_use_pngquant->isChecked()) {
        cmd += " -Q ";
    }

    for(auto &f : m_input_files) {
        cmd += " ";
        cmd += f;
    }

    ui->m_log_window->appendHtml("<div style=\"color: green;\">Running command:</div>");
    ui->m_log_window->appendHtml("<div style=\"color: blue;\">" + cmd + "</div>");

    QCoreApplication::processEvents();
    if (QProcess::execute(cmd) == 0) {
        ui->m_log_window->appendHtml("<div style=\"color: green;\">Done</div>");
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
        if (run_noteshrink_preview_cmd()) {
            update_preview_image();
        } else {
            QMessageBox::critical(nullptr, "Error", "noteshrink.py error");
        }
    } else if((QPushButton*)button == ui->m_params_button_box->button(QDialogButtonBox::Open) ) {
       m_input_files = QFileDialog::getOpenFileNames(nullptr, "Select images");
       if (m_input_files.empty()) {
           return; // :fixme: proper cleanup
       }
       // populate file list
       m_preview_files_model->setStringList(m_input_files);
       // preview first image (:fixme: also select it)
       set_preview_image(m_input_files[0]);
    } else if((QPushButton*)button == ui->m_params_button_box->button(QDialogButtonBox::Ok) ) {
        // this is the 'Run' button
        //QMessageBox::critical(nullptr, "Error", "Not implemented yet");
        run_noteshrink_full_cmd(); // :fixme: check for errors

    } else if ((QPushButton*)button == ui->m_params_button_box->button(QDialogButtonBox::RestoreDefaults)) {
        set_default_values();
    }
}


bool NoteshrinkDialog::set_preview_image(QString &img_path)
{
    bool rc = true;
    m_preview_image_src_path = img_path;
    if (QFile::exists(m_preview_image_tmp_path)) {
        QFile::remove(m_preview_image_tmp_path);
    }

    if (!m_preview_image_src_path.isNull()) {
        // copy src to tmp for initial preview
        if (QFile::copy(m_preview_image_src_path, m_preview_image_tmp_path)) {
            update_preview_image();

        } else {
            QMessageBox::critical(nullptr, "Error", "Cannot create temporary file");
            rc = false;
        }
    }
    return rc;
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

void NoteshrinkDialog::on_m_preview_files_clicked(const QModelIndex &index)
{
    QString item_text = index.data(Qt::DisplayRole).toString();
    set_preview_image(item_text);
}


void NoteshrinkDialog::set_default_values()
{
    ui->m_bkg_value_thres->setValue(25);
    ui->m_pixels_sample->setValue(5);
    ui->m_num_colors->setValue(8);
    ui->m_bkg_white->setChecked(false);
    ui->m_global_palette->setChecked(false);
    ui->m_do_not_saturate->setChecked(false);
    ui->m_use_pngcrush->setChecked(false);
    ui->m_use_pngquant->setChecked(false);
}
