project(ctrl)

add_library(${PROJECT_NAME}
    ../XC.h
    ../XCAssert.h
    ../XCReport.h
    ../XCType.h
    BoneEditMode.h
    BoneEditor.cpp BoneEditor.h
    BoneParam.h
    CmndName.cpp CmndName.h
    Driver.cpp Driver.h
    DriverResources.cpp DriverResources.h
    Exporter.cpp Exporter.h
    FFDEditor.cpp FFDEditor.h
    FFDParam.h
    GraphicStyle.h
    IEditor.h
    ImageFileLoader.cpp ImageFileLoader.h
    KeyBinding.cpp KeyBinding.h
    MeshEditor.cpp MeshEditor.h
    MeshParam.h
    Painter.cpp Painter.h
    PoseEditMode.h
    PoseEditor.cpp PoseEditor.h
    PoseParam.cpp PoseParam.h
    ProjectLoader.cpp ProjectLoader.h
    ProjectSaver.cpp ProjectSaver.h
    SRTEditor.cpp SRTEditor.h
    SRTParam.h
    ScopedModifier.h
    System.cpp System.h
    TimeLineEditor.cpp TimeLineEditor.h
    TimeLineRow.cpp TimeLineRow.h
    TimeLineUtil.cpp TimeLineUtil.h
    ToolType.h
    UILog.h
    UILogType.h
    UILogger.h
    VideoFormat.cpp VideoFormat.h
    bone/bone_AssignInfluence.h
    bone/bone_BindNodesMode.cpp bone/bone_BindNodesMode.h
    bone/bone_CreateMode.cpp bone/bone_CreateMode.h
    bone/bone_DeleteBone.cpp bone/bone_DeleteBone.h
    bone/bone_DeleteMode.cpp bone/bone_DeleteMode.h
    bone/bone_EraseInflMode.cpp bone/bone_EraseInflMode.h
    bone/bone_Focuser.cpp bone/bone_Focuser.h
    bone/bone_GeoBuilder.cpp bone/bone_GeoBuilder.h
    bone/bone_IMode.h
    bone/bone_InfluenceMode.cpp bone/bone_InfluenceMode.h
    bone/bone_KeyOwner.cpp bone/bone_KeyOwner.h
    bone/bone_MoveBone.h
    bone/bone_MoveJointMode.cpp bone/bone_MoveJointMode.h
    bone/bone_NodeSelector.cpp bone/bone_NodeSelector.h
    bone/bone_Notifier.cpp bone/bone_Notifier.h
    bone/bone_PaintInflMode.cpp bone/bone_PaintInflMode.h
    bone/bone_PushNewPoses.h
    bone/bone_PushNewTopPoses.h
    bone/bone_Renderer.cpp bone/bone_Renderer.h
    bone/bone_Target.h
    ffd/ffd_BrushMode.cpp ffd/ffd_BrushMode.h
    ffd/ffd_DragMode.cpp ffd/ffd_DragMode.h
    ffd/ffd_IMode.h
    ffd/ffd_KeyOwner.cpp ffd/ffd_KeyOwner.h
    ffd/ffd_MoveVertices.cpp ffd/ffd_MoveVertices.h
    ffd/ffd_Target.cpp ffd/ffd_Target.h
    ffd/ffd_Task.cpp ffd/ffd_Task.h
    ffd/ffd_TaskResource.cpp ffd/ffd_TaskResource.h
    mesh/mesh_CreateMode.cpp mesh/mesh_CreateMode.h
    mesh/mesh_DeleteMode.cpp mesh/mesh_DeleteMode.h
    mesh/mesh_Focuser.cpp mesh/mesh_Focuser.h
    mesh/mesh_IMode.h
    mesh/mesh_KeyOwner.cpp mesh/mesh_KeyOwner.h
    mesh/mesh_MeshAccessor.cpp mesh/mesh_MeshAccessor.h
    mesh/mesh_Notifier.cpp mesh/mesh_Notifier.h
    mesh/mesh_Renderer.cpp mesh/mesh_Renderer.h
    mesh/mesh_SplitMode.cpp mesh/mesh_SplitMode.h
    mesh/mesh_Target.h
    mesh/mesh_VtxMover.h
    pose/pose_BoneDynamics.cpp pose/pose_BoneDynamics.h
    pose/pose_DrawBoneMode.cpp pose/pose_DrawBoneMode.h
    pose/pose_ErasePoseMode.cpp pose/pose_ErasePoseMode.h
    pose/pose_IMode.h
    pose/pose_KeyOwner.cpp pose/pose_KeyOwner.h
    pose/pose_RigidBone.cpp pose/pose_RigidBone.h
    pose/pose_RotateBone.h
    pose/pose_RotateBones.cpp pose/pose_RotateBones.h
    pose/pose_Target.h
    pose/pose_TransBoneMode.cpp pose/pose_TransBoneMode.h
    srt/srt_CentroidMode.cpp srt/srt_CentroidMode.h
    srt/srt_CentroidMover.cpp srt/srt_CentroidMover.h
    srt/srt_FocusType.h
    srt/srt_IMode.h
    srt/srt_KeyOwner.cpp srt/srt_KeyOwner.h
    srt/srt_MoveMode.cpp srt/srt_MoveMode.h
    srt/srt_Symbol.cpp srt/srt_Symbol.h
    time/time_Current.cpp time/time_Current.h
    time/time_Focuser.cpp time/time_Focuser.h
    time/time_Renderer.cpp time/time_Renderer.h
    time/time_Scaler.cpp time/time_Scaler.h
        ExportParams.h
)

set(SHADERS_DIR ../../data/shader)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/FFDBlurVert.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/FFDEraseVert.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/FFDFocusVertexVert.glsl)
b_embed(${PROJECT_NAME} ${SHADERS_DIR}/FreeFormDeformVert.glsl)

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
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::OpenGL
    Qt::Xml
    cmnd
    core
    gl
    img
    thr
    util
)
