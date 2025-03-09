#include "util/MathUtil.h"
#include "util/Circle.h"
#include "util/CollDetect.h"
#include "core/Constant.h"
#include "ctrl/srt/srt_Symbol.h"

namespace {
static const float kSymbolSize = 100.0f;
static const float kScaleEdgeRange = 6.0f;
static const float kTransRange = 5.0f;
static const float kRotCircleRange = 9.0f;
static const float kRotCircleDrawRange = 5.0f;
} // namespace

using namespace core;

namespace ctrl {
namespace srt {

    //-----------------------------------------------------------------------------------
    float getSymbolScale(const core::CameraInfo& aCamera) {
        auto scrSize = aCamera.screenSize();
        auto symScale = xc_clamp((scrSize.width() + scrSize.height()) / 2500.0f, 0.01f, 100.0f);
        return symScale / aCamera.scale();
    }

    //-----------------------------------------------------------------------------------
    Symbol::Symbol() {}

    void Symbol::build(const QMatrix4x4& aLocalMtx, const QMatrix4x4& aWorldMtx, const CameraInfo& aCamera) {
        using util::MathUtil;

        const float scale = getSymbolScale(aCamera);
        const QSizeF size(kSymbolSize * scale, kSymbolSize * scale);
        const QMatrix4x4 matrix = aWorldMtx * aLocalMtx;

        for (int i = 0; i < 4; ++i) {
            const float sx = (i == 1 || i == 2) ? 1 : -1;
            const float sy = (i == 2 || i == 3) ? 1 : -1;
            const float x = sx * size.width();
            const float y = sy * size.height();
            p[i] = aCamera.toScreenPos(matrix * QPointF(x, y));
        }
        c = (p[0] + p[1] + p[2] + p[3]) / 4.0f;


        for (int i = 0; i < 4; ++i) {
            v[i] = (p[i] + p[(i + 1) % 4]) * 0.5f - c;
            ev[i] = p[i] - c;
        }

        for (int i = 0; i < 4; ++i) {
            if (QVector2D(v[i]).length() < Constant::normalizable()) {
                v[i] = -MathUtil::getRotateVector90Deg(QVector2D(v[(i + 1) % 4])).toPointF();

                if (QVector2D(v[i]).length() < Constant::normalizable()) {
                    v[i] = QPointF(1.0f, 0.0f);
                }
            }
        }

        for (int i = 0; i < 4; ++i) {
            if (QVector2D(ev[i]).length() < Constant::normalizable()) {
                ev[i] = -MathUtil::getRotateVector90Deg(QVector2D(ev[(i + 1) % 4])).toPointF();

                if (QVector2D(ev[i]).length() < Constant::normalizable()) {
                    ev[i] = QPointF(1.0f, 0.0f);
                }
            }
        }
    }

    Symbol::FocusData Symbol::findFocus(const QVector2D& aWorldPos) {
        using util::CollDetect;

        // rotate
        if (util::Circle(p[2], kRotCircleRange).isInside(aWorldPos)) {
            return FocusData(FocusType_Rotate, QVector2D(ev[2]).normalized());
        }

        // trans
        if (util::Circle(c, kTransRange).isInside(aWorldPos)) {
            return FocusData(FocusType_Trans, aWorldPos - QVector2D(c));
        }

        // scale
        {
            const float edge[2] = {0.5f * QVector2D(p[0] - p[1]).length(), 0.5f * QVector2D(p[1] - p[2]).length()};
            const float corner = 0.5f * (edge[0] <= edge[1] ? edge[0] : edge[1]);
            bool onCorner = false;
            QVector2D cornerV;

            for (int i = 0; i < 4; ++i) {
                if (util::Circle(p[i], corner).isInside(aWorldPos)) {
                    onCorner = true;
                    cornerV = QVector2D(ev[i]).normalized();
                    break;
                }
            }

            for (int i = 0; i < 4; ++i) {
                const int id = i % 2;

                util::Segment2D edge(QVector2D(p[i]), QVector2D(p[(i + 1) % 4] - p[i]));

                if (edge.dir.length() < Constant::normalizable()) {
                    continue;
                }

                if (CollDetect::getMinDistanceSquared(edge, aWorldPos) < kScaleEdgeRange * kScaleEdgeRange) {
                    if (onCorner) {
                        return FocusData(FocusType_Scale, cornerV);
                    } else {
                        return FocusData((id == 0) ? FocusType_ScaleY : FocusType_ScaleX, QVector2D(v[i]).normalized());
                    }
                }
            }
        }

        // sub trans
        if (CollDetect::isInside(p, 4, aWorldPos.toPointF())) {
            return FocusData(FocusType_Trans, aWorldPos - QVector2D(c));
        }

        return FocusData(FocusType_TERM, QVector2D());
    }

    void Symbol::draw(const RenderInfo& aInfo, QPainter& aPainter, FocusType aFocus, bool grayOut = false) const {
        (void)aInfo;

        QColor idleColor(100, 100, 255, 255);
        QColor focusColor(255, 255, 255, 255);

        if (grayOut) {
            idleColor = Qt::darkGray;
            focusColor = Qt::white;
        }

        const QBrush centerBrush(aFocus == FocusType_Trans ? focusColor : idleColor);
        const QBrush edgeXBrush((aFocus == FocusType_Scale || aFocus == FocusType_ScaleX) ? focusColor : idleColor);
        const QBrush edgeYBrush((aFocus == FocusType_Scale || aFocus == FocusType_ScaleY) ? focusColor : idleColor);
        const QBrush rotateBrush(aFocus == FocusType_Rotate ? focusColor : idleColor);

        // center cross
        {
            aPainter.setPen(QPen(centerBrush, 1.5f, Qt::SolidLine));
            aPainter.setBrush(centerBrush);
            aPainter.drawEllipse(c, kTransRange, kTransRange);
        }

        // scale rect
        aPainter.setPen(QPen(edgeXBrush, 1.5f, Qt::DashLine));
        aPainter.drawLine(p[1], p[2]);
        aPainter.drawLine(p[3], p[0]);
        aPainter.setPen(QPen(edgeYBrush, 1.5f, Qt::DashLine));
        aPainter.drawLine(p[0], p[1]);
        aPainter.drawLine(p[2], p[3]);

        // rotate handle
        {
            aPainter.setPen(QPen(rotateBrush, 1.5f, Qt::SolidLine));
            aPainter.setBrush(rotateBrush);
            aPainter.drawEllipse(p[2], kRotCircleDrawRange, kRotCircleDrawRange);
        }
    }

} // namespace srt
} // namespace ctrl
