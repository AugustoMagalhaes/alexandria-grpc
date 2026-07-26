#pragma once

#include "auth.grpc.pb.h"
#include "auth/AuthService.h"

class AuthServiceImpl final : public alexandria::v1::AuthService::Service {
public:
    explicit AuthServiceImpl(AuthService& authService);

    grpc::Status Login(grpc::ServerContext* context, const alexandria::v1::LoginRequest* request, alexandria::v1::LoginResponse* response) override;

private:
    AuthService& m_authService;
};