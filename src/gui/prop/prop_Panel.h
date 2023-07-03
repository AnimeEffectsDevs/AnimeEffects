#ifndef GUI_PROP_PANEL_H
#define GUI_PROP_PANEL_H

#include <functional>
#include <QPalette>
#include <QSize>
#include <QVBoxLayout>
#include "gui/prop/prop_KeyGroup.h"
#include "gui/prop/prop_ItemBase.h"

namespace gui {
namespace prop {

    class Panel: public QGroupBox {
        Q_OBJECT
    public:
        Panel(const QString& aTitle, QWidget* aParent);
        virtual ~Panel() {}
        void addGroup(QGroupBox* aGroup);
        void addStretch();

        std::function<void()> onCollapsed;

    private slots:
        void onClicked(bool aChecked);
        void onChildrenClicked(bool aChecked);

    private:
        QVBoxLayout* mLayout;
        QVector<QGroupBox*> mGroups;
        bool mChecked;
    };

} // namespace prop
} // namespace gui

#endif // GUI_PROP_PANEL_H
