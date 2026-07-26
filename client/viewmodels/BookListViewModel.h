#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <qqmlintegration.h>

class BookListViewModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVariantList books READ books NOTIFY booksChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit BookListViewModel(QObject* parent = nullptr);

    QVariantList books() const;
    bool isBusy() const;
    QString errorMessage() const;

    Q_INVOKABLE void refresh(const QString& search = QString());

signals:
    void booksChanged();
    void busyChanged();
    void errorMessageChanged();

private:
    QVariantList m_books;
    bool m_busy = false;
    QString m_errorMessage;

    void setBusy(bool busy);
    void setErrorMessage(const QString& message);
};