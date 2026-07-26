#include "service/UserService.h"

#include <algorithm>

#include "auth/PasswordHasher.h"

namespace {
constexpr size_t kMinimumPasswordLength = 6;
}

UserService::UserService(IUserRepository& repository)
    : m_repository(repository)
{
}

ServiceResult<User> UserService::createUser(const std::string& username, const std::string& password, Role role)
{
    if (username.empty()) {
        return ServiceResult<User>::fail("Username is required");
    }

    if (password.size() < kMinimumPasswordLength) {
        return ServiceResult<User>::fail("Password must have at least 6 characters");
    }

    if (m_repository.findCredentialsByUsername(username).has_value()) {
        return ServiceResult<User>::fail("Username is already taken");
    }

    User user;
    user.username = username;
    user.role = role;

    const std::string passwordHash = PasswordHasher::hash(password);
    auto created = m_repository.create(user, passwordHash);

    if (!created.has_value()) {
        return ServiceResult<User>::fail("Failed to persist the user");
    }

    return ServiceResult<User>::ok(*created);
}

std::vector<User> UserService::listUsers()
{
    return m_repository.findAll();
}

OperationResult UserService::deleteUser(int id)
{
    auto allUsers = m_repository.findAll();

    auto target = std::find_if(allUsers.begin(), allUsers.end(), [id](const User& user) {
        return user.id == id;
    });

    if (target == allUsers.end()) {
        return OperationResult::fail("User not found");
    }

    if (target->role == Role::Admin) {
        const auto adminCount = std::count_if(allUsers.begin(), allUsers.end(), [](const User& user) {
            return user.role == Role::Admin;
        });

        if (adminCount <= 1) {
            return OperationResult::fail("Cannot delete the last remaining admin");
        }
    }

    if (!m_repository.remove(id)) {
        return OperationResult::fail("User not found");
    }

    return OperationResult::ok();
}