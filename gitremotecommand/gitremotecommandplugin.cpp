#include "gitremotecommandplugin.h"
#include "gitremotecommandconstants.h"

#include "wizard/gitremotecommandpage.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <git/gitclient.h>

#include <projectexplorer/jsonwizard/jsonwizardfactory.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

using namespace ProjectExplorer;

namespace GitRemoteCommand {
namespace Internal {

static GitRemoteCommandPlugin *m_instance = nullptr;

GitRemoteCommandPlugin::GitRemoteCommandPlugin()
{
    // Create your members
    m_instance = this;
}

GitRemoteCommandPlugin::~GitRemoteCommandPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
    delete m_gitClient;
}

GitRemoteCommandPlugin *GitRemoteCommandPlugin::instance()
{
    return m_instance;
}

Git::Internal::GitClient *GitRemoteCommandPlugin::client()
{
    return m_instance->m_gitClient;
}

bool GitRemoteCommandPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    m_gitClient = new Git::Internal::GitClient;

    auto action = new QAction(tr("GitRemoteCommand Action"), this);
    Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::ACTION_ID,
                                                             Core::Context(Core::Constants::C_GLOBAL));
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
    connect(action, &QAction::triggered, this, &GitRemoteCommandPlugin::triggerAction);

    Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::MENU_ID);
    menu->menu()->setTitle(tr("GitRemoteCommand"));
    menu->addAction(cmd);
    Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    JsonWizardFactory::registerPageFactory(new Internal::GitRemoteCommandPageFactory);

    return true;
}

void GitRemoteCommandPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag GitRemoteCommandPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void GitRemoteCommandPlugin::triggerAction()
{
    QMessageBox::information(Core::ICore::mainWindow(),
                             tr("Action Triggered"),
                             tr("This is an action from GitRemoteCommand."));
}

} // namespace Internal
} // namespace GitRemoteCommand
