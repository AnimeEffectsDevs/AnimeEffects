#ifndef IMG_RESOURCEDATA_H
#define IMG_RESOURCEDATA_H

#include "img/BlendMode.h"
#include "img/Buffer.h"
#include <QPoint>
#include <functional>
namespace img {
class ResourceNode;
}

namespace img {

class ResourceData {
public:
    typedef std::function<bool(ResourceData& aData)> ImageLoader;

    ResourceData(const QString& aIdentifier, const ResourceNode* aSerialAddress);
    virtual ~ResourceData() {}

    void grabImage(const XCMemBlock& aBlock, const QSize& aSize, Format aFormat);
    XCMemBlock releaseImage();
    void freeImage();

    void setIdentifier(const QString& aId) {
        mIdentifier = aId;
    }
    void setPos(const QPoint& aPos);
    void setUserData(void* aData) {
        mUserData = aData;
    }
    void setIsLayer(bool aIsLayer) {
        mIsLayer = aIsLayer;
    }
    void setBlendMode(BlendMode aMode);
    void copyFrom(const ResourceData& aData);

    bool isLayer() const {
        return mIsLayer;
    }
    bool hasImage() const {
        return mBuffer.data();
    }
    const QString& identifier() const {
        return mIdentifier;
    }
    const img::Buffer& image() const {
        return mBuffer;
    }
    const QPoint& pos() const {
        return mPos;
    }
    void* userData() const {
        return mUserData;
    }
    BlendMode blendMode() const {
        return mBlendMode;
    }
    const ResourceNode* serialAddress() const {
        return mSerialAddress;
    }

    void setImageLoader(const ImageLoader& aLoader) {
        mImageLoader = aLoader;
    }
    bool loadImage() {
        return (mImageLoader && mImageLoader(*this));
    }

    QRect rect() const;
    QVector2D center() const;

    bool hasSameLayerDataWith(const ResourceData& aData); // it's heavy

private:
    img::Buffer mBuffer;
    QPoint mPos;
    void* mUserData;
    bool mIsLayer;
    QString mIdentifier;
    BlendMode mBlendMode;
    ImageLoader mImageLoader;
    const ResourceNode* mSerialAddress;
};

} // namespace img

#endif // IMG_RESOURCEDATA_H
