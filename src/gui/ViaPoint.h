#ifndef GUI_VIAPOINT_H
#define GUI_VIAPOINT_H

#include "core/Project.h"
#include "ctrl/UILogger.h"
#include "gl/DeviceInfo.h"
#include "util/NonCopyable.h"
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QWidget>
namespace img {
class ResourceNode;
}
namespace gui {
class ResourceDialog;
}
namespace gui {
class MainMenuBar;
}
namespace gui {
class KeyCommandMap;
}
namespace gui {
class KeyCommandInvoker;
}
namespace gui {
class MainViewSetting;
}
namespace gui {
class MouseSetting;
}

namespace gui {

class ViaPoint: private util::NonCopyable, public ctrl::UILogger {
public:
    ViaPoint(QWidget* aParent);

    void setProject(core::Project* aProject);

    void setMainMenuBar(MainMenuBar* aMainMenuBar);
    MainMenuBar* mainMenuBar() const {
        return mMainMenuBar;
    }

    void setResourceDialog(ResourceDialog* aResDialog);
    ResourceDialog* resourceDialog() const {
        return mResDialog;
    }

    img::ResourceNode* requireOneResource();

    void setLogView(QPlainTextEdit* aLogView);
    virtual void pushLog(const QString& aText, ctrl::UILogType aType);
    void pushUndoneLog(const QString& aText);
    void pushRedoneLog(const QString& aText);

    void setGLDeviceInfo(const gl::DeviceInfo&);
    const gl::DeviceInfo& glDeviceInfo() const;

    void setKeyCommandMap(KeyCommandMap* aMap);
    KeyCommandMap* keyCommandMap() const {
        return mKeyCommandMap;
    }

    void setKeyCommandInvoker(KeyCommandInvoker* aInvoker);
    void throwKeyPressingToKeyCommandInvoker(const QKeyEvent* aEvent);
    void throwKeyReleasingToKeyCommandInvoker(const QKeyEvent* aEvent);

    void setMainViewSetting(MainViewSetting& aSetting);
    MainViewSetting& mainViewSetting();
    const MainViewSetting& mainViewSetting() const;

    void setMouseSetting(MouseSetting& aSetting);
    MouseSetting& mouseSetting();
    const MouseSetting& mouseSetting() const;

    util::Signaler<void()> onVisualUpdated;

private:
    QWidget* mParent;
    core::Project* mProject;
    MainMenuBar* mMainMenuBar;
    ResourceDialog* mResDialog;
    QPlainTextEdit* mLogView;
    gl::DeviceInfo mGLDeviceInfo;
    KeyCommandMap* mKeyCommandMap;
    KeyCommandInvoker* mKeyCommandInvoker;
    MainViewSetting* mMainViewSetting;
    MouseSetting* mMouseSetting;
};

} // namespace gui

#endif // GUI_VIAPOINT_H
