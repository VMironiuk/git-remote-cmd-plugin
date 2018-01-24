#ifndef GITREMOTECOMMANDPAGE_H
#define GITREMOTECOMMANDPAGE_H

#include "../gitremotecommand_global.h"

#include <projectexplorer/jsonwizard/jsonwizardpagefactory.h>
#include <utils/shellcommandpage.h>

#include <QCoreApplication>

namespace GitRemoteCommand {
namespace Internal {

class GitRemoteCommandPageFactory : public ProjectExplorer::JsonWizardPageFactory
{
    Q_DECLARE_TR_FUNCTIONS(GitRemoteCommand::Internal::GitRemoteCommandPage)

public:
    GitRemoteCommandPageFactory();

    Utils::WizardPage *create(ProjectExplorer::JsonWizard *wizard, Core::Id typeId,
                              const QVariant &data) override;
    bool validateData(Core::Id typeId, const QVariant &data, QString *errorMessage) override;
};

class GitRemoteCommandPage : public Utils::ShellCommandPage
{
    Q_OBJECT

public:
    GitRemoteCommandPage();

    void initializePage() override;
    void setVersionControlId(const QString &id);
    void setRunMessage(const QString &msg);
    void setRepoCreationData(bool init, const QString &dir, const QString &repo);

private slots:
    void delayedInitialize();

private:
    QString m_vcsId;
    QString m_runMessage;
    QString m_remoteRepository;
    QString m_baseDirectory;
    bool m_doInitCommand = false;
};

} // namespace Internal
} // namespace GitRemoteCommand

#endif // GITREMOTECOMMANDPAGE_H
