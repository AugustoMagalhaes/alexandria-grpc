#pragma once

#include <gmock/gmock.h>

#include "repository/IUserRepository.h"

class MockUserRepository : public IUserRepository {
public:
    MOCK_METHOD(std::optional<User>, create, (const User& user, const std::string& passwordHash), (override));
    MOCK_METHOD(std::vector<User>, findAll, (), (override));
    MOCK_METHOD(bool, remove, (int id), (override));
    MOCK_METHOD(std::optional<UserCredentials>, findCredentialsByUsername, (const std::string& username), (override));
};