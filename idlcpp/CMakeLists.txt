set(IDLCPP_WIN_FLEX_BISON_ROOT "C:/win_flex_bison-2.5.25" CACHE PATH "win_flex_bison installation root")

if(WIN32)
    if(NOT FLEX_EXECUTABLE AND EXISTS "${IDLCPP_WIN_FLEX_BISON_ROOT}/win_flex.exe")
        set(FLEX_EXECUTABLE "${IDLCPP_WIN_FLEX_BISON_ROOT}/win_flex.exe" CACHE FILEPATH "flex executable" FORCE)
    endif()
    if(NOT BISON_EXECUTABLE AND EXISTS "${IDLCPP_WIN_FLEX_BISON_ROOT}/win_bison.exe")
        set(BISON_EXECUTABLE "${IDLCPP_WIN_FLEX_BISON_ROOT}/win_bison.exe" CACHE FILEPATH "bison executable" FORCE)
    endif()
endif()

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(idlcpp_bison_y "${CMAKE_CURRENT_SOURCE_DIR}/idlcpp.y")
set(idlcpp_bison_cpp "${CMAKE_CURRENT_SOURCE_DIR}/idlcpp.y.cpp")
set(idlcpp_bison_h "${CMAKE_CURRENT_SOURCE_DIR}/idlcpp.y.h")
set(idlcpp_flex_l "${CMAKE_CURRENT_SOURCE_DIR}/idlcpp.l")
set(idlcpp_flex_cpp "${CMAKE_CURRENT_SOURCE_DIR}/idlcpp.l.cpp")

bison_target(
    IdlcppParser
    "${idlcpp_bison_y}"
    "${idlcpp_bison_cpp}"
    DEFINES_FILE "${idlcpp_bison_h}"
)

flex_target(
    IdlcppScanner
    "${idlcpp_flex_l}"
    "${idlcpp_flex_cpp}"
)

add_flex_bison_dependency(IdlcppScanner IdlcppParser)

set(idlcpp_grammar_sources
    "${idlcpp_bison_y}"
    "${idlcpp_flex_l}"
)

source_group("Grammar" FILES ${idlcpp_grammar_sources})
source_group(
    "Grammar Generated"
    FILES
        ${BISON_IdlcppParser_OUTPUTS}
        ${FLEX_IdlcppScanner_OUTPUTS}
)

set(idlcpp_source_files
    "attribute_list_node.cpp"
    "attribute_node.cpp"
    "class_node.cpp"
    "common_funcs.cpp"
    "compiler.cpp"
    "delegate_node.cpp"
    "enum_node.cpp"
    "enumerator_list_node.cpp"
    "enumerator_node.cpp"
    "error_list.cpp"
    "field_node.cpp"
    "getter_setter_node.cpp"
    "header_file_generator.cpp"
    "identify_list_node.cpp"
    "identify_node.cpp"
    "idlcpp.cpp"
    "import_directory.cpp"
    "member_list_node.cpp"
    "member_node.cpp"
    "meta_header_file_generator.cpp"
    "meta_source_file_generator.cpp"
    "method_node.cpp"
    "namespace_node.cpp"
    "operator_node.cpp"
    "options.cpp"
    "parameter_list_node.cpp"
    "parameter_node.cpp"
    "platform.cpp"
    "program_node.cpp"
    "property_node.cpp"
    "raise_error.cpp"
    "scope_name_list_node.cpp"
    "scope_name_node.cpp"
    "source_file.cpp"
    "source_file_generator.cpp"
    "syntax_node_impl.cpp"
    "template_arguments.cpp"
    "template_class_instance_node.cpp"
    "template_parameter_list_node.cpp"
    "template_parameters_node.cpp"
    "token_list_node.cpp"
    "token_node.cpp"
    "type_declaration_node.cpp"
    "type_name_list_node.cpp"
    "type_name_node.cpp"
    "type_tree.cpp"
    "typedef_node.cpp"
    "utility.cpp"
    "yyfuncs.cpp"
)

add_executable(
    idlcpp
    ${idlcpp_source_files}
    ${idlcpp_grammar_sources}
    ${BISON_IdlcppParser_OUTPUTS}
    ${FLEX_IdlcppScanner_OUTPUTS}
)

set_source_files_properties(
    ${idlcpp_grammar_sources}
    PROPERTIES
        HEADER_FILE_ONLY TRUE
)

target_include_directories(
    idlcpp
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

if(MSVC)
    target_compile_definitions(idlcpp PRIVATE NOMINMAX YY_NO_UNISTD_H)
endif()
