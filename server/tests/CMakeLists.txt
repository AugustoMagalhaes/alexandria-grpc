add_executable(alexandria_server_tests
    main.cpp
    SqliteBookRepositoryTest.cpp
    SqliteUserRepositoryTest.cpp
    AuthServiceTest.cpp
    BookServiceTest.cpp
    UserServiceTest.cpp
    CsvServiceTest.cpp
)

target_link_libraries(alexandria_server_tests
    PRIVATE
        alexandria_persistence
        alexandria_auth
        alexandria_service
        GTest::gtest
        GTest::gmock
        Qt6::Core
)

include(GoogleTest)
gtest_discover_tests(alexandria_server_tests)