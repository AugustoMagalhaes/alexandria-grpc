#include "service/BookService.h"

BookService::BookService(IBookRepository& repository)
    : m_repository(repository)
{
}

bool BookService::isbnAlreadyRegistered(const std::string& isbn, int excludeId)
{
    auto existing = m_repository.findByIsbn(isbn);
    return existing.has_value() && existing->id != excludeId;
}

ServiceResult<Book> BookService::createBook(const std::string& title, const std::string& author, const std::string& isbn, int totalCopies)
{
    if (title.empty()) {
        return ServiceResult<Book>::fail("Title is required");
    }

    if (author.empty()) {
        return ServiceResult<Book>::fail("Author is required");
    }

    if (isbn.empty()) {
        return ServiceResult<Book>::fail("ISBN is required");
    }

    if (totalCopies < 1) {
        return ServiceResult<Book>::fail("A book must have at least one copy");
    }

    if (isbnAlreadyRegistered(isbn)) {
        return ServiceResult<Book>::fail("A book with this ISBN is already registered");
    }

    Book book;
    book.title = title;
    book.author = author;
    book.isbn = isbn;
    book.totalCopies = totalCopies;
    book.availableCopies = totalCopies;

    auto created = m_repository.create(book);

    if (!created.has_value()) {
        return ServiceResult<Book>::fail("Failed to persist the book");
    }

    return ServiceResult<Book>::ok(*created);
}

std::optional<Book> BookService::getBook(int id)
{
    return m_repository.findById(id);
}

std::vector<Book> BookService::listBooks(const std::string& search)
{
    return m_repository.findAll(search);
}

OperationResult BookService::updateBook(const Book& book)
{
    if (book.title.empty() || book.author.empty() || book.isbn.empty()) {
        return OperationResult::fail("Title, author and ISBN are required");
    }

    if (book.availableCopies < 0 || book.availableCopies > book.totalCopies) {
        return OperationResult::fail("Available copies must be between 0 and the total copies");
    }

    if (isbnAlreadyRegistered(book.isbn, book.id)) {
        return OperationResult::fail("A book with this ISBN is already registered");
    }

    if (!m_repository.update(book)) {
        return OperationResult::fail("Book not found");
    }

    return OperationResult::ok();
}

OperationResult BookService::deleteBook(int id)
{
    if (!m_repository.remove(id)) {
        return OperationResult::fail("Book not found");
    }

    return OperationResult::ok();
}