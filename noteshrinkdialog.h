#ifndef NOTESHRINKDIALOG_H
#define NOTESHRINKDIALOG_H

#include <vector>

#include <QDialog>
#include <QAbstractButton>
#include <QFile>
#include <QImage>
#include <QStringListModel>
#include <QSettings>
#include <QTemporaryDir>

#define NOTESHRINK_QT_VER "v10" // :release:

namespace Ui {
class NoteshrinkDialog;
}

class QProgressDialog;
class QProcess;

class NoteshrinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoteshrinkDialog(QWidget *parent = nullptr);
    ~NoteshrinkDialog();

    void set_input_files(const QStringList &input_files);

    bool init_ok() const
    {
        return (m_temp_dir != nullptr && m_noteshrink_bin_found && !m_abort);
    }

private slots:
    void on_m_params_button_box_clicked(QAbstractButton *button);

    void on_m_bkg_value_thres_valueChanged(int value);

    void on_m_pixels_sample_valueChanged(int value);

    void on_m_preview_files_clicked(const QModelIndex &index);

    void on_m_preproc_check_stateChanged(int arg1);

    void on_m_params2_button_box_clicked(QAbstractButton *button);

    void aboutToQuit();

private:

    static const QString m_convert_path;

    void update_preview_image();

    void run_preview();

    bool run_noteshrink_preview_cmd(const QString &orig, const QString &src, const QString &dst);

    bool run_noteshrink_full_cmd(QString &err_msg);

    QString compose_noteshrink_cmd(const QStringList &sources,
                                   const QString &additional_params);

    bool run_noteshrink_preproc_preview_cmd(const QString &src, const QString &dst);

    bool run_noteshrink_preproc_full_cmd();

    QString compose_convert_cmd(
            const QString &src, const QString &dst,
            int crop_left, int crop_top, int crop_right, int crop_bottom, int resize, bool normalize = false);

    QString compose_convert_cmd(const QStringList &sources);

    /**
     * @brief Check prerequisites before loading settings.
     * @return false if we should abort.
     */
    bool check_prereq_stage1();

    /**
     * @brief Check prerequisites after loading settings.
     * @return false if we should abort.
     */
    bool check_prereq_stage2();

    void enable_inputs();

    void disable_inputs();

    bool set_preview_image(QString &img_path);

    void set_default_values();

    void enable_preproc_inputs();

    void disable_preproc_inputs();

    void save_settings();

    void restore_settings();

    bool clean_up_old_files();

    void log_message(const QString &msg, bool print_to_stdout = false);

    void toggle_log_window(QPushButton *toggle_button);

    Ui::NoteshrinkDialog *ui;

    QString m_preview_image_src_path;

    QString m_preview_image_tmp_path;
    QString m_preview_image_pp_tmp_path;

    QImage m_preview_image;

    QStringListModel *m_preview_files_model;

    std::vector<QWidget*> m_inputs;

    std::vector<QWidget*> m_preproc_inputs;

    QStringList m_input_files;

    QSettings m_settings;

    bool m_preproc_available;

    QTemporaryDir *m_temp_dir;

    QString m_noteshrink_bin;

    bool m_noteshrink_bin_found;

    bool m_abort;
};

#endif // NOTESHRINKDIALOG_H
