#include "xutil.h"

#include <QString>
#include <QMap>

static QMap<QString, QString> s_HKLVariantMap;
static QMap<QString, QString> s_HKLLayoutMap;

static QString s_HKLVariant =
"00010409:dvorak,"
"00020409:intl,"
"00010C0C:fr-legacy,"
"00001009:multix,"
"0000100C:fr,"
"00000807:de_nodeadkeys,"
"0001041F:f,"
"0000041F:alt,";

static QString s_HKLLayout =
"00000423:by," // Belarussian
"00000402:bg," // Bulgarian
"00000405:cz," // Czech
"00000807:ch," // Swiss German
"00000c07:de," // Austrian German
"00000407:de," // German
"00001407:de," // Liechtenstein German
"00001007:de," // Luxembourg German
"00000406:dk," // Danish
"00000c09:us," // Australia
"00002809:us," // Belize
"00001009:us," // Canada
"00002409:us," // Caribbean
"00004009:us," // India
"00002009:us," // Jamaica
"00004409:us," // Malaysia
"00001409:us," // New Zealand
"00003409:us," // Philippines (Tagalog)
"00004809:us," // Singapore
"00001c09:us," // South Africa
"00002c09:us," // Trinidad and Tobago
"00000409:us," // United States
"00003009:us," // Zimbabwe
"00000c04:us," // Chinese / Hong Kong
"00001404:us," // Chinese / Macao
"00001004:us," // Chinese / Singapore
"00000804:us," // Chinese / Simplified
"00000404:us," // Chinese / Traditional
"00000412:us," // Korean
"00000413:us," // Dutch
"00001401:ara," // Arabic / Algeria
"00003c01:ara," // Arabic / Bahrain
"00000c01:ara," // Arabic / Egypt
"00000801:ara," // Arabic / Iraq
"00002c01:ara," // Arabic / Jordan
"00003401:ara," // Arabic / Kuwait
"00003001:ara," // Arabic / Lebanon
"00001001:ara," // Arabic / Libya
"00001801:ara," // Arabic / Morocco
"00002001:ara," // Arabic / Oman
"00004001:ara," // Arabic / Qatar
"00000401:ara," // Arabic / Saudi Arabia
"00002801:ara," // Arabic / Syria
"00001c01:ara," // Arabic / Tunisia
"00003801:ara," // Arabic / U.A.E.
"00002401:ara," // Arabic / Yemen
"00000429:ir," // Persian," //0x0439:in_deva //TBD add variant to variants.ini
"00000439:in," // Hindi
"00000421:us," // Indonesian
"0000041c:al," // Albanian
"0000042a:vn," // Vietnamese
"00000434:us," // Xhosa
"00000809:gb," // United Kingdom
"00001809:gb," // Ireland
"0000083c:ie," // Irish / Gaelic
"00000452:gb," // Welsh
"00000425:ee," // Estonian
"00000c0a:es," // Spanish / Castillian
"00000403:es," // Catalan
"0000042d:es," // Basque
"00000456:es," // Galician
"00002c0a:latam," // Spanish / Argentina
"0000400a:latam," // Spanish / Bolivia
"0000340a:latam," // Spanish / Chile
"0000240a:latam," // Spanish / Colombia
"0000140a:latam," // Spanish / Costa Rica
"00001c0a:latam," // Spanish / Dominican Republic
"0000300a:latam," // Spanish / Ecuador
"0000440a:latam," // Spanish / El Salvador
"0000100a:latam," // Spanish / Guatemala
"0000480a:latam," // Spanish / Honduras
"0000080a:latam," // Spanish / Mexico
"00004c0a:latam," // Spanish / Nicaragua
"0000180a:latam," // Spanish / Panama
"00003c0a:latam," // Spanish / Paraguay
"0000280a:latam," // Spanish / Peru
"0000500a:latam," // Spanish / Puerto Rico
"0000380a:latam," // Spanish / Uruguay
"0000200a:latam," // Spanish / Venezuela
"0000040b:fi," // Finnish
"0000040c:fr," // French / France
"0000140c:fr," // French / Luxembourg
"0000180c:fr," // French / Monaco
"0000047e:fr," // Breton
"00000482:fr," // Occitan
"0000080c:be," // French / Belgium
"00000813:be," // Dutch / Belgium
"00000c0c:ca," // French / Quebec," //0x100c:ch_fr //TBD add variant to variants.ini
"0000100c:ch," // French / Switzerland
"00000408:gr," // Greek
"0000040d:il," // Hebrew
"0000040e:hu," // Hungarian
"0000040f:is," // Icelandic
"00000410:it," // Italian
"00000810:ch," // Italian / Switzerland
"00000427:lt," // Lithuanian
"00000426:lv," // Latvian
"00000411:jp," // Japanese (106 Key)
"0000042f:mk," // Macedonian
"00000414:no," // Norwegian / Bokmal
"00000814:no," // Norwegian / Nynorsk," //0x0c3b:fi_smi //TBD add variant to variants.ini
"00000c3b:fi," // Northern Sami / Finland," //0x043b:no_smi //TBD add variant to variants.ini
"0000043b:no," // Northern Sami / Norway," //0x083b:se_smi //TBD add variant to variants.ini
"0000083b:se," // Northern Sami / Sweden
"00000415:pl," // Polish
"00000416:br," // Portuguese / Brazil
"00000816:pt," // Portuguese
"00000418:ro," // Romanian
"00000419:ru," // Russian
"0000041b:sk," // Slovakian
"00000424:si," // Slovenian
"0000041a:hr," // Serbo-Croatian / Croatia / Latin
"0000101a:ba," // Serbo-Croatian / Bosnia and Herzegovina / Latin
"00007c1a:ba," // Serbo-Croatian / Bosnia and Herzegovina / Cyrillic
"0000181a:cs," // Serbo-Croatian / Serbia and Montenegro / Latin
"00000c1a:cs," // Serbo-Croatian / Serbia and Montenegro / Cyrillic
"0000081d:se," // Swedish / Finland
"0000041d:se," // Swedish / Sweden
"0000041e:th," // Thai
"0000041f:tr," // Turkish (F layout)
"00000422:ua," // Ukrainian
        ;
static void LoadHKLVariant() {
    XUtils::LoadMap(s_HKLVariant, s_HKLVariantMap);
}

static void LoadHKLLayout() {
    XUtils::LoadMap(s_HKLLayout, s_HKLLayoutMap);
}

namespace XUtils {
QString StandKBVariant(const QString& hkl) {
    LoadHKLVariant();
    return s_HKLVariantMap[hkl];
}

QString StandKBLayout(const QString& hkl) {
    LoadHKLLayout();
    return s_HKLLayoutMap[hkl];
}

}
