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
    preferList.append("SimSong");
    preferList.append("宋体");
    preferList.append("WenQuanYi Micro Hei");
    preferList.append("文泉驿微米黑");

    foreach (QString font, preferList) {
        if (fontlist.contains(font)) {
            g_CurrentFont = QFont(font, 11);
            QApplication::setFont(g_CurrentFont);
            qDebug()<<"Load font: "<<font;
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

