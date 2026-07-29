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

int UserListViewModel::selectedCount() const
{
    return m_selectedIds.size();
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

void UserListViewModel::toggleSelection(int userId)
{
    if (m_selectedIds.contains(userId)) {
        m_selectedIds.remove(userId);
    } else {
        m_selectedIds.insert(userId);
    }
    emit selectionChanged();
}

void UserListViewModel::clearSelection()
{
    if (!m_selectedIds.isEmpty()) {
        m_selectedIds.clear();
        emit selectionChanged();
    }
}

bool UserListViewModel::isSelected(int userId) const
{
    return m_selectedIds.contains(userId);
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
            if (result.connectivityError) {
                Session::instance()->handleConnectivityIssue();
                return;
            }
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

        clearSelection();
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
            if (result.connectivityError) {
                Session::instance()->handleConnectivityIssue();
                return;
            }
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

void UserListViewModel::deleteSelected()
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
                refresh();
            }
        });

        QFuture<ClientResult<void>> future = QtConcurrent::run([&client, id]() {
            return client.deleteUser(id);
        });

        watcher->setFuture(future);
    }
}