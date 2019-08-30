#ifndef NOTESHRINKDIALOG_H
#define NOTESHRINKDIALOG_H

#include <vector>

#include <QDialog>
#include <QAbstractButton>
#include <QFile>
#include <QImage>
#include <QStringListModel>

namespace Ui {
class NoteshrinkDialog;
}

class NoteshrinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoteshrinkDialog(QWidget *parent = nullptr);
    ~NoteshrinkDialog();

private slots:
    void on_m_params_button_box_clicked(QAbstractButton *button);

    void on_m_bkg_value_thres_valueChanged(int value);

    void on_m_pixels_sample_valueChanged(int value);

    void on_m_preview_files_clicked(const QModelIndex &index);

private:

    void update_preview_image();

    bool run_noteshrink_preview_cmd();

    bool run_noteshrink_full_cmd();

    void enable_inputs();

    void disable_inputs();

    bool set_preview_image(QString &img_path);

    void set_default_values();

    Ui::NoteshrinkDialog *ui;

    QString m_preview_image_src_path;

    QString m_preview_image_tmp_path;
    QString m_preview_image_pp_tmp_path;

    QImage m_preview_image;

    QStringListModel *m_preview_files_model;

    std::vector<QWidget*> m_inputs;

    QStringList m_input_files;
};

#endif // NOTESHRINKDIALOG_H
