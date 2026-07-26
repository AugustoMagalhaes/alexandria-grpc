#pragma once

#include <optional>

#include <grpcpp/grpcpp.h>

#include "auth/AuthService.h"
#include "domain/User.h"

namespace auth_guard {

std::optional<User> authenticate(grpc::ServerContext* context, AuthService& authService);

grpc::Status requireRole(grpc::ServerContext* context, AuthService& authService, Role minimumRole, User* outUser);

}