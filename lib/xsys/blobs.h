#pragma once

#include <QObject>
#include <QString>
#include <QMap>

namespace Xapi {

class Blobs : public QObject
{
    Q_OBJECT
public:
    explicit Blobs(QObject *parent = 0);

    QString Get(const QString& qurl);

    QString Install(const QString &app, const QStringList &list);

    QString Install(const QString &app, const QString &qurl);
signals:

public slots:

private:
    QMap<QString, QString> m_BlobsMap;
};

}
