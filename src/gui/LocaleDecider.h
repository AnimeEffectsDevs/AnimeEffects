#ifndef GUI_LOCALEDECIDER_H
#define GUI_LOCALEDECIDER_H

#include "gui/LocaleParam.h"
#include <QString>
#include <QTranslator>

namespace gui {

class LocaleDecider {
public:
    LocaleDecider();
    const QString& fontFamily() const {
        return mLocaleParam.fontFamily;
    }
    const QString& fontSize() const {
        return mLocaleParam.fontSize;
    }
    LocaleParam localeParam() const {
        return mLocaleParam;
    }

    QTranslator* translator() {
        return mHasTranslator ? &mTranslator : nullptr;
    }

private:
    LocaleParam mLocaleParam;
    QTranslator mTranslator;
    bool mHasTranslator;
};

} // namespace gui

#endif // GUI_LOCALEDECIDER_H
