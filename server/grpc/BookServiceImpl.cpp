#include "grpc/BookServiceImpl.h"

#include "grpc/AuthGuard.h"
#include "grpc/ProtoConverters.h"

BookServiceImpl::BookServiceImpl(BookService& bookService, CsvService& csvService, AuthService& authService)
    : m_bookService(bookService)
    , m_csvService(csvService)
    , m_authService(authService)
{
}

grpc::Status BookServiceImpl::CreateBook(grpc::ServerContext* context, const alexandria::v1::CreateBookRequest* request, alexandria::v1::CreateBookResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::Admin, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    auto result = m_bookService.createBook(
        request->title(),
        request->author(),
        request->isbn(),
        request->total_copies(),
        request->school_code(),
        request->category(),
        request->keywords(),
        request->borrowable()
        );

    if (!result.success) {
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, result.error);
    }

    *response->mutable_book() = converters::toProto(*result.value);
    return grpc::Status::OK;
}

grpc::Status BookServiceImpl::GetBook(grpc::ServerContext* context, const alexandria::v1::GetBookRequest* request, alexandria::v1::GetBookResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::User, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    auto book = m_bookService.getBook(request->id());

    if (!book.has_value()) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, "Book not found");
    }

    *response->mutable_book() = converters::toProto(*book);
    return grpc::Status::OK;
}

grpc::Status BookServiceImpl::ListBooks(grpc::ServerContext* context, const alexandria::v1::ListBooksRequest* request, alexandria::v1::ListBooksResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::User, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    for (const Book& book : m_bookService.listBooks(request->search())) {
        *response->add_books() = converters::toProto(book);
    }

    return grpc::Status::OK;
}

grpc::Status BookServiceImpl::UpdateBook(grpc::ServerContext* context, const alexandria::v1::UpdateBookRequest* request, alexandria::v1::UpdateBookResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::Admin, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    Book book = converters::fromProto(request->book());
    auto result = m_bookService.updateBook(book);

    if (!result.success) {
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, result.error);
    }

    *response->mutable_book() = request->book();
    return grpc::Status::OK;
}

grpc::Status BookServiceImpl::DeleteBook(grpc::ServerContext* context, const alexandria::v1::DeleteBookRequest* request, alexandria::v1::DeleteBookResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::Admin, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    auto result = m_bookService.deleteBook(request->id());

    if (!result.success) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, result.error);
    }

    response->set_success(true);
    return grpc::Status::OK;
}

grpc::Status BookServiceImpl::ExportBooks(grpc::ServerContext* context, const alexandria::v1::ExportBooksRequest*, alexandria::v1::ExportBooksResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::Admin, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    response->set_csv_data(m_csvService.exportBooksToString());
    return grpc::Status::OK;
}

grpc::Status BookServiceImpl::ImportBooks(grpc::ServerContext* context, const alexandria::v1::ImportBooksRequest* request, alexandria::v1::ImportBooksResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::Admin, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    const CsvImportMode mode = request->mode() == alexandria::v1::CSV_IMPORT_MODE_REPLACE
                                   ? CsvImportMode::Replace
                                   : CsvImportMode::Append;

    auto result = m_csvService.importBooksFromString(request->csv_data(), mode);

    response->set_success(result.success);
    response->set_error(result.error);
    response->set_imported_count(result.importedCount);
    response->set_skipped_count(result.skippedCount);

    return grpc::Status::OK;
}