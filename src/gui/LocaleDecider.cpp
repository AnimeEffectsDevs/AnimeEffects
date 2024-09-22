#include <QFile>
#include <QLocale>
#include <QTextStream>
#include <QSettings>
#include <qobject.h>
#include "gui/LocaleDecider.h"
#include "battery/embed.hpp"

namespace gui {

LocaleDecider::LocaleDecider(): mLocaleParam(), mTranslator(), mHasTranslator() {
    QString locAbb;

    {
        QSettings settings;
        auto langVar = settings.value("generalsettings/language");
        auto language = langVar.isValid() ? langVar.toString() : QString();

        if (language == "English") {
        } else if (language == "Japanese") {
            locAbb = "ja";
        } else if (language == "Chinese") {
            locAbb = "zh";
        } else {
            auto language = QLocale::system().language();
            if (language == QLocale::Japanese) {
                locAbb = "ja";
            }
            if (language == QLocale::Chinese) {
                locAbb = "zh";
            }
        }
    }

    if (!locAbb.isEmpty()) {
        if (locAbb == "ja") {
            auto locale = b::embed<"../../data/locale/translation_ja.qm">();
            if (mTranslator.load(reinterpret_cast<const uchar*>(locale.data()), locale.length(), "data/locale")) {
                mHasTranslator = true;
            }
        } else if (locAbb == "zh") {
            auto locale = b::embed<"../../data/locale/translation_zh.qm">();
            if (mTranslator.load(reinterpret_cast<const uchar*>(locale.data()), locale.length(), "data/locale")) {
                mHasTranslator = true;
            }
        }
    }

    {
#if defined(Q_OS_WIN)
        const QString opt = "_win";
#elif defined(Q_OS_MAC)
        const QString opt = "_mac";
#else
        const QString opt = "";
#endif

        QString preference;
        if (locAbb == "ja") {
            auto tmp = b::embed<"../../data/locale/preference_ja.txt">();
            preference = QString::fromUtf8(tmp.data(), tmp.size());
        } else if (locAbb == "zh") {
            auto tmp = b::embed<"../../data/locale/preference_zh.txt">();
            preference = QString::fromUtf8(tmp.data(), tmp.size());
        } else {
            auto tmp = b::embed<"../../data/locale/preference.txt">();
            preference = QString::fromUtf8(tmp.data(), tmp.size());
        }

        for (const auto line : preference.split('\n')) {
            auto kv = line.split('=');
            if (kv.count() != 2)
                continue;
            
            auto key = kv[0].trimmed();
            auto value = kv[1].trimmed();

            if (key == "font_family" + opt) {
                mLocaleParam.fontFamily = value;
            } else if (key == "font_size" + opt) {
                mLocaleParam.fontSize = value;
            }
        }
    }
}

} // namespace gui
