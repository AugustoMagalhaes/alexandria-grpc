#pragma once

#include <optional>
#include <string>
#include <utility>

template <typename T>
struct ServiceResult {
    bool success = false;
    std::optional<T> value;
    std::string error;

    static ServiceResult<T> ok(T v)
    {
        ServiceResult<T> result;
        result.success = true;
        result.value = std::move(v);
        return result;
    }

    static ServiceResult<T> fail(std::string message)
    {
        ServiceResult<T> result;
        result.success = false;
        result.error = std::move(message);
        return result;
    }
};