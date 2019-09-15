#include "noteshrink_utils.h"

#include <QProcess>

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

}
