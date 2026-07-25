#pragma once

#include <QSqlDatabase>
#include <QString>

class Database {
public:
    explicit Database(const QString& path);

    QSqlDatabase& handle();
    bool migrate();

private:
    QSqlDatabase m_db;
};