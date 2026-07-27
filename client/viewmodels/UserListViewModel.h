#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <qqmlintegration.h>

class UserListViewModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVariantList users READ users NOTIFY usersChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit UserListViewModel(QObject* parent = nullptr);

    QVariantList users() const;
    bool isBusy() const;
    QString errorMessage() const;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void deleteUser(int id);

signals:
    void usersChanged();
    void busyChanged();
    void errorMessageChanged();

private:
    QVariantList m_users;
    bool m_busy = false;
    QString m_errorMessage;

    void setBusy(bool busy);
    void setErrorMessage(const QString& message);
};