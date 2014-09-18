#include "log.h"

#include <QTextStream>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

#include <iostream>

static QString g_LogPath;

void crashMessageOutput(QtMsgType type, const QMessageLogContext &, const QString & str)
{
    QString txt;
    switch (type) {
        case QtDebugMsg:
            txt = QString("Debug: %1").arg(str);
            break;
        case QtWarningMsg:
            txt = QString("Warning: %1").arg(str);
            break;
        case QtCriticalMsg:
            txt = QString("Critical: %1").arg(str);
            break;
        case QtFatalMsg:
            txt = QString("Fatal: %").arg(str);
            abort();
    }

    QFile outFile(g_LogPath);
    outFile.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;

    //fork output to stdout
    std::cout<<txt.toStdString()<<std::endl;
}

void InstallLogHandler() {
    g_LogPath = QDir::toNativeSeparators(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first() + "/" + "deepin-boot-maker.log");
    qDebug()<<"Install Log to "<<g_LogPath;
    qInstallMessageHandler(crashMessageOutput);
}

