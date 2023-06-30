#include "core/ObjectTree.h"
#include "cmnd/BasicCommands.h"
#include "cmnd/Stable.h"
#include "cmnd/Vector.h"
#include "core/BoneKeyUpdater.h"
#include "core/FolderNode.h"
#include "core/ImageKeyUpdater.h"
#include "core/LayerNode.h"
#include "core/ObjectTreeEvent.h"
#include "core/TimeCacheAccessor.h"
#include "util/LinkPointer.h"
#include "util/TreeUtil.h"
#include <algorithm>
#include <functional>

namespace core {

//-------------------------------------------------------------------------------------------------
class SortAndRenderCall {
    static bool compareDepth(Renderer::SortUnit a, Renderer::SortUnit b) {
        return a.depth < b.depth;
    }

    void pushNodeRecursive(ObjectNode* aNode, bool aPush) {
        if (!aNode || !aNode->isVisible())
            return;

        auto renderer = aNode->renderer();
        if (renderer) {
            if (aPush) {
                if (!renderer->isClipped()) {
                    Renderer::SortUnit unit;
                    unit.renderer = renderer;
                    unit.depth = mAccessor->get(*aNode).worldDepth();
                    mArray.push_back(unit);
                } else {
                    aPush = false;
                }
            }
        }

        auto& children = aNode->children();
        for (auto itr = children.rbegin(); itr != children.rend(); ++itr) {
            pushNodeRecursive(*itr, aPush);
        }
    }

    std::vector<Renderer::SortUnit> mArray;
    const TimeCacheAccessor* mAccessor;

public:
    SortAndRenderCall(): mArray(), mAccessor() {}

    void invoke(ObjectNode* aTopNode, const RenderInfo& aInfo, const TimeCacheAccessor& aAccessor) {
        mAccessor = &aAccessor;

        // prerender
        {
            ObjectNode::Iterator itr(aTopNode);
            while (itr.hasNext()) {
                auto renderer = itr.next()->renderer();
                if (renderer) {
                    renderer->prerender(aInfo, aAccessor);
                }
            }
        }

        // sort
        mArray.clear();
        pushNodeRecursive(aTopNode, true);
        std::stable_sort(mArray.begin(), mArray.end(), compareDepth);

        // render
        for (auto data : mArray) {
            data.renderer->render(aInfo, aAccessor);
        }
    }
};

} // namespace core

namespace core {

//-------------------------------------------------------------------------------------------------
ObjectTree::ObjectTree():
    mLifeLink(), mTopNode(), mCaller(new SortAndRenderCall()), mShaderHolder(), mTimeCacheLock() {}

ObjectTree::~ObjectTree() {}

void ObjectTree::render(const RenderInfo& aInfo, bool aUseWorkingCache) {
    if (mTopNode.data()) {
        TimeCacheAccessor accessor(*mTopNode.data(), mTimeCacheLock, aInfo.time, aUseWorkingCache);
        mCaller->invoke(mTopNode.data(), aInfo, accessor);
    }
}

cmnd::Vector ObjectTree::createNodeDeleter(ObjectNode& aNode) {
    cmnd::Vector commands;

    core::ObjectNode* parent = aNode.parent();
    XC_PTR_ASSERT(parent);
    if (!parent)
        return commands; // fail safe code

    auto index = parent->children().indexOf(&aNode);
    XC_ASSERT(index >= 0);
    if (index < 0)
        return commands; // fail safe code

    commands.push(BoneKeyUpdater::createNodeUnbinderForDelete(aNode));
    commands.push(ImageKeyUpdater::createResourceSleeperForDelete(aNode));
    commands.push(new cmnd::RemoveTree<core::ObjectNode>(&(parent->children()), index));
    commands.push(new cmnd::GrabDeleteObject<core::ObjectNode>(&aNode));

    return commands;
}

#if 0
cmnd::Vector ObjectTree::createNodeMover(
        const util::TreePos& aFrom, const util::TreePos& aTo)
{
    class MoveNodeCommand : public cmnd::Stable
    {
        util::LinkPointer<ObjectTree> mTree;
        util::TreePos mFrom;
        util::TreePos mTo;

    public:
        MoveNodeCommand(ObjectTree& aTree, const util::TreePos& aFrom, const util::TreePos& aTo)
            : mTree(aTree.pointee())
            , mFrom(aFrom)
            , mTo(aTo)
        {
        }

        virtual void undo()
        {
            if (!mTree) return;
            ObjectNode* target = mTree->eraseNode(mTo);
            mTree->insertNode(mFrom, target);
        }

        virtual void redo()
        {
            if (!mTree) return;
            ObjectNode* target = mTree->eraseNode(mFrom);
            mTree->insertNode(mTo, target);
        }
    };

    cmnd::Vector commands;
    commands.push(BoneKeyUpdater::createNodeUnbinderForMove(*this, aFrom, aTo));
    commands.push(new MoveNodeCommand(*this, aFrom, aTo));
    return commands;
}
#endif

cmnd::Vector ObjectTree::createNodesMover(
    const QVector<util::TreePos>& aRemoved, const QVector<util::TreePos>& aInserted) {
    class MoveNodesCommand: public cmnd::Stable {
        util::LinkPointer<ObjectTree> mTree;
        QVector<util::TreePos> mRemoved;
        QVector<util::TreePos> mInserted;
        BoneUnbindWorkspacePtr mWorkspace;

    public:
        MoveNodesCommand(ObjectTree& aTree, const QVector<util::TreePos>& aRemoved,
            const QVector<util::TreePos>& aInserted, const BoneUnbindWorkspacePtr& aWorkspace):
            mTree(aTree.pointee()),
            mRemoved(aRemoved), mInserted(aInserted), mWorkspace(aWorkspace) {}

        virtual void exec() {
            redo();
            mWorkspace.reset();
        }

        virtual void undo() {
            if (!mTree)
                return;

            QVector<ObjectNode*> nodes;
            for (auto itr = mInserted.rbegin(); itr != mInserted.rend(); ++itr) {
                ObjectNode* node = mTree->eraseNode(*itr);
                XC_PTR_ASSERT(node);
                nodes.push_back(node);
            }
            {
                auto nodeItr = nodes.begin();
                for (auto itr = mRemoved.rbegin(); itr != mRemoved.rend(); ++itr) {
                    mTree->insertNode(*itr, *nodeItr);
                    ++nodeItr;
                }
            }
        }

        virtual void redo() {
            if (!mTree)
                return;

            QVector<ObjectNode*> nodes;
            for (auto itr = mRemoved.begin(); itr != mRemoved.end(); ++itr) {
                // record one node
                if (mWorkspace) {
                    ObjectNode* node = mTree->findNode(*itr);
                    XC_PTR_ASSERT(node);
                    mWorkspace->push(*node);
                }
                // erase one node
                {
                    ObjectNode* node = mTree->eraseNode(*itr);
                    XC_PTR_ASSERT(node);
                    nodes.push_back(node);
                }
            }
            {
                auto nodeItr = nodes.begin();
                for (auto itr = mInserted.begin(); itr != mInserted.end(); ++itr) {
                    mTree->insertNode(*itr, *nodeItr);
                    ++nodeItr;
                }
            }
        }
    };

    cmnd::Vector commands;
    BoneUnbindWorkspacePtr workspace = std::make_shared<BoneUnbindWorkspace>();
    commands.push(new MoveNodesCommand(*this, aRemoved, aInserted, workspace));
    commands.push(BoneKeyUpdater::createNodesUnbinderForMove(*this, workspace));
    return commands;
}

ObjectNode* ObjectTree::findNode(const util::TreePos& aPos) {
    if (!aPos.isValid())
        return nullptr;

    ObjectNode::Children::Iterator itr;
    ObjectNode* current = topNode();

    for (int i = 1; i < aPos.depth() - 1; ++i) {
        itr = current->children().at(aPos.row(i));
        if (itr == current->children().end())
            return nullptr;
        current = *itr;
    }

    itr = current->children().at(aPos.tailRow());
    if (itr == current->children().end())
        return nullptr;

    ObjectNode* target = *itr;
    return target;
}

ObjectNode* ObjectTree::eraseNode(const util::TreePos& aPos) {
    ObjectNode::Children::Iterator itr;
    ObjectNode* current = topNode();

    for (int i = 1; i < aPos.depth() - 1; ++i) {
        itr = current->children().at(aPos.row(i));
        current = *itr;
    }

    itr = current->children().at(aPos.tailRow());
    // qDebug() << "erase" << current->name() << aPos.tailRow() << (itr != current->children().end() ? (*itr)->name() :
    // "invalid");

    ObjectNode* target = *itr;
    current->children().erase(itr);
    return target;
}

void ObjectTree::insertNode(const util::TreePos& aPos, ObjectNode* aNode) {
    XC_PTR_ASSERT(aNode);
    ObjectNode::Children::Iterator itr;
    ObjectNode* current = topNode();

    for (int i = 1; i < aPos.depth() - 1; ++i) {
        itr = current->children().at(aPos.row(i));
        current = *itr;
    }

    itr = current->children().at(aPos.tailRow());
    current->children().insert(itr, aNode);
    // qDebug() << "insert" << current->name() << aPos.tailRow() << aNode->name();
}

cmnd::Vector ObjectTree::createResourceUpdater(const ResourceEvent& aEvent) {
    cmnd::Vector result;
    if (mTopNode) {
        ObjectNode::Iterator itr(mTopNode.data());
        while (itr.hasNext()) {
            result.push(itr.next()->createResourceUpdater(aEvent));
        }
    }
    return result;
}

void ObjectTree::onTimeLineModified(TimeLineEvent& aEvent, bool) {
    BoneKeyUpdater::onTimeLineModified(aEvent);
}

void ObjectTree::onTreeRestructured(ObjectTreeEvent& aEvent, bool) {
    BoneKeyUpdater::onTreeRestructured(aEvent);
}

void ObjectTree::onResourceModified(ResourceEvent& aEvent, bool) {
    BoneKeyUpdater::onResourceModified(aEvent);
}

void ObjectTree::onProjectAttributeModified(ProjectEvent& aEvent, bool) {
    BoneKeyUpdater::onProjectAttributeModified(aEvent);
}

bool ObjectTree::serialize(Serializer& aOut) const {
    static const std::array<uint8, 8> kSignature = {'O', 'b', 'j', 'T', 'r', 'e', 'e', '_'};

    // signature
    auto pos = aOut.beginBlock(kSignature);

    // top node count
    aOut.write(topNode() ? 1 : 0);

    // nodes
    if (topNode()) {
        if (!serializeNode(aOut, *topNode())) {
            return false;
        }
    }

    aOut.endBlock(pos);

    return aOut.checkStream();
}

bool ObjectTree::serializeNode(Serializer& aOut, const ObjectNode& aNode) const {
    static const std::array<uint8, 8> kSignature = {'O', 'b', 'j', 'N', 'o', 'd', 'e', '_'};

    // block begin
    auto pos = aOut.beginBlock(kSignature);

    // type
    aOut.write((int)aNode.type());

    // child count
    aOut.write((int)aNode.children().size());

    // reference id
    aOut.writeID(&aNode);

    // serialize object node
    if (!aNode.serialize(aOut)) {
        return false;
    }

    // block end
    aOut.endBlock(pos);

    // check failure
    if (aOut.failure()) {
        return false;
    }

    // iterate children
    if (aNode.canHoldChild()) {
        for (auto child : aNode.children()) {
            XC_PTR_ASSERT(child);

            if (!serializeNode(aOut, *child)) {
                return false;
            }
        }
    }

    return !aOut.failure();
}

bool ObjectTree::deserialize(Deserializer& aIn) {
    // clear
    mTopNode.reset();

    // check block begin
    if (!aIn.beginBlock("ObjTree_"))
        return aIn.errored("invalid signature of object tree");

    // dive log scope
    aIn.pushLogScope("ObjectTree");

    // top node count
    int topNodeCount = 0;
    aIn.read(topNodeCount);

    if (topNodeCount == 1) {
        aIn.pushLogScope("ObjectNodes");

        // deserialize each node
        if (!deserializeNode(aIn, nullptr))
            return false;

        aIn.popLogScope();
    } else if (topNodeCount != 0) {
        return aIn.errored("invalid top node count");
    }

    // check block end
    if (!aIn.endBlock())
        return aIn.errored("invalid end of object tree");

    // rise log scope
    aIn.popLogScope();

    // end
    return aIn.checkStream();
}

bool ObjectTree::deserializeNode(Deserializer& aIn, ObjectNode* aParent) {
    // check block begin
    if (!aIn.beginBlock("ObjNode_"))
        return aIn.errored("invalid signature of object node");

    // dive log scope
    if (aParent)
        aIn.pushLogScope(aParent->name());

    // node type
    int type = 0;
    aIn.read(type);
    if (type >= ObjectType_TERM)
        return aIn.errored("invalid node type");

    // child count
    int childCount = 0;
    aIn.read(childCount);
    if (childCount < 0)
        return aIn.errored("invalid child count");

    // create node
    ObjectNode* node(createSerialNode(type));
    if (!node)
        return aIn.errored("failed to create node");

    // reference id
    if (!aIn.bindIDData(node)) {
        delete node;
        return aIn.errored("failed to bind reference id");
    }

    // deserialize node
    if (!node->deserialize(aIn)) {
        delete node;
        return aIn.errored("failed to deserialize node");
    }

    // push to tree
    if (!aParent) {
        grabTopNode(node);
    } else {
        aParent->children().pushBack(node);
    }

    // check block end
    if (!aIn.endBlock())
        return aIn.errored("invalid end of object node");

    // check failure
    if (aIn.failure())
        return aIn.errored("stream error");

    // check holdability
    if (!node->canHoldChild() && childCount > 0)
        return aIn.errored("invalid holdability");

    // iterate children
    for (int i = 0; i < childCount; ++i) {
        if (!deserializeNode(aIn, node))
            return false;
    }

    // rise log scope
    if (aParent)
        aIn.popLogScope();

    // progress report
    aIn.reportCurrent();

    // end
    return aIn.checkStream();
}

ObjectNode* ObjectTree::createSerialNode(int aType) {
    ObjectNode* node = nullptr;

    if (aType == ObjectType_Layer) {
        LayerNode* layer = new LayerNode(QString("no name"), mShaderHolder);
        layer->setVisibility(true);
        node = layer;
    } else if (aType == ObjectType_Folder) {
        FolderNode* folder = new FolderNode(QString("no name"));
        folder->setVisibility(true);
        node = folder;
    }

    return node;
}

} // namespace core
