bool Database::migrate()
{
    QSqlQuery query(m_db);

    const bool booksOk = query.exec(
        "CREATE TABLE IF NOT EXISTS books ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "author TEXT NOT NULL,"
        "isbn TEXT NOT NULL DEFAULT '',"
        "school_code TEXT NOT NULL DEFAULT '',"
        "category TEXT NOT NULL DEFAULT '',"
        "keywords TEXT NOT NULL DEFAULT '',"
        "borrowable INTEGER NOT NULL DEFAULT 1,"
        "total_copies INTEGER NOT NULL,"
        "available_copies INTEGER NOT NULL"
        ")"
        );

    query.exec("ALTER TABLE books ADD COLUMN school_code TEXT NOT NULL DEFAULT ''");
    query.exec("ALTER TABLE books ADD COLUMN category TEXT NOT NULL DEFAULT ''");
    query.exec("ALTER TABLE books ADD COLUMN keywords TEXT NOT NULL DEFAULT ''");
    query.exec("ALTER TABLE books ADD COLUMN borrowable INTEGER NOT NULL DEFAULT 1");

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