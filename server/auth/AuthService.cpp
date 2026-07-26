#include "auth/AuthService.h"

#include "auth/PasswordHasher.h"

AuthService::AuthService(IUserRepository& userRepository, SessionManager& sessionManager)
    : m_userRepository(userRepository)
    , m_sessionManager(sessionManager)
{
}

std::optional<LoginResult> AuthService::login(const std::string& username, const std::string& password)
{
    auto credentials = m_userRepository.findCredentialsByUsername(username);

    if (!credentials.has_value()) {
        return std::nullopt;
    }

    if (!PasswordHasher::verify(password, credentials->passwordHash)) {
        return std::nullopt;
    }

    LoginResult result;
    result.user = credentials->user;
    result.token = m_sessionManager.createSession(credentials->user);
    return result;
}

std::optional<User> AuthService::validateToken(const std::string& token) const
{
    return m_sessionManager.validateSession(token);
}