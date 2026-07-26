#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "service/BookService.h"
#include "mocks/MockBookRepository.h"

using ::testing::_;
using ::testing::Return;

namespace {

class BookServiceTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        service = std::make_unique<BookService>(repository);
    }

    MockBookRepository repository;
    std::unique_ptr<BookService> service;
};

}

TEST_F(BookServiceTest, CreateBookFailsWithEmptyTitle)
{
    auto result = service->createBook("", "Author", "123", 1);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "Title is required");
}

TEST_F(BookServiceTest, CreateBookFailsWithZeroCopies)
{
    auto result = service->createBook("Title", "Author", "123", 0);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "A book must have at least one copy");
}

TEST_F(BookServiceTest, CreateBookFailsWhenIsbnAlreadyExists)
{
    Book existing;
    existing.id = 5;
    existing.isbn = "123";

    EXPECT_CALL(repository, findByIsbn("123")).WillOnce(Return(existing));

    auto result = service->createBook("Title", "Author", "123", 2);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "A book with this ISBN is already registered");
}

TEST_F(BookServiceTest, CreateBookSucceedsAndSetsAvailableCopiesToTotal)
{
    EXPECT_CALL(repository, findByIsbn("123")).WillOnce(Return(std::nullopt));

    EXPECT_CALL(repository, create(_)).WillOnce([](const Book& book) {
        Book created = book;
        created.id = 1;
        return created;
    });

    auto result = service->createBook("Title", "Author", "123", 3);

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.value->availableCopies, 3);
}

TEST_F(BookServiceTest, UpdateBookFailsWhenIsbnBelongsToAnotherBook)
{
    Book conflicting;
    conflicting.id = 99;
    conflicting.isbn = "999";

    EXPECT_CALL(repository, findByIsbn("999")).WillOnce(Return(conflicting));

    Book book;
    book.id = 1;
    book.title = "Title";
    book.author = "Author";
    book.isbn = "999";
    book.totalCopies = 2;
    book.availableCopies = 2;

    auto result = service->updateBook(book);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "A book with this ISBN is already registered");
}

TEST_F(BookServiceTest, UpdateBookSucceedsWhenIsbnBelongsToSameBook)
{
    Book existing;
    existing.id = 1;
    existing.isbn = "999";

    EXPECT_CALL(repository, findByIsbn("999")).WillOnce(Return(existing));
    EXPECT_CALL(repository, update(_)).WillOnce(Return(true));

    Book book = existing;
    book.title = "Title";
    book.author = "Author";
    book.totalCopies = 2;
    book.availableCopies = 2;

    auto result = service->updateBook(book);

    EXPECT_TRUE(result.success);
}

TEST_F(BookServiceTest, UpdateBookFailsWhenAvailableCopiesExceedTotal)
{
    Book book;
    book.id = 1;
    book.title = "Title";
    book.author = "Author";
    book.isbn = "123";
    book.totalCopies = 2;
    book.availableCopies = 5;

    auto result = service->updateBook(book);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "Available copies must be between 0 and the total copies");
}

TEST_F(BookServiceTest, DeleteBookFailsWhenNotFound)
{
    EXPECT_CALL(repository, remove(42)).WillOnce(Return(false));

    auto result = service->deleteBook(42);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "Book not found");
}