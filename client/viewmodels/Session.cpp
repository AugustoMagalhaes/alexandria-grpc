#include "viewmodels/Session.h"

#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

Session* Session::s_instance = nullptr;

Session::Session(QObject* parent)
    : QObject(parent)
{
    m_client = std::make_unique<AlexandriaClient>("127.0.0.1:50051");
    s_instance = this;
}

Session* Session::instance()
{
    return s_instance;
}

AlexandriaClient& Session::client()
{
    return *m_client;
}

bool Session::isAuthenticated() const
{
    return m_authenticated;
}

bool Session::isAdmin() const
{
    return m_authenticated && m_role == Role::Admin;
}

bool Session::isBusy() const
{
    return m_busy;
}

QString Session::errorMessage() const
{
    return m_errorMessage;
}

void Session::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        emit busyChanged();
    }
}

void Session::setErrorMessage(const QString& message)
{
    m_errorMessage = message;
    emit errorMessageChanged();
}

void Session::login(const QString& username, const QString& password)
{
    setBusy(true);
    setErrorMessage(QString());

    auto* watcher = new QFutureWatcher<ClientResult<Role>>(this);

    QObject::connect(watcher, &QFutureWatcher<ClientResult<Role>>::finished, this, [this, watcher]() {
        auto result = watcher->result();
        watcher->deleteLater();

        setBusy(false);

        if (!result.success) {
            setErrorMessage(QString::fromStdString(result.error));
            return;
        }

        m_authenticated = true;
        m_role = *result.value;
        emit authenticationChanged();
        emit loginSucceeded();
    });

    QFuture<ClientResult<Role>> future = QtConcurrent::run([this, username, password]() {
        return m_client->login(username.toStdString(), password.toStdString());
    });

    watcher->setFuture(future);
}

void Session::logout()
{
    m_client->logout();
    m_authenticated = false;
    m_role = Role::User;
    emit authenticationChanged();
}