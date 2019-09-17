#ifndef NOTESHRINK_UTILS_H
#define NOTESHRINK_UTILS_H

#include <QString>
#include <QWidget>

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
bool exec_cmd(const QString &command, const QString &progress_text, QWidget *parent, QString &error_output);

}

#endif // NOTESHRINK_UTILS_H
