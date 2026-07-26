#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include <QCoreApplication>
#include <QString>

#include <grpcpp/grpcpp.h>

#include "auth/AuthService.h"
#include "auth/SessionManager.h"
#include "grpc/AuthServiceImpl.h"
#include "grpc/BookServiceImpl.h"
#include "grpc/UserServiceImpl.h"
#include "persistence/Database.h"
#include "persistence/SqliteBookRepository.h"
#include "persistence/SqliteUserRepository.h"
#include "service/BookService.h"
#include "service/UserService.h"

namespace {

std::string getEnvOrDefault(const char* name, const std::string& defaultValue)
{
    const char* value = std::getenv(name);
    return value != nullptr ? std::string(value) : defaultValue;
}

void seedInitialAdminIfNeeded(IUserRepository& userRepository, UserService& userService)
{
    if (!userRepository.findAll().empty()) {
        return;
    }

    auto result = userService.createUser("admin", "admin123", Role::Admin);

    if (result.success) {
        std::cout << "Created initial admin user (username: admin, password: admin123)" << std::endl;
        std::cout << "Please change this password after first login." << std::endl;
    }
}

}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    const std::string databasePath = getEnvOrDefault("ALEXANDRIA_DB_PATH", "alexandria.db");
    const std::string listenAddress = getEnvOrDefault("ALEXANDRIA_LISTEN_ADDRESS", "0.0.0.0:50051");

    Database database(QString::fromStdString(databasePath));

    if (!database.migrate()) {
        std::cerr << "Failed to migrate database schema" << std::endl;
        return 1;
    }

    SqliteBookRepository bookRepository(database);
    SqliteUserRepository userRepository(database);
    SessionManager sessionManager;

    BookService bookService(bookRepository);
    UserService userService(userRepository);
    AuthService authService(userRepository, sessionManager);

    seedInitialAdminIfNeeded(userRepository, userService);

    BookServiceImpl bookServiceImpl(bookService, authService);
    UserServiceImpl userServiceImpl(userService, authService);
    AuthServiceImpl authServiceImpl(authService);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(listenAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&bookServiceImpl);
    builder.RegisterService(&userServiceImpl);
    builder.RegisterService(&authServiceImpl);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

    if (!server) {
        std::cerr << "Failed to start gRPC server on " << listenAddress << std::endl;
        return 1;
    }

    std::cout << "Alexandria server listening on " << listenAddress << std::endl;
    std::cout << "Database: " << databasePath << std::endl;

    server->Wait();

    return 0;
}