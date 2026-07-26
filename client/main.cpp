#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "viewmodels/Session.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    Session session;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Session", &session);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Alexandria", "Main");
    return QGuiApplication::exec();
}