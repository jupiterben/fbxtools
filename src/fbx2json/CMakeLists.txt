file(GLOB_RECURSE SRC_FILES *.h *.cxx *.cpp)

# FBX
# set(FBX_DIR 2020.0.1)
find_package(FBX REQUIRED)

set(TARGET_NAME fbx2json)
add_executable(${TARGET_NAME} ${SRC_FILES})

target_include_directories(${TARGET_NAME} PRIVATE . "../3rd" ${FBX_INCLUDE_DIR})
target_link_libraries(${TARGET_NAME} PRIVATE ${FBX_LIBRARY} ${FBX_XML2_LIBRARY} ${FBX_ZLIB_LIBRARY})

# add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy ${FBX_BIN} $<TARGET_FILE_DIR:fbx2json>
#     COMMAND_EXPAND_LISTS
# )
