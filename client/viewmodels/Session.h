#pragma once

#include <memory>

#include <QObject>
#include <QString>

#include "core/AlexandriaClient.h"
#include "domain/User.h"

class Session : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool authenticated READ isAuthenticated NOTIFY authenticationChanged)
    Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY authenticationChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QString serverAddress READ serverAddress NOTIFY serverAddressChanged)
    Q_PROPERTY(bool connected READ isConnected NOTIFY serverAddressChanged)

public:
    explicit Session(QObject* parent = nullptr);

    static Session* instance();

    AlexandriaClient& client();

    bool isAuthenticated() const;
    bool isAdmin() const;
    bool isBusy() const;
    QString errorMessage() const;
    QString serverAddress() const;
    bool isConnected() const;

    Q_INVOKABLE void login(const QString& username, const QString& password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE void connectToServer(const QString& address);

signals:
    void authenticationChanged();
    void busyChanged();
    void errorMessageChanged();
    void loginSucceeded();
    void serverAddressChanged();

private:
    static Session* s_instance;

    std::unique_ptr<AlexandriaClient> m_client;
    bool m_authenticated = false;
    Role m_role = Role::User;
    bool m_busy = false;
    QString m_errorMessage;
    QString m_serverAddress;

    void setBusy(bool busy);
    void setErrorMessage(const QString& message);
};