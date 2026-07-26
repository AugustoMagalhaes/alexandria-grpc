#pragma once

#include <mutex>

#include <QSqlDatabase>
#include <QString>

class Database {
public:
    explicit Database(const QString& path);

    QSqlDatabase& handle();
    bool migrate();

    std::mutex& mutex();

private:
    QSqlDatabase m_db;
    std::mutex m_mutex;
};