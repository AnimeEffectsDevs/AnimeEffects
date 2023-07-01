#ifndef CORE_OBJECTNODEUTIL_H
#define CORE_OBJECTNODEUTIL_H

#include <vector>
#include <QRectF>
#include <QMatrix4x4>
#include "cmnd/Listener.h"
#include "core/ObjectNode.h"
#include "core/FolderNode.h"
#include "core/SRTExpans.h"
#include "core/TimeCacheAccessor.h"

namespace core {

namespace ObjectNodeUtil {
    float getInitialWorldDepth(ObjectNode& aNode);

    bool thereAreSomeKeysExceedingFrame(const ObjectNode* aRootNode, int aMaxFrame);

    void collectRenderClippees(
        ObjectNode& aNode, std::vector<Renderer::SortUnit>& aDest, const TimeCacheAccessor& aAccessor);

    class AttributeNotifier: public cmnd::Listener {
        Project& mProject;
        ObjectNode& mTarget;

    public:
        AttributeNotifier(Project& aProject, ObjectNode& aTarget);
        virtual void onExecuted();
        virtual void onUndone();
        virtual void onRedone();
    };

} // namespace ObjectNodeUtil

} // namespace core

#endif // CORE_OBJECTNODEUTIL_H
