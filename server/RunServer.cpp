#include "RunServer.h"

#include <csignal>
#include <iostream>
#include <thread>

#include <grpcpp/grpcpp.h>

int runServerHeadless(grpc::Server* server)
{
    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGINT);
    sigaddset(&signalSet, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &signalSet, nullptr);

    std::thread signalThread([&signalSet, server]() {
        int receivedSignal = 0;
        sigwait(&signalSet, &receivedSignal);
        std::cout << "\nReceived signal " << receivedSignal << ", shutting down gracefully..." << std::endl;
        server->Shutdown();
    });

    server->Wait();
    signalThread.join();

    std::cout << "Server stopped." << std::endl;
    return 0;
}

#ifdef ALEXANDRIA_SERVER_TRAY

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QColor>
#include <QFont>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QMetaObject>
#include <QPainter>
#include <QPixmap>
#include <QSystemTrayIcon>
#include <QVBoxLayout>
#include <QWidget>

namespace {

class ServerStatusWindow : public QWidget {
public:
    ServerStatusWindow(const QString& listenAddress, const QString& databasePath)
    {
        setWindowTitle(QStringLiteral("Alexandria Server"));
        setFixedSize(380, 180);

        auto* layout = new QVBoxLayout(this);

        auto* statusLabel = new QLabel(QStringLiteral("Server is running"), this);
        statusLabel->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 14px;"));

        auto* addressLabel = new QLabel(QStringLiteral("Listening on: %1").arg(listenAddress), this);
        auto* dbLabel = new QLabel(QStringLiteral("Database: %1").arg(databasePath), this);

        auto* hintLabel = new QLabel(
            QStringLiteral("Closing this window keeps the server running in the system tray."), this);
        hintLabel->setWordWrap(true);

        layout->addWidget(statusLabel);
        layout->addWidget(addressLabel);
        layout->addWidget(dbLabel);
        layout->addWidget(hintLabel);
    }

protected:
    void closeEvent(QCloseEvent* event) override
    {
        event->ignore();
        hide();
    }
};

QIcon buildTrayIcon()
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor("#2C5F6D"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(2, 2, 28, 28);

    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(16);
    painter.setFont(font);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, QStringLiteral("A"));

    return QIcon(pixmap);
}

}

int runServerWithTray(QApplication& app, grpc::Server* server, const std::string& listenAddress, const std::string& databasePath)
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        std::cout << "System tray is not available on this system, falling back to console mode." << std::endl;
        return runServerHeadless(server);
    }

    app.setQuitOnLastWindowClosed(false);

    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGINT);
    sigaddset(&signalSet, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &signalSet, nullptr);

    ServerStatusWindow window(QString::fromStdString(listenAddress), QString::fromStdString(databasePath));
    window.show();

    QSystemTrayIcon trayIcon(buildTrayIcon());
    trayIcon.setToolTip(QStringLiteral("Alexandria Server"));

    QMenu menu;

    QAction* showAction = menu.addAction(QStringLiteral("Show Window"));
    QObject::connect(showAction, &QAction::triggered, [&window]() {
        window.show();
        window.raise();
        window.activateWindow();
    });

    QAction* quitAction = menu.addAction(QStringLiteral("Quit"));
    QObject::connect(quitAction, &QAction::triggered, [server, &app]() {
        server->Shutdown();
        app.quit();
    });

    trayIcon.setContextMenu(&menu);
    trayIcon.show();

    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, [&window](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
            window.show();
            window.raise();
            window.activateWindow();
        }
    });

    std::thread signalThread([&signalSet, server, &app]() {
        int receivedSignal = 0;
        sigwait(&signalSet, &receivedSignal);
        server->Shutdown();
        QMetaObject::invokeMethod(&app, "quit", Qt::QueuedConnection);
    });
    signalThread.detach();

    std::thread grpcWaitThread([server]() {
        server->Wait();
    });

    const int exitCode = app.exec();

    server->Shutdown();
    grpcWaitThread.join();

    std::cout << "Server stopped." << std::endl;

    return exitCode;
}

#endif