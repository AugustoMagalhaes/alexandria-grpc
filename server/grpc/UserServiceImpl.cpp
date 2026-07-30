#include "grpc/UserServiceImpl.h"

#include "grpc/AuthGuard.h"
#include "grpc/ProtoConverters.h"

UserServiceImpl::UserServiceImpl(UserService& userService, AuthService& authService)
    : m_userService(userService)
    , m_authService(authService)
{
}

grpc::Status UserServiceImpl::CreateUser(grpc::ServerContext* context, const alexandria::v1::CreateUserRequest* request, alexandria::v1::CreateUserResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::Admin, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    auto result = m_userService.createUser(request->username(), request->password(), converters::fromProto(request->role()));

    if (!result.success) {
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, result.error);
    }

    *response->mutable_user() = converters::toProto(*result.value);
    return grpc::Status::OK;
}

grpc::Status UserServiceImpl::UpdateUser(grpc::ServerContext* context, const alexandria::v1::UpdateUserRequest* request, alexandria::v1::UpdateUserResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::Admin, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    auto result = m_userService.updateUser(request->id(), request->password(), converters::fromProto(request->role()));

    if (!result.success) {
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, result.error);
    }

    return grpc::Status::OK;
}

grpc::Status UserServiceImpl::ListUsers(grpc::ServerContext* context, const alexandria::v1::ListUsersRequest*, alexandria::v1::ListUsersResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::Admin, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    for (const User& user : m_userService.listUsers()) {
        *response->add_users() = converters::toProto(user);
    }

    return grpc::Status::OK;
}

grpc::Status UserServiceImpl::DeleteUser(grpc::ServerContext* context, const alexandria::v1::DeleteUserRequest* request, alexandria::v1::DeleteUserResponse* response)
{
    auto authStatus = auth_guard::requireRole(context, m_authService, Role::Admin, nullptr);
    if (!authStatus.ok()) {
        return authStatus;
    }

    auto result = m_userService.deleteUser(request->id());

    if (!result.success) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, result.error);
    }

    response->set_success(true);
    return grpc::Status::OK;
}