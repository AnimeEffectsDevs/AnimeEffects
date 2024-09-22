#include <QFile>
#include "gl/Global.h"
#include "gl/ExtendShader.h"
#include "ctrl/ffd/ffd_TaskResource.h"
#include "battery/embed.hpp"

namespace ctrl {
namespace ffd {

    //-------------------------------------------------------------------------------------------------
    TaskResource::TaskResource() {}

    void TaskResource::setup() {
        // load brush shader
        {
            auto tmp = b::embed<"../../data/shader/FreeFormDeformVert.glsl">();
            auto code = QString::fromUtf8(tmp.data(), tmp.size());

            for (int hard = 0; hard < kHardness; ++hard) {
                buildShader(mProgram[kTypeDeformer * kHardness + hard], code, kTypeDeformer, hard);
            }
        }

        // load eraser shader
        {
            auto tmp = b::embed<"../../data/shader/FFDEraseVert.glsl">();
            auto code = QString::fromUtf8(tmp.data(), tmp.size());

            for (int hard = 0; hard < kHardness; ++hard) {
                buildShader(mProgram[kTypeEraser * kHardness + hard], code, kTypeEraser, hard);
            }
        }

        // load focuser shader
        {
            auto tmp = b::embed<"../../data/shader/FFDFocusVertexVert.glsl">();
            auto code = QString::fromUtf8(tmp.data(), tmp.size());

            buildShader(mProgram[kTypeFocuser * kHardness], code, kTypeFocuser, 0);
        }

        // load blur path
        {
            auto tmp = b::embed<"../../data/shader/FFDBlurVert.glsl">();
            auto code = QString::fromUtf8(tmp.data(), tmp.size());

            buildBlurShader(mBlurProgram, code);
        }
    }

    gl::EasyShaderProgram& TaskResource::program(int aType, int aHard) {
        switch (aType) {
        case kTypeDeformer:
            return mProgram[kTypeDeformer * kHardness + aHard];
        case kTypeEraser:
            return mProgram[kTypeEraser * kHardness + aHard];
        case kTypeFocuser:
            return mProgram[kTypeFocuser * kHardness + 0];
        default:
            XC_ASSERT(0);
            return mProgram[0];
        }
    }

    const gl::EasyShaderProgram& TaskResource::program(int aType, int aHard) const {
        switch (aType) {
        case kTypeDeformer:
            return mProgram[kTypeDeformer * kHardness + aHard];
        case kTypeEraser:
            return mProgram[kTypeEraser * kHardness + aHard];
        case kTypeFocuser:
            return mProgram[kTypeFocuser * kHardness + 0];
        default:
            XC_ASSERT(0);
            return mProgram[0];
        }
    }

    void TaskResource::loadFile(const QString& aPath, QString& aDstCode) {
        QFile file(aPath);

        if (!file.open(QIODevice::ReadOnly)) {
            XC_FATAL_ERROR("FileIO Error", file.errorString(), aPath);
        }
        QTextStream in(&file);
        aDstCode = in.readAll();
    }

    void TaskResource::buildShader(gl::EasyShaderProgram& aProgram, const QString& aCode, int aType, int aHard) {
        gl::Global::Functions& ggl = gl::Global::functions();

        gl::ExtendShader source;

        // parse shader source
        source.openFromTextVert(aCode);

        // set variation
        if (aType != kTypeFocuser) {
            source.setVariationValue("HARDNESS", QString::number(aHard));
        }

        // resolve variation
        if (!source.resolveVariation()) {
            XC_FATAL_ERROR("OpenGL Error", "Failed to resolve shader variation.", source.log());
        }

        // set shader source
        if (!aProgram.setVertexSource(source)) {
            XC_FATAL_ERROR("OpenGL Error", "Failed to compile vertex shader.", aProgram.log());
        }

        // feedback
        if (aType == kTypeDeformer) {
            static const GLchar* kVaryings[] = {"outPosition", "outWeight"};
            ggl.glTransformFeedbackVaryings(aProgram.id(), 2, kVaryings, GL_SEPARATE_ATTRIBS);
        } else if (aType == kTypeEraser) {
            static const GLchar* kVaryings[] = {"outPosition"};
            ggl.glTransformFeedbackVaryings(aProgram.id(), 1, kVaryings, GL_SEPARATE_ATTRIBS);
        } else if (aType == kTypeFocuser) {
            static const GLchar* kVaryings[] = {"outPosition", "outWeight"};
            ggl.glTransformFeedbackVaryings(aProgram.id(), 2, kVaryings, GL_SEPARATE_ATTRIBS);
        }

        // link shader
        if (!aProgram.link()) {
            XC_FATAL_ERROR("OpenGL Error", "Failed to link shader.", aProgram.log());
        }
        XC_ASSERT(ggl.glGetError() == GL_NO_ERROR);
    }

    void TaskResource::buildBlurShader(gl::EasyShaderProgram& aProgram, const QString& aCode) {
        gl::Global::Functions& ggl = gl::Global::functions();

        gl::ExtendShader source;

        // parse shader source
        source.openFromTextVert(aCode);

        // resolve variation
        if (!source.resolveVariation()) {
            XC_FATAL_ERROR("OpenGL Error", "Failed to resolve shader variation.", source.log());
        }

        // set shader source
        if (!aProgram.setVertexSource(source)) {
            XC_FATAL_ERROR("OpenGL Error", "Failed to compile vertex shader.", aProgram.log());
        }

        // feedback
        static const GLchar* kVaryings[] = {"outPosition"};
        ggl.glTransformFeedbackVaryings(aProgram.id(), 1, kVaryings, GL_SEPARATE_ATTRIBS);

        // link shader
        if (!aProgram.link()) {
            XC_FATAL_ERROR("OpenGL Error", "Failed to link shader.", aProgram.log());
        }
        XC_ASSERT(ggl.glGetError() == GL_NO_ERROR);
    }

} // namespace ffd
} // namespace ctrl
