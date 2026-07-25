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

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()