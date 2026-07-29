#pragma once

#include <QObject>
#include <QSet>
#include <QString>
#include <QVariantList>
#include <qqmlintegration.h>

class UserListViewModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVariantList users READ users NOTIFY usersChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(int selectedCount READ selectedCount NOTIFY selectionChanged)

public:
    explicit UserListViewModel(QObject* parent = nullptr);

    QVariantList users() const;
    bool isBusy() const;
    QString errorMessage() const;
    int selectedCount() const;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void deleteUser(int id);
    Q_INVOKABLE void toggleSelection(int userId);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE bool isSelected(int userId) const;
    Q_INVOKABLE void deleteSelected();

signals:
    void usersChanged();
    void busyChanged();
    void errorMessageChanged();
    void selectionChanged();

private:
    QVariantList m_users;
    bool m_busy = false;
    QString m_errorMessage;
    QSet<int> m_selectedIds;
    int m_pendingDeletions = 0;

    void setBusy(bool busy);
    void setErrorMessage(const QString& message);
};