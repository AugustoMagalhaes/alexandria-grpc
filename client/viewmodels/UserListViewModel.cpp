#include "viewmodels/UserListViewModel.h"

#include <QFutureWatcher>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

#include "viewmodels/Session.h"

UserListViewModel::UserListViewModel(QObject* parent)
    : QObject(parent)
{
}

QVariantList UserListViewModel::users() const
{
    return m_users;
}

bool UserListViewModel::isBusy() const
{
    return m_busy;
}

QString UserListViewModel::errorMessage() const
{
    return m_errorMessage;
}

void UserListViewModel::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        emit busyChanged();
    }
}

void UserListViewModel::setErrorMessage(const QString& message)
{
    m_errorMessage = message;
    emit errorMessageChanged();
}

void UserListViewModel::refresh()
{
    setBusy(true);
    setErrorMessage(QString());

    auto* watcher = new QFutureWatcher<ClientResult<std::vector<User>>>(this);

    QObject::connect(watcher, &QFutureWatcher<ClientResult<std::vector<User>>>::finished, this, [this, watcher]() {
        auto result = watcher->result();
        watcher->deleteLater();

        setBusy(false);

        if (!result.success) {
            setErrorMessage(QString::fromStdString(result.error));
            return;
        }

        m_users.clear();
        for (const User& user : *result.value) {
            QVariantMap entry;
            entry["id"] = user.id;
            entry["username"] = QString::fromStdString(user.username);
            entry["isAdmin"] = user.role == Role::Admin;
            entry["roleLabel"] = user.role == Role::Admin ? QStringLiteral("Admin") : QStringLiteral("User");
            m_users.append(entry);
        }

        emit usersChanged();
    });

    AlexandriaClient& client = Session::instance()->client();
    QFuture<ClientResult<std::vector<User>>> future = QtConcurrent::run([&client]() {
        return client.listUsers();
    });

    watcher->setFuture(future);
}

void UserListViewModel::deleteUser(int id)
{
    setBusy(true);
    setErrorMessage(QString());

    auto* watcher = new QFutureWatcher<ClientResult<void>>(this);

    QObject::connect(watcher, &QFutureWatcher<ClientResult<void>>::finished, this, [this, watcher]() {
        auto result = watcher->result();
        watcher->deleteLater();

        if (!result.success) {
            setBusy(false);
            setErrorMessage(QString::fromStdString(result.error));
            return;
        }

        refresh();
    });

    AlexandriaClient& client = Session::instance()->client();
    QFuture<ClientResult<void>> future = QtConcurrent::run([&client, id]() {
        return client.deleteUser(id);
    });

    watcher->setFuture(future);
}