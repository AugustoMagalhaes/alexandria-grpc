add_library(alexandria_client_core STATIC
    core/AlexandriaClient.cpp
)

target_include_directories(alexandria_client_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(alexandria_client_core
    PUBLIC
        alexandria_domain
        alexandria_proto
)

qt_add_executable(alexandria_client
    main.cpp
    viewmodels/Session.cpp
    viewmodels/BookListViewModel.cpp
    viewmodels/BookFormViewModel.cpp
    viewmodels/UserListViewModel.cpp
    viewmodels/UserFormViewModel.cpp
    viewmodels/CsvViewModel.cpp
)

qt_add_qml_module(alexandria_client
    URI Alexandria
    QML_FILES
        qml/Theme.qml
        qml/AppButton.qml
        qml/AppTextField.qml
        qml/AppSpinBox.qml
        qml/AppCheckBox.qml
        qml/AppRadioButton.qml
        qml/Main.qml
        qml/LoginPage.qml
        qml/BookListPage.qml
        qml/ServerSetupPage.qml
        qml/BookFormDialog.qml
        qml/DeleteConfirmDialog.qml
        qml/AdminPanelPage.qml
        qml/UserFormDialog.qml
        qml/CsvImportDialog.qml
    SOURCES
        viewmodels/BookListViewModel.h
        viewmodels/BookFormViewModel.h
        viewmodels/UserListViewModel.h
        viewmodels/UserFormViewModel.h
        viewmodels/CsvViewModel.h
)

set_source_files_properties(qml/Theme.qml PROPERTIES
    QT_QML_SINGLETON_TYPE true
)

target_include_directories(alexandria_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/viewmodels
)

target_link_libraries(alexandria_client
    PRIVATE
        Qt6::Quick
        Qt6::Concurrent
        alexandria_client_core
)

install(TARGETS alexandria_client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()