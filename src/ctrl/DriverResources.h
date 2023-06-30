#ifndef DRIVERRESOURCES_H
#define DRIVERRESOURCES_H

#include "util/NonCopyable.h"
#include <QScopedPointer>
namespace core {
class MeshTransformerResource;
}
namespace ctrl {
namespace ffd {
    class TaskResource;
}
} // namespace ctrl

namespace ctrl {

class DriverResources: public util::NonCopyable {
public:
    DriverResources();
    ~DriverResources();

    void grabMeshTransformerResoure(core::MeshTransformerResource* aResource);
    core::MeshTransformerResource* meshTransformerResource() const;

    void grabFFDTaskResource(ffd::TaskResource* aResource);
    ffd::TaskResource* ffdTaskResource() const;

private:
    QScopedPointer<core::MeshTransformerResource> mMeshTransformerResource;
    QScopedPointer<ffd::TaskResource> mFFDTaskResource;
};

} // namespace ctrl

#endif // DRIVERRESOURCES_H
