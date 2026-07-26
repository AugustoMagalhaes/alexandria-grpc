#include "viewmodels/Session.h"

#include <QFutureWatcher>
#include <QSettings>
#include <QtConcurrent/QtConcurrent>

Session* Session::s_instance = nullptr;

Session::Session(QObject* parent)
    : QObject(parent)
{
    QSettings settings;
    m_serverAddress = settings.value("server/address").toString();
    m_client = std::make_unique<AlexandriaClient>(m_serverAddress.toStdString());
    s_instance = this;

    if (!m_serverAddress.isEmpty()) {
        checkConnection();
    }
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

QString Session::serverAddress() const
{
    return m_serverAddress;
}

bool Session::isServerConfigured() const
{
    return m_serverConfigured;
}

bool Session::isConnecting() const
{
    return m_connecting;
}

QString Session::connectionError() const
{
    return m_connectionError;
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

void Session::connectToServer(const QString& address)
{
    QSettings settings;
    settings.setValue("server/address", address);

    m_serverAddress = address;
    m_client = std::make_unique<AlexandriaClient>(address.toStdString());
    emit serverAddressChanged();

    checkConnection();
}

void Session::requestServerChange()
{
    m_serverConfigured = false;
    m_connectionError.clear();
    emit serverConfiguredChanged();
    emit connectionStateChanged();
}

void Session::checkConnection()
{
    m_connecting = true;
    m_connectionError.clear();
    emit connectionStateChanged();

    auto* watcher = new QFutureWatcher<ClientResult<void>>(this);

    QObject::connect(watcher, &QFutureWatcher<ClientResult<void>>::finished, this, [this, watcher]() {
        auto result = watcher->result();
        watcher->deleteLater();

        m_connecting = false;

        if (!result.success) {
            m_serverConfigured = false;
            m_connectionError = QString::fromStdString(result.error);
            emit connectionStateChanged();
            return;
        }

        m_serverConfigured = true;
        emit serverConfiguredChanged();
        emit connectionStateChanged();
    });

    AlexandriaClient* clientPtr = m_client.get();
    QFuture<ClientResult<void>> future = QtConcurrent::run([clientPtr]() {
        return clientPtr->checkConnection(3000);
    });

    watcher->setFuture(future);
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