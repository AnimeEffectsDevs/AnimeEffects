#include "core/TimeKeyExpans.h"
#include "ctrl/TimeLineUtil.h"
#include "gui/prop/prop_KeyAccessor.h"

namespace
{

const core::MoveKey::Data& getMoveKeyData(const core::ObjectNode& aTarget, int aFrame)
{
    auto key = aTarget.timeLine()->timeKey(core::TimeKeyType_Move, aFrame);
    XC_PTR_ASSERT(key);
    return ((const core::MoveKey*)key)->data();
}

const core::RotateKey::Data& getRotateKeyData(const core::ObjectNode& aTarget, int aFrame)
{
    auto key = aTarget.timeLine()->timeKey(core::TimeKeyType_Rotate, aFrame);
    XC_PTR_ASSERT(key);
    return ((const core::RotateKey*)key)->data();
}

const core::ScaleKey::Data& getScaleKeyData(const core::ObjectNode& aTarget, int aFrame)
{
    auto key = aTarget.timeLine()->timeKey(core::TimeKeyType_Scale, aFrame);
    XC_PTR_ASSERT(key);
    return ((const core::ScaleKey*)key)->data();
}

const core::DepthKey::Data& getDepthKeyData(const core::ObjectNode& aTarget, int aFrame)
{
    auto key = aTarget.timeLine()->timeKey(core::TimeKeyType_Depth, aFrame);
    XC_PTR_ASSERT(key);
    return ((const core::DepthKey*)key)->data();
}

const core::OpaKey::Data& getOpaKeyData(const core::ObjectNode& aTarget, int aFrame)
{
    auto key = aTarget.timeLine()->timeKey(core::TimeKeyType_Opa, aFrame);
    XC_PTR_ASSERT(key);
    return ((const core::OpaKey*)key)->data();
}

const core::HSVKey::Data& getHSVKeyData(const core::ObjectNode& aTarget, int aFrame)
{
    auto key = aTarget.timeLine()->timeKey(core::TimeKeyType_HSV, aFrame);
    XC_PTR_ASSERT(key);
    return ((const core::HSVKey*)key)->data();
}

#if 0
const core::PoseKey::Data& getPoseKeyData(const core::ObjectNode& aTarget, int aFrame)
{
    auto key = aTarget.timeLine()->timeKey(core::TimeKeyType_Pose, aFrame);
    XC_PTR_ASSERT(key);
    return ((const core::PoseKey*)key)->data();
}

const core::FFDKey::Data& getFFDKeyData(const core::ObjectNode& aTarget, int aFrame)
{
    auto key = aTarget.timeLine()->timeKey(core::TimeKeyType_FFD, aFrame);
    XC_PTR_ASSERT(key);
    return ((const core::FFDKey*)key)->data();
}
#endif

} // namespace

namespace gui {
namespace prop {

KeyAccessor::KeyAccessor()
    : mProject()
    , mTarget()
{
}

void KeyAccessor::setProject(core::Project* aProject)
{
    mProject = aProject;
}

void KeyAccessor::setTarget(core::ObjectNode* aTarget)
{
    mTarget = aTarget;
}

//-------------------------------------------------------------------------------------------------
#define ASSERT_AND_RETURN_INVALID_TARGET() \
    XC_ASSERT(isValid());                  \
    if (!isValid()) return;                \


//-------------------------------------------------------------------------------------------------
void KeyAccessor::assignDefaultDepth(float aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto key = (const core::DepthKey*)currline().defaultKey(core::TimeKeyType_Depth);
    XC_PTR_ASSERT(key);
    core::DepthKey::Data newData = key->data();
    newData.setDepth(aNext);
    ctrl::TimeLineUtil::assignDepthKeyData(
                *mProject, *mTarget, core::TimeLine::kDefaultKeyIndex, newData);
}

void KeyAccessor::assignDefaultOpacity(float aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto key = (const core::OpaKey*)currline().defaultKey(core::TimeKeyType_Opa);
    XC_PTR_ASSERT(key);
    core::OpaKey::Data newData = key->data();
    newData.setOpacity(aNext);
    ctrl::TimeLineUtil::assignOpaKeyData(
                *mProject, *mTarget, core::TimeLine::kDefaultKeyIndex, newData);
}

void KeyAccessor::assignDefaultHSV(QList<int> aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto key = (const core::HSVKey*)currline().defaultKey(core::TimeKeyType_HSV);
    XC_PTR_ASSERT(key);
    core::HSVKey::Data newData = key->data();
    newData.setHSV(aNext);
    ctrl::TimeLineUtil::assignHSVKeyData(
                *mProject, *mTarget, core::TimeLine::kDefaultKeyIndex, newData);
}

void KeyAccessor::assignDefaultImageResource(img::ResourceNode& aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    ctrl::TimeLineUtil::assignImageKeyResource(
                *mProject, *mTarget, core::TimeLine::kDefaultKeyIndex, aNext);
}

void KeyAccessor::assignDefaultImageOffset(const QVector2D& aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    ctrl::TimeLineUtil::assignImageKeyOffset(
                *mProject, *mTarget, core::TimeLine::kDefaultKeyIndex, aNext);
}

void KeyAccessor::assignDefaultImageCellSize(int aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    ctrl::TimeLineUtil::assignImageKeyCellSize(
                *mProject, *mTarget, core::TimeLine::kDefaultKeyIndex, aNext);
}

//-------------------------------------------------------------------------------------------------
void KeyAccessor::assignMoveEasing(util::Easing::Param aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    XC_ASSERT(aNext.isValidParam());
    const int frame = getFrame();
    auto newData = getMoveKeyData(*mTarget, frame);
    newData.easing() = aNext;

    ctrl::TimeLineUtil::assignMoveKeyData(*mProject, *mTarget, frame, newData);
}

void KeyAccessor::assignMoveSpline(int aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    XC_ASSERT(0 <= aNext && aNext < core::MoveKey::SplineType_TERM);
    const int frame = getFrame();
    auto newData = getMoveKeyData(*mTarget, frame);
    newData.setSpline((core::MoveKey::SplineType)aNext);

    ctrl::TimeLineUtil::assignMoveKeyData(*mProject, *mTarget, frame, newData);
}

void KeyAccessor::assignMovePosition(const QVector2D& aNewPos)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    const int frame = getFrame();
    auto newData = getMoveKeyData(*mTarget, frame);
    newData.setPos(aNewPos);

    ctrl::TimeLineUtil::assignMoveKeyData(*mProject, *mTarget, frame, newData);
}

void KeyAccessor::assignMoveCentroid(const QVector2D& aNewCentroid)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    const int frame = getFrame();
    auto newData = getMoveKeyData(*mTarget, frame);
    newData.setCentroid(aNewCentroid);

    ctrl::TimeLineUtil::assignMoveKeyData(*mProject, *mTarget, frame, newData);
}

//-------------------------------------------------------------------------------------------------
void KeyAccessor::assignRotateEasing(util::Easing::Param aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    XC_ASSERT(aNext.isValidParam());
    const int frame = getFrame();
    auto newData = getRotateKeyData(*mTarget, frame);
    newData.easing() = aNext;

    ctrl::TimeLineUtil::assignRotateKeyData(*mProject, *mTarget, frame, newData);
}

void KeyAccessor::assignRotateAngle(float aAngle)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    const int frame = getFrame();
    auto newData = getRotateKeyData(*mTarget, frame);
    newData.setRotate(aAngle);

    ctrl::TimeLineUtil::assignRotateKeyData(*mProject, *mTarget, frame, newData);
}

//-------------------------------------------------------------------------------------------------
void KeyAccessor::assignScaleEasing(util::Easing::Param aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    XC_ASSERT(aNext.isValidParam());
    const int frame = getFrame();
    auto newData = getScaleKeyData(*mTarget, frame);
    newData.easing() = aNext;
    ctrl::TimeLineUtil::assignScaleKeyData(*mProject, *mTarget, frame, newData);
}

void KeyAccessor::assignScaleRate(const QVector2D& aNewScale)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    const int frame = getFrame();
    auto newData = getScaleKeyData(*mTarget, frame);
    newData.setScale(aNewScale);

    ctrl::TimeLineUtil::assignScaleKeyData(*mProject, *mTarget, frame, newData);
}

//-------------------------------------------------------------------------------------------------
void KeyAccessor::assignDepthEasing(util::Easing::Param aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    XC_ASSERT(aNext.isValidParam());
    const int frame = getFrame();
    auto newData = getDepthKeyData(*mTarget, frame);
    newData.easing() = aNext;

    ctrl::TimeLineUtil::assignDepthKeyData(*mProject, *mTarget, frame, newData);
}

void KeyAccessor::assignDepthPosition(float aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    const int frame = getFrame();
    auto newData = getDepthKeyData(*mTarget, frame);
    newData.setDepth(aNext);

    ctrl::TimeLineUtil::assignDepthKeyData(*mProject, *mTarget, frame, newData);
}

//-------------------------------------------------------------------------------------------------
void KeyAccessor::assignOpacity(float aOpacity)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    const int frame = getFrame();
    auto newData = getOpaKeyData(*mTarget, frame);
    newData.setOpacity(aOpacity);

    ctrl::TimeLineUtil::assignOpaKeyData(*mProject, *mTarget, frame, newData);
}

void KeyAccessor::assignHSV(int aValue, QString aType)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    const int frame = getFrame();
    auto newData = getHSVKeyData(*mTarget, frame);
    if (aType == "hue"){
        newData.setHue(aValue);
    }
    else if (aType == "sat"){
        newData.setSaturation(aValue);
    }
    else if (aType == "val"){
        newData.setValue(aValue);
    }
    else if (aType == "abs"){
        newData.setAbsolute(aValue);
    }

    ctrl::TimeLineUtil::assignHSVKeyData(*mProject, *mTarget, frame, newData);
}

void KeyAccessor::assignOpaEasing(util::Easing::Param aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    XC_ASSERT(aNext.isValidParam());
    const int frame = getFrame();
    auto newData = getOpaKeyData(*mTarget, frame);
    newData.easing() = aNext;

    ctrl::TimeLineUtil::assignOpaKeyData(*mProject, *mTarget, frame, newData);
}

void KeyAccessor::assignHSVEasing(util::Easing::Param aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    XC_ASSERT(aNext.isValidParam());
    const int frame = getFrame();
    auto newData = getHSVKeyData(*mTarget, frame);
    newData.easing() = aNext;

    ctrl::TimeLineUtil::assignHSVKeyData(*mProject, *mTarget, frame, newData);
}

void KeyAccessor::assignPoseEasing(util::Easing::Param aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    XC_ASSERT(aNext.isValidParam());
    ctrl::TimeLineUtil::assignPoseKeyEasing(*mProject, *mTarget, getFrame(), aNext);
}

void KeyAccessor::assignFFDEasing(util::Easing::Param aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    XC_ASSERT(aNext.isValidParam());
    ctrl::TimeLineUtil::assignFFDKeyEasing(*mProject, *mTarget, getFrame(), aNext);
}

void KeyAccessor::assignImageResource(img::ResourceNode& aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    ctrl::TimeLineUtil::assignImageKeyResource(*mProject, *mTarget, getFrame(), aNext);
}

void KeyAccessor::assignImageOffset(const QVector2D& aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    ctrl::TimeLineUtil::assignImageKeyOffset(*mProject, *mTarget, getFrame(), aNext);
}

void KeyAccessor::assignImageCellSize(int aNext)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    ctrl::TimeLineUtil::assignImageKeyCellSize(*mProject, *mTarget, getFrame(), aNext);
}

//-------------------------------------------------------------------------------------------------

template <typename T>
void assignParam(T * key){
    QSettings settings;
    util::Easing::Param aNext;
    aNext.type = util::Easing::easingToEnum(QString());
    aNext.range = util::Easing::rangeToEnum(QString());
    key->data().easing() = aNext;
}

//-------------------------------------------------------------------------------------------------

void KeyAccessor::knockNewMove()
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto newKey = new core::MoveKey();
    newKey->setPos(currline().current().srt().pos());
    newKey->setCentroid(currline().current().srt().centroid());
    assignParam(newKey);
    ctrl::TimeLineUtil::pushNewMoveKey(*mProject, *mTarget, getFrame(), newKey);
}
void KeyAccessor::knockNewRotate()
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto newKey = new core::RotateKey();
    newKey->setRotate(currline().current().srt().rotate());
    assignParam(newKey);
    ctrl::TimeLineUtil::pushNewRotateKey(*mProject, *mTarget, getFrame(), newKey);
}
void KeyAccessor::knockNewScale()
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto newKey = new core::ScaleKey();
    newKey->setScale(currline().current().srt().scale());
    assignParam(newKey);
    ctrl::TimeLineUtil::pushNewScaleKey(*mProject, *mTarget, getFrame(), newKey);
}

void KeyAccessor::knockNewDepth()
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto newKey = new core::DepthKey();
    newKey->setDepth(currline().current().depth());
    assignParam(newKey);
    ctrl::TimeLineUtil::pushNewDepthKey(*mProject, *mTarget, getFrame(), newKey);
}

void KeyAccessor::knockNewOpacity()
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto newKey = new core::OpaKey();
    newKey->data() = currline().current().opa();
    assignParam(newKey);
    ctrl::TimeLineUtil::pushNewOpaKey(*mProject, *mTarget, getFrame(), newKey);
}

void KeyAccessor::knockNewHSV()
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto newKey = new core::HSVKey();
    newKey->data() = currline().current().hsv();
    assignParam(newKey);
    ctrl::TimeLineUtil::pushNewHSVKey(*mProject, *mTarget, getFrame(), newKey);
}

void KeyAccessor::knockNewPose()
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto newKey = new core::PoseKey();
    core::BoneKey* parentKey = currline().current().bone().areaKey();
    XC_PTR_ASSERT(parentKey);
    if (parentKey == (core::BoneKey*)currline().current().poseParent())
    {
        newKey->data() = currline().current().pose();
    }
    else
    {
        newKey->data().createBonesBy(*parentKey);
    }
    assignParam(newKey);
    ctrl::TimeLineUtil::pushNewPoseKey(*mProject, *mTarget, getFrame(), newKey, parentKey);
}

void KeyAccessor::knockNewFFD()
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto newKey = new core::FFDKey();
    newKey->data() = currline().current().ffd();
    core::TimeKey* parentKey = currline().current().ffdMeshParent();
    assignParam(newKey);
    ctrl::TimeLineUtil::pushNewFFDKey(*mProject, *mTarget, getFrame(), newKey, parentKey);
}

void KeyAccessor::knockNewImage(const img::ResourceHandle& aHandle)
{
    ASSERT_AND_RETURN_INVALID_TARGET();
    auto newKey = new core::ImageKey();
    newKey->setImage(aHandle);
    newKey->resetGridMesh();
    newKey->setImageOffsetByCenter();
    assignParam(newKey);
    ctrl::TimeLineUtil::pushNewImageKey(*mProject, *mTarget, getFrame(), newKey);
}

} // namespace prop
} // namespace gui
