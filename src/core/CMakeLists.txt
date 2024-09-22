project(core)

add_library(${PROJECT_NAME}
    ../XC.h
    ../XCAssert.h
    ../XCReport.h
    ../XCType.h
    AbstractCursor.cpp AbstractCursor.h
    Animator.h
    Bone2.cpp Bone2.h
    BoneExpans.cpp BoneExpans.h
    BoneInfluenceMap.cpp BoneInfluenceMap.h
    BoneKey.cpp BoneKey.h
    BoneKeyUpdater.cpp BoneKeyUpdater.h
    BoneShape.cpp BoneShape.h
    CameraInfo.cpp CameraInfo.h
    ClippingFrame.cpp ClippingFrame.h
    Constant.h
    DepthKey.cpp DepthKey.h
    Deserializer.cpp Deserializer.h
    DestinationTexturizer.cpp DestinationTexturizer.h
    FFDKey.cpp FFDKey.h
    FFDKeyUpdater.cpp FFDKeyUpdater.h
    FolderNode.cpp FolderNode.h
    Frame.h
    GridMesh.cpp GridMesh.h
    HSVKey.cpp
    HeightMap.cpp HeightMap.h
    HsvKey.h
    ImageKey.cpp ImageKey.h
    ImageKeyUpdater.cpp ImageKeyUpdater.h
    LayerMesh.cpp LayerMesh.h
    LayerNode.cpp LayerNode.h
    MeshKey.cpp MeshKey.h
    MeshKeyUtil.cpp MeshKeyUtil.h
    MeshTransformer.cpp MeshTransformer.h
    MeshTransformerResource.cpp MeshTransformerResource.h
    MoveKey.cpp MoveKey.h
    ObjectNode.h
    ObjectNodeUtil.cpp ObjectNodeUtil.h
    ObjectTree.cpp ObjectTree.h
    ObjectTreeEvent.cpp ObjectTreeEvent.h
    ObjectTreeNotifier.cpp ObjectTreeNotifier.h
    ObjectType.h
    OpaKey.cpp OpaKey.h
    PoseKey.cpp PoseKey.h
    PosePalette.cpp PosePalette.h
    Project.cpp Project.h
    ProjectEvent.cpp ProjectEvent.h
    RenderInfo.h
    Renderer.h
    ResourceEvent.cpp ResourceEvent.h
    ResourceHolder.cpp ResourceHolder.h
    ResourceUpdatingWorkspace.cpp ResourceUpdatingWorkspace.h
    RotateKey.cpp RotateKey.h
    SRTExpans.cpp SRTExpans.h
    ScaleKey.cpp ScaleKey.h
    Serializer.cpp Serializer.h
    ShaderHolder.cpp ShaderHolder.h
    TimeCacheAccessor.cpp TimeCacheAccessor.h
    TimeCacheLock.cpp TimeCacheLock.h
    TimeFormat.cpp TimeFormat.h
    TimeInfo.h
    TimeKey.cpp TimeKey.h
    TimeKeyBlender.cpp TimeKeyBlender.h
    TimeKeyExpans.cpp TimeKeyExpans.h
    TimeKeyGatherer.cpp TimeKeyGatherer.h
    TimeKeyPos.h
    TimeKeyType.h
    TimeLine.cpp TimeLine.h
    TimeLineEvent.cpp TimeLineEvent.h
)

set(SHADERS_DIR ../../data/shader)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/ClipperWritingFrag.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/ClipperWritingVert.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/GridDrawingFrag.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/GridDrawingVert.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/HSVAdjustFrag.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/HSVAdjustVert.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/LayerDrawingFrag.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/LayerDrawingVert.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/MeshTransformVert.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/PartialScreenCopyingFrag.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/PartialScreenCopyingVert.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/TestBlurVert.glsl)

target_include_directories(${PROJECT_NAME} PUBLIC
    ..
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    AE_MAJOR_VERSION=1
    AE_MICRO_VERSION=0
    AE_MINOR_VERSION=6
    AE_PROJECT_FORMAT_MAJOR_VERSION=0
    AE_PROJECT_FORMAT_MINOR_VERSION=7
    AE_PROJECT_FORMAT_OLDEST_MAJOR_VERSION=0
    AE_PROJECT_FORMAT_OLDEST_MINOR_VERSION=4
    USE_GL_CORE_PROFILE
    SHADERS_DIR="${SHADERS_DIR}"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::OpenGL
    Qt::Xml
    cmnd
    gl
    img
    thr
    util
)
