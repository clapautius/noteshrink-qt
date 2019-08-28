#ifndef NOTESHRINKDIALOG_H
#define NOTESHRINKDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QFile>
#include <QImage>

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
    void on_m_preview_button_box_clicked(QAbstractButton *button);

    void on_m_params_button_box_clicked(QAbstractButton *button);

private:

    void update_preview_image();

    bool run_noteshrink_cmd();

    Ui::NoteshrinkDialog *ui;

    QString m_preview_image_src_path;

    QString m_preview_image_tmp_path;

    QImage m_preview_image;
};

#endif // NOTESHRINKDIALOG_H
