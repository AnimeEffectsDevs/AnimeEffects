set(SRC
mesh_CreateMode.cpp
mesh_DeleteMode.cpp
mesh_Focuser.cpp
mesh_KeyOwner.cpp
mesh_MeshAccessor.cpp
mesh_Notifier.cpp
mesh_Renderer.cpp
mesh_SplitMode.cpp
)
add_library(${LIB_MESH} ${SRC})