#include "persistence/Database.h"

#include <QSqlQuery>
#include <QUuid>

Database::Database(const QString& path)
{
    const QString connectionName = QUuid::createUuid().toString();
    m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    m_db.setDatabaseName(path);
    m_db.open();
}

QSqlDatabase& Database::handle()
{
    return m_db;
}

bool Database::migrate()
{
    QSqlQuery query(m_db);

    const bool booksOk = query.exec(
        "CREATE TABLE IF NOT EXISTS books ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "author TEXT NOT NULL,"
        "isbn TEXT NOT NULL,"
        "total_copies INTEGER NOT NULL,"
        "available_copies INTEGER NOT NULL"
        ")"
        );

    const bool usersOk = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password_hash TEXT NOT NULL,"
        "role INTEGER NOT NULL"
        ")"
        );

    return booksOk && usersOk;
}

std::mutex& Database::mutex()
{
    return m_mutex;
}