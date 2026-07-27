#pragma once

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

class UserFormViewModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool isAdmin READ isAdmin WRITE setIsAdmin NOTIFY isAdminChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit UserFormViewModel(QObject* parent = nullptr);

    QString username() const;
    void setUsername(const QString& username);

    QString password() const;
    void setPassword(const QString& password);

    bool isAdmin() const;
    void setIsAdmin(bool admin);

    bool isBusy() const;
    QString errorMessage() const;

    Q_INVOKABLE void reset();
    Q_INVOKABLE void save();

signals:
    void usernameChanged();
    void passwordChanged();
    void isAdminChanged();
    void busyChanged();
    void errorMessageChanged();
    void saved();

private:
    QString m_username;
    QString m_password;
    bool m_isAdmin = false;
    bool m_busy = false;
    QString m_errorMessage;

    void setBusy(bool busy);
    void setErrorMessage(const QString& message);
};