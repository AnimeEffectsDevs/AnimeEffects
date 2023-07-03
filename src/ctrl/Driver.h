#ifndef CTRL_DRIVER_H
#define CTRL_DRIVER_H

#include "core/Project.h"
#include "core/AbstractCursor.h"
#include "core/RenderInfo.h"
#include "core/ObjectNode.h"
#include "core/TimeKeyBlender.h"
#include "ctrl/ToolType.h"
#include "ctrl/IEditor.h"
#include "ctrl/SRTEditor.h"
#include "ctrl/FFDEditor.h"
#include "ctrl/BoneEditor.h"
#include "ctrl/PoseEditor.h"
#include "ctrl/MeshEditor.h"
#include "ctrl/GraphicStyle.h"
#include "ctrl/DriverResources.h"
#include "ctrl/UILogger.h"

namespace ctrl {

class Driver {
public:
    Driver(core::Project& aProject, DriverResources& aResources, GraphicStyle& aGraphicStyle, UILogger& aUILogger);

    void setTarget(core::ObjectNode* aNode);
    core::ObjectNode* currentTarget() const { return mCurrentNode; }
    void setTool(ToolType aType);
    bool updateCursor(const core::AbstractCursor& aCursor, const core::CameraInfo& aCamera);
    void updateFrame();
    void updateKey(core::TimeLineEvent& aEvent, bool aUndo);
    void updateTree(core::ObjectTreeEvent& aEvent, bool aUndo);
    void updateResource(core::ResourceEvent& aEvent, bool aUndo);
    void updateProjectAttribute();

    void renderGL(const core::RenderInfo& aRenderInfo, core::ObjectNode* aGridTarget);
    void renderQt(const core::RenderInfo& aRenderInfo, QPainter& aPainter);

    void updateParam(const SRTParam& aParam);
    void updateParam(const FFDParam& aParam);
    void updateParam(const BoneParam& aParam);
    void updateParam(const PoseParam& aParam);
    void updateParam(const MeshParam& aParam);

private:
    void drawOutline(const core::RenderInfo& aRenderInfo, QPainter& aPainter);
    void drawBanMark(const core::RenderInfo& aRenderInfo, QPainter& aPainter);

    core::Project& mProject;
    DriverResources& mResources;
    GraphicStyle& mGraphicStyle;
    UILogger& mUILogger;
    ToolType mToolType;
    core::TimeKeyBlender mBlender;
    QScopedPointer<IEditor> mEditor;
    core::ObjectNode* mCurrentNode;
    int mOnUpdating;
    bool mRejectedTarget;
};

} // namespace ctrl

#endif // CTRL_DRIVER_H
