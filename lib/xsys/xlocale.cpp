#include "xutil.h"

#include <QString>
#include <QMap>


#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>


static QMap<QString, QString> s_LocaleMap;

static QString s_LoacleContet =
"aa_DJ:aa_DJ.UTF-8,"
"aa_ER:aa_ER,"
"aa_ET:aa_ET,"
"af_ZA:af_ZA.UTF-8,"
"am_ET:am_ET,"
"an_ES:an_ES.UTF-8,"
"ar_AE:ar_AE.UTF-8,"
"ar_BH:ar_BH.UTF-8,"
"ar_DZ:ar_DZ.UTF-8,"
"ar_EG:ar_EG.UTF-8,"
"ar_IN:ar_IN,"
"ar_IQ:ar_IQ.UTF-8,"
"ar_JO:ar_JO.UTF-8,"
"ar_KW:ar_KW.UTF-8,"
"ar_LB:ar_LB.UTF-8,"
"ar_LY:ar_LY.UTF-8,"
"ar_MA:ar_MA.UTF-8,"
"ar_OM:ar_OM.UTF-8,"
"ar_QA:ar_QA.UTF-8,"
"ar_SA:ar_SA.UTF-8,"
"ar_SD:ar_SD.UTF-8,"
"ar_SY:ar_SY.UTF-8,"
"ar_TN:ar_TN.UTF-8,"
"ar_YE:ar_YE.UTF-8,"
"az_AZ:az_AZ.UTF-8,"
"as_IN:as_IN.UTF-8,"
"ast_ES:ast_ES.UTF-8,"
"be_BY:be_BY.UTF-8,"
"bg_BG:bg_BG.UTF-8,"
"bn_BD:bn_BD,"
"bn_IN:bn_IN,"
"bo_CN:bo_CN,"
"bo_IN:bo_IN,"
"br_FR:br_FR.UTF-8,"
"bs_BA:bs_BA.UTF-8,"
"byn_ER:byn_ER,"
"ca_AD:ca_AD.UTF-8,"
"ca_ES:ca_ES.UTF-8,"
"ca_FR:ca_FR.UTF-8,"
"ca_IT:ca_IT.UTF-8,"
"crh_UA:crh_UA,"
"cs_CZ:cs_CZ.UTF-8,"
"csb_PL:csb_PL,"
"cy_GB:cy_GB.UTF-8,"
"da_DK:da_DK.UTF-8,"
"de_AT:de_AT.UTF-8,"
"de_BE:de_BE.UTF-8,"
"de_CH:de_CH.UTF-8,"
"de_DE:de_DE.UTF-8,"
"de_LU:de_LU.UTF-8,"
"dz_BT:dz_BT,"
"el_GR:el_GR.UTF-8,"
"el_CY:el_CY.UTF-8,"
"en_AU:en_AU.UTF-8,"
"en_BW:en_BW.UTF-8,"
"en_CA:en_CA.UTF-8,"
"en_DK:en_DK.UTF-8,"
"en_GB:en_GB.UTF-8,"
"en_HK:en_HK.UTF-8,"
"en_IE:en_IE.UTF-8,"
"en_IN:en_IN,"
"en_NZ:en_NZ.UTF-8,"
"en_PH:en_PH.UTF-8,"
"en_SG:en_SG.UTF-8,"
"en_US:en_US.UTF-8,"
"en_ZA:en_ZA.UTF-8,"
"en_ZW:en_ZW.UTF-8,"
"eo:eo.UTF-8,"
"es_AR:es_AR.UTF-8,"
"es_BO:es_BO.UTF-8,"
"es_CL:es_CL.UTF-8,"
"es_CO:es_CO.UTF-8,"
"es_CR:es_CR.UTF-8,"
"es_DO:es_DO.UTF-8,"
"es_EC:es_EC.UTF-8,"
"es_ES:es_ES.UTF-8,"
"es_GT:es_GT.UTF-8,"
"es_HN:es_HN.UTF-8,"
"es_MX:es_MX.UTF-8,"
"es_NI:es_NI.UTF-8,"
"es_PA:es_PA.UTF-8,"
"es_PE:es_PE.UTF-8,"
"es_PR:es_PR.UTF-8,"
"es_PY:es_PY.UTF-8,"
"es_SV:es_SV.UTF-8,"
"es_US:es_US.UTF-8,"
"es_UY:es_UY.UTF-8,"
"es_VE:es_VE.UTF-8,"
"et_EE:et_EE.UTF-8,"
"eu_ES:eu_ES.UTF-8,"
"eu_FR:eu_FR.UTF-8,"
"fa_IR:fa_IR,"
"fi_FI:fi_FI.UTF-8,"
"fo_FO:fo_FO.UTF-8,"
"fr_BE:fr_BE.UTF-8,"
"fr_CA:fr_CA.UTF-8,"
"fr_CH:fr_CH.UTF-8,"
"fr_FR:fr_FR.UTF-8,"
"fr_LU:fr_LU.UTF-8,"
"fy_NL:fy_NL,"
"ga_IE:ga_IE.UTF-8,"
"gd_GB:gd_GB.UTF-8,"
"gez_ER:gez_ER,"
"gez_ET:gez_ET,"
"gl_ES:gl_ES.UTF-8,"
"gu_IN:gu_IN,"
"gv_GB:gv_GB.UTF-8,"
"he_IL:he_IL.UTF-8,"
"hi_IN:hi_IN,"
"hr_HR:hr_HR.UTF-8,"
"hsb_DE:hsb_DE.UTF-8,"
"hu_HU:hu_HU.UTF-8,"
"hy_AM:hy_AM,"
"ia:ia,"
"id_ID:id_ID.UTF-8,"
"is_IS:is_IS.UTF-8,"
"it_CH:it_CH.UTF-8,"
"it_IT:it_IT.UTF-8,"
"iw_IL:iw_IL.UTF-8,"
"ja_JP:ja_JP.UTF-8,"
"ka_GE:ka_GE.UTF-8,"
"kk_KZ:kk_KZ.UTF-8,"
"kl_GL:kl_GL.UTF-8,"
"km_KH:km_KH,"
"kn_IN:kn_IN,"
"ko_KR:ko_KR.UTF-8,"
"ku_TR:ku_TR.UTF-8,"
"kw_GB:kw_GB.UTF-8,"
"ky_KG:ky_KG,"
"lg_UG:lg_UG.UTF-8,"
"lo_LA:lo_LA,"
"lt_LT:lt_LT.UTF-8,"
"lv_LV:lv_LV.UTF-8,"
"mai_IN:mai_IN,"
"mg_MG:mg_MG.UTF-8,"
"mi_NZ:mi_NZ.UTF-8,"
"mk_MK:mk_MK.UTF-8,"
"ml_IN:ml_IN,"
"mn_MN:mn_MN,"
"mr_IN:mr_IN,"
"ms_MY:ms_MY.UTF-8,"
"mt_MT:mt_MT.UTF-8,"
"nb_NO:nb_NO.UTF-8,"
"ne_NP:ne_NP,"
"nl_BE:nl_BE.UTF-8,"
"nl_NL:nl_NL.UTF-8,"
"nn_NO:nn_NO.UTF-8,"
"no_NO:no_NO.UTF-8,"
"nr_ZA:nr_ZA,"
"nso_ZA:nso_ZA,"
"oc_FR:oc_FR.UTF-8,"
"om_ET:om_ET,"
"om_KE:om_KE.UTF-8,"
"or_IN:or_IN,"
"pa_IN:pa_IN,"
"pa_PK:pa_PK,"
"pl_PL:pl_PL.UTF-8,"
"pt_BR:pt_BR.UTF-8,"
"pt_PT:pt_PT.UTF-8,"
"ro_RO:ro_RO.UTF-8,"
"ru_RU:ru_RU.UTF-8,"
"ru_UA:ru_UA.UTF-8,"
"rw_RW:rw_RW,"
"sa_IN:sa_IN,"
"se_NO:se_NO,"
"si_LK:si_LK,"
"sid_ET:sid_ET,"
"sk_SK:sk_SK.UTF-8,"
"sl_SI:sl_SI.UTF-8,"
"so_DJ:so_DJ.UTF-8,"
"so_ET:so_ET,"
"so_KE:so_KE.UTF-8,"
"so_SO:so_SO.UTF-8,"
"sq_AL:sq_AL.UTF-8,"
"sr_ME:sr_ME,"
"sr_RS:sr_RS,"
"ss_ZA:ss_ZA,"
"st_ZA:st_ZA.UTF-8,"
"sv_FI:sv_FI.UTF-8,"
"sv_SE:sv_SE.UTF-8,"
"ta_IN:ta_IN,"
"te_IN:te_IN,"
"tg_TJ:tg_TJ.UTF-8,"
"th_TH:th_TH.UTF-8,"
"ti_ER:ti_ER,"
"ti_ET:ti_ET,"
"tig_ER:tig_ER,"
"tl_PH:tl_PH.UTF-8,"
"tn_ZA:tn_ZA,"
"tr_CY:tr_CY.UTF-8,"
"tr_TR:tr_TR.UTF-8,"
"ts_ZA:ts_ZA,"
"tt_RU:tt_RU.UTF-8,"
"uk_UA:uk_UA.UTF-8,"
"ur_PK:ur_PK,"
"uz_UZ:uz_UZ.UTF-8,"
"ve_ZA:ve_ZA,"
"vi_VN:vi_VN,"
"wa_BE:wa_BE.UTF-8,"
"wo_SN:wo_SN,"
"xh_ZA:xh_ZA.UTF-8,"
"yi_US:yi_US.UTF-8,"
"zh_CN:zh_CN.UTF-8,"
"zh_HK:zh_HK.UTF-8,"
"zh_SG:zh_SG.UTF-8,"
"zh_TW:zh_TW.UTF-8,"
"zu_ZA:zu_ZA.UTF-8,";

 static void LoadLocaleMap() {
    XUtils::LoadMap(s_LoacleContet, s_LocaleMap);
}

namespace XUtils {

QString StandLoacle(const QString& locale) {
    LoadLocaleMap();
    QString std = s_LocaleMap[locale];
    return std;
}

QVector<Language> LoadSupportLanguage() {
    QFile json(":/support_languages.json");
    qDebug()<<"Open File"<<json.open(QIODevice::ReadOnly);

    QVector<Language> supportLanguagesList;
    QJsonParseError json_error;
    QJsonDocument supportLanguages = QJsonDocument::fromJson(json.readAll(), &json_error);
    json.close();
    if(json_error.error == QJsonParseError::NoError) {
        if (supportLanguages.isArray()) {
            QJsonArray languresArray = supportLanguages.array();
            int size = languresArray.size();
            for (int i = 0; i < size; ++i) {
                Language language;
                QJsonValue languageJson = languresArray.at(i);
                QJsonObject languageObject = languageJson.toObject();
                language.Locale =languageObject.take("Locale").toString();
                language.Description = languageObject.take("Description").toString();
                language.LanguageCode = languageObject.take("LanguageCode").toString();
                language.CountryCode = languageObject.take("CountryCode").toString();
                supportLanguagesList.push_back(language);
            }
        }
    }
    return supportLanguagesList;
}


}
