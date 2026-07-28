#include "persistence/SqliteBookRepository.h"

#include <QSqlQuery>
#include <QVariant>

namespace {

Book bookFromQuery(const QSqlQuery& query)
{
    Book book;
    book.id = query.value("id").toInt();
    book.title = query.value("title").toString().toStdString();
    book.author = query.value("author").toString().toStdString();
    book.isbn = query.value("isbn").toString().toStdString();
    book.schoolCode = query.value("school_code").toString().toStdString();
    book.category = query.value("category").toString().toStdString();
    book.keywords = query.value("keywords").toString().toStdString();
    book.borrowable = query.value("borrowable").toInt() != 0;
    book.totalCopies = query.value("total_copies").toInt();
    book.availableCopies = query.value("available_copies").toInt();
    return book;
}

}

SqliteBookRepository::SqliteBookRepository(Database& database)
    : m_database(database)
{
}

std::optional<Book> SqliteBookRepository::create(const Book& book)
{
    std::lock_guard<std::mutex> lock(m_database.mutex());

    QSqlQuery query(m_database.handle());
    query.prepare(
        "INSERT INTO books (title, author, isbn, school_code, category, keywords, borrowable, total_copies, available_copies) "
        "VALUES (:title, :author, :isbn, :school_code, :category, :keywords, :borrowable, :total_copies, :available_copies)"
        );
    query.bindValue(":title", QString::fromStdString(book.title));
    query.bindValue(":author", QString::fromStdString(book.author));
    query.bindValue(":isbn", QString::fromStdString(book.isbn));
    query.bindValue(":school_code", QString::fromStdString(book.schoolCode));
    query.bindValue(":category", QString::fromStdString(book.category));
    query.bindValue(":keywords", QString::fromStdString(book.keywords));
    query.bindValue(":borrowable", book.borrowable ? 1 : 0);
    query.bindValue(":total_copies", book.totalCopies);
    query.bindValue(":available_copies", book.availableCopies);

    if (!query.exec()) {
        return std::nullopt;
    }

    const int newId = query.lastInsertId().toInt();

    QSqlQuery selectQuery(m_database.handle());
    selectQuery.prepare("SELECT * FROM books WHERE id = :id");
    selectQuery.bindValue(":id", newId);

    if (!selectQuery.exec() || !selectQuery.next()) {
        return std::nullopt;
    }

    return bookFromQuery(selectQuery);
}

std::optional<Book> SqliteBookRepository::findById(int id)
{
    std::lock_guard<std::mutex> lock(m_database.mutex());

    QSqlQuery query(m_database.handle());
    query.prepare("SELECT * FROM books WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }

    return bookFromQuery(query);
}

std::optional<Book> SqliteBookRepository::findByIsbn(const std::string& isbn)
{
    std::lock_guard<std::mutex> lock(m_database.mutex());

    QSqlQuery query(m_database.handle());
    query.prepare("SELECT * FROM books WHERE isbn = :isbn");
    query.bindValue(":isbn", QString::fromStdString(isbn));

    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }

    return bookFromQuery(query);
}

std::vector<Book> SqliteBookRepository::findAll(const std::string& search)
{
    std::lock_guard<std::mutex> lock(m_database.mutex());

    QSqlQuery query(m_database.handle());

    if (search.empty()) {
        query.prepare("SELECT * FROM books ORDER BY title");
    } else {
        query.prepare(
            "SELECT * FROM books "
            "WHERE title LIKE :search "
            "OR author LIKE :search "
            "OR category LIKE :search "
            "OR school_code LIKE :search "
            "OR keywords LIKE :search "
            "ORDER BY title"
            );
        query.bindValue(":search", "%" + QString::fromStdString(search) + "%");
    }

    std::vector<Book> books;

    if (!query.exec()) {
        return books;
    }

    while (query.next()) {
        books.push_back(bookFromQuery(query));
    }

    return books;
}

bool SqliteBookRepository::update(const Book& book)
{
    std::lock_guard<std::mutex> lock(m_database.mutex());

    QSqlQuery query(m_database.handle());
    query.prepare(
        "UPDATE books SET title = :title, author = :author, isbn = :isbn, "
        "school_code = :school_code, category = :category, keywords = :keywords, "
        "borrowable = :borrowable, total_copies = :total_copies, available_copies = :available_copies "
        "WHERE id = :id"
        );
    query.bindValue(":title", QString::fromStdString(book.title));
    query.bindValue(":author", QString::fromStdString(book.author));
    query.bindValue(":isbn", QString::fromStdString(book.isbn));
    query.bindValue(":school_code", QString::fromStdString(book.schoolCode));
    query.bindValue(":category", QString::fromStdString(book.category));
    query.bindValue(":keywords", QString::fromStdString(book.keywords));
    query.bindValue(":borrowable", book.borrowable ? 1 : 0);
    query.bindValue(":total_copies", book.totalCopies);
    query.bindValue(":available_copies", book.availableCopies);
    query.bindValue(":id", book.id);

    return query.exec() && query.numRowsAffected() > 0;
}

bool SqliteBookRepository::remove(int id)
{
    std::lock_guard<std::mutex> lock(m_database.mutex());

    QSqlQuery query(m_database.handle());
    query.prepare("DELETE FROM books WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec() && query.numRowsAffected() > 0;
}