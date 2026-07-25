#include "persistence/SqliteUserRepository.h"

#include <QSqlQuery>
#include <QVariant>

namespace {

User userFromQuery(const QSqlQuery& query)
{
    User user;
    user.id = query.value("id").toInt();
    user.username = query.value("username").toString().toStdString();
    user.role = static_cast<Role>(query.value("role").toInt());
    return user;
}

}

SqliteUserRepository::SqliteUserRepository(Database& database)
    : m_database(database)
{
}

std::optional<User> SqliteUserRepository::create(const User& user, const std::string& passwordHash)
{
    QSqlQuery query(m_database.handle());
    query.prepare(
        "INSERT INTO users (username, password_hash, role) "
        "VALUES (:username, :password_hash, :role)"
        );
    query.bindValue(":username", QString::fromStdString(user.username));
    query.bindValue(":password_hash", QString::fromStdString(passwordHash));
    query.bindValue(":role", static_cast<int>(user.role));

    if (!query.exec()) {
        return std::nullopt;
    }

    User created = user;
    created.id = query.lastInsertId().toInt();
    return created;
}

std::vector<User> SqliteUserRepository::findAll()
{
    QSqlQuery query(m_database.handle());
    query.prepare("SELECT id, username, role FROM users ORDER BY username");

    std::vector<User> users;

    if (!query.exec()) {
        return users;
    }

    while (query.next()) {
        users.push_back(userFromQuery(query));
    }

    return users;
}

bool SqliteUserRepository::remove(int id)
{
    QSqlQuery query(m_database.handle());
    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec() && query.numRowsAffected() > 0;
}

std::optional<UserCredentials> SqliteUserRepository::findCredentialsByUsername(const std::string& username)
{
    QSqlQuery query(m_database.handle());
    query.prepare("SELECT * FROM users WHERE username = :username");
    query.bindValue(":username", QString::fromStdString(username));

    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }

    UserCredentials credentials;
    credentials.user = userFromQuery(query);
    credentials.passwordHash = query.value("password_hash").toString().toStdString();
    return credentials;
}