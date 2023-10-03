#ifndef CTRL_SRT_CENTROIDMODE_H
#define CTRL_SRT_CENTROIDMODE_H

#include "core/Project.h"
#include "core/ObjectNode.h"
#include "ctrl/srt/srt_IMode.h"
#include "ctrl/srt/srt_KeyOwner.h"
#include "ctrl/srt/srt_CentroidMover.h"

namespace ctrl {
namespace srt {

    //-------------------------------------------------------------------------------------------------
    class CentroidMode: public IMode {
    public:
        CentroidMode(core::Project& aProject, core::ObjectNode& aTarget, KeyOwner& aKey);
        virtual void updateParam(const SRTParam&);
        virtual bool updateCursor(const core::CameraInfo&, const core::AbstractCursor&);
        virtual void renderQt(const core::RenderInfo& aInfo, QPainter& aPainter);

    private:
        void moveCentroid(const QVector2D& aNewCentroid, const QVector2D& aNewPosition);
        QVector2D getWorldSymbolPos() const;
        core::Project& mProject;
        core::ObjectNode& mTarget;
        KeyOwner& mKeyOwner;

        bool mFocusing;
        bool mMoving;
        QVector2D mBaseVec;
        QVector2D mBasePosition;
        QVector2D mBaseCentroid;
        CentroidMover* mCommandRef;
        bool mAdjustPosition;
    };

} // namespace srt
} // namespace ctrl

#endif // CTRL_SRT_CENTROIDMODE_H
