qt_add_executable(alexandria_client
    main.cpp
)

qt_add_qml_module(alexandria_client
    URI Alexandria
    QML_FILES
        qml/Main.qml
)

target_link_libraries(alexandria_client
    PRIVATE Qt6::Quick
)

install(TARGETS alexandria_client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
