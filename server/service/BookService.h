#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/Book.h"
#include "repository/IBookRepository.h"
#include "service/OperationResult.h"
#include "service/ServiceResult.h"

class BookService {
public:
    explicit BookService(IBookRepository& repository);

    ServiceResult<Book> createBook(const std::string& title, const std::string& author, const std::string& isbn, int totalCopies);
    std::optional<Book> getBook(int id);
    std::vector<Book> listBooks(const std::string& search);
    OperationResult updateBook(const Book& book);
    OperationResult deleteBook(int id);

private:
    IBookRepository& m_repository;

    bool isbnAlreadyRegistered(const std::string& isbn, int excludeId = 0);
};