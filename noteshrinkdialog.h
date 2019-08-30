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

private:

    void update_preview_image();

    bool run_noteshrink_cmd();

    void enable_inputs();

    void disable_inputs();

    Ui::NoteshrinkDialog *ui;

    QString m_preview_image_src_path;

    QString m_preview_image_tmp_path;
    QString m_preview_image_pp_tmp_path;

    QImage m_preview_image;

    QStringListModel *m_preview_files_model;

    std::vector<QWidget*> m_inputs;
};

#endif // NOTESHRINKDIALOG_H
