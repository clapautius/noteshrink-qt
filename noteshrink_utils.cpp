#include "noteshrink_utils.h"

#include <iostream>

#include <QProcess>
#include <QCoreApplication>
#include <QProgressDialog>

namespace ns_utils
{

bool binary_exec_p(const QString &command)
{
    QProcess program;
    program.start(command);
    bool started = program.waitForStarted();
    if (!program.waitForFinished(2000)) // 2 Second timeout
        program.kill();
    return started;
}


bool exec_cmd(const QString &command, const QString &progress_text, QWidget *parent, QString &error_output)
{
    bool rc = false;
    QProcess program;
    QProgressDialog progress(progress_text, "", 0, 0, parent, Qt::Dialog);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(nullptr);
    program.start(command);
    std::cout << "Executing command: " << command.toStdString() << std::endl;
    bool started = program.waitForStarted();
    std::cout << "Program started flag: " << started << std::endl;
    if (started) {
        while (true) {
            program.waitForFinished(100); // 100 ms timeout
            if (program.state() == QProcess::NotRunning) {
                std::cout << "status: " << program.exitStatus() << ", code: " << program.exitCode() << std::endl;
                if (program.exitStatus() == QProcess::NormalExit && program.exitCode() == 0) {
                    rc = true;
                    break;
                } else {
                    rc = false;
                    QByteArray output_bin;
                    output_bin = program.readAllStandardError();
                    error_output = QString(output_bin);
                    break;
                }
            } else {
                // update progress
                QCoreApplication::processEvents();
            }
        }
    } else {
        rc = false;
    }
    return rc;
}


}
