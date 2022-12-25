set(LIB ${PROJECT_NAME}_gl)
set(SRC
BufferObject.cpp
ComputeTexture1D.cpp
DeviceInfo.cpp
EasyShaderProgram.cpp
EasyTextureDrawer.cpp
ExtendShader.cpp
FontDrawer.cpp
FrameBuffer.cpp
Global.cpp
PrimitiveDrawer.cpp
Root.cpp
Task.cpp
TextObject.cpp
Texture.cpp
Triangulator.cpp
Util.cpp
VertexArrayObject.cpp
)
add_library(${LIB} ${SRC})