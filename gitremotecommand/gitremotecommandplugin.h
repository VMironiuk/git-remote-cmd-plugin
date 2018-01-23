#pragma once

#include "gitremotecommand_global.h"

#include <extensionsystem/iplugin.h>

namespace GitRemoteCommand {
namespace Internal {

class GitRemoteCommandPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "GitRemoteCommand.json")

public:
    GitRemoteCommandPlugin();
    ~GitRemoteCommandPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private:
    void triggerAction();
};

} // namespace Internal
} // namespace GitRemoteCommand
