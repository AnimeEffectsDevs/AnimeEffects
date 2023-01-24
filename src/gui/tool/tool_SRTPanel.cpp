#include "gui/tool/tool_SRTPanel.h"
#include "gui/tool/tool_ItemTable.h"

namespace
{
static const int kButtonSize = 23;
static const int kButtonSpace = kButtonSize;
}

namespace gui {
namespace tool {

SRTPanel::SRTPanel(QWidget* aParent, GUIResources& aResources)
    : QGroupBox(aParent)
    , mResources(aResources)
    , mParam()
    , mTypeGroup()
    , mAddMove()
    , mAddRotate()
    , mAddScale()
    , mAdjust()
{
    this->setTitle(tr("Transform"));
    createMode();
    updateTypeParam(mParam.mode);
}

void SRTPanel::createMode()
{
    // mode
    mTypeGroup.reset(new SingleOutItem(2, QSize(kButtonSpace, kButtonSpace), this));
    mTypeGroup->setChoice(mParam.mode);
    mTypeGroup->setToolTips(QStringList() <<
                            tr("Transform") <<
                            tr("Move Centroid"));
    mTypeGroup->setIcons(QVector<QIcon>() << mResources.icon("move") << mResources.icon("transcent"));
    mTypeGroup->connect([=](int aIndex)
    {
        this->mParam.mode = aIndex;
        this->updateTypeParam(aIndex);
        this->onParamUpdated(true);
    });

    mAddMove.reset(new CheckBoxItem(tr("Force move key"), this));
    mAddMove->setToolTip(tr("The transformation will create a move key regardless of its type."));
    mAddMove->connect([=](bool aChecked)
    {
        this->mParam.necessarilyMove = aChecked;
        this->onParamUpdated(false);
    });
    mAddRotate.reset(new CheckBoxItem(tr("Force rotate key"), this));
    mAddRotate->setToolTip(tr("The transformation will create a rotate key regardless of its type."));
    mAddRotate->connect([=](bool aChecked)
    {
        this->mParam.necessarilyRotate = aChecked;
        this->onParamUpdated(false);
    });
    mAddScale.reset(new CheckBoxItem(tr("Force scale key"), this));
    mAddScale->setToolTip(tr("The transformation will create a scale key regardless of its type."));
    mAddScale->connect([=](bool aChecked)
    {
        this->mParam.necessarilyScale = aChecked;
        this->onParamUpdated(false);
    });

    mAdjust.reset(new CheckBoxItem(tr("Adjust centroid position"), this));
    mAdjust->setToolTip(tr("Move the centroid in relation to the image, the opposite will be true when not toggled."));
    mAdjust->setChecked(mParam.adjustPosition);
    mAdjust->connect([=](bool aChecked)
    {
        this->mParam.adjustPosition = aChecked;
        this->onParamUpdated(false);
    });
}

void SRTPanel::updateTypeParam(int aType)
{
    auto showSRT = aType == 0;
    auto showCent = aType == 1;

    mAddMove->setVisible(showSRT);
    mAddRotate->setVisible(showSRT);
    mAddScale->setVisible(showSRT);

    mAdjust->setVisible(showCent);
}

int SRTPanel::updateGeometry(const QPoint& aPos, int aWidth)
{
    static const int kItemLeft = 8;
    static const int kItemTop = 26;

    const int itemWidth = aWidth - kItemLeft * 2;
    QPoint curPos(kItemLeft, kItemTop);

    // type
    curPos.setY(mTypeGroup->updateGeometry(curPos, itemWidth) + curPos.y() + 5);

    if (mParam.mode == 0)
    {
        curPos.setY(mAddMove->updateGeometry(curPos, itemWidth) + curPos.y());
        curPos.setY(mAddRotate->updateGeometry(curPos, itemWidth) + curPos.y());
        curPos.setY(mAddScale->updateGeometry(curPos, itemWidth) + curPos.y());
        curPos.setY(curPos.y() + 5);
    }
    else
    {
        curPos.setY(mAdjust->updateGeometry(curPos, itemWidth) + curPos.y());
        curPos.setY(curPos.y() + 5);
    }

    // myself
    this->setGeometry(aPos.x(), aPos.y(), aWidth, curPos.y());

    return aPos.y() + curPos.y();
}

} // namespace tool
} // namespace gui

