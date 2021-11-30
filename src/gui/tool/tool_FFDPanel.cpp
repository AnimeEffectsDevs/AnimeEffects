#include "gui/tool/tool_FFDPanel.h"
#include "gui/tool/tool_ItemTable.h"

namespace
{
static const int kButtonSize = 23;
static const int kButtonSpace = kButtonSize;
}

namespace gui {
namespace tool {

FFDPanel::FFDPanel(QWidget* aParent, GUIResources& aResources)
    : QGroupBox(aParent)
    , mResources(aResources)
    , mParam()
    , mTypeGroup()
    , mHardnessGroup()
    , mRadius()
    , mPressure()
    , mBlur()
    , mEraseHardnessGroup()
    , mEraseRadius()
    , mErasePressure()
{
    this->setTitle(tr("Free-form Deformation"));
    createBrush();
    updateTypeParam(mParam.type);
}

void FFDPanel::createBrush()
{
    // type
    mTypeGroup.reset(new SingleOutItem(3, QSize(kButtonSpace, kButtonSpace), this));
    mTypeGroup->setChoice(mParam.type);
    mTypeGroup->setToolTips(QStringList() <<
                            tr("Drag Vertex") <<
                            tr("Deform Mesh") <<
                            tr("Erase Deforming"));
    mTypeGroup->setIcons(QVector<QIcon>() <<
                         mResources.icon("move") <<
                         mResources.icon("pencil") <<
                         mResources.icon("eraser"));
    mTypeGroup->connect([=](int aIndex)
    {
        this->mParam.type = (ctrl::FFDParam::Type)aIndex;
        this->updateTypeParam(this->mParam.type);
        this->onParamUpdated(true);
    });

    // hardness
    mHardnessGroup.reset(new SingleOutItem(3, QSize(kButtonSpace, kButtonSpace), this));
    mHardnessGroup->setChoice(mParam.hardness);
    mHardnessGroup->setToolTips(QStringList() <<
                                tr("Second order") << // This is a weird one, I like the names Hidefuku chose, but they aren't very useful for understanding//
                                tr("Fourth order") << // what this function actually does, I'm unaware as to why it is measured in orders (次) and //
                                tr("Eighth order"));  // I'm specially confused as to why it goes from 2(ニ次), to 4(四次) and then to 8(八次), when the hardness is //
    mHardnessGroup->setIcons(QVector<QIcon>() <<  // seemingly meassured from 1 to 3. If you know why and have a more accurate translation, please push it :) //
                             mResources.icon("hardness1") <<
                             mResources.icon("hardness2") <<
                             mResources.icon("hardness3"));
    mHardnessGroup->connect([=](int aIndex)
    {
        this->mParam.hardness = aIndex;
        this->onParamUpdated(false);
    });

    static const int kScale = 100;

    // radius
    mRadius.reset(new SliderItem(tr("Radius"), this->palette(), this));
    mRadius->setAttribute(util::Range(5, 1000), mParam.radius, 50);
    mRadius->connectOnChanged([=](int aValue)
    {
        this->mParam.radius = aValue;
        this->onParamUpdated(false);
    });

    // pressure
    mPressure.reset(new SliderItem(tr("Pressure"), this->palette(), this));
    mPressure->setAttribute(util::Range(0, kScale), mParam.pressure * kScale, kScale / 10);
    mPressure->connectOnMoved([=](int aValue)
    {
        this->mParam.pressure = (float)aValue / kScale;
        this->onParamUpdated(false);
    });

    // blur
    mBlur.reset(new SliderItem(tr("Blur"), this->palette(), this));
    mBlur->setAttribute(util::Range(0, kScale), mParam.blur * kScale, kScale / 10);
    mBlur->connectOnMoved([=](int aValue)
    {
        this->mParam.blur = (float)aValue / kScale;
        this->onParamUpdated(false);
    });

    // erase hardness
    mEraseHardnessGroup.reset(new SingleOutItem(3, QSize(kButtonSpace, kButtonSpace), this));
    mEraseHardnessGroup->setChoice(mParam.eraseHardness);
    mEraseHardnessGroup->setToolTips(QStringList() <<
                                     tr("Second order") << // Should we just keep the names as is?
                                     tr("Fourth order") <<
                                     tr("Eighth order"));
    mEraseHardnessGroup->setIcons(QVector<QIcon>() <<
                             mResources.icon("hardness1") <<
                             mResources.icon("hardness2") <<
                             mResources.icon("hardness3"));
    mEraseHardnessGroup->connect([=](int aIndex)
    {
        this->mParam.eraseHardness = aIndex;
        this->onParamUpdated(false);
    });

    // erase radius
    mEraseRadius.reset(new SliderItem(tr("Radius"), this->palette(), this));
    mEraseRadius->setAttribute(util::Range(5, 1000), mParam.eraseRadius, 50);
    mEraseRadius->connectOnChanged([=](int aValue)
    {
        this->mParam.eraseRadius = aValue;
        this->onParamUpdated(false);
    });

    // erase pressure
    mErasePressure.reset(new SliderItem(tr("Pressure"), this->palette(), this));
    mErasePressure->setAttribute(util::Range(0, kScale), mParam.erasePressure * kScale, kScale / 10);
    mErasePressure->connectOnMoved([=](int aValue)
    {
        this->mParam.erasePressure = (float)aValue / kScale;
        this->onParamUpdated(false);
    });
}

void FFDPanel::updateTypeParam(ctrl::FFDParam::Type aType)
{
    const bool showPencil = (aType == ctrl::FFDParam::Type_Pencil);
    const bool showEraser = (aType == ctrl::FFDParam::Type_Eraser);

    mHardnessGroup->setVisible(showPencil);
    mRadius->setVisible(showPencil);
    mPressure->setVisible(showPencil);
    mBlur->setVisible(showPencil);

    mEraseHardnessGroup->setVisible(showEraser);
    mEraseRadius->setVisible(showEraser);
    mErasePressure->setVisible(showEraser);
}

int FFDPanel::updateGeometry(const QPoint& aPos, int aWidth)
{
    static const int kItemLeft = 8;
    static const int kItemTop = 26;

    const int itemWidth = aWidth - kItemLeft * 2;
    QPoint curPos(kItemLeft, kItemTop);

    // type
    curPos.setY(mTypeGroup->updateGeometry(curPos, itemWidth) + curPos.y() + 5);

    if (mParam.type == ctrl::FFDParam::Type_Pencil)
    {
        // hardness
        curPos.setY(mHardnessGroup->updateGeometry(curPos, itemWidth) + curPos.y() + 5);
        // radius
        curPos.setY(mRadius->updateGeometry(curPos, itemWidth) + curPos.y() + 5);
        // pressure
        curPos.setY(mPressure->updateGeometry(curPos, itemWidth) + curPos.y() + 5);
        // blur
        curPos.setY(mBlur->updateGeometry(curPos, itemWidth) + curPos.y() + 5);
    }
    else if (mParam.type == ctrl::FFDParam::Type_Eraser)
    {
        // erase hardness
        curPos.setY(mEraseHardnessGroup->updateGeometry(curPos, itemWidth) + curPos.y() + 5);
        // eraseRadius
        curPos.setY(mEraseRadius->updateGeometry(curPos, itemWidth) + curPos.y() + 5);
        // erasePressure
        curPos.setY(mErasePressure->updateGeometry(curPos, itemWidth) + curPos.y() + 5);
    }

    this->setGeometry(aPos.x(), aPos.y(), aWidth, curPos.y());

    return aPos.y() + curPos.y();
}

} // namespace tool
} // namespace gui

