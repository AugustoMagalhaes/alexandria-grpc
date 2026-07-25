#include "auth/SessionManager.h"

#include <sodium.h>

#include <iomanip>
#include <sstream>

namespace {

std::string generateToken()
{
    constexpr size_t tokenBytes = 32;
    unsigned char buffer[tokenBytes];
    randombytes_buf(buffer, tokenBytes);

    std::ostringstream stream;
    for (unsigned char byte : buffer) {
        stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return stream.str();
}

}

std::string SessionManager::createSession(const User& user)
{
    const std::string token = generateToken();

    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions[token] = user;
    return token;
}

std::optional<User> SessionManager::validateSession(const std::string& token) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_sessions.find(token);
    if (it == m_sessions.end()) {
        return std::nullopt;
    }

    return it->second;
}

void SessionManager::invalidateSession(const std::string& token)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions.erase(token);
}