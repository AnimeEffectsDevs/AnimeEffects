set(LIB ${PROJECT_NAME}_ctrl)
set(LIB_BONE ${LIB}_bone)
set(LIB_FFD ${LIB}_ffd)
set(LIB_MESH ${LIB}_mesh)
set(LIB_POSE ${LIB}_pose)
set(LIB_SRT ${LIB}_srt)
set(LIB_TIME ${LIB}_time)
set(LIBS ${LIB_BONE} ${LIB_FFD} ${LIB_MESH} ${LIB_POSE} ${LIB_SRT} ${LIB_TIME})

set(SRC
BoneEditor.cpp
CmndName.cpp
Driver.cpp
DriverResources.cpp
Exporter.cpp
FFDEditor.cpp
ImageFileLoader.cpp
KeyBinding.cpp
MeshEditor.cpp
Painter.cpp
PoseEditor.cpp
PoseParam.cpp
ProjectLoader.cpp
ProjectSaver.cpp
SRTEditor.cpp
System.cpp
TimeLineEditor.cpp
TimeLineRow.cpp
TimeLineUtil.cpp
VideoFormat.cpp
)
add_library(${LIB} ${SRC})
add_subdirectory(bone)
add_subdirectory(ffd)
add_subdirectory(mesh)
add_subdirectory(pose)
add_subdirectory(srt)
add_subdirectory(time)
target_link_libraries(${LIB} ${LIBS})
