add_executable(
    test_lua
    "test_lua.cpp"
)

target_include_directories(
    test_lua
    PRIVATE
        "${lua_include_dir}"
)

target_link_libraries(
    test_lua
    PRIVATE
        pafcore
        paflua
        lua::lua
)

add_custom_command(
    TARGET test_lua
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/test_lua.lua"
        "$<TARGET_FILE_DIR:test_lua>/test_lua.lua"
)

if(WIN32)
    add_custom_command(
        TARGET test_lua
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${lua_dll_path}"
            "$<TARGET_FILE_DIR:test_lua>/lua.dll"
    )
endif()
