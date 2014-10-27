#include "log.h"

#include <QTextStream>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextCodec>
#include <iostream>

static QString g_LogPath;
#include <io.h>
#include <fcntl.h>
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
    ts.setCodec("utf-8");
    ts << txt << endl;

    std::locale locale("");
    std::wcout.imbue(locale);
    std::wcout<<"[debug]"<<txt.toStdWString()<<std::endl;
}

void InstallLogHandler() {
    g_LogPath = QDir::toNativeSeparators(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first() + "/" + "deepin-windows-installer.log");
    qDebug()<<"Install Log to "<<g_LogPath;
    qInstallMessageHandler(crashMessageOutput);
}

