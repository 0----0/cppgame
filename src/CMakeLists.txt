
add_executable(cppgame main.cpp AssetManager.cpp TestGame2.cpp
PlayerController.cpp Renderer.cpp EnemyAI.cpp EnemyList.cpp extern/mikktspace.c
LuaInterface.cpp TriShotEnemyAI.cpp Assets/Image.cpp Assets/Geometry.cpp
Assets/UnindexedGeometry.cpp nuklear/nuklear.c GUI/ColorPicker.cpp)

target_link_libraries(cppgame ${LIB})
