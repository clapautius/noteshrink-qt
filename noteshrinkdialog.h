#ifndef NOTESHRINKDIALOG_H
#define NOTESHRINKDIALOG_H

#include <vector>

#include <QDialog>
#include <QAbstractButton>
#include <QFile>
#include <QImage>
#include <QStringListModel>
#include <QSettings>

#define NOTESHRINK_QT_VER "v5-beta" // :release

namespace Ui {
class NoteshrinkDialog;
}

class NoteshrinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoteshrinkDialog(QWidget *parent = nullptr);
    ~NoteshrinkDialog();

    void set_input_files(const QStringList &input_files);

private slots:
    void on_m_params_button_box_clicked(QAbstractButton *button);

    void on_m_bkg_value_thres_valueChanged(int value);

    void on_m_pixels_sample_valueChanged(int value);

    void on_m_preview_files_clicked(const QModelIndex &index);

    void on_m_preproc_check_stateChanged(int arg1);

    void on_m_params2_button_box_clicked(QAbstractButton *button);

    void aboutToQuit();

private:

    void update_preview_image();

    void run_preview();

    bool run_noteshrink_preview_cmd(const QString &src, const QString &dst);

    bool run_noteshrink_full_cmd();

    QString compose_noteshrink_cmd(const QStringList &sources,
                                   const QString &additional_params);

    bool run_noteshrink_preproc_preview_cmd(const QString &src, const QString &dst);

    bool run_noteshrink_preproc_full_cmd();

    QString compose_convert_cmd(
            const QString &src, const QString &dst,
            int crop_left, int crop_top, int crop_right, int crop_bottom, int resize);

    QString compose_convert_cmd(const QStringList &sources);

    void check_prereq();

    void enable_inputs();

    void disable_inputs();

    bool set_preview_image(QString &img_path);

    void set_default_values();

    void enable_preproc_inputs();

    void disable_preproc_inputs();

    void save_settings();

    void restore_settings();

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
};

#endif // NOTESHRINKDIALOG_H
