#include "grpc/AuthServiceImpl.h"

#include "grpc/AuthGuard.h"
#include "grpc/ProtoConverters.h"

AuthServiceImpl::AuthServiceImpl(AuthService& authService)
    : m_authService(authService)
{
}

grpc::Status AuthServiceImpl::Login(grpc::ServerContext*, const alexandria::v1::LoginRequest* request, alexandria::v1::LoginResponse* response)
{
    auto result = m_authService.login(request->username(), request->password());

    if (!result.has_value()) {
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid username or password");
    }

    response->set_token(result->token);
    response->set_role(converters::toProto(result->user.role));
    return grpc::Status::OK;
}

grpc::Status AuthServiceImpl::ValidateToken(grpc::ServerContext* context, const alexandria::v1::ValidateTokenRequest*, alexandria::v1::ValidateTokenResponse* response)
{
    auto user = auth_guard::authenticate(context, m_authService);

    if (!user.has_value()) {
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid or expired session token");
    }

    response->set_role(converters::toProto(user->role));
    return grpc::Status::OK;
}