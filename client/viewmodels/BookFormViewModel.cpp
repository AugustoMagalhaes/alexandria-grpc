#include "viewmodels/BookFormViewModel.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

#include "domain/Book.h"
#include "viewmodels/Session.h"

BookFormViewModel::BookFormViewModel(QObject* parent)
    : QObject(parent)
{
}

int BookFormViewModel::bookId() const
{
    return m_bookId;
}

void BookFormViewModel::setBookId(int id)
{
    if (m_bookId != id) {
        m_bookId = id;
        emit bookIdChanged();
    }
}

QString BookFormViewModel::title() const
{
    return m_title;
}

void BookFormViewModel::setTitle(const QString& title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
    }
}

QString BookFormViewModel::author() const
{
    return m_author;
}

void BookFormViewModel::setAuthor(const QString& author)
{
    if (m_author != author) {
        m_author = author;
        emit authorChanged();
    }
}

QString BookFormViewModel::isbn() const
{
    return m_isbn;
}

void BookFormViewModel::setIsbn(const QString& isbn)
{
    if (m_isbn != isbn) {
        m_isbn = isbn;
        emit isbnChanged();
    }
}

int BookFormViewModel::totalCopies() const
{
    return m_totalCopies;
}

void BookFormViewModel::setTotalCopies(int copies)
{
    if (m_totalCopies != copies) {
        m_totalCopies = copies;
        emit totalCopiesChanged();
    }
}

int BookFormViewModel::availableCopies() const
{
    return m_availableCopies;
}

void BookFormViewModel::setAvailableCopies(int copies)
{
    if (m_availableCopies != copies) {
        m_availableCopies = copies;
        emit availableCopiesChanged();
    }
}

bool BookFormViewModel::isBusy() const
{
    return m_busy;
}

QString BookFormViewModel::errorMessage() const
{
    return m_errorMessage;
}

bool BookFormViewModel::isEditing() const
{
    return m_bookId != 0;
}

void BookFormViewModel::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        emit busyChanged();
    }
}

void BookFormViewModel::setErrorMessage(const QString& message)
{
    m_errorMessage = message;
    emit errorMessageChanged();
}

void BookFormViewModel::reset()
{
    setBookId(0);
    setTitle(QString());
    setAuthor(QString());
    setIsbn(QString());
    setTotalCopies(1);
    setAvailableCopies(1);
    setErrorMessage(QString());
}

void BookFormViewModel::save()
{
    setBusy(true);
    setErrorMessage(QString());

    AlexandriaClient& client = Session::instance()->client();
    const bool editing = isEditing();

    const int id = m_bookId;
    const std::string title = m_title.toStdString();
    const std::string author = m_author.toStdString();
    const std::string isbn = m_isbn.toStdString();
    const int totalCopies = m_totalCopies;
    const int availableCopies = m_availableCopies;

    auto* watcher = new QFutureWatcher<ClientResult<void>>(this);

    QObject::connect(watcher, &QFutureWatcher<ClientResult<void>>::finished, this, [this, watcher]() {
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

        emit saved();
    });

    QFuture<ClientResult<void>> future = QtConcurrent::run([&client, editing, id, title, author, isbn, totalCopies, availableCopies]() {
        if (editing) {
            Book book;
            book.id = id;
            book.title = title;
            book.author = author;
            book.isbn = isbn;
            book.totalCopies = totalCopies;
            book.availableCopies = availableCopies;
            return client.updateBook(book);
        }

        auto result = client.createBook(title, author, isbn, totalCopies);

        ClientResult<void> voidResult;
        voidResult.success = result.success;
        voidResult.error = result.error;
        voidResult.connectivityError = result.connectivityError;
        return voidResult;
    });

    watcher->setFuture(future);
}