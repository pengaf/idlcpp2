set(project_name pafcore)

set(header_files
    "auto_run.h"
    "debug.h"
    "flat_set.h"
    "memory.h"
    "overload.h"
    "std_deque.h"
    "std_forward_list.h"
    "std_list.h"
    "std_map.h"
    "std_queue.h"
    "std_set.h"
    "std_string.h"
    "std_unordered_map.h"
    "std_unordered_set.h"
    "std_vector.h"
    "subclass_invoker.h"
    "utility.h"
    "variant.h"
)

set(interface_files
    "argument.i"
    "buffer.i"
    "class_type.i"
    "delegate.i"
    "dynamic_object.i"
    "enumerator.i"
    "enum_type.i"
    "instance_field.i"
    "instance_method.i"
    "instance_property.i"
    "iterator.i"
    "metadata.i"
    "name_space.i"
    "notify_handler.i"
    "notify_handler_list.i"
    "object.i"
    "primitive_type.i"
    "reflection.i"
    "result.i"
    "static_field.i"
    "static_method.i"
    "static_property.i"
    "string.i"
    "system.i"
    "type.i"
    "type_alias.i"
    "typedef.i"
    "void_type.i"
)

set(source_files
    "argument.cpp"
    "buffer.cpp"
    "class_type.cpp"
    "delegate.cpp"
    "dynamic_object.cpp"
    "enumerator.cpp"
    "enum_type.cpp"
    "instance_field.cpp"
    "instance_method.cpp"
    "instance_property.cpp"
    "iterator.cpp"
    "metadata.cpp"
    "name_space.cpp"
    "notify_handler.cpp"
    "notify_handler_list.cpp"
    "object.cpp"
    "overload.cpp"
    "primitive_type.cpp"
    "reflection.cpp"
    "result.cpp"
    "static_field.cpp"
    "static_method.cpp"
    "static_property.cpp"
    "string.cpp"
    "system.cpp"
    "type.cpp"
    "type_alias.cpp"
    "typedef.cpp"
    "utility.cpp"
    "variant.cpp"
    "void_type.cpp"
)

set(windows_cpp_only
    "dllmain.cpp"
)

set(windows_resource_only
    "pafcore.rc"
    "resource.h"
)

set(windows_only_sources
    ${windows_cpp_only}
    ${windows_resource_only}
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
            -emPAFCORE_EXPORT
            "-pc${CMAKE_CURRENT_SOURCE_DIR}"
            "${interface_path}"
        DEPENDS
            idlcpp
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

set(PAFCORE_IDL_GENERATED_FILES "${generated_files}" CACHE INTERNAL "idlcpp outputs from pafcore .i (source tree)")

set(pafcore_header_paths)
foreach(_f IN LISTS header_files)
    list(APPEND pafcore_header_paths "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
endforeach()

set(pafcore_interface_paths)
foreach(_f IN LISTS interface_files)
    list(APPEND pafcore_interface_paths "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
endforeach()

set(pafcore_source_paths)
foreach(_f IN LISTS source_files)
    list(APPEND pafcore_source_paths "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
endforeach()

source_group("Header Files" FILES ${pafcore_header_paths})
source_group("Interface Files" FILES ${pafcore_interface_paths})
source_group("IDL Generated" FILES ${generated_files})
source_group("Source Files" FILES ${pafcore_source_paths})
if(WIN32)
    set(pafcore_win_cpp_paths)
    foreach(_f IN LISTS windows_cpp_only)
        list(APPEND pafcore_win_cpp_paths "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
    endforeach()
    set(pafcore_win_res_paths)
    foreach(_f IN LISTS windows_resource_only)
        list(APPEND pafcore_win_res_paths "${CMAKE_CURRENT_SOURCE_DIR}/${_f}")
    endforeach()
    source_group("Source Files" FILES ${pafcore_win_cpp_paths})
    source_group("Resource Files" FILES ${pafcore_win_res_paths})
endif()

add_library(
    ${project_name}
    SHARED
    ${pafcore_header_paths}
    ${pafcore_source_paths}
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

target_compile_definitions(
    ${project_name}
    PRIVATE
        PAFCORE_EXPORTS
        PAF_MEMORY_BACKEND=${PAF_MEMORY_BACKEND_DEFINE}
)

if(PAF_MEMORY_BACKEND_TARGET)
    target_link_libraries(${project_name} PRIVATE ${PAF_MEMORY_BACKEND_TARGET})
endif()

if(WIN32)
    target_sources(
        ${project_name}
        PRIVATE
            ${pafcore_win_cpp_paths}
            ${pafcore_win_res_paths}
    )
    target_compile_definitions(${project_name} PRIVATE _UNICODE UNICODE)
else()
    target_link_libraries(${project_name} PRIVATE dl)
endif()
