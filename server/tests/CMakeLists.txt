add_executable(alexandria_server_tests
    main.cpp
    SqliteBookRepositoryTest.cpp
    SqliteUserRepositoryTest.cpp
)

target_link_libraries(alexandria_server_tests
    PRIVATE
        alexandria_persistence
        GTest::gtest
        Qt6::Core
)

include(GoogleTest)
gtest_discover_tests(alexandria_server_tests)