#pragma once

#include <string>

struct OperationResult {
    bool success = false;
    std::string error;

    static OperationResult ok();
    static OperationResult fail(std::string message);
};