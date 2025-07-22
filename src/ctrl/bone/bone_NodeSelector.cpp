#include <algorithm>
#include "core/TimeKeyBlender.h"
#include "ctrl/bone/bone_NodeSelector.h"

using namespace core;

namespace ctrl {
namespace bone {

    NodeSelector::Tag::Tag(): node(), parent(), children(), originRect(), sortedRect(), isDir(false), isOpened(false) {}

    NodeSelector::NodeSelector(ObjectNode& aTopNode, const GraphicStyle& aStyle):
        mGraphicStyle(aStyle),
        mTopNode(aTopNode),
        mTopTag(),
        mCurrentTopTag(),
        mCurrentFocus(),
        mCurrentSelect(),
        mIconFocused(false),
        mFocusChanged(false),
        mSortVector() {
        resetTags(mTopNode, mTopTag);
        mTopTag.isOpened = true;
        mCurrentTopTag = &mTopTag;
    }

    void NodeSelector::resetTags(ObjectNode& aNode, Tag& aTag) {
        aTag = Tag();
        aTag.node = &aNode;
        aTag.isDir = aNode.canHoldChild();

        for (auto childNode : aNode.children()) {
            aTag.children.push_back(Tag());
            resetTags(*childNode, aTag.children.back());
            aTag.children.back().parent = &aTag;
        }
    }

    void NodeSelector::initGeometries() { setGeometryRecursive(mTopTag); }

    void NodeSelector::setGeometryRecursive(Tag& aTag) {
        aTag.originRect = getNodeRectF(*aTag.node);
        aTag.sortedRect = aTag.originRect;

        for (auto& childTag : aTag.children) {
            setGeometryRecursive(childTag);
        }
    }

    bool NodeSelector::compareNodeTagHeight(Tag* a, Tag* b) { return a->originRect.y() < b->originRect.y(); }

    void NodeSelector::sortCurrentGeometries(const core::CameraInfo& aCamera) {
        mSortVector.clear();

        auto childrenCount = mCurrentTopTag->children.size();
        const int sortCount = childrenCount + (mCurrentTopTag->invisibleTop() ? 0 : 1);
        if (sortCount == 0)
            return;

        if ((int)mSortVector.size() < sortCount) {
            mSortVector.reserve(sortCount);
        }

        if (!mCurrentTopTag->invisibleTop()) {
            mSortVector.push_back(mCurrentTopTag);
        }
        for (auto& childTag : mCurrentTopTag->children) {
            mSortVector.push_back(&childTag);
        }
        std::stable_sort(mSortVector.begin(), mSortVector.end(), compareNodeTagHeight);

        // initialize a geometry of the highest tag
        auto highestTag = mSortVector[0];
        highestTag->sortedRect.moveTopLeft(aCamera.toScreenPos(highestTag->originRect.topLeft()));

        for (int i = 1; i < (int)mSortVector.size(); ++i) {
            Tag* tag = mSortVector[i];
            tag->sortedRect.moveTopLeft(aCamera.toScreenPos(tag->originRect.topLeft()));

            for (int k = 0; k < i; ++k) {
                Tag* higherTag = mSortVector[k];
                if (tag->sortedRect.intersects(higherTag->sortedRect)) {
                    tag->sortedRect.moveTop(higherTag->sortedRect.bottom());
                }
            }
        }
    }

    ObjectNode* NodeSelector::updateFocus(const CameraInfo& aCamera, const QVector2D& aPos) {
        (void)aCamera;
        const QPointF scrPointF = aPos.toPointF();

        mFocusChanged = false;
        auto prevIconFocused = mIconFocused;
        auto prevCurrentFocus = mCurrentFocus;
        mIconFocused = false;
        mCurrentFocus = nullptr;

        // update intersection with the top
        if (!mCurrentTopTag->invisibleTop()) {
            if (updateIntersection(*mCurrentTopTag, scrPointF)) {
                mFocusChanged = (prevIconFocused != mIconFocused || prevCurrentFocus != mCurrentFocus);
                return mCurrentTopTag->node;
            }
        }

        // update intersections with children
        for (auto& childTag : mCurrentTopTag->children) {
            if (updateIntersection(childTag, scrPointF)) {
                mFocusChanged = (prevIconFocused != mIconFocused || prevCurrentFocus != mCurrentFocus);
                return childTag.node;
            }
        }

        mFocusChanged = (prevIconFocused != mIconFocused || prevCurrentFocus != mCurrentFocus);
        return nullptr;
    }

    bool NodeSelector::updateIntersection(Tag& aTag, const QPointF& aPos) {
        XC_ASSERT(!aTag.invisibleTop());

        QRectF scrRect = aTag.sortedRect;
        const float iconSize = scrRect.height();

        const QRectF scrIconRect(scrRect.topLeft() + QPointF(-iconSize, 0.0f), QSizeF(iconSize, iconSize));
        if (scrRect.contains(aPos)) {
            mCurrentFocus = &aTag;
            mIconFocused = false;
            return true;
        } else if (scrIconRect.contains(aPos)) {
            mCurrentFocus = &aTag;
            mIconFocused = true;
            return true;
        }

        return false;
    }

    QRectF NodeSelector::getNodeRectF(ObjectNode& aNode) const {
        XC_PTR_ASSERT(aNode.timeLine());
        auto mtx = aNode.timeLine()->current().srt().worldSRTMatrix();
        auto pos = mtx.column(3).toVector3D();

        const QRect bb = mGraphicStyle.boundingRect(aNode.name());
        return QRectF(pos.toPointF(), QSizeF(bb.size()));
    }

    ObjectNode* NodeSelector::click(const CameraInfo& aCamera) {
        if (mCurrentFocus) {
            if (mIconFocused) {
                if (mCurrentFocus->isOpened) {
                    auto parentNode = mCurrentFocus->node->parent();
                    XC_PTR_ASSERT(parentNode);
                    mCurrentTopTag = mCurrentFocus->parent;
                    mCurrentFocus->isOpened = false;
                } else {
                    mCurrentTopTag = mCurrentFocus;
                    mCurrentTopTag->isOpened = true;
                }
                mCurrentSelect = nullptr;
                mCurrentFocus = nullptr;
                mIconFocused = false;
                sortCurrentGeometries(aCamera);
                return nullptr;
            } else {
                mCurrentSelect = mCurrentFocus;
                mCurrentFocus = nullptr;
                return mCurrentSelect->node;
            }
        }
        return nullptr;
    }

    void NodeSelector::clearFocus() {
        mCurrentFocus = nullptr;
        mIconFocused = false;
        mFocusChanged = true;
    }

    bool NodeSelector::focusChanged() const { return mFocusChanged; }

    ObjectNode* NodeSelector::selectingNode() { return mCurrentSelect ? mCurrentSelect->node : nullptr; }

    void NodeSelector::clearSelection() { mCurrentSelect = nullptr; }

    NodeSelector::Tag* NodeSelector::findVisibleTag(const core::ObjectNode& aNode) const {
        if (!mCurrentTopTag->invisibleTop()) {
            if (mCurrentTopTag->node == &aNode)
                return mCurrentTopTag;
        }

        for (auto& child : mCurrentTopTag->children) {
            if (child.node == &aNode)
                return &child;
        }
        return nullptr;
    }

    void NodeSelector::renderBindings(
        const RenderInfo& aInfo, QPainter& aPainter, const QMatrix4x4& aTargetMtx, const Bone2* aTopBone
    ) {
        Bone2::ConstIterator itr(aTopBone);
        while (itr.hasNext()) {
            auto bone = itr.next();
            auto parent = bone->parent();
            auto bpos = aInfo.camera.toScreenPos(aTargetMtx.map(QVector3D(bone->worldPos()))).toPointF();
            if (parent) {
                auto pbpos = aInfo.camera.toScreenPos(aTargetMtx.map(QVector3D(parent->worldPos()))).toPointF();
                bpos = (bpos + pbpos) / 2.0f;
            }

            aPainter.setBrush(Qt::NoBrush);
            aPainter.setPen(QPen(QBrush(QColorConstants::Svg::darkmagenta), 2.0f, Qt::DashLine));


            for (auto node : bone->bindingNodes()) {
                auto tag = findVisibleTag(*node);
                if (tag) {
                    auto npos = tag->sortedRect.center();
                    aPainter.drawLine(bpos, npos);
                }
            }
        }
    }

    void NodeSelector::renderTags(const core::RenderInfo& aInfo, QPainter& aPainter) {
        auto tagHeight = mCurrentTopTag->sortedRect.height();
        QPixmap iconPix = mGraphicStyle.icon("dooropen").pixmap(tagHeight);

        QFont font = mGraphicStyle.font();
        aPainter.setFont(font);
        aPainter.setRenderHint(QPainter::Antialiasing);

        if (!mCurrentTopTag->invisibleTop()) {
            renderOneNode(*mCurrentTopTag, iconPix, 0, aInfo, aPainter);
        }

        for (auto childTag : mCurrentTopTag->children) {
            renderOneNode(childTag, iconPix, 0, aInfo, aPainter);
        }

        if (mCurrentFocus) {
            renderOneNode(*mCurrentFocus, iconPix, 1, aInfo, aPainter);
        }
        if (mCurrentSelect) {
            renderOneNode(*mCurrentSelect, iconPix, 2, aInfo, aPainter);
        }
    }

    void NodeSelector::renderOneNode(
        const Tag& aTag, QPixmap& aIconPix, int aColorType, const RenderInfo& aInfo, QPainter& aPainter
    ) {
        (void)aInfo;
        QBrush textBrush(QColor(255, 255, 255, 255));
        QBrush backBrush(QColor(0, 0, 0, 200));

        if (aColorType == 1) {
            textBrush.setColor(QColor(0, 0, 0, 255));
            backBrush.setColor(QColor(255, 255, 255, 255));
        } else if (aColorType == 2) {
            textBrush.setColor(QColor(0, 0, 0, 255));
            backBrush.setColor(QColor(192, 192, 255, 255));
        }

        auto nodeName = aTag.node->name();
        const QRect scrRect = aTag.sortedRect.toRect();

        aPainter.setPen(Qt::NoPen);
        aPainter.setBrush(backBrush);

        // background
        aPainter.drawRect(scrRect.marginsAdded(QMargins(2, 0, 2, 0)));

        // text
        aPainter.setPen(QPen(textBrush, 1.0f));
        aPainter.setBrush(textBrush);
        aPainter.drawText(scrRect, Qt::AlignCenter, nodeName);

        // icon
        if (aTag.isDir) {
            const int iconWidth = aTag.sortedRect.height();
            const QSize iconSize(iconWidth, iconWidth);
            const QPoint iconOffset(-iconWidth, 0);
            const QRect iconRect(scrRect.topLeft() + iconOffset, iconSize);
            aPainter.drawPixmap(iconRect, aIconPix);
        }
    }

} // namespace bone
} // namespace ctrl
