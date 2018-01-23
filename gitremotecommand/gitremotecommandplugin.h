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
    ~GitRemoteCommandPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace GitRemoteCommand
