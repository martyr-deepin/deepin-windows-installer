#include "config.h"

#include <QApplication>
#include <QDebug>

/*select the best font*/
#include <QFontDatabase>

QFont g_CurrentFont;

QFont CurrentFont() {
    return g_CurrentFont;
}

void LoadFonts() {
    QFontDatabase database;
    QStringList fontlist = database.families();

    QStringList preferList;
    preferList.append("Microsoft YaHei UI");
    preferList.append("微软雅黑");
    preferList.append("SimHei");
    preferList.append("黑体");

    foreach (QString font, preferList) {
        if (fontlist.contains(font)) {
            g_CurrentFont = QFont(font, 11);
            QApplication::setFont(g_CurrentFont);
            qDebug()<<"Load font: "<<font
                    <<"Current"<<g_CurrentFont.family();
            return;
        }
    }
}

#include <xutil.h>
#include <QPixmap>
#include <QPainter>
#include <QFont>

void GenerateFontImage() {
    QVector<XUtils::Language> m_Languages = XUtils::LoadSupportLanguage();

    QVector<XUtils::Language>::Iterator langItor = m_Languages.begin();
    for (;langItor != m_Languages.end(); ++langItor) {
        QPixmap pixmap(130, 14);
        QColor red(35,35,35,0);
        pixmap.fill(red);

        QPen pen(QColor(135,135,135));
        QPainter painter(&pixmap);
        painter.setPen(pen);
        int fontsize = 9;
        if (langItor->Locale == "am_ET" || langItor->Locale == "ti_ET") {
             QFont font("Nyala", fontsize);
             painter.setFont(font);
        } else if (langItor->Locale == "iu_CA") {
            QFont font("Euphemia", fontsize);
            painter.setFont(font);
        } else if (langItor->Locale == "or_IN"){
            QFont font("Kalinga Bold",fontsize);
            painter.setFont(font);
        } else {
            QFont font("Microsoft YaHei UI", fontsize);
            painter.setFont(font);
        }

        painter.drawText(QPointF(0,11), langItor->Description); // bottom left corner of the text seems to start at this point
        pixmap.save(langItor->Locale+".png");
    }
}

#include <QTranslator>
#include <QFile>

void LoadTranslate(QApplication& app) {
    QTranslator *translator = new QTranslator();
    QString tnapplang;
    QString tnappcoun;
    QString clangcode = "";
    QStringList allappargs = app.arguments();
    QList<QPair<QString, QString> > oppairs;
    for (QList<QString>::const_iterator i = allappargs.constBegin(); i < allappargs.constEnd(); ++i) {
        if (i->count('=') == 1) {
            oppairs.append(QPair<QString, QString>(i->section('=', 0, 0).simplified(), i->section('=',1, 1).simplified()));
        }
    }
    for (QList<QPair<QString, QString> >::const_iterator i = oppairs.constBegin(); i < oppairs.constEnd(); ++i) {
        if (i->first.contains("lang", Qt::CaseInsensitive))
        {
            clangcode = i->second;
            tnapplang = clangcode.left(2);
            if (clangcode.contains('_') && clangcode.size() == 5)
            {
                tnappcoun = clangcode.section('_', -1, -1);
            }
            break;
        }
    }
    if (clangcode.isEmpty())
    {
        clangcode = QLocale::system().name();
        tnapplang = clangcode.left(2);
        if (clangcode.contains('_') && clangcode.size() == 5)
        {
            tnappcoun = clangcode.section('_', -1, -1);
        }
    }

    QString tranlateUrl;
    if (tnappcoun.isEmpty()) {
        tranlateUrl = QString(":/po/%1.qm").arg(tnapplang);
    } else {
        tranlateUrl = QString(":/po/%1_%2.qm").arg(tnapplang).arg(tnappcoun);
    }

    if (!QFile::exists(tranlateUrl)) {
        tranlateUrl = ":/po/en_US.qm";
    }

    qDebug()<<&app<<"Load translate file: "<<tranlateUrl;

    if (!translator->load(tranlateUrl)){
        qDebug()<<"Load Translater Failed";
    }
    app.installTranslator(translator);
}
