#include "gitremotecommandpage.h"

using namespace Core;
using namespace ProjectExplorer;

namespace GitRemoteCommand {
namespace Internal {

GitRemoteCommandPageFactory::GitRemoteCommandPageFactory()
{
    setTypeIdsSuffix(QLatin1String("GitRemoteCommand"));
}

Utils::WizardPage *GitRemoteCommandPageFactory::create(JsonWizard *wizard, Id typeId,
                                                       const QVariant &data)
{
    // TODO: add implementation
}

bool GitRemoteCommandPageFactory::validateData(Id typeId, const QVariant &data,
                                              QString *errorMessage)
{
    // TODO: add implementation
}

GitRemoteCommandPage::GitRemoteCommandPage()
{
    setTitle(tr("Initialize the Git Repository"));
}

} // namepsace Internal
} // namespace GitRemoteCommand
