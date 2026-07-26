#pragma once

#include <optional>
#include <string>
#include <utility>

template <typename T>
struct ClientResult {
    bool success = false;
    std::optional<T> value;
    std::string error;

    static ClientResult<T> ok(T v)
    {
        ClientResult<T> result;
        result.success = true;
        result.value = std::move(v);
        return result;
    }

    static ClientResult<T> fail(std::string message)
    {
        ClientResult<T> result;
        result.success = false;
        result.error = std::move(message);
        return result;
    }
};

template <>
struct ClientResult<void> {
    bool success = false;
    std::string error;

    static ClientResult<void> ok()
    {
        ClientResult<void> result;
        result.success = true;
        return result;
    }

    static ClientResult<void> fail(std::string message)
    {
        ClientResult<void> result;
        result.success = false;
        result.error = std::move(message);
        return result;
    }
};