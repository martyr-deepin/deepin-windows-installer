#include "xutil.h"

#include <QStringList>
#include <QString>
#include <QMap>

namespace XUtils {

void LoadMap(const QString &content, QMap<QString, QString> &map) {
    QStringList list = content.split(",");

    for(const QString& locale: list) {
        QStringList pair = locale.split(":");
        if (pair.first().isEmpty())
            continue;
        map.insert(pair.first(), pair.last());
    }
}

}
