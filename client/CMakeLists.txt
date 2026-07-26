qt_add_executable(alexandria_client
    main.cpp
    core/AlexandriaClient.cpp
    viewmodels/Session.cpp
    viewmodels/BookListViewModel.cpp
    viewmodels/BookFormViewModel.cpp
)

qt_add_qml_module(alexandria_client
    URI Alexandria
    QML_FILES
        qml/Main.qml
        qml/LoginPage.qml
        qml/BookListPage.qml
        qml/ServerSetupPage.qml
        qml/BookFormDialog.qml
        qml/DeleteConfirmDialog.qml
    SOURCES
        viewmodels/BookListViewModel.h
        viewmodels/BookFormViewModel.h
)

target_include_directories(alexandria_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/viewmodels
)

target_link_libraries(alexandria_client
    PRIVATE
        Qt6::Quick
        Qt6::Concurrent
        alexandria_domain
        alexandria_proto
)

install(TARGETS alexandria_client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)