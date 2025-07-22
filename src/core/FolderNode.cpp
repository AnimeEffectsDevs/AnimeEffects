#include <algorithm>
#include "core/FolderNode.h"
#include "core/ObjectNodeUtil.h"
#include "core/TimeKeyExpans.h"
#include "core/DepthKey.h"

namespace core {

FolderNode::FolderNode(const QString& aName):
    mName(aName),
    mIsVisible(true),
    mIsSlimmedDown(),
    mInitialRect(),
    mHeightMap(),
    mTimeLine(),
    mIsClipped(),
    mClippees() {}

FolderNode::~FolderNode() { qDeleteAll(children()); }

void FolderNode::setDefaultPosture(const QVector2D& aPos) {
    getOrCreateDefaultKey<MoveKey, TimeKeyType_Move>(mTimeLine)->data().setPos(aPos);
    getOrCreateDefaultKey<RotateKey, TimeKeyType_Rotate>(mTimeLine);
    getOrCreateDefaultKey<ScaleKey, TimeKeyType_Scale>(mTimeLine);
}

void FolderNode::setDefaultDepth(float aValue) {
    getOrCreateDefaultKey<DepthKey, TimeKeyType_Depth>(mTimeLine)->setDepth(aValue);
}

void FolderNode::setDefaultOpacity(float aValue) {
    getOrCreateDefaultKey<OpaKey, TimeKeyType_Opa>(mTimeLine)->setOpacity(aValue);
}

void FolderNode::grabHeightMap(HeightMap* aNode) { mHeightMap.reset(aNode); }

bool FolderNode::isClipper() const {
    if (mIsClipped) return false;

    auto prev = this->prevSib();
    return prev && prev->renderer() && prev->renderer()->isClipped();
}

void FolderNode::prerender(const RenderInfo&, const TimeCacheAccessor&) {}

void pushRenderRecursive(core::ObjectNode& aNode, std::vector<core::Renderer::SortUnit>& aDest,
    const core::TimeCacheAccessor& aAccessor, const core::RenderInfo aInfo) {
    
    if (!aNode.isVisible() || !aNode.renderer()) return;

    core::Renderer::SortUnit unit;
    unit.renderer = aNode.renderer();
    unit.depth = aAccessor.get(aNode).worldDepth();
    unit.timeline = aNode.timeLine();
    aDest.push_back(unit);

    for (auto child : aNode.children()) {
        while (child) {
            pushRenderRecursive(*child, aDest, aAccessor, aInfo);
            child = child->prevSib();
        }
    }
}

void FolderNode::render(const RenderInfo& aInfo, const TimeCacheAccessor& aAccessor) {
    if (!mIsVisible || aAccessor.get(mTimeLine).opa().isZero() || aInfo.isGrid) return;

    // render clippees
    renderClippees(aInfo, aAccessor);

    if (!mTimeLine.isEmpty(TimeKeyType_HSV) && aInfo.time.frame.get() >= mTimeLine.map(TimeKeyType_HSV).values().first()->frame()) {
        auto children = this->children();
        for (auto p : children) {
            while (p) {
                pushRenderRecursive(*p, mRenders, aAccessor, aInfo);
                p = p->prevSib();
            }
        }
        if (!mRenders.empty()) {
            // Depth sorting
            std::stable_sort(
                mRenders.begin(), mRenders.end(),
                [=](core::Renderer::SortUnit a, core::Renderer::SortUnit b) { return a.depth < b.depth; }
            );
        }
        
        
        auto hsvData = aAccessor.get(mTimeLine).hsv().hsv();
        for (auto child : mRenders) {
            child.renderer->renderHSV(aInfo, aAccessor, hsvData);
        }
        mRenders.clear();
    }
}

void FolderNode::renderClippees(const RenderInfo& aInfo, const TimeCacheAccessor& aAccessor) {
    if (!aInfo.clippingFrame || !isClipper())
        return;

    // reset clippees
    ObjectNodeUtil::collectRenderClippees(*this, mClippees, aAccessor);

    // clipping frame
    auto& frame = *aInfo.clippingFrame;

    const uint8 clippingId = frame.forwardClippingId();

    RenderInfo childInfo = aInfo;
    childInfo.clippingId = clippingId;

    uint32 stamp = frame.renderStamp() + 1;

    for (auto clippee : mClippees) {
        XC_PTR_ASSERT(clippee.renderer);

        // write clipper as necessary
        if (stamp != frame.renderStamp()) {
            renderClipper(aInfo, aAccessor, clippingId);
            stamp = frame.renderStamp();
        }

        // render child
        clippee.renderer->render(childInfo, aAccessor);
    }
}

void FolderNode::renderClipper(const RenderInfo& aInfo, const TimeCacheAccessor& aAccessor, uint8 aClipperId) {
    for (auto child : this->children()) {
        if (child->renderer()) {
            child->renderer()->renderClipper(aInfo, aAccessor, aClipperId);
        }
    }
}


void FolderNode::renderHSV(const RenderInfo& aInfo, const TimeCacheAccessor& aAccessor, QList<int> HSVData) {
    for (auto child : this->children()) {
        if (child->renderer()) {
            child->renderer()->renderHSV(aInfo, aAccessor, HSVData);
        }
    }
}

float FolderNode::initialDepth() const {
    auto key = (DepthKey*)mTimeLine.defaultKey(TimeKeyType_Depth);
    return key ? key->depth() : 0.0f;
}

void FolderNode::setClipped(bool aIsClipped) { mIsClipped = aIsClipped; }

bool FolderNode::serialize(Serializer& aOut) const {
    static const std::array<uint8, 8> kSignature = {'F', 'o', 'l', 'd', 'e', 'r', 'N', 'd'};

    // block begin
    auto pos = aOut.beginBlock(kSignature);

    // name
    aOut.write(mName);
    // visibility
    aOut.write(mIsVisible);
    // slim-down
    aOut.write(mIsSlimmedDown);
    // initial rect
    aOut.write(mInitialRect);
    // clipping
    aOut.write(mIsClipped);

    // timeline
    if (!mTimeLine.serialize(aOut)) {
        return false;
    }

    // block end
    aOut.endBlock(pos);

    return !aOut.failure();
}

bool FolderNode::deserialize(Deserializer& aIn) {
    // check block begin
    if (!aIn.beginBlock("FolderNd"))
        return aIn.errored("invalid signature of folder node");

    // name
    aIn.read(mName);
    // visibility
    aIn.read(mIsVisible);
    // slim-down
    aIn.read(mIsSlimmedDown);
    // initial rect
    aIn.read(mInitialRect);
    // clipping
    aIn.read(mIsClipped);

    // timeline
    if (!mTimeLine.deserialize(aIn))
        return aIn.errored("failed to deserialize time line");

    // check block end
    if (!aIn.endBlock())
        return aIn.errored("invalid end of folder node");

    return !aIn.failure();
}

} // namespace core
