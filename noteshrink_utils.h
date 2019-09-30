#ifndef NOTESHRINK_UTILS_H
#define NOTESHRINK_UTILS_H

#include <QString>
#include <functional>

class QProcess;

namespace ns_utils
{

/**
 * @brief binary_exec_p
 * @param command
 * @return true if the command can be executed, false otherwise
 */
bool binary_exec_p(const QString &command);

/**
 * @brief exec_cmd : executes an external command and displays a simple progress bar.
 * @param command
 * @return true if command was executed succesfully, false otherwise.
 */
bool exec_cmd(const QString &command, QString &error_output, int interval = 100,
              std::function<void(QProcess&)> f = nullptr);

}

#endif // NOTESHRINK_UTILS_H
