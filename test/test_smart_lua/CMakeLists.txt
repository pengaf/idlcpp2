add_executable(
    test_smart_lua
    "test_smart_lua.cpp"
)

target_include_directories(
    test_smart_lua
    PRIVATE
        "${lua_include_dir}"
)

target_link_libraries(
    test_smart_lua
    PRIVATE
        pafcore
        paflua
        test_smart_types
        lua::lua
)

add_custom_command(
    TARGET test_smart_lua
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/test_smart_lua.lua"
        "$<TARGET_FILE_DIR:test_smart_lua>/test_smart_lua.lua"
)

if(WIN32)
    add_custom_command(
        TARGET test_smart_lua
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${lua_dll_path}"
            "$<TARGET_FILE_DIR:test_smart_lua>/lua.dll"
    )
endif()
