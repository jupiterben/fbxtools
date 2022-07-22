cmake_minimum_required(VERSION 3.13)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project(FBXTOOLS)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "-Od") # 调试包不优化
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG ") # release包优化
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /ZI")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4819")
elseif(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS_DEBUG "-g4")
    set(CMAKE_EXECUTABLE_SUFFIX ".js")

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s SINGLE_FILE=1 -s WASM=1" CACHE STRING "")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
endif()

# add_subdirectory(main)
# add_subdirectory(src/convert)
add_subdirectory(src/fbx2json)

# add_subdirectory(test)
