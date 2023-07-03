#ifndef GL_PRIMITIVEDRAWER_H
#define GL_PRIMITIVEDRAWER_H

#include <vector>
#include <functional>
#include <QPolygonF>
#include <QBrush>
#include <QPen>
#include "gl/BufferObject.h"
#include "gl/EasyShaderProgram.h"
#include "gl/Texture.h"

namespace gl {

class PrimitiveDrawer {
public:
    enum PenStyle { PenStyle_Solid, PenStyle_Dash, PenStyle_Dot, Style_TERM };

    PrimitiveDrawer(int aVtxCountOfSlot = 512, int aSlotCount = 8);
    virtual ~PrimitiveDrawer();

    void setViewMatrix(const QMatrix4x4& aViewMtx);

    void begin();
    void end();

    void setBrush(const QColor& aColor);
    void setPen(const QColor& aColor, float aWidth = 1.0f, PenStyle = PenStyle_Solid);
    void setBrushEnable(bool aIsEnable);
    void setPenEnable(bool aIsEnable);
    void setAntiAliasing(bool aIsEnable);

    void drawPoint(const QPointF& aCenter);

    void drawLine(const QPointF& aFrom, const QPointF& aTo);
    void drawLine(const QLineF& aLine) { drawLine(aLine.p1(), aLine.p2()); }

    void drawRect(const QRect& aRect);
    void drawRect(const QRectF& aRect);

    void drawCircle(const QPointF& aCenter, float aRadius);
    void drawEllipse(const QPointF& aCenter, float aRadiusX, float aRadiusY);

    void drawPolyline(const QPoint* aPoints, int aCount);
    void drawPolyline(const QPointF* aPoints, int aCount);

    void drawConvexPolygon(const QPoint* aPoints, int aCount);
    void drawConvexPolygon(const QPointF* aPoints, int aCount);

    void drawPolygon(const QPoint* aPoints, int aCount);
    void drawPolygon(const QPointF* aPoints, int aCount);
    void drawPolygon(const QPolygonF& aPolygon);

    void drawTexture(const QRectF& aRect, gl::Texture& aTexture);
    void drawTexture(const QRectF& aRect, gl::Texture& aTexture, const QRectF& aSrcRect);
    void drawTexture(const QRectF& aRect, GLuint aTexture);
    void drawTexture(const QRectF& aRect, GLuint aTexture, const QSize& aTexSize, const QRectF& aSrcRect);

private:
    enum Type { Type_Draw, Type_Brush, Type_Pen, Type_Texture, Type_Ability, Type_TERM };

    enum ShaderType { ShaderType_Plane, ShaderType_Stipple, ShaderType_Texture, ShaderType_TERM };

    struct Command {
        Type type;
        union Attribute {
            struct Draw {
                GLenum prim;
                int count;
                bool usePen;
            } draw;

            struct Brush {
                QRgb color;
            } brush;

            struct Pen {
                QRgb color;
                float width;
                PenStyle style;
            } pen;

            struct Texture {
                GLuint id;
                QRgb color;
            } texture;

            struct Ability {
                bool hasBrush;
                bool hasPen;
                bool hasMSAA;
            } ability;

        } attr;
    };

    struct State {
        State();
        void set(const Command& aCommand);
        bool hasDifferentValueWith(const Command& aCommand) const;
        bool operator==(const State& aRhs) const;
        bool operator!=(const State& aRhs) const;
        QRgb brushColor;
        QRgb penColor;
        float penWidth;
        PenStyle penStyle;
        GLuint texture;
        QRgb textureColor;
        bool hasBrush;
        bool hasPen;
        bool hasMSAA;
    };

    struct PlaneShader {
        bool init();
        gl::EasyShaderProgram program;
        int locPosition;
        int locViewMtx;
        int locColor;
    };

    struct StippleShader {
        bool init();
        gl::EasyShaderProgram program;
        int locPosition;
        int locLength;
        int locViewMtx;
        int locScreenSize;
        int locColor;
        int locWave;
    };

    struct TextureShader {
        bool init();
        gl::EasyShaderProgram program;
        int locPosition;
        int locTexCoord;
        int locViewMtx;
        int locColor;
        int locTexture;
    };

    void drawConvexPolygonImpl(const std::function<QPointF(int)>& aGetPos, int aCount);
    void drawOutline(const std::function<QPointF(int)>& aGetPos, int aCount, bool aForce = false);
    void drawPolygonImpl(const QVector<gl::Vector2>& aTriangles);
    void drawEllipseImpl(const QPointF& aCenter, float aRadiusX, float aRadiusY, int aDivision);

    void pushStateCommand(const Command& aCommand);
    void
    pushDrawCommand(const Command& aCommand, const gl::Vector2* aPositions, const gl::Vector2* aSubCoords = nullptr);
    void flushCommands();

    void bindAppositeShader(int aSlotIndex);
    void setColorToCurrentShader(bool aUsePen);
    void unbindCurrentShader();

    PlaneShader mPlaneShader;
    StippleShader mStippleShader;
    TextureShader mTextureShader;
    ShaderType mCurrentShader;

    int mVtxCountOfSlot;
    int mSlotCount;
    std::vector<GLuint> mPosSlotIds;
    std::vector<GLuint> mSubSlotIds;
    int mCurrentSlotIndex;
    int mCurrentSlotSize;

    QMatrix4x4 mViewMtx;
    QSize mScreenSize;
    float mPixelScale;

    QVector<Command> mScheduledCommands;
    bool mInDrawing;
    State mAppliedState;
    State mScheduledState;

    QVector<gl::Vector2> mPosBuffer;
    QVector<gl::Vector2> mSubBuffer;
};

} // namespace gl

#endif // GL_PRIMITIVEDRAWER_H
