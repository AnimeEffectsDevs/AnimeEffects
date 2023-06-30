#ifndef GL_VERTEXARRAYOBJECT_H
#define GL_VERTEXARRAYOBJECT_H

#include "util/NonCopyable.h"
#include <QGL>

namespace gl {

class VertexArrayObject: private util::NonCopyable {
public:
    VertexArrayObject();
    ~VertexArrayObject();

    GLuint id() const {
        return mId;
    }

    void bind();
    void release();

private:
    GLuint mId;
};

} // namespace gl

#endif // GL_VERTEXARRAYOBJECT_H
