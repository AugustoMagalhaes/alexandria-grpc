#pragma once

#include "auth/AuthService.h"
#include "book.grpc.pb.h"
#include "service/BookService.h"

class BookServiceImpl final : public alexandria::v1::BookService::Service {
public:
    BookServiceImpl(BookService& bookService, AuthService& authService);

    grpc::Status CreateBook(grpc::ServerContext* context, const alexandria::v1::CreateBookRequest* request, alexandria::v1::CreateBookResponse* response) override;
    grpc::Status GetBook(grpc::ServerContext* context, const alexandria::v1::GetBookRequest* request, alexandria::v1::GetBookResponse* response) override;
    grpc::Status ListBooks(grpc::ServerContext* context, const alexandria::v1::ListBooksRequest* request, alexandria::v1::ListBooksResponse* response) override;
    grpc::Status UpdateBook(grpc::ServerContext* context, const alexandria::v1::UpdateBookRequest* request, alexandria::v1::UpdateBookResponse* response) override;
    grpc::Status DeleteBook(grpc::ServerContext* context, const alexandria::v1::DeleteBookRequest* request, alexandria::v1::DeleteBookResponse* response) override;

private:
    BookService& m_bookService;
    AuthService& m_authService;
};