#pragma once

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

class BookFormViewModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int bookId READ bookId WRITE setBookId NOTIFY bookIdChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    Q_PROPERTY(QString isbn READ isbn WRITE setIsbn NOTIFY isbnChanged)
    Q_PROPERTY(int totalCopies READ totalCopies WRITE setTotalCopies NOTIFY totalCopiesChanged)
    Q_PROPERTY(int availableCopies READ availableCopies WRITE setAvailableCopies NOTIFY availableCopiesChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(bool isEditing READ isEditing NOTIFY bookIdChanged)

public:
    explicit BookFormViewModel(QObject* parent = nullptr);

    int bookId() const;
    void setBookId(int id);

    QString title() const;
    void setTitle(const QString& title);

    QString author() const;
    void setAuthor(const QString& author);

    QString isbn() const;
    void setIsbn(const QString& isbn);

    int totalCopies() const;
    void setTotalCopies(int copies);

    int availableCopies() const;
    void setAvailableCopies(int copies);

    bool isBusy() const;
    QString errorMessage() const;
    bool isEditing() const;

    Q_INVOKABLE void reset();
    Q_INVOKABLE void save();

signals:
    void bookIdChanged();
    void titleChanged();
    void authorChanged();
    void isbnChanged();
    void totalCopiesChanged();
    void availableCopiesChanged();
    void busyChanged();
    void errorMessageChanged();
    void saved();

private:
    int m_bookId = 0;
    QString m_title;
    QString m_author;
    QString m_isbn;
    int m_totalCopies = 1;
    int m_availableCopies = 1;
    bool m_busy = false;
    QString m_errorMessage;

    void setBusy(bool busy);
    void setErrorMessage(const QString& message);
};