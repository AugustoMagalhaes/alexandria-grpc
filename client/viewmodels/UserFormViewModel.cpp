#include "viewmodels/UserFormViewModel.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

#include "viewmodels/Session.h"

UserFormViewModel::UserFormViewModel(QObject* parent)
    : QObject(parent)
{
}

int UserFormViewModel::userId() const
{
    return m_userId;
}

void UserFormViewModel::setUserId(int id)
{
    if (m_userId != id) {
        m_userId = id;
        emit userIdChanged();
    }
}

QString UserFormViewModel::username() const
{
    return m_username;
}

void UserFormViewModel::setUsername(const QString& username)
{
    if (m_username != username) {
        m_username = username;
        emit usernameChanged();
    }
}

QString UserFormViewModel::password() const
{
    return m_password;
}

void UserFormViewModel::setPassword(const QString& password)
{
    if (m_password != password) {
        m_password = password;
        emit passwordChanged();
    }
}

bool UserFormViewModel::isAdmin() const
{
    return m_isAdmin;
}

void UserFormViewModel::setIsAdmin(bool admin)
{
    if (m_isAdmin != admin) {
        m_isAdmin = admin;
        emit isAdminChanged();
    }
}

bool UserFormViewModel::isBusy() const
{
    return m_busy;
}

QString UserFormViewModel::errorMessage() const
{
    return m_errorMessage;
}

bool UserFormViewModel::isEditing() const
{
    return m_userId != 0;
}

void UserFormViewModel::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        emit busyChanged();
    }
}

void UserFormViewModel::setErrorMessage(const QString& message)
{
    m_errorMessage = message;
    emit errorMessageChanged();
}

void UserFormViewModel::reset()
{
    setUserId(0);
    setUsername(QString());
    setPassword(QString());
    setIsAdmin(false);
    setErrorMessage(QString());
}

void UserFormViewModel::save()
{
    setBusy(true);
    setErrorMessage(QString());

    AlexandriaClient& client = Session::instance()->client();
    const bool editing = isEditing();

    const int id = m_userId;
    const std::string username = m_username.toStdString();
    const std::string password = m_password.toStdString();
    const Role role = m_isAdmin ? Role::Admin : Role::User;

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

    QFuture<ClientResult<void>> future = QtConcurrent::run([&client, editing, id, username, password, role]() {
        if (editing) {
            return client.updateUser(id, password, role);
        }

        auto result = client.createUser(username, password, role);

        ClientResult<void> voidResult;
        voidResult.success = result.success;
        voidResult.error = result.error;
        voidResult.connectivityError = result.connectivityError;
        return voidResult;
    });

    watcher->setFuture(future);
}