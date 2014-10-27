#pragma once

#include <QString>
#include <QMap>

namespace XUtils {

void LoadMap(const QString &content, QMap<QString, QString> &map);

QString StandLoacle(const QString& locale);

QString StandTimezone(const QString &country, const QString &gmt);

QString StandKBVariant(const QString& hkl);

QString StandKBLayout(const QString& hkl);

}

