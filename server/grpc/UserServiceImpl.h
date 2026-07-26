#pragma once

#include "auth/AuthService.h"
#include "service/UserService.h"
#include "user.grpc.pb.h"

class UserServiceImpl final : public alexandria::v1::UserService::Service {
public:
    UserServiceImpl(UserService& userService, AuthService& authService);

    grpc::Status CreateUser(grpc::ServerContext* context, const alexandria::v1::CreateUserRequest* request, alexandria::v1::CreateUserResponse* response) override;
    grpc::Status ListUsers(grpc::ServerContext* context, const alexandria::v1::ListUsersRequest* request, alexandria::v1::ListUsersResponse* response) override;
    grpc::Status DeleteUser(grpc::ServerContext* context, const alexandria::v1::DeleteUserRequest* request, alexandria::v1::DeleteUserResponse* response) override;

private:
    UserService& m_userService;
    AuthService& m_authService;
};