file(GLOB_RECURSE SRC_FILES *.h *.cxx *.cpp)

set(THIS_TARGET convert)
add_executable(${THIS_TARGET} ${SRC_FILES})

target_include_directories(${THIS_TARGET} PRIVATE . ${FBX_INCLUDE_DIRS})
target_link_libraries(${THIS_TARGET} PRIVATE ${FBX_LIBRARIES})

# add_custom_command(TARGET ${THIS_TARGET} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy ${FBX_BIN} $<TARGET_FILE_DIR:convert>
#     COMMAND_EXPAND_LISTS
# )
