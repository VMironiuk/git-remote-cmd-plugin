#include "gitremotecommandpage.h"

#include <gitclient.h>

#include <coreplugin/iversioncontrol.h>
#include <coreplugin/vcsmanager.h>
#include <projectexplorer/jsonwizard/jsonwizard.h>

#include <utils/qtcassert.h>

#include <QAbstractButton>
#include <QDebug>
#include <QDir>
#include <QTimer>

using namespace Core;
using namespace ProjectExplorer;

namespace GitRemoteCommand {
namespace Internal {

static char GITREMOTECOMMAND_VCSID[] = "vcsId";
static char GITREMOTECOMMAND_RUN_MESSAGE[] = "trRunMessage";
static char GITREMOTECOMMAND_REMOTE_REPO[] = "remoteRepository";
static char GITREMOTECOMMAND_BASE_DIR[] = "baseDirectory";
static char GITREMOTECOMMAND_INIT[] = "doInitCommand";

//-------------------------------------------------------------------------------------------------
// GitRemoteCommandPageFactory
//-------------------------------------------------------------------------------------------------

GitRemoteCommandPageFactory::GitRemoteCommandPageFactory()
{
    setTypeIdsSuffix(QLatin1String("GitRemoteCommand"));
}

Utils::WizardPage *GitRemoteCommandPageFactory::create(JsonWizard *wizard, Id typeId,
                                                       const QVariant &data)
{
    Q_UNUSED(wizard);

    QTC_ASSERT(canCreate(typeId), return nullptr);

    QVariantMap tmp = data.toMap();

    auto page = new GitRemoteCommandPage;
    page->setVersionControlId(tmp.value(QLatin1String(GITREMOTECOMMAND_VCSID)).toString());
    page->setRunMessage(tmp.value(QLatin1String(GITREMOTECOMMAND_RUN_MESSAGE)).toString());
    page->setRepoCreationData(tmp.value(QLatin1String(GITREMOTECOMMAND_INIT)).toBool(),
                              tmp.value(QLatin1String(GITREMOTECOMMAND_BASE_DIR)).toString(),
                              tmp.value(QLatin1String(GITREMOTECOMMAND_REMOTE_REPO)).toString());

    return page;
}

bool GitRemoteCommandPageFactory::validateData(Id typeId, const QVariant &data,
                                              QString *errorMessage)
{
    QTC_ASSERT(canCreate(typeId), return false);

    QString errMsg;
    if (data.type() != QVariant::Map)
        errMsg = tr("\"data\" is no JSON object in \"GitRemoteCommand\" page.");

    if (errMsg.isEmpty()) {
        QVariantMap tmp = data.toMap();

        QString str = tmp.value(QLatin1String(GITREMOTECOMMAND_VCSID)).toString();
        if (str.isEmpty()) {
            errMsg = tr("\"%1\" not set in \"data\" section of \"GitRemoteCommand\" page.")
                    .arg(QLatin1String(GITREMOTECOMMAND_VCSID));
        }
        str = tmp.value(QLatin1String(GITREMOTECOMMAND_REMOTE_REPO)).toString();
        if (str.isEmpty()) {
            errMsg = tr("\"%1\" not set in \"data\" section of \"GitRemoteCommand\" page.")
                    .arg(QLatin1String(GITREMOTECOMMAND_REMOTE_REPO));
        }
        str = tmp.value(QLatin1String(GITREMOTECOMMAND_BASE_DIR)).toString();
        if (str.isEmpty()) {
            errMsg = tr("\"%1\" not set in \"data\" section of \"GitRemoteCommand\" page.")
                    .arg(QLatin1String(GITREMOTECOMMAND_BASE_DIR));;
        }
        str = tmp.value(QLatin1String(GITREMOTECOMMAND_RUN_MESSAGE)).toString();
        if (str.isEmpty()) {
            errMsg = tr("\"%1\" not set in \"data\" section of \"GitRemoteCommand\" page.")
                    .arg(QLatin1String(GITREMOTECOMMAND_RUN_MESSAGE));
        }
        const QVariant initCommit = tmp.value(QLatin1String(GITREMOTECOMMAND_INIT));
        if (initCommit.isNull()) {
            errMsg = tr("\"%1\" not set in \"data\" section of \"GitRemoteCommand\" page.")
                    .arg(QLatin1String(GITREMOTECOMMAND_INIT));
        }
    }

    if (errorMessage)
        *errorMessage = errMsg;

    return errMsg.isEmpty();
}

//-------------------------------------------------------------------------------------------------
// GitRemoteCommandPage
//-------------------------------------------------------------------------------------------------

GitRemoteCommandPage::GitRemoteCommandPage()
{
    setTitle(tr("Initialize the Git Repository"));
}

void GitRemoteCommandPage::initializePage()
{
    // Delay real initialization till after QWizard is done with its setup.
    // Otherwise QWizard will reset our disabled back button again.
    QTimer::singleShot(0, this, &GitRemoteCommandPage::delayedInitialize);
}

void GitRemoteCommandPage::setVersionControlId(const QString &id)
{
    m_vcsId = id;
}

void GitRemoteCommandPage::setRunMessage(const QString &msg)
{
    m_runMessage = msg;
}

void GitRemoteCommandPage::setRepoCreationData(bool init, const QString &dir, const QString &repo)
{
    m_doInitCommand = init;
    m_baseDirectory = dir;
    m_remoteRepository = repo;
}

void GitRemoteCommandPage::delayedInitialize()
{
    auto wiz = qobject_cast<JsonWizard *>(wizard());
    QTC_ASSERT(wiz, return);

    const QString vcsId = wiz->expander()->expand(m_vcsId);
    IVersionControl *vc = VcsManager::versionControl(Id::fromString(vcsId));
    if (!vc) {
        qWarning() << QCoreApplication::translate("GitRemoteCommand::GitRemoteCommandPage",
                                                  "\"%1\" (%2) not found.")
                      .arg(QLatin1String(GITREMOTECOMMAND_VCSID), vcsId);
        return;
    }
    if (!vc->isConfigured()) {
        qWarning() << QCoreApplication::translate("GitRemoteCommand::GitRemoteCommandPage",
                                                  "Version control \"%1\" is not configured.")
                      .arg(vcsId);
        return;
    }
    if (!vc->supportsOperation(IVersionControl::CreateRepositoryOperation)) {
        qWarning() << QCoreApplication::translate("GitRemoteCommand::GitRemoteCommandPage",
                                                  "Version control \"%1\" does not support"
                                                  " a repository creation.")
                      .arg(vcsId);
        return;
    }

    const QString repo = wiz->expander()->expand(m_remoteRepository);
    if (repo.isEmpty()) {
        qWarning() << QCoreApplication::translate("GitRemoteCommand::GitRemoteCommandPage",
                                                  "\"%1\" is empty when trying to run"
                                                  " \"git add remote\" command.")
                      .arg(QLatin1String(GITREMOTECOMMAND_REMOTE_REPO));
        return;
    }

    const QDir baseDir(wiz->expander()->expand(m_baseDirectory));
    if (!baseDir.exists()) {
        if (!baseDir.mkdir(baseDir.absolutePath())) {
        qWarning() << QCoreApplication::translate("GitRemoteCommand::GitRemoteCommandPage",
                                                  "\"%1\" (%2) couldn't create.")
                      .arg(QLatin1String(GITREMOTECOMMAND_BASE_DIR), baseDir.absolutePath());
        return;
        }
    }

    const QString runMessage = wiz->expander()->expand(m_runMessage);
    if (!runMessage.isEmpty())
        setStartedStatus(runMessage);

    GitClient *git = new GitClient(baseDir.absolutePath(), repo);
    connect(git, &GitClient::executionFinished, git, &GitClient::deleteLater);
    git->execute();
    wizard()->button(QWizard::FinishButton)->setEnabled(true);

    // TODO: start shell command to visualise the process
}

} // namepsace Internal
} // namespace GitRemoteCommand
