#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "auth/AuthService.h"
#include "auth/PasswordHasher.h"
#include "mocks/MockUserRepository.h"

using ::testing::Return;

namespace {

class AuthServiceTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        service = std::make_unique<AuthService>(repository, sessionManager);
    }

    MockUserRepository repository;
    SessionManager sessionManager;
    std::unique_ptr<AuthService> service;
};

UserCredentials sampleCredentials(const std::string& plainPassword)
{
    UserCredentials credentials;
    credentials.user.id = 1;
    credentials.user.username = "librarian";
    credentials.user.role = Role::Admin;
    credentials.passwordHash = PasswordHasher::hash(plainPassword);
    return credentials;
}

}

TEST_F(AuthServiceTest, LoginFailsWhenUserDoesNotExist)
{
    EXPECT_CALL(repository, findCredentialsByUsername("ghost"))
    .WillOnce(Return(std::nullopt));

    auto result = service->login("ghost", "any-password");

    EXPECT_FALSE(result.has_value());
}

TEST_F(AuthServiceTest, LoginFailsWhenPasswordIsWrong)
{
    EXPECT_CALL(repository, findCredentialsByUsername("librarian"))
    .WillOnce(Return(sampleCredentials("correct-password")));

    auto result = service->login("librarian", "wrong-password");

    EXPECT_FALSE(result.has_value());
}

TEST_F(AuthServiceTest, LoginSucceedsAndReturnsToken)
{
    EXPECT_CALL(repository, findCredentialsByUsername("librarian"))
    .WillOnce(Return(sampleCredentials("correct-password")));

    auto result = service->login("librarian", "correct-password");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->user.username, "librarian");
    EXPECT_FALSE(result->token.empty());
}

TEST_F(AuthServiceTest, ValidateTokenReturnsUserAfterLogin)
{
    EXPECT_CALL(repository, findCredentialsByUsername("librarian"))
    .WillOnce(Return(sampleCredentials("correct-password")));

    auto loginResult = service->login("librarian", "correct-password");
    ASSERT_TRUE(loginResult.has_value());

    auto validated = service->validateToken(loginResult->token);

    ASSERT_TRUE(validated.has_value());
    EXPECT_EQ(validated->username, "librarian");
}

TEST_F(AuthServiceTest, ValidateTokenFailsForUnknownToken)
{
    auto validated = service->validateToken("nonexistent-token");

    EXPECT_FALSE(validated.has_value());
}