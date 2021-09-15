#pragma once

#include <iostream>

#ifndef __SERDEPP_EXCEPTION_HPP__
#define __SERDEPP_EXCEPTION_HPP__

namespace serde {
    struct exception : std::exception {
        explicit exception() {}
        virtual ~exception() noexcept override = default;
        virtual const char* what() const noexcept override {return "";}
    };

    struct unimplemented_error :exception {
        explicit unimplemented_error(std::string what) : what_("unimplemented function: " + what + '\n') {}
        virtual ~unimplemented_error() noexcept override = default;
        virtual const char* what() const noexcept override {return what_.c_str();}
    protected:
        std::string what_;
    };

    struct variant_error :exception {
        explicit variant_error(std::string what) : what_("varaint error: " + what + '\n') {}
        virtual ~variant_error() noexcept override = default;
        virtual const char* what() const noexcept override {return what_.c_str();}
    protected:
        std::string what_;
    };

    struct enum_error : exception {
    public:
        explicit enum_error(std::string what) : what_("unregisted enum value: " + what + '\n') {}
        virtual ~enum_error() noexcept override = default;
        virtual const char* what() const noexcept override {return what_.c_str();}
    protected:
        std::string what_;
    };

    struct unregisted_data_error : exception {
    public:
        explicit unregisted_data_error(std::string what) : what_("unregisted data: " + what + '\n') {}
        virtual ~unregisted_data_error() noexcept override = default;
        virtual const char* what() const noexcept override {return what_.c_str();}
    protected:
        std::string what_;
    };

    struct attribute_error : exception {
    public:
        explicit attribute_error(std::string what) : what_("attribute error: " + what + '\n') {}
        virtual ~attribute_error() noexcept override = default;
        virtual const char* what() const noexcept override {return what_.c_str();}
    protected:
        std::string what_;
    };
}

#endif
