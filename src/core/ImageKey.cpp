#include "core/ImageKey.h"
#include "Project.h"
#include "core/Constant.h"
#include "img/BlendMode.h"
#include "img/ResourceNode.h"
#include "qjsonobject.h"
#include "util/LinkPointer.h"

namespace core {

//-------------------------------------------------------------------------------------------------
ImageKey::Data::Data(): mEasing(), mResHandle(), mBlendMode(img::BlendMode_Normal), mImageOffset(), mGridMesh() {
}

ImageKey::Data::Data(const Data& aRhs):
    mEasing(aRhs.mEasing), mResHandle(aRhs.mResHandle), mBlendMode(aRhs.mBlendMode), mImageOffset() {
    setImageOffset(aRhs.mImageOffset);
    mGridMesh = aRhs.mGridMesh;
}

ImageKey::Data& ImageKey::Data::operator=(const Data& aRhs) {
    mEasing = aRhs.mEasing;
    mResHandle = aRhs.mResHandle;
    mBlendMode = aRhs.mBlendMode;
    setImageOffset(aRhs.mImageOffset);
    mGridMesh = aRhs.mGridMesh;
    return *this;
}

void ImageKey::Data::setImageOffset(const QVector2D& aOffset) {
    const QVector2D offset(xc_clamp(aOffset.x(), Constant::transMin(), Constant::transMax()),
        xc_clamp(aOffset.y(), Constant::transMin(), Constant::transMax()));
    mImageOffset = offset;
    mGridMesh.setOriginOffset(offset);
}

//-------------------------------------------------------------------------------------------------
ImageKey::Cache::Cache(): mTexture() {
}

//-------------------------------------------------------------------------------------------------
ImageKey::ImageKey(): mData(), mCache(), mSleepCount(0) {
    mData.resource().setOriginKeeping(true);
}

TimeKey* ImageKey::createClone() {
    auto newKey = new ImageKey();
    newKey->mData = this->mData;
    newKey->resetTextureCache();
    return newKey;
}

void ImageKey::setImage(const img::ResourceHandle& aResource, img::BlendMode aMode) {
    mData.setBlendMode(aMode);
    setImage(aResource);
}

void ImageKey::setImage(const img::ResourceHandle& aResource) {
    mData.resource() = aResource;
    resetTextureCache();
}

void ImageKey::setImageOffset(const QVector2D& aOffset) {
    mData.setImageOffset(aOffset);
}

void ImageKey::setImageOffsetByCenter() {
    if (hasImage()) {
        auto size = mData.resource()->image().pixelSize();
        mData.setImageOffset(QVector2D(-size.width() * 0.5f, -size.height() * 0.5f));
    }
}

void ImageKey::resetGridMesh(int aCellSize) {
    if (mData.resource()->hasImage()) {
        auto data = mData.resource()->image().data();
        auto size = mData.resource()->image().pixelSize();

        // const int cellPx = std::max(std::min(8, pixelSize.width() / 4), 2);
        auto cell = std::max(aCellSize, Constant::imageCellSizeMin());
        while (img::GridMeshCreator::getCellTableCount(size, cell) > Constant::imageCellCountMax()) {
            ++cell;
        }
        cell = std::min(cell, Constant::imageCellSizeMax());
        mData.gridMesh().createFromImage(data, size, cell);
    }
}

void ImageKey::resetTextureCache() {
    if (mData.resource()->hasImage()) {
        auto imageData = mData.resource()->image().data();
        auto pixelSize = mData.resource()->image().pixelSize();

        // make a gl texture
        mCache.texture().create(pixelSize, imageData);
        mCache.texture().setFilter(GL_LINEAR);
        mCache.texture().setWrap(GL_CLAMP_TO_BORDER, QColor(0, 0, 0, 0));
    }
}

void ImageKey::sleep() {
    ++mSleepCount;
    if (mSleepCount == 1) {
        mData.resource().setOriginKeeping(false);
    }
}

void ImageKey::awake() {
    XC_ASSERT(mSleepCount > 0);
    --mSleepCount;
    if (mSleepCount == 0) {
        mData.resource().setOriginKeeping(true);
    }
}

QJsonObject ImageKey::serializeToJson() const {
    QJsonObject image;
    image["Identifier"] = mData.resource()->serialAddress()->handle()->identifier();
    image["BlendMode"] = getBlendNameFromBlendMode(mData.blendMode());
    image["OffsetX"] = mData.imageOffset().x();
    image["OffsetY"] = mData.imageOffset().y();
    image["GridMesh"] = mData.gridMesh().serializeToJson();
    return image;
}

img::ResourceNode* returnMatch(img::ResourceNode* image, QString match) {
    // Get main
    if (image->data().identifier() == match) {
        return image;
    }
    // Get siblings
    img::ResourceNode* current = image;
    while (current->nextSib()) {
        if (current->data().identifier() == match) {
            return current;
        }
        if (!current->children().empty()) {
            for (auto child : current->children()) {
                auto recMatch = returnMatch(child, match);
                if (recMatch != nullptr) {
                    return recMatch;
                };
            }
        }
        current = current->nextSib();
    }
    // Get last sibling
    if (current->data().identifier() == match) {
        return current;
    }
    if (!current->children().empty()) {
        for (auto child : current->children()) {
            auto recMatch = returnMatch(child, match);
            if (recMatch != nullptr) {
                return recMatch;
            };
        }
    }
    // Get children
    for (auto child : image->children()) {
        current = child;
        while (current->nextSib()) {
            if (current->data().identifier() == match) {
                return current;
            }
            if (!current->children().empty()) {
                for (auto child : current->children()) {
                    auto recMatch = returnMatch(child, match);
                    if (recMatch != nullptr) {
                        return recMatch;
                    };
                }
            }
            current = current->nextSib();
        }
    }
    // Get last child
    if (current->data().identifier() == match) {
        return current;
    }
    if (!current->children().empty()) {
        for (auto child : current->children()) {
            auto recMatch = returnMatch(child, match);
            if (recMatch != nullptr) {
                return recMatch;
            };
        }
    }
    return nullptr;
}

bool ImageKey::deserializeFromJson(QJsonObject json, util::LifeLink::Pointee<Project> project) {
    mData.resource().reset();
    // image id
    json = json["Image"].toObject();
    QString identifier = json["Identifier"].toString();
    img::ResourceNode* nodePtr = nullptr;
    for (const ResourceHolder::ImageTree& image : project.address->resourceHolder().imageTrees()) {
        auto match = returnMatch(image.topNode, identifier);
        // qDebug("-------");
        if (match != nullptr) {
            // qDebug() << match->data().identifier();
            nodePtr = match;
        }
        // qDebug("-------");
    }
    if (nodePtr == nullptr) {
        return false;
    }
    this->mData.resource() = nodePtr->handle();
    this->resetTextureCache();

    // image offset
    QVector2D offset{static_cast<float>(json["OffsetX"].toDouble()), static_cast<float>(json["OffsetY"].toDouble())};
    mData.setImageOffset(offset);
    // grid mesh
    if (!mData.gridMesh().deserializeFromJson(json)) {
        return false;
    }
    // blend mode
    QString bname = json["BlendMode"].toString();
    auto bmode = img::getBlendModeFromQuadId(bname);
    if (!(bmode == img::BlendMode_TERM)) {
        mData.setBlendMode(bmode);
    }
    return true;
}

bool ImageKey::serialize(Serializer& aOut) const {
    // easing
    aOut.write(mData.easing());
    // image id
    aOut.writeID(mData.resource()->serialAddress());
    // blend mode
    aOut.writeFixedString(img::getQuadIdFromBlendMode(mData.blendMode()), 4);
    // image offset
    aOut.write(mData.imageOffset());
    // grid mesh
    if (!mData.gridMesh().serialize(aOut)) {
        return false;
    }

    return aOut.checkStream();
}

bool ImageKey::deserialize(Deserializer& aIn) {
    mData.resource().reset();

    aIn.pushLogScope("ImageKey");

    // easing
    if (!aIn.read(mData.easing())) {
        return aIn.errored("invalid easing param");
    }
    // image id
    {
        auto solver = [=](void* aPtr) {
            this->mData.resource() = ((img::ResourceNode*)aPtr)->handle();
            this->resetTextureCache();
        };
        if (!aIn.orderIDData(solver)) {
            return aIn.errored("invalid image reference id");
        }
    }
    // blend mode
    {
        QString bname;
        aIn.readFixedString(bname, 4);
        auto bmode = img::getBlendModeFromQuadId(bname);
        if (bmode == img::BlendMode_TERM) {
            return aIn.errored("invalid image blending mode");
        }
        mData.setBlendMode(bmode);
    }
    // image offset
    {
        QVector2D offset;
        aIn.read(offset);
        mData.setImageOffset(offset);
    }
    // grid mesh
    if (!mData.gridMesh().deserialize(aIn)) {
        return aIn.errored("failed to deserialize grid mesh");
    }

    aIn.popLogScope();
    return aIn.checkStream();
}

} // namespace core
