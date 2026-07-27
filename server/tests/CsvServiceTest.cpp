#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>

#include "persistence/Database.h"
#include "persistence/SqliteBookRepository.h"
#include "service/CsvService.h"

namespace {

class CsvServiceTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        database = std::make_unique<Database>(":memory:");
        database->migrate();
        repository = std::make_unique<SqliteBookRepository>(*database);
        service = std::make_unique<CsvService>(*repository);
        filePath = "/tmp/alexandria_csv_test.csv";
    }

    void TearDown() override
    {
        std::remove(filePath.c_str());
    }

    std::unique_ptr<Database> database;
    std::unique_ptr<SqliteBookRepository> repository;
    std::unique_ptr<CsvService> service;
    std::string filePath;
};

std::string readFile(const std::string& path)
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

}

TEST_F(CsvServiceTest, ExportWritesHeaderWhenNoBooks)
{
    auto result = service->exportBooks(filePath);

    ASSERT_TRUE(result.success);
    EXPECT_EQ(readFile(filePath), "title,author,isbn,total_copies\n");
}

TEST_F(CsvServiceTest, ExportWritesBookRows)
{
    Book book;
    book.title = "Clean Code";
    book.author = "Robert Martin";
    book.isbn = "9780132350884";
    book.totalCopies = 3;
    book.availableCopies = 3;
    repository->create(book);

    auto result = service->exportBooks(filePath);

    ASSERT_TRUE(result.success);
    EXPECT_EQ(readFile(filePath), "title,author,isbn,total_copies\nClean Code,Robert Martin,9780132350884,3\n");
}

TEST_F(CsvServiceTest, ExportEscapesFieldsWithComma)
{
    Book book;
    book.title = "War and Peace, Vol. 1";
    book.author = "Tolstoy";
    book.isbn = "111";
    book.totalCopies = 1;
    book.availableCopies = 1;
    repository->create(book);

    auto result = service->exportBooks(filePath);

    ASSERT_TRUE(result.success);
    EXPECT_EQ(readFile(filePath), "title,author,isbn,total_copies\n\"War and Peace, Vol. 1\",Tolstoy,111,1\n");
}

TEST_F(CsvServiceTest, ImportAppendAddsBooksToExisting)
{
    Book existing;
    existing.title = "Existing Book";
    existing.author = "Someone";
    existing.isbn = "000";
    existing.totalCopies = 1;
    existing.availableCopies = 1;
    repository->create(existing);

    std::ofstream file(filePath);
    file << "title,author,isbn,total_copies\n";
    file << "Clean Code,Robert Martin,9780132350884,3\n";
    file.close();

    auto result = service->importBooks(filePath, CsvImportMode::Append);

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.importedCount, 1);
    EXPECT_EQ(repository->findAll("").size(), 2u);
}

TEST_F(CsvServiceTest, ImportReplaceRemovesExistingBooksFirst)
{
    Book existing;
    existing.title = "Existing Book";
    existing.author = "Someone";
    existing.isbn = "000";
    existing.totalCopies = 1;
    existing.availableCopies = 1;
    repository->create(existing);

    std::ofstream file(filePath);
    file << "title,author,isbn,total_copies\n";
    file << "Clean Code,Robert Martin,9780132350884,3\n";
    file.close();

    auto result = service->importBooks(filePath, CsvImportMode::Replace);

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.importedCount, 1);

    auto books = repository->findAll("");
    ASSERT_EQ(books.size(), 1u);
    EXPECT_EQ(books[0].title, "Clean Code");
}

TEST_F(CsvServiceTest, ImportSkipsMalformedRows)
{
    std::ofstream file(filePath);
    file << "title,author,isbn,total_copies\n";
    file << "Valid Book,Author,123,2\n";
    file << "Malformed Row Missing Fields\n";
    file << "Another Valid,Author2,456,not_a_number\n";
    file.close();

    auto result = service->importBooks(filePath, CsvImportMode::Append);

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.importedCount, 1);
    EXPECT_EQ(result.skippedCount, 2);
}

TEST_F(CsvServiceTest, ImportParsesQuotedFieldsWithCommas)
{
    std::ofstream file(filePath);
    file << "title,author,isbn,total_copies\n";
    file << "\"War and Peace, Vol. 1\",Tolstoy,111,1\n";
    file.close();

    auto result = service->importBooks(filePath, CsvImportMode::Append);

    ASSERT_TRUE(result.success);
    ASSERT_EQ(result.importedCount, 1);

    auto books = repository->findAll("");
    ASSERT_EQ(books.size(), 1u);
    EXPECT_EQ(books[0].title, "War and Peace, Vol. 1");
}

TEST_F(CsvServiceTest, ExportFailsWhenPathIsInvalid)
{
    auto result = service->exportBooks("/nonexistent_directory/file.csv");

    EXPECT_FALSE(result.success);
}

TEST_F(CsvServiceTest, ExportToStringMatchesFileExport)
{
    Book book;
    book.title = "Clean Code";
    book.author = "Robert Martin";
    book.isbn = "9780132350884";
    book.totalCopies = 3;
    book.availableCopies = 3;
    repository->create(book);

    std::string csv = service->exportBooksToString();

    EXPECT_EQ(csv, "title,author,isbn,total_copies\nClean Code,Robert Martin,9780132350884,3\n");
}

TEST_F(CsvServiceTest, ImportFromStringAppendsBooks)
{
    std::string csv = "title,author,isbn,total_copies\nClean Code,Robert Martin,9780132350884,3\n";

    auto result = service->importBooksFromString(csv, CsvImportMode::Append);

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.importedCount, 1);
    EXPECT_EQ(repository->findAll("").size(), 1u);
}