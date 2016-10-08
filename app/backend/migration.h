#ifndef MIGRATION_H
#define MIGRATION_H

#include <QObject>

class Migration : public QObject
{
    Q_OBJECT
public:
    explicit Migration(const QString& root, QObject *parent = 0);

    bool run();

signals:

public slots:


private:
    void createMigrationDataConfig();
    void createVolumeLetterConfig();

    void runMigrationScript();

    QString rootDir;
    QString migrationDataConfigFile;
    QString volumeLetterConfigFile;
};

typedef QMap<QString, QString> Selection;

typedef QMap<QString, Selection*> Config;

bool collectNetWork(Config *cfg) ;

#endif // MIGRATION_H
