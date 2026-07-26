#include "grpc/AuthGuard.h"

namespace auth_guard {

namespace {
constexpr char kAuthorizationKey[] = "authorization";
}

std::optional<User> authenticate(grpc::ServerContext* context, AuthService& authService)
{
    auto it = context->client_metadata().find(kAuthorizationKey);

    if (it == context->client_metadata().end()) {
        return std::nullopt;
    }

    const std::string token(it->second.data(), it->second.size());
    return authService.validateToken(token);
}

grpc::Status requireRole(grpc::ServerContext* context, AuthService& authService, Role minimumRole, User* outUser)
{
    auto user = authenticate(context, authService);

    if (!user.has_value()) {
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Missing or invalid session token");
    }

    if (minimumRole == Role::Admin && user->role != Role::Admin) {
        return grpc::Status(grpc::StatusCode::PERMISSION_DENIED, "Administrator privileges required");
    }

    if (outUser != nullptr) {
        *outUser = *user;
    }

    return grpc::Status::OK;
}

}