include(../common.pri)

TARGET = util
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ..

SOURCES += \
    CollDetect.cpp \
    MathUtil.cpp \
    NetworkUtil.cpp \
    TextUtil.cpp \
    TreePos.cpp \
    PackBits.cpp \
    TreeUtil.cpp \
    Triangle2D.cpp \
    IndexTable.cpp \
    Triangle2DPos.cpp \
    Dir4.cpp \
    Easing.cpp \
    TriangleRasterizer.cpp \
    ByteBuffer.cpp \
    EasingName.cpp

HEADERS += \
    NetworkUtil.h \
    Signaler.h \
    CollDetect.h \
    Segment2D.h \
    FixedObject.h \
    LifeLink.h \
    LinkPointer.h \
    MathUtil.h \
    TextUtil.h \
    TreeIterator.h \
    TreePos.h \
    TreeNodeBase.h \
    Circle.h \
    PlacePointer.h \
    NonCopyable.h \
    Easing.h \
    FergusonCoonsSpline.h \
    SlotId.h \
    StreamReader.h \
    StreamWriter.h \
    PackBits.h \
    Range.h \
    TreeUtil.h \
    Triangle2D.h \
    ArrayBlock.h \
    IndexTable.h \
    Dir4.h \
    IEasyIterator.h \
    ITreeSeeker.h \
    TreeSeekIterator.h \
    IDAssigner.h \
    IDSolver.h \
    Triangle2DPos.h \
    BinarySpacePartition2D.h \
    SelectArgs.h \
    IProgressReporter.h \
    Finally.h \
    TriangleRasterizer.h \
    DealtList.h \
    ByteBuffer.h \
    ArrayBuffer.h \
    EasingName.h
