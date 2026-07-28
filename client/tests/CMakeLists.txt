add_executable(alexandria_client_tests
    main.cpp
    IntegrationTest.cpp
)

target_link_libraries(alexandria_client_tests
    PRIVATE
        alexandria_persistence
        alexandria_auth
        alexandria_service
        alexandria_grpc_server
        alexandria_client_core
        GTest::gtest
        Qt6::Core
)

include(GoogleTest)
gtest_discover_tests(alexandria_client_tests)