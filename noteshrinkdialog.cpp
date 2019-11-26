#include "noteshrinkdialog.h"
#include "ui_noteshrinkdialog.h"
#include "noteshrink_utils.h"

#include <iostream>

#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QFile>
#include <QPushButton>
#include <QDir>
#include <QProgressDialog>

NoteshrinkDialog::NoteshrinkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoteshrinkDialog),
    m_preproc_available(false), m_temp_dir(nullptr),
    m_noteshrink_bin_found(false)
{
    ui->setupUi(this);
    m_preview_files_model = new QStringListModel();
    ui->m_preview_files->setModel(m_preview_files_model);

    // setup buttons & icons
    QPushButton *button = ui->m_params_button_box->button(QDialogButtonBox::Apply);
    button->setText("Preview");
    button->setIcon(QIcon(":/resources/document-print-preview.png"));
    button = ui->m_params_button_box->button(QDialogButtonBox::Ok);
    button->setText("Run");
    button->setIcon(QIcon(":/resources/system-run.png"));
    button = ui->m_params_button_box->button(QDialogButtonBox::Open);
    button->setIcon(QIcon(":/resources/gtk-open.png"));
    button = ui->m_params_button_box->button(QDialogButtonBox::RestoreDefaults);
    button->setIcon(QIcon(":/resources/reset.png"));
    button = ui->m_params2_button_box->button(QDialogButtonBox::Help);
    button->setText("About");
    button->setIcon(QIcon(":/resources/system-about.png"));
    button = ui->m_params2_button_box->button(QDialogButtonBox::Apply);
    button->setText("Hide log window");
    button->setIcon(QIcon(":/resources/view-media-lyrics.png"));

    QPushButton *def_button = ui->m_params_button_box->button(QDialogButtonBox::Open);
    def_button->setDefault(true);

    // put all input controls in a vector
    // :fixme: get rid of the old-style cast
    for (QWidget *w : {(QWidget*)ui->m_params_button_box, (QWidget*)ui->m_bkg_value_thres,
                       (QWidget*)ui->m_pixels_sample, (QWidget*)ui->m_num_colors,
                       (QWidget*)ui->m_params2_button_box,
                       (QWidget*)ui->m_noteshrink_params2_box}) {
        m_inputs.push_back(w);
    }

    // put all input controls in a vector
    // :fixme: get rid of the old-style cast
    for (QWidget *w : {(QWidget*)ui->m_crop_top, (QWidget*)ui->m_crop_left,
                       (QWidget*)ui->m_crop_bottom, (QWidget*)ui->m_crop_right,
                       (QWidget*)ui->m_resize, (QWidget*)ui->m_crop_label,
                       (QWidget*)ui->m_resize_label}) {
        m_preproc_inputs.push_back(w);
    }

    restore_settings();
    check_prereq();
    if (m_preproc_available) {
        m_inputs.push_back((QWidget*)ui->m_preproc_check);
    }

    // create temp dir & setup temp files
    m_temp_dir = new QTemporaryDir();
    if (!m_temp_dir->isValid()) {
        QMessageBox::information(nullptr, "Warning", "Error creating temp dir. at default location. "
                                                     "Trying current dir.");
        delete m_temp_dir;
        m_temp_dir = new QTemporaryDir("./");
        if (!m_temp_dir->isValid()) {
            QMessageBox::critical(nullptr, "Error", "Error creating temp dir. in current dir.");
            delete m_temp_dir;
            m_temp_dir = nullptr;
        }
        // :fixme: - try again using the src. dir of the images
    }
    if (m_temp_dir) {
        QDir temp_dir_path(m_temp_dir->path());
        m_preview_image_tmp_path = temp_dir_path.filePath("noteshrink-qt-tmp0000.png");
        m_preview_image_pp_tmp_path = temp_dir_path.filePath("noteshrink-qt-tmp0000_post.png");
    }
}

NoteshrinkDialog::~NoteshrinkDialog()
{
    delete ui;
    delete m_temp_dir;
}


void NoteshrinkDialog::update_preview_image()
{
    m_preview_image.load(m_preview_image_tmp_path);
    ui->m_preview_area->setPixmap(QPixmap::fromImage(m_preview_image));
    ui->m_preview_area->setScaledContents(true);
    ui->m_preview_area->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}


QString NoteshrinkDialog::compose_noteshrink_cmd(
        const QStringList &sources, const QString &additional_params)
{
    QString cmd = m_noteshrink_bin;
    cmd += " -v ";
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
    if (!additional_params.isEmpty()) {
        cmd += additional_params;
    }
    for(auto &f : sources) {
        cmd += " ";
        cmd += f;
    }
    return cmd;
}


bool NoteshrinkDialog::run_noteshrink_preview_cmd(
        const QString &orig, const QString &src, const QString &dst)
{
    bool rc = false;
    bool postprocess = false;
    QString cmd;

    disable_inputs();

    if (ui->m_do_not_saturate->isChecked() || ui->m_use_pngcrush->isChecked() ||
            ui->m_use_pngquant->isChecked()) {
        postprocess = true;
    }

    QString extra_params;
    // output
    extra_params = " -b ";
    extra_params += dst;
    extra_params += " -c \"/bin/true\" ";

    cmd = compose_noteshrink_cmd(QStringList(src), extra_params);

    ui->m_log_window->appendHtml("<div style=\"color: green;\">Running command:</div>");
    ui->m_log_window->appendHtml("<div style=\"color: blue;\">" + cmd + "</div>");

    QProgressDialog progress("Shrinking notes ...", "", 0, 0, this, Qt::Dialog);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(nullptr);

    QCoreApplication::processEvents();
    QString error_msg;
    if (ns_utils::exec_cmd(cmd, error_msg) == ns_utils::kExecExitOk) {
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
        QFileInfo orig_file(orig);
        ui->m_preview_label->setText(orig_file.fileName() + " (" + QString::number(size) + "K)");
        rc = true;
    } else {
        rc = false;
        ui->m_log_window->appendHtml("<div style=\"color: red;\">Error:</div>");
        ui->m_log_window->appendPlainText(error_msg);
    }
    ui->m_log_window->appendPlainText("");
    enable_inputs();
    return rc;
}


bool NoteshrinkDialog::clean_up_old_files()
{
    QDir current_dir(".");
    QStringList filters;
    filters << "page*.png" << "page*_post.png" << "output.pdf";
    QStringList old_files = current_dir.entryList(filters);
    if (old_files.size() > 0) {
        QMessageBox::StandardButton button =
                QMessageBox::information(this, "Overwrite?", "Old output files detected, delete them?",
                                         QMessageBox::Cancel | QMessageBox::Ok);
        if (button == QMessageBox::Ok) {
            for (auto &old_file : old_files) {
                log_message(QString("Deleting old file: " + old_file), true);
                if (!QFile::remove(old_file)) {
                    QMessageBox::warning(this, "Error", QString("Error deleting file: ") + old_file);
                    return false;
                }
            }
        } else {
            return false;
        }
    }
    return true;
}


bool NoteshrinkDialog::run_noteshrink_full_cmd(QString &err_msg)
{
    bool rc = false;

    // save current dir. and change dir. to path of the first image
    QDir orig_dir = QDir::current();
    QFileInfo first_img(m_input_files[0]);
    QDir::setCurrent(first_img.absolutePath());
    ui->m_log_window->appendPlainText(QString("Current dir. is now: ") + first_img.absolutePath());

    if (!clean_up_old_files()) {
        err_msg = "Operation aborted";
        // restore previous dir.
        QDir::setCurrent(orig_dir.absolutePath());
        ui->m_log_window->appendPlainText(QString("Current dir. is now: ") + orig_dir.absolutePath());
        return false;
    }

    disable_inputs();

    if (ui->m_preproc_check->isChecked()) {
        run_noteshrink_preproc_full_cmd();
    }

    QString cmd;
    QStringList sources;
    for(auto &f : m_input_files) {
        if (ui->m_preproc_check->isChecked()) {
            sources.push_back(f + "-preproc.png");
        } else {
            sources.push_back(f);
        }
    }
    cmd = compose_noteshrink_cmd(sources, "");
    ui->m_log_window->appendHtml("<div style=\"color: green;\">Running command:</div>");
    ui->m_log_window->appendHtml("<div style=\"color: blue;\">" + cmd + "</div>");

    int total_files = m_input_files.size();
    QProgressDialog progress("Shrinking notes ...", "", 0, total_files + 2, this, Qt::Dialog);
    progress.setMinimumDuration(0);
    progress.setValue(1);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(nullptr);

    QCoreApplication::processEvents();
    QString error_msg;
    int current_file_no = 0;
    std::function<void(QProcess&)> update_func = [&](QProcess &)
    {
        QString waiting;
        if (current_file_no == total_files) {
            waiting = "output.pdf";
        } else if (current_file_no < total_files) {
            waiting.sprintf("page%04d.png", current_file_no);
        }
        std::cout <<"Waiting for file: " << waiting.toStdString().c_str() << std::endl;
        if (!waiting.isEmpty() && QFile::exists(waiting)) {
            std::cout << "Detected file: " << waiting.toStdString().c_str() << std::endl;
            if (current_file_no <= total_files + 2) {
                std::cout << "Increment current value: " << progress.value() << std::endl;
                progress.setValue(progress.value() + 1);
            }
            current_file_no++;
        }
    };
    if (ns_utils::exec_cmd(cmd, error_msg, 100, update_func) == ns_utils::kExecExitOk) {
        ui->m_log_window->appendHtml("<div style=\"color: green;\">Done</div>");
        rc = true;
    } else {
        ui->m_log_window->appendHtml("<div style=\"color: red;\">Error:</div>");
        ui->m_log_window->appendPlainText(error_msg);
        QMessageBox::critical(nullptr, "Error", "Error executing noteshrink");
    }
    ui->m_log_window->appendPlainText("");

    // restore previous dir.
    QDir::setCurrent(orig_dir.absolutePath());
    ui->m_log_window->appendPlainText(QString("Current dir. is now: ") + orig_dir.absolutePath());

    enable_inputs();
    return rc;
}


void NoteshrinkDialog::on_m_params_button_box_clicked(QAbstractButton *button)
{
    // this is the Preview button
    if((QPushButton*)button == ui->m_params_button_box->button(QDialogButtonBox::Apply)) {
        run_preview();
    } else if((QPushButton*)button == ui->m_params_button_box->button(QDialogButtonBox::Open) ) {
       set_input_files(QFileDialog::getOpenFileNames(nullptr, "Select images"));
    } else if((QPushButton*)button == ui->m_params_button_box->button(QDialogButtonBox::Ok) ) {
        // this is the 'Run' button
        QString err_msg("noteshrink.py error"); // generic error
        if (m_input_files.size() == 0) {
            QMessageBox::information(nullptr, "Error", "No images to convert");
        } else if (!run_noteshrink_full_cmd(err_msg)) {
            QMessageBox::critical(this, "Error", err_msg);
        }
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
            QFileInfo orig_file(img_path);
            ui->m_preview_label->setText(orig_file.fileName());
        } else {
            QMessageBox::critical(nullptr, "Error", "Cannot create temporary file");
            rc = false;
        }
    }
    return rc;
}


void NoteshrinkDialog::on_m_bkg_value_thres_valueChanged(int value)
{
    ui->m_bkg_value_thres_label->setText(QString("Bkg. value threshold %: ") +
                                         QString::number(value));
}

void NoteshrinkDialog::on_m_pixels_sample_valueChanged(int value)
{
    ui->m_pixels_sample_label->setText(QString("Pixels to sample (%): ") +
                                       QString::number(value));
}


void NoteshrinkDialog::enable_inputs()
{
    for_each(m_inputs.begin(), m_inputs.end(),
             [] (QWidget *w) { w->setEnabled(true); });
    if (ui->m_preproc_check->isChecked()) {
        enable_preproc_inputs();
    }
}


void NoteshrinkDialog::disable_inputs()
{
    for_each(m_inputs.begin(), m_inputs.end(),
             [] (QWidget *w) { w->setEnabled(false); });
    if (ui->m_preproc_check->isChecked()) {
        disable_preproc_inputs();
    }
}


void NoteshrinkDialog::enable_preproc_inputs()
{
    for_each(m_preproc_inputs.begin(), m_preproc_inputs.end(),
             [] (QWidget *w) { w->setEnabled(true); });
}


void NoteshrinkDialog::disable_preproc_inputs()
{
    for_each(m_preproc_inputs.begin(), m_preproc_inputs.end(),
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
    ui->m_global_palette->setChecked(false);
    ui->m_keep_filenames_order->setChecked(false);
    ui->m_bkg_white->setChecked(false);
    ui->m_do_not_saturate->setChecked(false);
    ui->m_use_pngcrush->setChecked(false);
    ui->m_use_pngquant->setChecked(false);
}

void NoteshrinkDialog::on_m_preproc_check_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        // :fixme: - check if 'convert' exists
        enable_preproc_inputs();
    } else if (arg1 == Qt::Unchecked) {
        disable_preproc_inputs();
    }
}


void NoteshrinkDialog::run_preview()
{
    bool rc = true;
    QString src, dst;
    if (m_preview_image_src_path.isEmpty()) {
        QMessageBox::information(nullptr, "Error", "No image selected for preview");
        return;
    }
    dst = m_preview_image_tmp_path.left(m_preview_image_tmp_path.size() - 8);
    src = m_preview_image_src_path;
    QString orig = src;
    if (ui->m_preproc_check->isChecked()) {
        src = m_preview_image_src_path;
        dst = m_preview_image_tmp_path.left(m_preview_image_tmp_path.size() - 8) + "_preproc.png";
        if (run_noteshrink_preproc_preview_cmd(src, dst)) {
            rc = true;
            src = dst;
            dst = m_preview_image_tmp_path.left(m_preview_image_tmp_path.size() - 8);
        } else {
            rc = false;
            QMessageBox::critical(nullptr, "Error", "Error pre-processing image");
        }
    }
    if (rc) {
        if (run_noteshrink_preview_cmd(orig, src, dst)) {
            update_preview_image();
        } else {
            QMessageBox::critical(nullptr, "Error", "noteshrink.py error");
        }
    }
}


QString NoteshrinkDialog::compose_convert_cmd(
        const QString &src, const QString &dst,
        int crop_left, int crop_top, int crop_right, int crop_bottom, int resize)
{
    QString cmd = "convert ";
    int orig_width = 0, orig_height = 0;

    // get size of the original image
    {
        QImage src_image(src);
        orig_width = src_image.width();
        orig_height = src_image.height();
    }

    // convert syntax: WxH+Xoff+Yoff
    cmd += src;
    cmd += " -strip "; // get rid of EXIF that can make noteshrink.py crash sometimes
    cmd += " -crop ";
    int new_width = orig_width - crop_left - crop_right;
    int new_height = orig_height - crop_top - crop_bottom;
    cmd += QString::number(new_width) + "x" + QString::number(new_height) + "+" +
            QString::number(crop_top) + "+" + QString::number(crop_left) + " ";
    cmd += " +repage ";

    if (resize > 0) {
        cmd += " -resize ";
        cmd += QString::number(resize);
        cmd += "% ";
    }
    cmd += dst;
    return cmd;
}


bool NoteshrinkDialog::run_noteshrink_preproc_preview_cmd(const QString &src, const QString &dst)
{
    bool rc = false;
    QString cmd = "convert ";
    int crop_top = 0, crop_left = 0, crop_right = 0, crop_bottom = 0;
    crop_top = ui->m_crop_top->value();
    crop_left = ui->m_crop_left->value();
    crop_right = ui->m_crop_right->value();
    crop_bottom = ui->m_crop_bottom->value();

    disable_inputs();

    cmd = compose_convert_cmd(src, dst,
                              crop_left, crop_top, crop_right, crop_bottom, ui->m_resize->value());
    ui->m_log_window->appendHtml("<div style=\"color: green;\">Running command:</div>");
    ui->m_log_window->appendHtml("<div style=\"color: blue;\">" + cmd + "</div>");
    QCoreApplication::processEvents();
    QString error_msg;
    QProgressDialog progress("Pre-processing ...", "", 0, 0, this, Qt::Dialog);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(nullptr);
    if (ns_utils::exec_cmd(cmd, error_msg) == ns_utils::kExecExitOk) {
        ui->m_log_window->appendHtml("<div style=\"color: green;\">Done</div>");
        rc = true;
    } else {
        ui->m_log_window->appendHtml("<div style=\"color: red;\">Error:</div>");
        ui->m_log_window->appendPlainText(error_msg);
        QMessageBox::critical(nullptr, "Error", "Error executing convert");
    }
    ui->m_log_window->appendPlainText("");
    return rc;
}


bool NoteshrinkDialog::run_noteshrink_preproc_full_cmd()
{
    bool rc = false;
    QString cmd;
    QString dst;
    int crop_top = 0, crop_left = 0, crop_right = 0, crop_bottom = 0;
    crop_top = ui->m_crop_top->value();
    crop_left = ui->m_crop_left->value();
    crop_right = ui->m_crop_right->value();
    crop_bottom = ui->m_crop_bottom->value();

    disable_inputs();
    QProgressDialog progress("Pre-processing ...", "", 0, m_input_files.size() + 1, this, Qt::Dialog);
    progress.setMinimumDuration(0);
    progress.setValue(1);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(nullptr);

    for(auto &f : m_input_files) {
        dst = f + "-preproc.png";
        cmd = compose_convert_cmd(f, dst, crop_left, crop_top, crop_right, crop_bottom, ui->m_resize->value());
        ui->m_log_window->appendHtml("<div style=\"color: green;\">Running command:</div>");
        ui->m_log_window->appendHtml("<div style=\"color: blue;\">" + cmd + "</div>");
        QCoreApplication::processEvents();
        QString error_msg;
        if (ns_utils::exec_cmd(cmd, error_msg) == ns_utils::kExecExitOk) {
            ui->m_log_window->appendHtml("<div style=\"color: green;\">Done</div>");
            rc = true;
        } else {
            ui->m_log_window->appendHtml("<div style=\"color: red;\">Error:</div>");
            ui->m_log_window->appendPlainText(error_msg);
            QMessageBox::critical(nullptr, "Error", "Error executing convert");
            break;
        }
        ui->m_log_window->appendPlainText("");
        progress.setValue(progress.value() + 1);
    }
    return rc;
}


void NoteshrinkDialog::set_input_files(const QStringList &input_files)
{
    m_input_files = input_files;
    if (m_input_files.empty()) {
        return; // :fixme: proper cleanup
    }
    // populate file list
    m_preview_files_model->setStringList(m_input_files);
    // preview first image (:fixme: also select it)
    set_preview_image(m_input_files[0]);
}

void NoteshrinkDialog::on_m_params2_button_box_clicked(QAbstractButton *button)
{
    if ((QPushButton*)button == ui->m_params2_button_box->button(QDialogButtonBox::Help)) {
        // don't use '\n' so that the HTML format is automatically recognized
        QString html_message = QString("noteshrink-qt ver: ") + NOTESHRINK_QT_VER;
        html_message += "<br>Tudor M. Pristavu";
        html_message += "<br><a href=\"https://github.com/clapautius/noteshrink-qt\">https://github.com/clapautius/noteshrink-qt</a>";
        html_message += "<hr>GUI front-end for noteshrink.py";
        QMessageBox::about(nullptr, "About", html_message);
    } else if ((QPushButton*)button == ui->m_params2_button_box->button(QDialogButtonBox::Apply)) {
        toggle_log_window((QPushButton*)button);
    }
}


void NoteshrinkDialog::aboutToQuit()
{
    save_settings();
}


void NoteshrinkDialog::save_settings()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    m_settings.setValue("preproc-on", ui->m_preproc_check->isChecked());
    m_settings.setValue("preproc-crop-left", ui->m_crop_left->value());
    m_settings.setValue("preproc-crop-top", ui->m_crop_top->value());
    m_settings.setValue("preproc-crop-right", ui->m_crop_right->value());
    m_settings.setValue("preproc-crop-bottom", ui->m_crop_bottom->value());
    m_settings.setValue("preproc-resize", ui->m_resize->value());
    m_settings.setValue("bkg-value-threshold", ui->m_bkg_value_thres->value());
    m_settings.setValue("pixels-to-sample", ui->m_pixels_sample->value());
    m_settings.setValue("no-of-colors", ui->m_num_colors->value());
    m_settings.setValue("use-global-palette", ui->m_global_palette->isChecked());
    m_settings.setValue("keep-filenames-order", ui->m_keep_filenames_order->isChecked());
    m_settings.setValue("make-bkg-white", ui->m_bkg_white->isChecked());
    m_settings.setValue("dont-saturate-colors", ui->m_do_not_saturate->isChecked());
    m_settings.setValue("use-pngquant", ui->m_use_pngquant->isChecked());
    m_settings.setValue("use-pngcrush", ui->m_use_pngcrush->isChecked());
    if (!m_noteshrink_bin.isEmpty()) {
        m_settings.setValue("noteshrink-bin", m_noteshrink_bin);
    }
    m_settings.sync();
}


void NoteshrinkDialog::restore_settings()
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (m_settings.contains("preproc-on")) {
        std::cout << "found preproc-on param, value is " << m_settings.value("preproc-on").toBool() << std::endl;
        ui->m_preproc_check->setChecked(m_settings.value("preproc-on").toBool());
    }
    if (m_settings.contains("preproc-crop-left")) {
        ui->m_crop_left->setValue(m_settings.value("preproc-crop-left").toInt());
    }
    if (m_settings.contains("preproc-crop-top")) {
        ui->m_crop_top->setValue(m_settings.value("preproc-crop-top").toInt());
    }
    if (m_settings.contains("preproc-crop-right")) {
        ui->m_crop_right->setValue(m_settings.value("preproc-crop-right").toInt());
    }
    if (m_settings.contains("preproc-crop-bottom")) {
        ui->m_crop_bottom->setValue(m_settings.value("preproc-crop-bottom").toInt());
    }
    if (m_settings.contains("preproc-resize")) {
        ui->m_resize->setValue(m_settings.value("preproc-resize").toInt());
    }
    if (m_settings.contains("bkg-value-threshold")) {
        ui->m_bkg_value_thres->setValue(m_settings.value("bkg-value-threshold").toInt());
    }
    if (m_settings.contains("pixels-to-sample")) {
        ui->m_pixels_sample->setValue(m_settings.value("pixels-to-sample").toInt());
    }
    if (m_settings.contains("no-of-colors")) {
        ui->m_num_colors->setValue(m_settings.value("no-of-colors").toInt());
    }
    if (m_settings.contains("use-global-palette")) {
        ui->m_global_palette->setChecked(m_settings.value("use-global-palette").toBool());
    }
    if (m_settings.contains("keep-filenames-order")) {
        ui->m_keep_filenames_order->setChecked(m_settings.value("keep-filenames-order").toBool());
    }
    if (m_settings.contains("make-bkg-white")) {
        ui->m_bkg_white->setChecked(m_settings.value("make-bkg-white").toBool());
    }
    if (m_settings.contains("dont-saturate-colors")) {
        ui->m_do_not_saturate->setChecked(m_settings.value("dont-saturate-colors").toBool());
    }
    if (m_settings.contains("use-pngquant")) {
        ui->m_use_pngquant->setChecked(m_settings.value("use-pngquant").toBool());
    }
    if (m_settings.contains("use-pngcrush")) {
        ui->m_use_pngcrush->setChecked(m_settings.value("use-pngcrush").toBool());
    }
    if (m_settings.contains("noteshrink-bin")) {
        m_noteshrink_bin = m_settings.value("noteshrink-bin").toString();
        m_noteshrink_bin_found = true;
    }
}

/**
  * Check prerequisites (binaries for conversion, etc.).
  *
  * @return false if we should abort
  */
bool NoteshrinkDialog::check_prereq()
{
    // if first run - check if noteshrink.py binary is available
    if (m_noteshrink_bin.isEmpty()) {
        m_noteshrink_bin = "noteshrink.py";
        if (!ns_utils::binary_exec_p(m_noteshrink_bin)) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::information(nullptr, "Information",
                                     "noteshrink.py binary not found.\nIf it is installed, press 'Open' and select it.",
                                             QMessageBox::Open | QMessageBox::Abort,
                                             QMessageBox::Abort);
            if (reply == QMessageBox::Abort) {
                return false;
            } else if (reply == QMessageBox::Open) {
                QString bin = QFileDialog::getOpenFileName(nullptr, "Select noteshirnk.py binary");
                if (bin.isEmpty()) {
                    return false;
                }
                m_noteshrink_bin = bin;
                m_settings.setValue("noteshrink-bin", m_noteshrink_bin);
                m_settings.sync();
                m_noteshrink_bin_found = true;
                log_message(QString("noteshrink.py path is now: ") + m_noteshrink_bin, true);
            } else {
                return false;
            }
        } else {
            m_noteshrink_bin_found = true;
            log_message(QString("noteshrink.py path is: ") + m_noteshrink_bin, true);
        }
    } else {
        log_message(QString("noteshrink.py path found in settings, "
                            "skip detection, path is: ") + m_noteshrink_bin, true);
        m_noteshrink_bin_found = true;
    }

    // check if ImageMagick's 'convert' is available
    if (!ns_utils::binary_exec_p("convert")) {
        QMessageBox::information(nullptr, "Information",
                                 "ImageMagick's \"convert\" application not found.\nPre-processing will not be available.");
        m_preproc_available = false;
        ui->m_preproc_check->setEnabled(false);
    } else {
        m_preproc_available = true;
    }
    return true;
}


void NoteshrinkDialog::log_message(const QString &msg, bool print_to_stdout)
{
    ui->m_log_window->appendPlainText(msg);
    if (print_to_stdout) {
        std::cout << msg.toStdString().c_str() << std::endl;
    }
}


void NoteshrinkDialog::toggle_log_window(QPushButton *toggle_button)
{
    if (ui->m_log_box->isHidden()) {
        ui->m_log_box->show();
        toggle_button->setText("Hide log window");
    } else {
        ui->m_log_box->hide();
        toggle_button->setText("Show log window");
    }
}
