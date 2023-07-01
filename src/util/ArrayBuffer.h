#ifndef UTIL_ARRAYBUFFER_H
#define UTIL_ARRAYBUFFER_H

#include <utility>
#include "XC.h"

namespace util {

template<typename tObject> class ArrayBuffer {
    tObject* mObjects;
    int mCount;

public:
    ArrayBuffer(): mObjects(), mCount() {}

    ArrayBuffer(tObject* aObjects, int aCount): mObjects(), mCount() {
        reset(aObjects, aCount);
    }

    ArrayBuffer(const ArrayBuffer<tObject>& aRhs): mObjects(), mCount(aRhs.mCount) {
        if (mCount > 0) {
            mObjects = new tObject[mCount];
            for (int i = 0; i < mCount; ++i) {
                mObjects[i] = aRhs.mObjects[i];
            }
        }
    }

    ArrayBuffer& operator=(const ArrayBuffer<tObject>& aRhs) {
        reset();
        mCount = aRhs.mCount;
        if (mCount > 0) {
            mObjects = new tObject[mCount];
            for (int i = 0; i < mCount; ++i) {
                mObjects[i] = aRhs.mObjects[i];
            }
        }
        return *this;
    }

    virtual ~ArrayBuffer() {
        reset();
    }

    void reset() {
        if (mObjects) {
            delete[] mObjects;
            mObjects = nullptr;
            mCount = 0;
        }
    }

    void reset(tObject* aObjects, int aCount) {
        XC_ASSERT((aObjects && aCount > 0) || (!aObjects && aCount == 0));
        reset();
        mObjects = aObjects;
        mCount = aCount;
    }

    void construct(int aCount) {
        reset();
        if (aCount > 0) {
            reset(new tObject[aCount], aCount);
        }
    }

    void swap(ArrayBuffer<tObject>& aRhs) {
        std::swap(mObjects, aRhs.mObjects);
        std::swap(mCount, aRhs.mCount);
    }

    explicit operator bool() const {
        return mObjects;
    }

    tObject* data() {
        return mObjects;
    }

    const tObject* data() const {
        return mObjects;
    }

    int count() const {
        return mCount;
    }

    tObject& operator[](int aIndex) {
        return mObjects[aIndex];
    }

    const tObject& operator[](int aIndex) const {
        return mObjects[aIndex];
    }

    tObject& at(int aIndex) {
        XC_PTR_ASSERT(mObjects);
        XC_ASSERT(0 <= aIndex && aIndex < mCount);
        return mObjects[aIndex];
    }

    const tObject& at(int aIndex) const {
        XC_PTR_ASSERT(mObjects);
        XC_ASSERT(0 <= aIndex && aIndex < mCount);
        return mObjects[aIndex];
    }
};


} // namespace util

#endif // UTIL_ARRAYBUFFER_H
