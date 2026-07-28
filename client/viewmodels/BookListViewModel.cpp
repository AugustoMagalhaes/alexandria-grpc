#include "viewmodels/BookListViewModel.h"

#include <QFutureWatcher>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

#include "viewmodels/Session.h"

BookListViewModel::BookListViewModel(QObject* parent)
    : QObject(parent)
{
}

QVariantList BookListViewModel::books() const
{
    return m_books;
}

bool BookListViewModel::isBusy() const
{
    return m_busy;
}

QString BookListViewModel::errorMessage() const
{
    return m_errorMessage;
}

void BookListViewModel::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        emit busyChanged();
    }
}

void BookListViewModel::setErrorMessage(const QString& message)
{
    m_errorMessage = message;
    emit errorMessageChanged();
}

void BookListViewModel::refresh(const QString& search)
{
    if (Session::instance() == nullptr) {
        setErrorMessage("Session not initialized");
        return;
    }

    m_lastSearch = search;

    setBusy(true);
    setErrorMessage(QString());

    auto* watcher = new QFutureWatcher<ClientResult<std::vector<Book>>>(this);

    QObject::connect(watcher, &QFutureWatcher<ClientResult<std::vector<Book>>>::finished, this, [this, watcher]() {
        auto result = watcher->result();
        watcher->deleteLater();

        setBusy(false);

        if (!result.success) {
            if (result.connectivityError) {
                Session::instance()->handleConnectivityIssue();
                return;
            }
            setErrorMessage(QString::fromStdString(result.error));
            return;
        }

        m_books.clear();
        for (const Book& book : *result.value) {
            QVariantMap entry;
            entry["id"] = book.id;
            entry["title"] = QString::fromStdString(book.title);
            entry["author"] = QString::fromStdString(book.author);
            entry["isbn"] = QString::fromStdString(book.isbn);
            entry["totalCopies"] = book.totalCopies;
            entry["availableCopies"] = book.availableCopies;
            m_books.append(entry);
        }

        emit booksChanged();
    });

    AlexandriaClient& client = Session::instance()->client();
    QFuture<ClientResult<std::vector<Book>>> future = QtConcurrent::run([&client, search]() {
        return client.listBooks(search.toStdString());
    });

    watcher->setFuture(future);
}

void BookListViewModel::deleteBook(int id)
{
    setBusy(true);
    setErrorMessage(QString());

    auto* watcher = new QFutureWatcher<ClientResult<void>>(this);

    QObject::connect(watcher, &QFutureWatcher<ClientResult<void>>::finished, this, [this, watcher]() {
        auto result = watcher->result();
        watcher->deleteLater();

        if (!result.success) {
            setBusy(false);
            if (result.connectivityError) {
                Session::instance()->handleConnectivityIssue();
                return;
            }
            setErrorMessage(QString::fromStdString(result.error));
            return;
        }

        refresh(m_lastSearch);
    });

    AlexandriaClient& client = Session::instance()->client();
    QFuture<ClientResult<void>> future = QtConcurrent::run([&client, id]() {
        return client.deleteBook(id);
    });

    watcher->setFuture(future);
}