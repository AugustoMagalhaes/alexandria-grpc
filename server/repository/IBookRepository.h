#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/Book.h"

class IBookRepository {
public:
    virtual ~IBookRepository() = default;

    virtual std::optional<Book> create(const Book& book) = 0;
    virtual std::optional<Book> findById(int id) = 0;
    virtual std::vector<Book> findAll(const std::string& search) = 0;
    virtual bool update(const Book& book) = 0;
    virtual bool remove(int id) = 0;
    virtual std::optional<Book> findByIsbn(const std::string& isbn) = 0;
};