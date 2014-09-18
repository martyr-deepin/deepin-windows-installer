#include "xutil.h"

#include <QString>
#include <QMap>

static QMap<QString, QString> s_HKLVariantMap;

static QString s_HKLVariant =
"00010409:dvorak,"
"00020409:intl,"
"00010C0C:fr-legacy,"
"00001009:multix,"
"0000100C:fr,"
"00000807:de_nodeadkeys,"
"0001041F:f,"
"0000041F:alt,";


static void LoadHKLVariant() {
    XUtils::LoadMap(s_HKLVariant, s_HKLVariantMap);
}

#include <QDebug>

namespace XUtils {
QString StandKBVariant(const QString& hkl) {
    LoadHKLVariant();
    return s_HKLVariantMap[hkl];
}

}
