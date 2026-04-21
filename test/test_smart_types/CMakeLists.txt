set(project_name test_smart_types)

set(interface_files
    "test_smart_types.i"
)

set(source_files
    "test_smart_types.cpp"
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
            -emTEST_SMART_TYPES_EXPORT
            "-pc${CMAKE_CURRENT_SOURCE_DIR}/../../paf/pafcore"
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

add_library(
    ${project_name}
    STATIC
    ${source_files}
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
)

target_compile_definitions(
    ${project_name}
    PUBLIC
        TEST_SMART_TYPES_EXPORT=
)

target_link_libraries(
    ${project_name}
    PUBLIC
        pafcore
)
