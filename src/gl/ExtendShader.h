#ifndef GL_EXTENDSHADER_H
#define GL_EXTENDSHADER_H

#include <QString>
#include <vector>

namespace gl {

class ExtendShader {
public:
    ExtendShader();
    bool openFromFile(const QString& aFilePath, QString& originalCode);
    bool openFromFileVert(const QString& aFilePath);
    bool openFromFileFrag(const QString& aFilePath);

    void openFromText(const QString& aText, QString& originalCode);
    void openFromTextVert(const QString& aText);
    void openFromTextFrag(const QString& aText);

    void setVariationValue(const QString& aName, const QString& aValue);
    bool resolveVariation();

    const QString& vertexCode() const {
        return mVertexCode;
    }
    const QString& fragmentCode() const {
        return mFragmentCode;
    }

    const QString& log() const {
        return mLog;
    }

private:
    struct VariationUnit {
        QString name;
        QString value;
    };
    QString mOriginalCodeVert;
    QString mOriginalCodeFrag;
    QString mVertexCode;
    QString mFragmentCode;
    std::vector<VariationUnit> mVariation;
    QString mLog;
};

} // namespace gl

#endif // GL_EXTENDSHADER_H
