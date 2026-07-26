#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "domain/User.h"

class SessionManager {
public:
    std::string createSession(const User& user);
    std::optional<User> validateSession(const std::string& token) const;
    void invalidateSession(const std::string& token);

private:
    mutable std::mutex m_mutex;
    std::unordered_map<std::string, User> m_sessions;
};