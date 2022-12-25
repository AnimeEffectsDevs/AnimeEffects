set(SRC
bone_BindNodesMode.cpp
bone_CreateMode.cpp
bone_DeleteBone.cpp
bone_DeleteMode.cpp
bone_EraseInflMode.cpp
bone_Focuser.cpp
bone_GeoBuilder.cpp
bone_InfluenceMode.cpp
bone_KeyOwner.cpp
bone_MoveJointMode.cpp
bone_NodeSelector.cpp
bone_Notifier.cpp
bone_PaintInflMode.cpp
bone_Renderer.cpp
)
add_library(${LIB_BONE} ${SRC})