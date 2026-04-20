add_executable(
    test1
    "list_meta_types.cpp"
)

target_link_libraries(
    test1
    PRIVATE
        pafcore
)
