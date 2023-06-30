#include "gui/res/res_Notifier.h"
#include "gui/ResourceDialog.h"

using namespace core;

namespace gui {
namespace res {

    //-------------------------------------------------------------------------------------------------
    ChangeFilePathNotifier::ChangeFilePathNotifier(ViaPoint& aViaPoint, const img::ResourceNode& aNode):
        mViaPoint(aViaPoint), mNode(aNode) {
    }

    void ChangeFilePathNotifier::notify(bool) {
        if (mViaPoint.resourceDialog()) {
            mViaPoint.resourceDialog()->updateResourcePath();
        }
    }

    //-------------------------------------------------------------------------------------------------
    ModificationNotifier::ModificationNotifier(
        ViaPoint& aViaPoint, core::Project& aProject, const util::TreePos& aRootPos):
        mViaPoint(aViaPoint),
        mProject(aProject), mRootPos(aRootPos), mEvent(aProject) {
        XC_ASSERT(mRootPos.isValid());
        mEvent.setType(core::ResourceEvent::Type_Reload);
    }

    void ModificationNotifier::notify(bool aIsUndo) {
        mProject.onResourceModified(mEvent, aIsUndo);
        mViaPoint.onVisualUpdated();
    }

    //-------------------------------------------------------------------------------------------------
    AddNewOneNotifier::AddNewOneNotifier(ViaPoint& aViaPoint, core::Project& aProject):
        mViaPoint(aViaPoint), mProject(aProject), mEvent(aProject) {
        mEvent.setType(core::ResourceEvent::Type_AddTree);
    }

    void AddNewOneNotifier::notify(bool aIsUndo) {
        mProject.onResourceModified(mEvent, aIsUndo);
    }

    //-------------------------------------------------------------------------------------------------
    DeleteNotifier::DeleteNotifier(ViaPoint& aViaPoint, core::Project& aProject):
        mViaPoint(aViaPoint), mProject(aProject), mEvent(aProject) {
        mEvent.setType(core::ResourceEvent::Type_Delete);
    }

    void DeleteNotifier::notify(bool aIsUndo) {
        mProject.onResourceModified(mEvent, aIsUndo);
    }

    //-------------------------------------------------------------------------------------------------
    RenameNotifier::RenameNotifier(ViaPoint& aViaPoint, core::Project& aProject, const util::TreePos& aRootPos):
        mViaPoint(aViaPoint), mProject(aProject), mEvent(aProject), mRootPos(aRootPos) {
        mEvent.setType(core::ResourceEvent::Type_Rename);
    }

    void RenameNotifier::notify(bool aIsUndo) {
        mProject.onResourceModified(mEvent, aIsUndo);
    }

} // namespace res
} // namespace gui
