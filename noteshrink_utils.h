#ifndef NOTESHRINK_UTILS_H
#define NOTESHRINK_UTILS_H

#include <QString>

namespace ns_utils
{

/**
 * @brief binary_exec_p
 * @param command
 * @return true if the command can be executed, false otherwise
 */
bool binary_exec_p(const QString &command);

}

#endif // NOTESHRINK_UTILS_H
