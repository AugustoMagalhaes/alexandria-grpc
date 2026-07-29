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

int BookListViewModel::selectedCount() const
{
    return m_selectedIds.size();
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

void BookListViewModel::toggleSelection(int bookId)
{
    if (m_selectedIds.contains(bookId)) {
        m_selectedIds.remove(bookId);
    } else {
        m_selectedIds.insert(bookId);
    }
    emit selectionChanged();
}

void BookListViewModel::clearSelection()
{
    if (!m_selectedIds.isEmpty()) {
        m_selectedIds.clear();
        emit selectionChanged();
    }
}

bool BookListViewModel::isSelected(int bookId) const
{
    return m_selectedIds.contains(bookId);
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
            entry["schoolCode"] = QString::fromStdString(book.schoolCode);
            entry["category"] = QString::fromStdString(book.category);
            entry["keywords"] = QString::fromStdString(book.keywords);
            entry["borrowable"] = book.borrowable;
            entry["totalCopies"] = book.totalCopies;
            entry["availableCopies"] = book.availableCopies;
            m_books.append(entry);
        }

        clearSelection();
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

void BookListViewModel::deleteSelected()
{
    if (m_selectedIds.isEmpty()) {
        return;
    }

    setBusy(true);
    setErrorMessage(QString());

    const QList<int> ids = m_selectedIds.values();
    m_pendingDeletions = ids.size();

    AlexandriaClient& client = Session::instance()->client();

    for (int id : ids) {
        auto* watcher = new QFutureWatcher<ClientResult<void>>(this);

        QObject::connect(watcher, &QFutureWatcher<ClientResult<void>>::finished, this, [this, watcher]() {
            auto result = watcher->result();
            watcher->deleteLater();

            --m_pendingDeletions;

            if (!result.success && result.connectivityError) {
                Session::instance()->handleConnectivityIssue();
                return;
            }

            if (!result.success) {
                setErrorMessage(QString::fromStdString(result.error));
            }

            if (m_pendingDeletions <= 0) {
                refresh(m_lastSearch);
            }
        });

        QFuture<ClientResult<void>> future = QtConcurrent::run([&client, id]() {
            return client.deleteBook(id);
        });

        watcher->setFuture(future);
    }
}