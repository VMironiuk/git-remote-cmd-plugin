#ifndef GITCLIENT_H
#define GITCLIENT_H

#include <QObject>

class QProcess;
class GitClient : public QObject
{
    Q_OBJECT
public:
    GitClient(const QString &workingDirectory, const QString &repo, QObject *parent = nullptr);
    void execute();
signals:
    void executionFinished() const;
private slots:
    void onCommandFinished(int exitCode);
private:
    QProcess *m_git = nullptr;
    QList<QStringList> m_arguments;
    int m_currentArguments = 0;
    QString m_workingDirectory;
};

#endif // GITCLIENT_H
