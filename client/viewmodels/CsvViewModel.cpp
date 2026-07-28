#include "viewmodels/CsvViewModel.h"

#include <QFile>
#include <QFutureWatcher>
#include <QIODevice>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>

#include "core/AlexandriaClient.h"
#include "viewmodels/Session.h"

CsvViewModel::CsvViewModel(QObject* parent)
    : QObject(parent)
{
}

bool CsvViewModel::isBusy() const
{
    return m_busy;
}

QString CsvViewModel::statusMessage() const
{
    return m_statusMessage;
}

bool CsvViewModel::statusIsError() const
{
    return m_statusIsError;
}

void CsvViewModel::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        emit busyChanged();
    }
}

void CsvViewModel::setStatus(const QString& message, bool isError)
{
    m_statusMessage = message;
    m_statusIsError = isError;
    emit statusMessageChanged();
}

void CsvViewModel::exportToFile(const QString& filePath)
{
    setBusy(true);
    setStatus(QString(), false);

    AlexandriaClient& client = Session::instance()->client();

    auto* watcher = new QFutureWatcher<ClientResult<std::string>>(this);

    QObject::connect(watcher, &QFutureWatcher<ClientResult<std::string>>::finished, this, [this, watcher, filePath]() {
        auto result = watcher->result();
        watcher->deleteLater();

        setBusy(false);

        if (!result.success) {
            if (result.connectivityError) {
                Session::instance()->handleConnectivityIssue();
                return;
            }
            setStatus(QString::fromStdString(result.error), true);
            return;
        }

        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            setStatus(tr("Could not write to the selected file"), true);
            return;
        }

        QTextStream stream(&file);
        stream << QString::fromStdString(*result.value);
        file.close();

        setStatus(tr("Books exported successfully"), false);
    });

    QFuture<ClientResult<std::string>> future = QtConcurrent::run([&client]() {
        return client.exportBooksCsv();
    });

    watcher->setFuture(future);
}

void CsvViewModel::importFromFile(const QString& filePath, bool replace)
{
    setBusy(true);
    setStatus(QString(), false);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setBusy(false);
        setStatus(tr("Could not read the selected file"), true);
        return;
    }

    QTextStream stream(&file);
    const std::string csvData = stream.readAll().toStdString();
    file.close();

    AlexandriaClient& client = Session::instance()->client();

    auto* watcher = new QFutureWatcher<ClientResult<CsvImportSummary>>(this);

    QObject::connect(watcher, &QFutureWatcher<ClientResult<CsvImportSummary>>::finished, this, [this, watcher]() {
        auto result = watcher->result();
        watcher->deleteLater();

        setBusy(false);

        if (!result.success) {
            if (result.connectivityError) {
                Session::instance()->handleConnectivityIssue();
                return;
            }
            setStatus(QString::fromStdString(result.error), true);
            return;
        }

        const auto& summary = *result.value;

        if (summary.skippedCount > 0) {
            setStatus(tr("Imported %1 book(s), %2 row(s) skipped due to errors")
                          .arg(summary.importedCount)
                          .arg(summary.skippedCount), false);
        } else {
            setStatus(tr("Imported %1 book(s) successfully").arg(summary.importedCount), false);
        }

        emit importFinished();
    });

    QFuture<ClientResult<CsvImportSummary>> future = QtConcurrent::run([&client, csvData, replace]() {
        return client.importBooksCsv(csvData, replace);
    });

    watcher->setFuture(future);
}