#pragma once

#include "persistence/Database.h"
#include "repository/IUserRepository.h"

class SqliteUserRepository : public IUserRepository {
public:
    explicit SqliteUserRepository(Database& database);

    std::optional<User> create(const User& user, const std::string& passwordHash) override;
    std::vector<User> findAll() override;
    bool remove(int id) override;
    bool updateRole(int id, Role role) override;
    bool updatePassword(int id, const std::string& passwordHash) override;
    std::optional<UserCredentials> findCredentialsByUsername(const std::string& username) override;

private:
    Database& m_database;
};