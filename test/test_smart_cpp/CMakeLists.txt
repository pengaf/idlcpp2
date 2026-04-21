add_executable(
    test_smart_cpp
    "test_smart_cpp.cpp"
)

target_link_libraries(
    test_smart_cpp
    PRIVATE
        pafcore
        test_smart_types
)
