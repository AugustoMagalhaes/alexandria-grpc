#include <gtest/gtest.h>

#include "persistence/Database.h"
#include "persistence/SqliteUserRepository.h"

namespace {

class SqliteUserRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        database = std::make_unique<Database>(":memory:");
        database->migrate();
        repository = std::make_unique<SqliteUserRepository>(*database);
    }

    std::unique_ptr<Database> database;
    std::unique_ptr<SqliteUserRepository> repository;
};

User sampleUser()
{
    User user;
    user.username = "librarian";
    user.role = Role::Admin;
    return user;
}

}

TEST_F(SqliteUserRepositoryTest, CreateReturnsUserWithId)
{
    auto created = repository->create(sampleUser(), "hashed_password");

    ASSERT_TRUE(created.has_value());
    EXPECT_GT(created->id, 0);
    EXPECT_EQ(created->username, "librarian");
}

TEST_F(SqliteUserRepositoryTest, FindCredentialsReturnsNulloptWhenMissing)
{
    auto credentials = repository->findCredentialsByUsername("ghost");

    EXPECT_FALSE(credentials.has_value());
}

TEST_F(SqliteUserRepositoryTest, FindCredentialsReturnsStoredHash)
{
    repository->create(sampleUser(), "hashed_password");

    auto credentials = repository->findCredentialsByUsername("librarian");

    ASSERT_TRUE(credentials.has_value());
    EXPECT_EQ(credentials->passwordHash, "hashed_password");
    EXPECT_EQ(credentials->user.role, Role::Admin);
}

TEST_F(SqliteUserRepositoryTest, FindAllListsUsers)
{
    repository->create(sampleUser(), "hashed_password");

    auto users = repository->findAll();

    ASSERT_EQ(users.size(), 1u);
    EXPECT_EQ(users[0].username, "librarian");
}

TEST_F(SqliteUserRepositoryTest, RemoveDeletesUser)
{
    auto created = repository->create(sampleUser(), "hashed_password");

    const bool removed = repository->remove(created->id);
    auto users = repository->findAll();

    EXPECT_TRUE(removed);
    EXPECT_TRUE(users.empty());
}