#include "gui/tool/tool_MeshPanel.h"
#include "gui/tool/tool_ItemTable.h"

namespace
{
int kButtonSize = 23;
int kButtonSpace = kButtonSize;
}

namespace gui {
namespace tool {

MeshPanel::MeshPanel(QWidget* aParent, GUIResources& aResources)
    : QGroupBox(aParent)
    , mResources(aResources)
    , mParam()
    , mTypeGroup()
{
    this->setTitle(tr("Mesh Editing"));
    createMode();
}

void MeshPanel::createMode()
{
    if(mResources.getTheme().contains("high_dpi")){
        kButtonSize = 36;
        kButtonSpace = kButtonSize;
    }
    // type
    mTypeGroup.reset(new SingleOutItem(3, QSize(kButtonSpace, kButtonSpace), this));
    mTypeGroup->setChoice(mParam.mode);
    mTypeGroup->setToolTips(QStringList() <<
                            tr("Add a vertex") <<
                            tr("Delete a vertex") <<
                            tr("Split a polygon"));
    mTypeGroup->setIcons(QVector<QIcon>() <<
                         mResources.icon("plus") <<
                         mResources.icon("minus") <<
                         mResources.icon("pencil"));
    mTypeGroup->connect([=](int aIndex){
        this->mParam.mode = aIndex;
        this->onParamUpdated(true);
    });
}

int MeshPanel::updateGeometry(const QPoint& aPos, int aWidth)
{
    static const int kItemLeft = 8;
    static const int kItemTop = 26;

    const int itemWidth = aWidth - kItemLeft * 2;
    QPoint curPos(kItemLeft, kItemTop);

    // type
    curPos.setY(mTypeGroup->updateGeometry(curPos, itemWidth) + curPos.y() + 5);

    // myself
    this->setGeometry(aPos.x(), aPos.y(), aWidth, curPos.y());

    return aPos.y() + curPos.y();
}

} // namespace tool
} // namespace gui
