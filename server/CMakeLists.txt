find_package(PkgConfig REQUIRED)

add_library(alexandria_domain INTERFACE)
target_include_directories(alexandria_domain INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(alexandria_repository INTERFACE)
target_link_libraries(alexandria_repository INTERFACE alexandria_domain)

add_library(alexandria_persistence STATIC
    persistence/Database.cpp
    persistence/SqliteBookRepository.cpp
    persistence/SqliteUserRepository.cpp
)

target_include_directories(alexandria_persistence PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(alexandria_persistence
    PUBLIC
        alexandria_repository
        Qt6::Sql
)

pkg_check_modules(SODIUM REQUIRED IMPORTED_TARGET libsodium)

add_library(alexandria_auth STATIC
    auth/PasswordHasher.cpp
    auth/SessionManager.cpp
    auth/AuthService.cpp
)

target_include_directories(alexandria_auth PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(alexandria_auth
    PUBLIC
        alexandria_repository
        PkgConfig::SODIUM
)

add_library(alexandria_service STATIC
    service/OperationResult.cpp
    service/BookService.cpp
    service/UserService.cpp
)

target_include_directories(alexandria_service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(alexandria_service
    PUBLIC
        alexandria_repository
    PRIVATE
        alexandria_auth
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()