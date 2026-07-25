#pragma once

#include <optional>
#include <string>
#include <vector>

#include "domain/User.h"

struct UserCredentials {
    User user;
    std::string passwordHash;
};

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual std::optional<User> create(const User& user, const std::string& passwordHash) = 0;
    virtual std::vector<User> findAll() = 0;
    virtual bool remove(int id) = 0;
    virtual std::optional<UserCredentials> findCredentialsByUsername(const std::string& username) = 0;
};