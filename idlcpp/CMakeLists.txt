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


set(idlcpp_header_files
    "attribute_list_node.h"
    "attribute_node.h"
    "class_node.h"
    "common_funcs.h"
    "compiler.h"
    "compound_type_node.h"
    "entity_node.h"
    "enum_node.h"
    "enumerator_list_node.h"
    "enumerator_node.h"
    "error_list.h"
    "field_node.h"
    "header_file_generator.h"
    "identifier_list_node.h"
    "identifier_node.h"
    "idlcpp.h"
    "import_directory.h"
    "member_list_node.h"
    "member_node.h"
    "meta_header_file_generator.h"
    "meta_source_file_generator.h"
    "method_node.h"
    "namespace_node.h"
    "options.h"
    "platform.h"
    "program_node.h"
    "property_accessor_list_node.h"
    "property_accessor_node.h"
    "property_node.h"
    "raise_error.h"
    "scope_name_list_node.h"
    "scope_name_node.h"
    "source_file.h"
    "source_file_generator.h"
    "syntax_node_impl.h"
    "template_arguments.h"
    "template_class_instance_node.h"
    "template_parameters_node.h"
    "token_list_node.h"
    "token_node.h"
    "type_declaration_node.h"
    "type_name_list_node.h"
    "type_name_node.h"
    "type_tree.h"
    "typedef_node.h"
    "utility.h"
    "variable_list_node.h"
    "variable_node.h"
    "yyfuncs.h"
    "scope_node.h"
)

set(idlcpp_source_files
    "attribute_list_node.cpp"
    "attribute_node.cpp"
    "class_node.cpp"
    "common_funcs.cpp"
    "compiler.cpp"
    "compound_type_node.cpp"
    "entity_node.cpp"
    "enum_node.cpp"
    "enumerator_list_node.cpp"
    "enumerator_node.cpp"
    "error_list.cpp"
    "field_node.cpp"
    "header_file_generator.cpp"
    "identifier_list_node.cpp"
    "identifier_node.cpp"
    "idlcpp.cpp"
    "import_directory.cpp"
    "member_list_node.cpp"
    "member_node.cpp"
    "meta_header_file_generator.cpp"
    "meta_source_file_generator.cpp"
    "method_node.cpp"
    "namespace_node.cpp"
    "options.cpp"
    "platform.cpp"
    "program_node.cpp"
    "property_accessor_list_node.cpp"
    "property_accessor_node.cpp"
    "property_node.cpp"
    "raise_error.cpp"
    "scope_name_list_node.cpp"
    "scope_name_node.cpp"
    "source_file.cpp"
    "source_file_generator.cpp"
    "syntax_node_impl.cpp"
    "template_arguments.cpp"
    "template_class_instance_node.cpp"
    "template_parameters_node.cpp"
    "token_list_node.cpp"
    "token_node.cpp"
    "type_declaration_node.cpp"
    "type_name_list_node.cpp"
    "type_name_node.cpp"
    "type_tree.cpp"
    "typedef_node.cpp"
    "utility.cpp"
    "variable_list_node.cpp"
    "variable_node.cpp"
    "yyfuncs.cpp"
)

source_group("Grammar" FILES ${idlcpp_grammar_sources})
source_group(
    "Grammar Generated"
    FILES
        ${BISON_IdlcppParser_OUTPUTS}
        ${FLEX_IdlcppScanner_OUTPUTS}
)
source_group("Header Files" FILES ${idlcpp_header_files})
source_group("Source Files" FILES ${idlcpp_source_files})


add_executable(
    idlcpp
    ${idlcpp_header_files}
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
