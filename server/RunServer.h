#pragma once

#include <string>

namespace grpc {
class Server;
}

int runServerHeadless(grpc::Server* server);

#ifdef ALEXANDRIA_SERVER_TRAY
class QApplication;

int runServerWithTray(QApplication& app, grpc::Server* server, const std::string& listenAddress, const std::string& databasePath);
#endif