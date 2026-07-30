#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "auth.grpc.pb.h"
#include "book.grpc.pb.h"
#include "user.grpc.pb.h"

#include "core/ClientResult.h"
#include "domain/Book.h"
#include "domain/User.h"

struct CsvImportSummary {
    int importedCount = 0;
    int skippedCount = 0;
};

class AlexandriaClient {
public:
    explicit AlexandriaClient(const std::string& serverAddress);

    static bool isConnectivityError(const grpc::Status& status);

    ClientResult<void> checkConnection(int timeoutMs);

    ClientResult<Role> login(const std::string& username, const std::string& password);
    ClientResult<Role> validateToken(const std::string& token);
    ClientResult<void> updateUser(int id, const std::string& password, Role role);
    void logout();
    bool isAuthenticated() const;
    Role currentRole() const;
    void setToken(const std::string& token);
    std::string token() const;

    ClientResult<Book> createBook(
        const std::string& title,
        const std::string& author,
        const std::string& isbn,
        int totalCopies,
        const std::string& schoolCode = "",
        const std::string& category = "",
        const std::string& keywords = "",
        bool borrowable = true
    );

    ClientResult<Book> getBook(int id);
    ClientResult<std::vector<Book>> listBooks(const std::string& search);
    ClientResult<void> updateBook(const Book& book);
    ClientResult<void> deleteBook(int id);

    ClientResult<User> createUser(const std::string& username, const std::string& password, Role role);
    ClientResult<std::vector<User>> listUsers();
    ClientResult<void> deleteUser(int id);

    ClientResult<std::string> exportBooksCsv();
    ClientResult<CsvImportSummary> importBooksCsv(const std::string& csvData, bool replace);

private:
    std::shared_ptr<grpc::Channel> m_channel;
    std::unique_ptr<alexandria::v1::AuthService::Stub> m_authStub;
    std::unique_ptr<alexandria::v1::BookService::Stub> m_bookStub;
    std::unique_ptr<alexandria::v1::UserService::Stub> m_userStub;

    mutable std::mutex m_tokenMutex;
    std::string m_token;
    Role m_role = Role::User;
    bool m_authenticated = false;

    void attachToken(grpc::ClientContext& context) const;
};