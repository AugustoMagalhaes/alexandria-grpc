#pragma once

#include <optional>
#include <string>

#include "auth/SessionManager.h"
#include "domain/User.h"
#include "repository/IUserRepository.h"

struct LoginResult {
    User user;
    std::string token;
};

class AuthService {
public:
    AuthService(IUserRepository& userRepository, SessionManager& sessionManager);

    std::optional<LoginResult> login(const std::string& username, const std::string& password);
    std::optional<User> validateToken(const std::string& token) const;

private:
    IUserRepository& m_userRepository;
    SessionManager& m_sessionManager;
};