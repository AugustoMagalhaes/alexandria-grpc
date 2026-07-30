#pragma once

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

class UserFormViewModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool isAdmin READ isAdmin WRITE setIsAdmin NOTIFY isAdminChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(bool isEditing READ isEditing NOTIFY userIdChanged)

public:
    explicit UserFormViewModel(QObject* parent = nullptr);

    int userId() const;
    void setUserId(int id);

    QString username() const;
    void setUsername(const QString& username);

    QString password() const;
    void setPassword(const QString& password);

    bool isAdmin() const;
    void setIsAdmin(bool admin);

    bool isBusy() const;
    QString errorMessage() const;
    bool isEditing() const;

    Q_INVOKABLE void reset();
    Q_INVOKABLE void save();

signals:
    void userIdChanged();
    void usernameChanged();
    void passwordChanged();
    void isAdminChanged();
    void busyChanged();
    void errorMessageChanged();
    void saved();

private:
    int m_userId = 0;
    QString m_username;
    QString m_password;
    bool m_isAdmin = false;
    bool m_busy = false;
    QString m_errorMessage;

    void setBusy(bool busy);
    void setErrorMessage(const QString& message);
};