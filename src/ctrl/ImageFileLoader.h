#ifndef CTRL_IMAGEFILELOADER_H
#define CTRL_IMAGEFILELOADER_H

#include <QFileInfo>
#include <QString>
#include <QScopedPointer>
#include "util/IProgressReporter.h"
#include "gl/DeviceInfo.h"
#include "core/Project.h"
#include "core/ObjectTree.h"
#include "img/Util.h"
#include "core/FolderNode.h"
#include "img/ORAReader.h"


namespace ctrl {

class ImageFileLoader {
public:
    explicit ImageFileLoader(gl::DeviceInfo  aDeviceInfo);

    void setCanvasSize(const QSize& aSize, bool aForce);

    bool load(const QString& aPath, core::Project& aProject, util::IProgressReporter& aReporter);

    [[nodiscard]] const QString& log() const { return mLog; }

private:
    bool createEmptyCanvas(core::Project& aProject, const QString& aTopName, const QSize& aCanvasSize);

    bool loadPsd(core::Project& aProject, util::IProgressReporter& aReporter);

    bool loadOra(core::Project& aProject, util::IProgressReporter& aReporter);

    bool loadImage(core::Project& aProject, util::IProgressReporter& aReporter);

    static QRect calculateBoundingRectFromChildren(const core::ObjectNode& aNode);
    static void setDefaultPosturesFromInitialRects(core::ObjectNode& aNode);
    bool checkTextureSizeError(uint32 aWidth, uint32 aHeight);

    QString mLog;
    QFileInfo mFileInfo;
    gl::DeviceInfo mGLDeviceInfo;
    QSize mCanvasSize;
    bool mForceCanvasSize;
    static void parseOraLayer(layer &lyr, core::FolderNode* current, img::ResourceNode* resCurrent, const float* globalDepth, const float* parentDepth, core::Project* aProject);
    static void parseOraStack(stack &stk, std::vector<core::FolderNode*>& treeStack, std::vector<img::ResourceNode*>& resStack, float* globalDepth, QRect rect, core::Project* aProject, int* progress, util::IProgressReporter& aReporter);
};

} // namespace ctrl

#endif // CTRL_IMAGEFILELOADER_H
