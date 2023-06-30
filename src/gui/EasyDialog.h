#ifndef GUI_EASYDIALOG_H
#define GUI_EASYDIALOG_H

#include <QDialog>
#include <QMenuBar>
#include <QVBoxLayout>
#include <functional>

namespace gui {

class EasyDialog: public QDialog {
public:
    EasyDialog(const QString& aTitle, QWidget* aParent, bool aIsModal = true);
    virtual ~EasyDialog() {
    }

    void setMenuBar(QMenuBar* aMenuBar);
    void setMainWidget(QWidget* aWidget, bool aAlignLeft = true);
    void setMainLayout(QLayout* aLayout, bool aAlignLeft = true);
    void setOkCancel(const std::function<bool(int)>& aFunction);
    void setOkCancel() {
        setOkCancel([=](int) -> bool { return true; });
    }
    void setOkEnable(bool aIsEnable);
    void setCancel(const std::function<bool(int)>& aFunction);
    void fixSize();

private:
    QVBoxLayout* mLayout;
    QPushButton* mOk;
};

} // namespace gui

#endif // GUI_EASYDIALOG_H
