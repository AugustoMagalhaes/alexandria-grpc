#include "core/AlexandriaClient.h"

namespace {

alexandria::v1::Role toProto(Role role)
{
    return role == Role::Admin ? alexandria::v1::ROLE_ADMIN : alexandria::v1::ROLE_USER;
}

Role fromProto(alexandria::v1::Role role)
{
    return role == alexandria::v1::ROLE_ADMIN ? Role::Admin : Role::User;
}

Book fromProto(const alexandria::v1::Book& proto)
{
    Book book;
    book.id = proto.id();
    book.title = proto.title();
    book.author = proto.author();
    book.isbn = proto.isbn();
    book.totalCopies = proto.total_copies();
    book.availableCopies = proto.available_copies();
    return book;
}

User fromProto(const alexandria::v1::User& proto)
{
    User user;
    user.id = proto.id();
    user.username = proto.username();
    user.role = fromProto(proto.role());
    return user;
}

bool isConnectivity(const grpc::Status& status)
{
    return status.error_code() == grpc::StatusCode::UNAVAILABLE;
}

}

AlexandriaClient::AlexandriaClient(const std::string& serverAddress)
{
    m_channel = grpc::CreateChannel(serverAddress, grpc::InsecureChannelCredentials());
    m_authStub = alexandria::v1::AuthService::NewStub(m_channel);
    m_bookStub = alexandria::v1::BookService::NewStub(m_channel);
    m_userStub = alexandria::v1::UserService::NewStub(m_channel);
}

void AlexandriaClient::attachToken(grpc::ClientContext& context) const
{
    std::lock_guard<std::mutex> lock(m_tokenMutex);
    if (!m_token.empty()) {
        context.AddMetadata("authorization", m_token);
    }
}

bool AlexandriaClient::isConnectivityError(const grpc::Status& status)
{
    return isConnectivity(status);
}

ClientResult<void> AlexandriaClient::checkConnection(int timeoutMs)
{
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(timeoutMs);

    if (m_channel->WaitForConnected(deadline)) {
        return ClientResult<void>::ok();
    }

    return ClientResult<void>::fail("Could not reach the server at the configured address.", true);
}

ClientResult<Role> AlexandriaClient::login(const std::string& username, const std::string& password)
{
    alexandria::v1::LoginRequest request;
    request.set_username(username);
    request.set_password(password);

    alexandria::v1::LoginResponse response;
    grpc::ClientContext context;

    grpc::Status status = m_authStub->Login(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<Role>::fail(status.error_message(), isConnectivity(status));
    }

    {
        std::lock_guard<std::mutex> lock(m_tokenMutex);
        m_token = response.token();
        m_role = fromProto(response.role());
        m_authenticated = true;
    }

    return ClientResult<Role>::ok(m_role);
}

void AlexandriaClient::logout()
{
    std::lock_guard<std::mutex> lock(m_tokenMutex);
    m_token.clear();
    m_authenticated = false;
}

bool AlexandriaClient::isAuthenticated() const
{
    std::lock_guard<std::mutex> lock(m_tokenMutex);
    return m_authenticated;
}

Role AlexandriaClient::currentRole() const
{
    std::lock_guard<std::mutex> lock(m_tokenMutex);
    return m_role;
}

ClientResult<Book> AlexandriaClient::createBook(const std::string& title, const std::string& author, const std::string& isbn, int totalCopies)
{
    alexandria::v1::CreateBookRequest request;
    request.set_title(title);
    request.set_author(author);
    request.set_isbn(isbn);
    request.set_total_copies(totalCopies);

    alexandria::v1::CreateBookResponse response;
    grpc::ClientContext context;
    attachToken(context);

    grpc::Status status = m_bookStub->CreateBook(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<Book>::fail(status.error_message(), isConnectivity(status));
    }

    return ClientResult<Book>::ok(fromProto(response.book()));
}

ClientResult<Book> AlexandriaClient::getBook(int id)
{
    alexandria::v1::GetBookRequest request;
    request.set_id(id);

    alexandria::v1::GetBookResponse response;
    grpc::ClientContext context;
    attachToken(context);

    grpc::Status status = m_bookStub->GetBook(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<Book>::fail(status.error_message(), isConnectivity(status));
    }

    return ClientResult<Book>::ok(fromProto(response.book()));
}

ClientResult<std::vector<Book>> AlexandriaClient::listBooks(const std::string& search)
{
    alexandria::v1::ListBooksRequest request;
    request.set_search(search);

    alexandria::v1::ListBooksResponse response;
    grpc::ClientContext context;
    attachToken(context);

    grpc::Status status = m_bookStub->ListBooks(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<std::vector<Book>>::fail(status.error_message(), isConnectivity(status));
    }

    std::vector<Book> books;
    for (const auto& protoBook : response.books()) {
        books.push_back(fromProto(protoBook));
    }

    return ClientResult<std::vector<Book>>::ok(std::move(books));
}

ClientResult<void> AlexandriaClient::updateBook(const Book& book)
{
    alexandria::v1::UpdateBookRequest request;
    auto* protoBook = request.mutable_book();
    protoBook->set_id(book.id);
    protoBook->set_title(book.title);
    protoBook->set_author(book.author);
    protoBook->set_isbn(book.isbn);
    protoBook->set_total_copies(book.totalCopies);
    protoBook->set_available_copies(book.availableCopies);

    alexandria::v1::UpdateBookResponse response;
    grpc::ClientContext context;
    attachToken(context);

    grpc::Status status = m_bookStub->UpdateBook(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<void>::fail(status.error_message(), isConnectivity(status));
    }

    return ClientResult<void>::ok();
}

ClientResult<void> AlexandriaClient::deleteBook(int id)
{
    alexandria::v1::DeleteBookRequest request;
    request.set_id(id);

    alexandria::v1::DeleteBookResponse response;
    grpc::ClientContext context;
    attachToken(context);

    grpc::Status status = m_bookStub->DeleteBook(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<void>::fail(status.error_message(), isConnectivity(status));
    }

    return ClientResult<void>::ok();
}

ClientResult<User> AlexandriaClient::createUser(const std::string& username, const std::string& password, Role role)
{
    alexandria::v1::CreateUserRequest request;
    request.set_username(username);
    request.set_password(password);
    request.set_role(toProto(role));

    alexandria::v1::CreateUserResponse response;
    grpc::ClientContext context;
    attachToken(context);

    grpc::Status status = m_userStub->CreateUser(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<User>::fail(status.error_message(), isConnectivity(status));
    }

    return ClientResult<User>::ok(fromProto(response.user()));
}

ClientResult<std::vector<User>> AlexandriaClient::listUsers()
{
    alexandria::v1::ListUsersRequest request;
    alexandria::v1::ListUsersResponse response;
    grpc::ClientContext context;
    attachToken(context);

    grpc::Status status = m_userStub->ListUsers(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<std::vector<User>>::fail(status.error_message(), isConnectivity(status));
    }

    std::vector<User> users;
    for (const auto& protoUser : response.users()) {
        users.push_back(fromProto(protoUser));
    }

    return ClientResult<std::vector<User>>::ok(std::move(users));
}

ClientResult<void> AlexandriaClient::deleteUser(int id)
{
    alexandria::v1::DeleteUserRequest request;
    request.set_id(id);

    alexandria::v1::DeleteUserResponse response;
    grpc::ClientContext context;
    attachToken(context);

    grpc::Status status = m_userStub->DeleteUser(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<void>::fail(status.error_message(), isConnectivity(status));
    }

    return ClientResult<void>::ok();
}

ClientResult<std::string> AlexandriaClient::exportBooksCsv()
{
    alexandria::v1::ExportBooksRequest request;
    alexandria::v1::ExportBooksResponse response;
    grpc::ClientContext context;
    attachToken(context);

    grpc::Status status = m_bookStub->ExportBooks(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<std::string>::fail(status.error_message(), isConnectivity(status));
    }

    return ClientResult<std::string>::ok(response.csv_data());
}

ClientResult<CsvImportSummary> AlexandriaClient::importBooksCsv(const std::string& csvData, bool replace)
{
    alexandria::v1::ImportBooksRequest request;
    request.set_csv_data(csvData);
    request.set_mode(replace ? alexandria::v1::CSV_IMPORT_MODE_REPLACE : alexandria::v1::CSV_IMPORT_MODE_APPEND);

    alexandria::v1::ImportBooksResponse response;
    grpc::ClientContext context;
    attachToken(context);

    grpc::Status status = m_bookStub->ImportBooks(&context, request, &response);

    if (!status.ok()) {
        return ClientResult<CsvImportSummary>::fail(status.error_message(), isConnectivity(status));
    }

    if (!response.success()) {
        return ClientResult<CsvImportSummary>::fail(response.error());
    }

    CsvImportSummary summary;
    summary.importedCount = response.imported_count();
    summary.skippedCount = response.skipped_count();
    return ClientResult<CsvImportSummary>::ok(summary);
}