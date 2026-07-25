#include <gtest/gtest.h>

#include "persistence/Database.h"
#include "persistence/SqliteBookRepository.h"

namespace {

class SqliteBookRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        database = std::make_unique<Database>(":memory:");
        database->migrate();
        repository = std::make_unique<SqliteBookRepository>(*database);
    }

    std::unique_ptr<Database> database;
    std::unique_ptr<SqliteBookRepository> repository;
};

Book sampleBook()
{
    Book book;
    book.title = "Clean Code";
    book.author = "Robert Martin";
    book.isbn = "9780132350884";
    book.totalCopies = 3;
    book.availableCopies = 3;
    return book;
}

}

TEST_F(SqliteBookRepositoryTest, CreateReturnsBookWithId)
{
    auto created = repository->create(sampleBook());

    ASSERT_TRUE(created.has_value());
    EXPECT_GT(created->id, 0);
    EXPECT_EQ(created->title, "Clean Code");
}

TEST_F(SqliteBookRepositoryTest, FindByIdReturnsNulloptWhenMissing)
{
    auto found = repository->findById(999);

    EXPECT_FALSE(found.has_value());
}

TEST_F(SqliteBookRepositoryTest, FindByIdReturnsCreatedBook)
{
    auto created = repository->create(sampleBook());
    auto found = repository->findById(created->id);

    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->isbn, "9780132350884");
}

TEST_F(SqliteBookRepositoryTest, FindAllFiltersBySearchTerm)
{
    repository->create(sampleBook());

    Book other;
    other.title = "The Pragmatic Programmer";
    other.author = "Andy Hunt";
    other.isbn = "9780135957059";
    other.totalCopies = 2;
    other.availableCopies = 2;
    repository->create(other);

    auto results = repository->findAll("Clean");

    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0].title, "Clean Code");
}

TEST_F(SqliteBookRepositoryTest, UpdateModifiesExistingBook)
{
    auto created = repository->create(sampleBook());
    created->availableCopies = 1;

    const bool updated = repository->update(*created);
    auto found = repository->findById(created->id);

    EXPECT_TRUE(updated);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->availableCopies, 1);
}

TEST_F(SqliteBookRepositoryTest, RemoveDeletesBook)
{
    auto created = repository->create(sampleBook());

    const bool removed = repository->remove(created->id);
    auto found = repository->findById(created->id);

    EXPECT_TRUE(removed);
    EXPECT_FALSE(found.has_value());
}