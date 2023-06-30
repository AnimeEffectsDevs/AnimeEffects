#ifndef GUI_MAINDISPLAYSTYLE
#define GUI_MAINDISPLAYSTYLE

#include "ctrl/GraphicStyle.h"
#include "gui/GUIResources.h"
#include "util/NonCopyable.h"
#include <QFont>
#include <QFontMetrics>

namespace gui {

class MainDisplayStyle: public ctrl::GraphicStyle, private util::NonCopyable {
public:
    MainDisplayStyle(const QFont& aFont, GUIResources& aResources):
        mFont(aFont), mFontMetrics(aFont), mResources(aResources) {}

    virtual QFont font() const {
        return mFont;
    }

    virtual QRect boundingRect(const QString& aText) const {
        return mFontMetrics.boundingRect(aText);
    }

    virtual QIcon icon(const QString& aName) const {
        return mResources.icon(aName);
    }

private:
    QFont mFont;
    QFontMetrics mFontMetrics;
    GUIResources& mResources;
};

} // namespace gui

#endif // GUI_MAINDISPLAYSTYLE
