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

private:
    Ui::NoteshrinkDialog *ui;

    QString m_preview_image_path;

    QFile m_preview_image_file_tmp;

    QImage m_preview_image;
};

#endif // NOTESHRINKDIALOG_H
