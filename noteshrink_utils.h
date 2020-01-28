#ifndef NOTESHRINK_UTILS_H
#define NOTESHRINK_UTILS_H

#include <QString>
#include <functional>

class QProcess;

namespace ns_utils
{

enum ExecResult {
    kExecExitOk,
    kExecExitError, // including crash
    kExecNotStarted,
    kExecExitAborted
};

/**
 * @brief binary_exec_p
 * @param command
 * @return true if the command can be executed, false otherwise
 */
bool binary_exec_p(const QString &command, int kill_timeout = 2000);

/**
 * @brief exec_cmd : executes an external command and displays a simple progress bar.
 * @param command
 * @param cancel_p : cancel predicate - returns true if the command should be aborted.
 *
 * @return true if command was executed succesfully, false otherwise.
 */
ExecResult exec_cmd(const QString &command, QString &error_output, int interval = 100,
                    std::function<void(QProcess&)> f = nullptr,
                    std::function<bool(void)> cancel_p = nullptr);

}

#endif // NOTESHRINK_UTILS_H
