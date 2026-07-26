#include "service/OperationResult.h"

OperationResult OperationResult::ok()
{
    OperationResult result;
    result.success = true;
    return result;
}

OperationResult OperationResult::fail(std::string message)
{
    OperationResult result;
    result.success = false;
    result.error = std::move(message);
    return result;
}