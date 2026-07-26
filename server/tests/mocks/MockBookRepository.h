#pragma once

#include <gmock/gmock.h>

#include "repository/IBookRepository.h"

class MockBookRepository : public IBookRepository {
public:
    MOCK_METHOD(std::optional<Book>, create, (const Book& book), (override));
    MOCK_METHOD(std::optional<Book>, findById, (int id), (override));
    MOCK_METHOD(std::optional<Book>, findByIsbn, (const std::string& isbn), (override));
    MOCK_METHOD(std::vector<Book>, findAll, (const std::string& search), (override));
    MOCK_METHOD(bool, update, (const Book& book), (override));
    MOCK_METHOD(bool, remove, (int id), (override));
};