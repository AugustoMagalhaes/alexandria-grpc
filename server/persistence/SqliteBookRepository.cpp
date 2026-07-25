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
    QSqlQuery query(m_database.handle());
    query.prepare(
        "INSERT INTO books (title, author, isbn, total_copies, available_copies) "
        "VALUES (:title, :author, :isbn, :total_copies, :available_copies)"
        );
    query.bindValue(":title", QString::fromStdString(book.title));
    query.bindValue(":author", QString::fromStdString(book.author));
    query.bindValue(":isbn", QString::fromStdString(book.isbn));
    query.bindValue(":total_copies", book.totalCopies);
    query.bindValue(":available_copies", book.availableCopies);

    if (!query.exec()) {
        return std::nullopt;
    }

    return findById(query.lastInsertId().toInt());
}

std::optional<Book> SqliteBookRepository::findById(int id)
{
    QSqlQuery query(m_database.handle());
    query.prepare("SELECT * FROM books WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }

    return bookFromQuery(query);
}

std::vector<Book> SqliteBookRepository::findAll(const std::string& search)
{
    QSqlQuery query(m_database.handle());

    if (search.empty()) {
        query.prepare("SELECT * FROM books ORDER BY title");
    } else {
        query.prepare(
            "SELECT * FROM books "
            "WHERE title LIKE :search OR author LIKE :search "
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
    QSqlQuery query(m_database.handle());
    query.prepare(
        "UPDATE books SET title = :title, author = :author, isbn = :isbn, "
        "total_copies = :total_copies, available_copies = :available_copies "
        "WHERE id = :id"
        );
    query.bindValue(":title", QString::fromStdString(book.title));
    query.bindValue(":author", QString::fromStdString(book.author));
    query.bindValue(":isbn", QString::fromStdString(book.isbn));
    query.bindValue(":total_copies", book.totalCopies);
    query.bindValue(":available_copies", book.availableCopies);
    query.bindValue(":id", book.id);

    return query.exec() && query.numRowsAffected() > 0;
}

bool SqliteBookRepository::remove(int id)
{
    QSqlQuery query(m_database.handle());
    query.prepare("DELETE FROM books WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec() && query.numRowsAffected() > 0;
}