#include "blobs.h"

#include "xsys.h"

#include <QStringList>
#include <QDebug>

namespace Xapi {

static Blobs s_Blobs;

Blobs& GetBlobs() {
    return  s_Blobs;
}

Blobs::Blobs(QObject *parent) :
    QObject(parent)
{
}

Blobs::~Blobs() {
    foreach(QString path, m_BlobsMap) {
       Xapi::RmDir(path);
       Xapi::RmFile(path);
    }
}

QString Blobs::Get(const QString &qurl) {
    return "\"" + m_BlobsMap[qurl] + "\"";
}

QString Blobs::Install(const QString &app, const QStringList &list) {
    QString installDir = InsertTmpFileList(QString(app).remove("."), list);
    if (m_BlobsMap[app].isEmpty()) {
        m_BlobsMap[app] = installDir + "/" + app;
    }
    //add remove list
    m_BlobsMap[app+"InstallDir"] = installDir;
    return m_BlobsMap[app];
}

QString Blobs::Install(const QString &app, const QString &qurl) {
    if (m_BlobsMap[app].isEmpty()) {
        m_BlobsMap[app] = InsertTmpFile(qurl);
    }
    return m_BlobsMap[app];
}

}


