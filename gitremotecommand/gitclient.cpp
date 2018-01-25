#include "gitclient.h"

#include <QDebug>
#include <QDir>
#include <QProcess>

GitClient::GitClient(const QString &workingDirectory, const QString &repo, QObject *parent)
    : QObject(parent),
      m_workingDirectory(workingDirectory)
{
    m_arguments.append(QStringList() << "init");
    if (!repo.isEmpty()) {
        m_arguments.append(QStringList() << "remote" << "add" << "origin" << repo);
        m_arguments.append(QStringList() << "push" << "-u" << "origin" << "master");
    }
}

void GitClient::execute()
{
    if (m_workingDirectory.isEmpty()) {
        qCritical() << "Working directory is not specified!";
        return;
    }

    QDir dir(m_workingDirectory);
    if (!dir.exists())
        dir.mkdir(m_workingDirectory);

    m_git = new QProcess(this);
    m_git->setProgram("git");
    m_git->setWorkingDirectory(m_workingDirectory);
    m_git->setArguments(m_arguments[m_currentArguments++]);

    connect(m_git, SIGNAL(finished(int)), this, SLOT(onCommandFinished(int)));
    m_git->start();
}

void GitClient::onCommandFinished(int exitCode)
{
    Q_UNUSED(exitCode);

    if (m_currentArguments == m_arguments.count()) {
        emit executionFinished();
        return;
    }
    m_git->setArguments(m_arguments[m_currentArguments++]);
    m_git->start();
}
