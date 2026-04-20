set(project_name paflua)
set(paflua_library_type SHARED)

if(WIN32)
    set(lua_root "D:/reference/lua-5.5.0" CACHE PATH "Lua installation root")
    set(lua_include_dir "${lua_root}/src" CACHE PATH "Lua include directory")
    set(lua_dll_path "${lua_root}/lib/lua.dll" CACHE FILEPATH "Lua shared library (lua.dll)")
    set(lua_implib_path "${lua_root}/lib/lua.lib" CACHE FILEPATH "Lua import library (with lua.dll)")

    if(NOT EXISTS "${lua_include_dir}/lua.hpp")
        message(FATAL_ERROR "Lua header not found: ${lua_include_dir}/lua.hpp")
    endif()

    if(NOT EXISTS "${lua_implib_path}")
        message(FATAL_ERROR "Lua import library not found: ${lua_implib_path}")
    endif()

    if(NOT EXISTS "${lua_dll_path}")
        message(FATAL_ERROR "Lua DLL not found: ${lua_dll_path}")
    endif()

    if(NOT TARGET lua::lua)
        add_library(lua::lua SHARED IMPORTED GLOBAL)
        set_target_properties(
            lua::lua
            PROPERTIES
                IMPORTED_LOCATION "${lua_dll_path}"
                IMPORTED_IMPLIB "${lua_implib_path}"
                INTERFACE_INCLUDE_DIRECTORIES "${lua_include_dir}"
        )
    endif()
else()
    find_path(
        lua_include_dir
        NAMES lua.hpp lua.h
        PATHS
            /usr/local/include
            /usr/include
        PATH_SUFFIXES
            lua5.4
            lua54
            lua5.3
            lua53
    )

    if(NOT lua_include_dir)
        message(FATAL_ERROR "Lua headers not found. Install Lua headers or set lua_include_dir.")
    endif()

    set(_saved_find_library_suffixes "${CMAKE_FIND_LIBRARY_SUFFIXES}")
    if(APPLE)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".dylib" ".so")
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
    endif()

    find_library(
        lua_shared_library
        NAMES lua5.4 lua54 lua5.3 lua53 lua
        PATHS
            /usr/local/lib
            /usr/lib
            /usr/lib/x86_64-linux-gnu
        NO_CACHE
    )

    set(CMAKE_FIND_LIBRARY_SUFFIXES "${_saved_find_library_suffixes}")

    if(lua_shared_library)
        set(lua_library_path "${lua_shared_library}")
        set(lua_import_type SHARED)
    else()
        find_library(
            lua_static_library
            NAMES lua5.4 lua54 lua5.3 lua53 lua
            PATHS
                /usr/local/lib
                /usr/lib
                /usr/lib/x86_64-linux-gnu
            NO_CACHE
        )

        if(NOT lua_static_library)
            message(FATAL_ERROR "Lua library not found. Install Lua or set lua_static_library.")
        endif()

        set(lua_library_path "${lua_static_library}")
        set(lua_import_type STATIC)
        set(paflua_library_type STATIC)
        message(WARNING "Shared Lua library not found. Building paflua as STATIC with ${lua_static_library}.")
    endif()

    if(NOT TARGET lua::lua)
        add_library(lua::lua ${lua_import_type} IMPORTED GLOBAL)
        set_target_properties(
            lua::lua
            PROPERTIES
                IMPORTED_LOCATION "${lua_library_path}"
                INTERFACE_INCLUDE_DIRECTORIES "${lua_include_dir}"
        )
    endif()
endif()

set(interface_files
    "lua_call_back.i"
)

set(source_files
    "lua_call_back.cpp"
    "lua_subclass_invoker.cpp"
    "lua_wrapper.cpp"
    "paflua.cpp"
)

set(generated_files)
foreach(interface_file IN LISTS interface_files)
    get_filename_component(interface_name "${interface_file}" NAME_WE)
    set(interface_path "${CMAKE_CURRENT_SOURCE_DIR}/${interface_file}")
    set(generated_header "${CMAKE_CURRENT_SOURCE_DIR}/${interface_name}.h")
    set(generated_internal "${CMAKE_CURRENT_SOURCE_DIR}/${interface_name}.ic")
    set(generated_meta_header "${CMAKE_CURRENT_SOURCE_DIR}/${interface_name}.mh")
    set(generated_meta_source "${CMAKE_CURRENT_SOURCE_DIR}/${interface_name}.mc")

    add_custom_command(
        OUTPUT
            "${generated_header}"
            "${generated_internal}"
            "${generated_meta_header}"
            "${generated_meta_source}"
        MAIN_DEPENDENCY "${interface_path}"
        COMMAND $<TARGET_FILE:idlcpp>
            -emPAFLUA_EXPORT
            "-pc${CMAKE_CURRENT_SOURCE_DIR}/../pafcore"
            "${interface_path}"
        DEPENDS
            idlcpp
            ${PAFCORE_IDL_GENERATED_FILES}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        VERBATIM
    )

    list(APPEND generated_files
        "${generated_header}"
        "${generated_internal}"
        "${generated_meta_header}"
        "${generated_meta_source}"
    )
endforeach()

set(paflua_source_paths)
foreach(_f IN LISTS source_files)
    list(APPEND paflua_source_paths "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
endforeach()

set(paflua_interface_paths)
foreach(_f IN LISTS interface_files)
    list(APPEND paflua_interface_paths "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
endforeach()

source_group("Source Files" FILES ${paflua_source_paths})
source_group("Interface Files" FILES ${paflua_interface_paths})
source_group("IDL Generated" FILES ${generated_files})

add_library(
    ${project_name}
    ${paflua_library_type}
    ${paflua_source_paths}
)

target_sources(
    ${project_name}
    PRIVATE
        ${generated_files}
)

set_source_files_properties(
    ${generated_files}
    PROPERTIES
        HEADER_FILE_ONLY TRUE
)

target_include_directories(
    ${project_name}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${lua_include_dir}"
)

target_compile_definitions(${project_name} PRIVATE PAFLUA_EXPORTS)

if(WIN32)
    target_compile_definitions(${project_name} PRIVATE _UNICODE UNICODE)
endif()

target_link_libraries(
    ${project_name}
    PRIVATE
        pafcore
        lua::lua
)
