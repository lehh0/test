#ifndef BACKEND_INCLUDE_CONTRACT_HPP
#define BACKEND_INCLUDE_CONTRACT_HPP

#include <filesystem>

#include <boost/current_function.hpp>
#include <fmt/xchar.h>
#include <include/result_code.hpp>
#include <spdlog/spdlog.h>

constexpr bool IsFail(ResultCode rc)
{
    return rc != ResultCode::sOk;
}

constexpr bool IsFail(bool rc)
{
    return !rc;
}

constexpr const char* FormatResultCode(ResultCode rc)
{
    switch (rc) 
    {
        case ResultCode::sOk:
            return "ResultCode::sOk";
        case ResultCode::eFalse:
            return "ResultCode::eFalse";
        case ResultCode::eFail:
            return "ResultCode::eFail";
        default:
            return "unknown ResultCode";
    }
}

constexpr const char *FormatResultCode(bool rc) {
    return rc
        ? "ResultCode::sOk"
        : "ResultCode::eFalse";
}

class ResultCodeException
    : public std::exception
{
public:
    ResultCodeException(ResultCode rc, std::string msg)
        : std::exception()
        , m_rc(rc)
        , m_msg(std::move(msg))
    {
    }

    ResultCodeException(bool rc, std::string msg)
        : std::exception()
        , m_rc(rc 
            ? ResultCode::sOk
            : ResultCode::eFalse)
        , m_msg(std::move(msg))
    { 
    }

    const ResultCode m_rc;
    const std::string m_msg;
};

#define TRACE_DEBUG(msg) \
    spdlog::debug(BOOST_CURRENT_FUNCTION " {}", msg);

#define TRACE_INFO(msg) \
    spdlog::info(BOOST_CURRENT_FUNCTION " {}", msg);

#define TRACE_ERROR(msg) \
    spdlog::error(BOOST_CURRENT_FUNCTION " {}", msg);

#define TRACE_ERROR_RC(resultCode, msg)               \
    spdlog::error(BOOST_CURRENT_FUNCTION " ({}) {}",  \
            FormatResultCode(resultCode), msg);

#define CHECK(funcCall, msg)                         \
    {                                                \
        auto resultCode = (funcCall);                \
        if (IsFail(resultCode)) {                    \
            throw ResultCodeException(resultCode,    \
                     BOOST_CURRENT_FUNCTION msg);    \
        }                                            \
    }

#define CHECK_SUCCEEDED(funcCall, msg)            \
    {                                             \
        bool resultCode = (funcCall);             \
        if (IsFail(resultCode)) {                 \
            throw ResultCodeException(resultCode, \
                BOOST_CURRENT_FUNCTION msg);      \
        }                                         \
    }

#define CHECK_RETURN(funcCall, msg)               \
    {                                             \
        auto resultCode = (funcCall);             \
        if (IsFail(resultCode)) {                 \
            TRACE_ERROR_RC(resultCode, msg);      \
            return resultCode;                    \
        }                                         \
    }

#define CHECK_SUCCEEDED_RETURN(funcCall, msg)     \
    {                                             \
        bool result = (funcCall);                 \
        if (IsFail(result)) {                     \
            TRACE_ERROR_RC(result, msg);          \
            return result;                        \
        }                                         \
    }

inline ResultCode ManageException(std::string msg)
{
    try 
    {
        throw;
    }
    catch (const ResultCodeException &e) 
    {
        TRACE_ERROR_RC(e.m_rc, fmt::format("{} {}", msg, e.m_msg));
    }
    catch (const std::exception& e) 
    {
        TRACE_ERROR(fmt::format("{} {}", msg, e.what()));
    }
    catch (...) 
    {
        TRACE_ERROR(fmt::format("{} unknown error", msg));
    }

    return ResultCode::eFail;
}

#endif// BACKEND_INCLUDE_CONTRACT_HPP