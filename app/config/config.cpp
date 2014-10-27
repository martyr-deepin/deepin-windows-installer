#include "config.h"

#include <QApplication>
#include <QDebug>

/*select the best font*/
#include <QFontDatabase>
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
            QApplication::setFont(QFont(font, 11));
            qDebug()<<"Load font: "<<font;
            return;
        }
    }
}

