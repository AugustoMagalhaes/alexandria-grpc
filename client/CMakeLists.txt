qt_add_executable(alexandria_client
    main.cpp
    core/AlexandriaClient.cpp
)

qt_add_qml_module(alexandria_client
    URI Alexandria
    QML_FILES
        qml/Main.qml
)

target_include_directories(alexandria_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(alexandria_client
    PRIVATE
        Qt6::Quick
        alexandria_domain
        alexandria_proto
)

install(TARGETS alexandria_client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)