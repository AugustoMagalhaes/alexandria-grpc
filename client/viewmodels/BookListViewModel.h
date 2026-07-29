#pragma once

#include <QObject>
#include <QSet>
#include <QString>
#include <QVariantList>
#include <qqmlintegration.h>

class BookListViewModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVariantList books READ books NOTIFY booksChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(int selectedCount READ selectedCount NOTIFY selectionChanged)

public:
    explicit BookListViewModel(QObject* parent = nullptr);

    QVariantList books() const;
    bool isBusy() const;
    QString errorMessage() const;
    int selectedCount() const;

    Q_INVOKABLE void refresh(const QString& search = QString());
    Q_INVOKABLE void deleteBook(int id);
    Q_INVOKABLE void toggleSelection(int bookId);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE bool isSelected(int bookId) const;
    Q_INVOKABLE void deleteSelected();

signals:
    void booksChanged();
    void busyChanged();
    void errorMessageChanged();
    void selectionChanged();

private:
    QVariantList m_books;
    bool m_busy = false;
    QString m_errorMessage;
    QString m_lastSearch;
    QSet<int> m_selectedIds;
    int m_pendingDeletions = 0;

    void setBusy(bool busy);
    void setErrorMessage(const QString& message);
};