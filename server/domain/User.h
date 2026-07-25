#pragma once

#include <string>

#include "domain/Role.h"

struct User {
    int id = 0;
    std::string username;
    Role role = Role::User;
};