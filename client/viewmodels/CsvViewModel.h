#pragma once

#include <QObject>
#include <QString>
#include <qqmlintegration.h>

class CsvViewModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(bool statusIsError READ statusIsError NOTIFY statusMessageChanged)

public:
    explicit CsvViewModel(QObject* parent = nullptr);

    bool isBusy() const;
    QString statusMessage() const;
    bool statusIsError() const;

    Q_INVOKABLE void exportToFile(const QString& filePath);
    Q_INVOKABLE void importFromFile(const QString& filePath, bool replace);

signals:
    void busyChanged();
    void statusMessageChanged();
    void importFinished();

private:
    bool m_busy = false;
    QString m_statusMessage;
    bool m_statusIsError = false;

    void setBusy(bool busy);
    void setStatus(const QString& message, bool isError);
};