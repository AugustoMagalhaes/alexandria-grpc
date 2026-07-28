#include <gtest/gtest.h>

#include <memory>
#include <thread>

#include <grpcpp/grpcpp.h>

#include "auth/AuthService.h"
#include "auth/SessionManager.h"
#include "core/AlexandriaClient.h"
#include "grpc/AuthServiceImpl.h"
#include "grpc/BookServiceImpl.h"
#include "grpc/UserServiceImpl.h"
#include "persistence/Database.h"
#include "persistence/SqliteBookRepository.h"
#include "persistence/SqliteUserRepository.h"
#include "service/BookService.h"
#include "service/CsvService.h"
#include "service/UserService.h"

namespace {

constexpr char kTestServerAddress[] = "127.0.0.1:50151";

class IntegrationTest : public ::testing::Test {
protected:
    static void SetUpTestSuite()
    {
        database = std::make_unique<Database>(":memory:");
        database->migrate();

        bookRepository = std::make_unique<SqliteBookRepository>(*database);
        userRepository = std::make_unique<SqliteUserRepository>(*database);
        sessionManager = std::make_unique<SessionManager>();

        bookService = std::make_unique<BookService>(*bookRepository);
        csvService = std::make_unique<CsvService>(*bookRepository);
        userService = std::make_unique<UserService>(*userRepository);
        authService = std::make_unique<AuthService>(*userRepository, *sessionManager);

        userService->createUser("admin", "admin123", Role::Admin);
        userService->createUser("reader", "reader123", Role::User);

        bookServiceImpl = std::make_unique<BookServiceImpl>(*bookService, *csvService, *authService);
        userServiceImpl = std::make_unique<UserServiceImpl>(*userService, *authService);
        authServiceImpl = std::make_unique<AuthServiceImpl>(*authService);

        grpc::ServerBuilder builder;
        builder.AddListeningPort(kTestServerAddress, grpc::InsecureServerCredentials());
        builder.RegisterService(bookServiceImpl.get());
        builder.RegisterService(userServiceImpl.get());
        builder.RegisterService(authServiceImpl.get());

        server = builder.BuildAndStart();
    }

    static void TearDownTestSuite()
    {
        server->Shutdown();
        server.reset();
    }

    void SetUp() override
    {
        client = std::make_unique<AlexandriaClient>(kTestServerAddress);
    }

    std::unique_ptr<AlexandriaClient> client;

    static std::unique_ptr<Database> database;
    static std::unique_ptr<SqliteBookRepository> bookRepository;
    static std::unique_ptr<SqliteUserRepository> userRepository;
    static std::unique_ptr<SessionManager> sessionManager;
    static std::unique_ptr<BookService> bookService;
    static std::unique_ptr<CsvService> csvService;
    static std::unique_ptr<UserService> userService;
    static std::unique_ptr<AuthService> authService;
    static std::unique_ptr<BookServiceImpl> bookServiceImpl;
    static std::unique_ptr<UserServiceImpl> userServiceImpl;
    static std::unique_ptr<AuthServiceImpl> authServiceImpl;
    static std::unique_ptr<grpc::Server> server;
};

std::unique_ptr<Database> IntegrationTest::database;
std::unique_ptr<SqliteBookRepository> IntegrationTest::bookRepository;
std::unique_ptr<SqliteUserRepository> IntegrationTest::userRepository;
std::unique_ptr<SessionManager> IntegrationTest::sessionManager;
std::unique_ptr<BookService> IntegrationTest::bookService;
std::unique_ptr<CsvService> IntegrationTest::csvService;
std::unique_ptr<UserService> IntegrationTest::userService;
std::unique_ptr<AuthService> IntegrationTest::authService;
std::unique_ptr<BookServiceImpl> IntegrationTest::bookServiceImpl;
std::unique_ptr<UserServiceImpl> IntegrationTest::userServiceImpl;
std::unique_ptr<AuthServiceImpl> IntegrationTest::authServiceImpl;
std::unique_ptr<grpc::Server> IntegrationTest::server;

}

TEST_F(IntegrationTest, LoginWithValidCredentialsSucceeds)
{
    auto result = client->login("admin", "admin123");

    ASSERT_TRUE(result.success);
    EXPECT_EQ(*result.value, Role::Admin);
}

TEST_F(IntegrationTest, LoginWithInvalidCredentialsFails)
{
    auto result = client->login("admin", "wrong-password");

    EXPECT_FALSE(result.success);
}

TEST_F(IntegrationTest, CreateBookWithoutAuthenticationFails)
{
    auto result = client->createBook("Unauthorized Book", "Author", "000", 1);

    EXPECT_FALSE(result.success);
}

TEST_F(IntegrationTest, CreateBookAsUserFails)
{
    client->login("reader", "reader123");

    auto result = client->createBook("User Book", "Author", "111", 1);

    EXPECT_FALSE(result.success);
}

TEST_F(IntegrationTest, FullBookLifecycleAsAdmin)
{
    auto login = client->login("admin", "admin123");
    ASSERT_TRUE(login.success);

    auto created = client->createBook("Integration Test Book", "Test Author", "999-INTEGRATION", 2);
    ASSERT_TRUE(created.success);
    EXPECT_GT(created.value->id, 0);
    EXPECT_EQ(created.value->availableCopies, 2);

    auto listed = client->listBooks("Integration Test");
    ASSERT_TRUE(listed.success);
    ASSERT_EQ(listed.value->size(), 1u);
    EXPECT_EQ((*listed.value)[0].title, "Integration Test Book");

    Book toUpdate = (*listed.value)[0];
    toUpdate.availableCopies = 1;
    auto updated = client->updateBook(toUpdate);
    EXPECT_TRUE(updated.success);

    auto fetched = client->getBook(toUpdate.id);
    ASSERT_TRUE(fetched.success);
    EXPECT_EQ(fetched.value->availableCopies, 1);

    auto deleted = client->deleteBook(toUpdate.id);
    EXPECT_TRUE(deleted.success);

    auto afterDelete = client->getBook(toUpdate.id);
    EXPECT_FALSE(afterDelete.success);
}

TEST_F(IntegrationTest, CsvExportAndImportRoundTrip)
{
    auto login = client->login("admin", "admin123");
    ASSERT_TRUE(login.success);

    client->createBook("CSV Book One", "Author One", "CSV-1", 1);
    client->createBook("CSV Book Two", "Author Two", "CSV-2", 2);

    auto exported = client->exportBooksCsv();
    ASSERT_TRUE(exported.success);
    EXPECT_NE(exported.value->find("CSV Book One"), std::string::npos);
    EXPECT_NE(exported.value->find("CSV Book Two"), std::string::npos);
}