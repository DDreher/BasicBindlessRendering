#pragma once

#ifdef NDEBUG
    #define CHECK_MSG(Expression, ...)
    #define CHECK(Expression)
    #define VERIFY_MSG(Expression, ...) Expression
    #define VERIFY(Expression) Expression
    #define CHECK_NO_ENTRY()
#else
    #include <filesystem>

    #include "spdlog/fmt/fmt.h"

    #define STRINGIFY(x) #x
    #define INTERNAL_ASSERT_IMPL(Expression, Msg) if(!(Expression)) { LOG_ERROR(Msg); __debugbreak(); }
    #define ASSERT_WITH_MSG(Expression, Msg)\
                            INTERNAL_ASSERT_IMPL(Expression, fmt::format("Assertion '{0}' failed at {1}:{2} - Message: {3}",\
                            STRINGIFY(Expression), std::filesystem::path(__FILE__).filename().string(), __LINE__, Msg))

    #define CHECK_MSG(Expression, ...) ASSERT_WITH_MSG(Expression, fmt::format(__VA_ARGS__))
    #define CHECK(Expression) ASSERT_WITH_MSG(Expression, "No message specified")
    #define VERIFY_MSG(Expression, ...) CHECK_MSG(Expression, __VA_ARGS__)
    #define VERIFY(Expression) CHECK(Expression)
    
    #define CHECK_NO_ENTRY() CHECK_MSG(false, "You should not be here!");
#endif
