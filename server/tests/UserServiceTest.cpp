#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "service/UserService.h"
#include "mocks/MockUserRepository.h"

using ::testing::_;
using ::testing::Return;

namespace {

class UserServiceTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        service = std::make_unique<UserService>(repository);
    }

    MockUserRepository repository;
    std::unique_ptr<UserService> service;
};

}

TEST_F(UserServiceTest, CreateUserFailsWithShortPassword)
{
    auto result = service->createUser("librarian", "123", Role::Admin);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "Password must have at least 6 characters");
}

TEST_F(UserServiceTest, CreateUserFailsWhenUsernameTaken)
{
    UserCredentials existing;
    existing.user.username = "librarian";

    EXPECT_CALL(repository, findCredentialsByUsername("librarian"))
        .WillOnce(Return(existing));

    auto result = service->createUser("librarian", "password123", Role::Admin);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "Username is already taken");
}

TEST_F(UserServiceTest, CreateUserSucceeds)
{
    EXPECT_CALL(repository, findCredentialsByUsername("librarian"))
    .WillOnce(Return(std::nullopt));

    EXPECT_CALL(repository, create(_, _)).WillOnce([](const User& user, const std::string&) {
        User created = user;
        created.id = 1;
        return created;
    });

    auto result = service->createUser("librarian", "password123", Role::Admin);

    ASSERT_TRUE(result.success);
    EXPECT_EQ(result.value->username, "librarian");
}

TEST_F(UserServiceTest, DeleteUserFailsWhenNotFound)
{
    EXPECT_CALL(repository, findAll()).WillOnce(Return(std::vector<User>{}));

    auto result = service->deleteUser(1);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "User not found");
}

TEST_F(UserServiceTest, DeleteUserFailsWhenLastAdmin)
{
    User admin;
    admin.id = 1;
    admin.role = Role::Admin;

    EXPECT_CALL(repository, findAll()).WillOnce(Return(std::vector<User>{admin}));

    auto result = service->deleteUser(1);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "Cannot delete the last remaining admin");
}

TEST_F(UserServiceTest, DeleteUserSucceedsWhenAnotherAdminExists)
{
    User admin1;
    admin1.id = 1;
    admin1.role = Role::Admin;

    User admin2;
    admin2.id = 2;
    admin2.role = Role::Admin;

    EXPECT_CALL(repository, findAll()).WillOnce(Return(std::vector<User>{admin1, admin2}));
    EXPECT_CALL(repository, remove(1)).WillOnce(Return(true));

    auto result = service->deleteUser(1);

    EXPECT_TRUE(result.success);
}

TEST_F(UserServiceTest, DeleteUserSucceedsForRegularUser)
{
    User regular;
    regular.id = 3;
    regular.role = Role::User;

    EXPECT_CALL(repository, findAll()).WillOnce(Return(std::vector<User>{regular}));
    EXPECT_CALL(repository, remove(3)).WillOnce(Return(true));

    auto result = service->deleteUser(3);

    EXPECT_TRUE(result.success);
}