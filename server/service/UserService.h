#pragma once

#include <string>
#include <vector>

#include "domain/User.h"
#include "repository/IUserRepository.h"
#include "service/OperationResult.h"
#include "service/ServiceResult.h"

class UserService {
public:
    explicit UserService(IUserRepository& repository);

    ServiceResult<User> createUser(const std::string& username, const std::string& password, Role role);
    std::vector<User> listUsers();
    OperationResult deleteUser(int id);

private:
    IUserRepository& m_repository;
};