#include "viewmodels/UserFormViewModel.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>
#include "viewmodels/Session.h"

UserFormViewModel::UserFormViewModel(QObject* parent)
    : QObject(parent)
{
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
    setUsername(QString());
    setPassword(QString());
    setIsAdmin(false);
    setErrorMessage(QString());
}

void UserFormViewModel::save()
{
    qDebug() << "UserForm::save start, busy=" << m_busy;
    setBusy(true);
    setErrorMessage(QString());

    AlexandriaClient& client = Session::instance()->client();
    const std::string username = m_username.toStdString();
    const std::string password = m_password.toStdString();
    const Role role = m_isAdmin ? Role::Admin : Role::User;

    auto* watcher = new QFutureWatcher<ClientResult<User>>(this);

    QObject::connect(watcher, &QFutureWatcher<ClientResult<User>>::finished, this, [this, watcher]() {
        qDebug() << "UserForm::save finished callback fired";
        auto result = watcher->result();
        watcher->deleteLater();

        setBusy(false);

        if (!result.success) {
            setErrorMessage(QString::fromStdString(result.error));
            return;
        }

        emit saved();
    });

    QFuture<ClientResult<User>> future = QtConcurrent::run([&client, username, password, role]() {
        return client.createUser(username, password, role);
    });

    watcher->setFuture(future);
}