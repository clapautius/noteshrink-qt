#include "noteshrink_utils.h"

#include <iostream>

#include <QProcess>
#include <QCoreApplication>

namespace ns_utils
{

bool binary_exec_p(const QString &command, int kill_timeout)
{
    QProcess program;
    program.start(command);
    bool started = program.waitForStarted();
    if (!program.waitForFinished(kill_timeout))
        program.kill();
    return started;
}


ExecResult exec_cmd(const QString &command, QString &error_output, int interval,
                    std::function<void(QProcess&)> f,
                    std::function<bool(void)> cancel_p)
{
    ExecResult rc = kExecNotStarted;
    QProcess program;
    program.start(command);
    std::cout << "Executing command: " << command.toStdString() << std::endl;
    bool started = program.waitForStarted();
    std::cout << "Program started flag: " << started << std::endl;
    if (started) {
        while (true) {
            program.waitForFinished(interval); // timeout (ms)
            if (program.state() == QProcess::NotRunning) {
                std::cout << "status: " << program.exitStatus() << ", code: " << program.exitCode() << std::endl;
                if (program.exitStatus() == QProcess::NormalExit && program.exitCode() == 0) {
                    rc = kExecExitOk;
                    break;
                } else {
                    if (cancel_p && cancel_p()) {
                        rc = kExecExitAborted;
                        break;
                    } else {
                        rc = kExecExitError;
                        QByteArray output_bin;
                        output_bin = program.readAllStandardError();
                        error_output = QString(output_bin);
                        break;
                    }
                }
            } else {
                // update progress
                QCoreApplication::processEvents();
                if (f != nullptr) {
                    f(program);
                }
            }
            if (cancel_p && cancel_p()) {
                std::cout << "trying to terminate noteshrink.py process" << std::endl;
                program.terminate();
            }
        }
    }
    return rc;
}

}
