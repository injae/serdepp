#pragma once

#include <iostream>
#include <fmt/format.h>

#ifndef __SERDEPP_EXCEPTION_HPP__
#define __SERDEPP_EXCEPTION_HPP__

namespace serde {
    struct exception : std::exception {
        explicit exception() {}
        virtual ~exception() noexcept override = default;
        virtual const char* what() const noexcept override {return "";}
    };

    struct unimplemented_error :exception {
        explicit unimplemented_error(std::string what) : what_(fmt::format("unimplemented function: {}\n", what)) {}
        virtual ~unimplemented_error() noexcept override = default;
        virtual const char* what() const noexcept override {return what_.c_str();}
    protected:
        std::string what_;
    };

    struct enum_error : exception {
    public:
        explicit enum_error(std::string what) : what_(fmt::format("unregisted enum value: {}\n", what)) {}
        virtual ~enum_error() noexcept override = default;
        virtual const char* what() const noexcept override {return what_.c_str();}
    protected:
        std::string what_;
    };

    struct unregisted_data_error : exception {
    public:
        explicit unregisted_data_error(std::string what) : what_(fmt::format("unregisted data: {}\n", what)) {}
        virtual ~unregisted_data_error() noexcept override = default;
        virtual const char* what() const noexcept override {return what_.c_str();}
    protected:
        std::string what_;
    };
}


#endif
