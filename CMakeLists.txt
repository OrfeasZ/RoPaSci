cmake_minimum_required(VERSION 3.3)
project(ropasci)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fpermissive -std=c++11")

set(SOURCE_FILES
        Src/Game/Entities/BlockEntity.cpp
        Src/Game/Entities/BlockEntity.h
        Src/Game/Entities/GridEntity.cpp
        Src/Game/Entities/GridEntity.h
        Src/Game/Entities/IEntity.h
        Src/Game/Entities/LifeEntity.cpp
        Src/Game/Entities/LifeEntity.h
        Src/Game/Entities/ScoringEntity.cpp
        Src/Game/Entities/ScoringEntity.h
        Src/Game/Main.cpp
        Src/Game/Main.h
        Src/Managers/Input/KeyAction.h
        Src/Managers/Input/MouseAction.h
        Src/Managers/Model/ModelData.h
        Src/Managers/Shader/Program.h
        Src/Managers/Task/GenericTask.h
        Src/Managers/Task/TBBContextID.cpp
        Src/Managers/Task/TBBContextID.h
        Src/Managers/Task/TBBTaskSet.h
        Src/Managers/AudioManager.cpp
        Src/Managers/AudioManager.h
        Src/Managers/CameraManager.cpp
        Src/Managers/CameraManager.h
        Src/Managers/IManager.h
        Src/Managers/InputManager.cpp
        Src/Managers/InputManager.h
        Src/Managers/ModelManager.cpp
        Src/Managers/ModelManager.h
        Src/Managers/SceneManager.cpp
        Src/Managers/SceneManager.h
        Src/Managers/ShaderManager.cpp
        Src/Managers/ShaderManager.h
        Src/Managers/SimulationManager.cpp
        Src/Managers/SimulationManager.h
        Src/Managers/TaskManager.cpp
        Src/Managers/TaskManager.h
        Src/Managers/TextureManager.cpp
        Src/Managers/TextureManager.h
        Src/Misc/StdAfx.h
        Src/Rendering/Objects/Model.cpp
        Src/Rendering/Objects/Model.h
        Src/Rendering/Textures/CharacterTexture.cpp
        Src/Rendering/Textures/CharacterTexture.h
        Src/Rendering/Textures/ITexture.h
        Src/Rendering/Textures/Texture.cpp
        Src/Rendering/Textures/Texture.h
        Src/Rendering/Textures/TextureCreateDesc.h
        Src/Rendering/DebugRenderer.cpp
        Src/Rendering/DebugRenderer.h
        Src/Rendering/IRenderer.h
        Src/Rendering/MainRenderer.cpp
        Src/Rendering/MainRenderer.h
        Src/Rendering/UIRenderer.cpp
        Src/Rendering/UIRenderer.h
        Src/Util/EasyLock.h
        Src/Util/EngineTimer.cpp
        Src/Util/EngineTimer.h
        Src/Util/Log.cpp
        Src/Util/Log.h
        Src/Util/SpinLock.h
        Src/Util/Utils.cpp
        Src/Util/Utils.h
        Src/VFS/Backends/WinBackend.cpp
        Src/VFS/Backends/WinBackend.h
        Src/VFS/Files/NativeFile.cpp
        Src/VFS/Files/NativeFile.h
        Src/VFS/FileSystem.cpp
        Src/VFS/FileSystem.h
        Src/VFS/FSBackend.h
        Src/VFS/FSFile.h
        Src/Application.cpp
        Src/Application.h
        Src/RoPaSci.cpp Src/VFS/Backends/PosixBackend.cpp Src/VFS/Backends/PosixBackend.h)

include_directories(Depends/include)
include_directories(Src)

if (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        link_directories(Depends/lib/win64)
    else (CMAKE_SIZEOF_VOID_P EQUAL 8)
        link_directories(Depends/lib/win32)
    endif (CMAKE_SIZEOF_VOID_P EQUAL 8)
else (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        link_directories(Depends/lib/elf64)
    else (CMAKE_SIZEOF_VOID_P EQUAL 8)
        link_directories(Depends/lib/elf32)
    endif (CMAKE_SIZEOF_VOID_P EQUAL 8)
endif(MSVC)

add_executable(ropasci ${SOURCE_FILES})

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

target_link_libraries(ropasci
        freetype glew glfw3 sfml-audio sfml-system
        tbb tbb_preview tbbmalloc tbbmalloc_proxy X11 Xrandr Xi Xxf86vm
        GL Xinerama Xcursor rt m stdc++)

# target_link_libraries(ropasci optimized flac freetype glew glfw3 ogg openal32 sfml-audio-s sfml-system-s tbb tbb_preview tbbmalloc tbbmalloc_proxy vorbis vorbisenc vorbisfile)

