#include <fstream>
#include "qdir.h"
#include "util/IDSolver.h"
#include "ctrl/ProjectLoader.h"
#include "core/Deserializer.h"
#include <qstandardpaths.h>

namespace ctrl
{

ProjectLoader::ProjectLoader()
    : mLog()
    , mVersion()
{
}

bool ProjectLoader::load(
        const QString& aPath, core::Project& aProject,
        const gl::DeviceInfo& aGLDeviceInfo,
        util::IProgressReporter& aReporter)
{
    XC_DEBUG_REPORT() << "project path =" << aPath;
    std::ifstream file(aPath.toLocal8Bit(), /*std::ios::in |*/ std::ios::binary);

    if (file.fail())
    {
        QSettings settings;
        QStringList recentfiles = settings.value("projectloader/recents").toStringList();
        qInfo() << aPath;
        if (recentfiles.contains(aPath)){
            auto unavailableIndex = recentfiles.indexOf(aPath);
            recentfiles.removeAt(unavailableIndex);
            settings.setValue("projectloader/recents", recentfiles);
            qInfo() << recentfiles;
            mLog.push_back("Project removed, renamed or otherwise unavailable, please open it manually.\n"
                            "This path has been removed from your recents.");
        }

        mLog.push_back("Can not open the project file.");
        return false;
    }

    // max file size
    const size_t maxFileSize = (size_t)file.seekg(0, std::ios::end).tellg();
    file.seekg(0, std::ios::beg);

    // setup reporter
    int rShiftCount = 0;
    {
        size_t maxSize = maxFileSize;
        while (maxSize > (size_t)std::numeric_limits<int>::max())
        {
            maxSize >>= 1;
            ++rShiftCount;
        }
        aReporter.setSection("Loading the Project File...");
        aReporter.setMaximum((int)maxSize);
        aReporter.setProgress(0);
    }

    // setup reader
    util::LEStreamReader in(file);

    if (!readHeader(in))
    {
        mLog.push_back("Failed to read header.");
        return false;
    }

    if (!readGlobalBlock(in, aProject))
    {
        mLog.push_back("Failed to read global block.");
        return false;
    }

    core::Deserializer::IDSolverType idSolver;
    core::Deserializer deserializer(
                in, idSolver, maxFileSize, mVersion,
                aGLDeviceInfo, aReporter, rShiftCount);
    deserializer.reportCurrent();

    // resources block
    if (!aProject.resourceHolder().deserialize(deserializer))
    {
        QString log = "Failed to read resources block. : ";
        for (auto scope : deserializer.logScopes())
        {
            log += scope + "/";
        }
        mLog.push_back(log);

        for (auto deslog : deserializer.log())
        {
            mLog.push_back(deslog);
        }
        return false;
    }

    deserializer.reportCurrent();

    // object tree block
    if (!aProject.objectTree().deserialize(deserializer))
    {
        QString log = "Failed to read object tree block. : ";
        for (auto scope : deserializer.logScopes())
        {
            log += scope + "/";
        }
        mLog.push_back(log);

        for (auto deslog : deserializer.log())
        {
            mLog.push_back(deslog);
        }
        return false;
    }

    deserializer.reportCurrent();

    // solve id references
    if (!idSolver.solve())
    {
        mLog.push_back("Failed to solve id references.");
        return false;
    }

    mLog.push_back("Success.");
    return true;
}

bool ProjectLoader::readHeader(util::LEStreamReader& aIn)
{
    // signature
    const std::string signature = aIn.readString(6);
    if (signature != "ANIMFX") return false;

    // endian
    uint8 endian[2];
    aIn.readBuf(endian, 2);
    if (endian[0] != 0xff || endian[1] != 0x00) return false;

    // major version
    const int majorVersion = aIn.readUInt32();
    // minor version
    const int minorVersion = aIn.readUInt32();

    if (majorVersion < AE_PROJECT_FORMAT_OLDEST_MAJOR_VERSION ||
            (majorVersion == AE_PROJECT_FORMAT_OLDEST_MAJOR_VERSION &&
             minorVersion < AE_PROJECT_FORMAT_OLDEST_MINOR_VERSION))
    {
        mLog.push_back("The version of the file is too old to read it.");
        return false;
    }
    else if (majorVersion > AE_PROJECT_FORMAT_MAJOR_VERSION ||
             (majorVersion == AE_PROJECT_FORMAT_MAJOR_VERSION &&
              minorVersion > AE_PROJECT_FORMAT_MINOR_VERSION))
    {
        mLog.push_back("The file has a version defined later than this executable.");
        return false;
    }

    mVersion = QVersionNumber(majorVersion, minorVersion);

    // reserved
    if (!aIn.skipZeroArea(16)) return false;

    return !aIn.isFailed();
}

bool ProjectLoader::readGlobalBlock(util::LEStreamReader& aIn, core::Project& aProject)
{
    // signature
    const std::string signature = aIn.readString(4);
    if (signature != "GLBL") return false;

    // length
    const uint32 length = aIn.readUInt32();
    if (length != 64) return false;

    // width
    const uint32 width = aIn.readUInt32();
    // height
    const uint32 height = aIn.readUInt32();
    // max frame
    const uint32 maxFrame = aIn.readUInt32();
    // fps
    const uint32 fps = aIn.readUInt32();
    // loop
    const uint8 loop = aIn.readByte();

    // reserved
    if (!aIn.skipZeroArea(47)) return false;

    if (aIn.isFailed()) return false;

    // set values
    aProject.attribute().setImageSize(QSize((int)width, (int)height));
    aProject.attribute().setMaxFrame(maxFrame);
    aProject.attribute().setFps(fps);
    aProject.attribute().setLoop((bool)loop);
    return true;
}

} // namespace ctrl
