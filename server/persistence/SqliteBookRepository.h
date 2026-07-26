#pragma once

#include "persistence/Database.h"
#include "repository/IBookRepository.h"

class SqliteBookRepository : public IBookRepository {
public:
    explicit SqliteBookRepository(Database& database);

    std::optional<Book> create(const Book& book) override;
    std::optional<Book> findById(int id) override;
    std::vector<Book> findAll(const std::string& search) override;
    bool update(const Book& book) override;
    bool remove(int id) override;
    std::optional<Book> findByIsbn(const std::string& isbn) override;

private:
    Database& m_database;
};