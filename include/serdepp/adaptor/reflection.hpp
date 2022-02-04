#pragma once

#ifndef __SERDEPP_ADAPTOR_RFLECTION_HPP__
#define __SERDEPP_ADAPTOR_RFLECTION_HPP__

#include <serdepp/serde.hpp>
#include <string_view>

namespace serde {
    struct serde_struct_size {
        DERIVE_SERDE(serde_struct_size, _SF_(size))
        constexpr serde_struct_size() = default;
        constexpr void visit() { size++; }
        size_t size=0;
        template<typename T>
        inline constexpr size_t type_size() {
            return serde::serialize<serde_struct_size>(T{}).size;
        }
    };
    
    template<>
    struct serde_adaptor_helper<serde_struct_size> : derive_serde_adaptor_helper<serde_struct_size> {
        inline constexpr static bool is_null(serde_struct_size &adaptor, std::string_view key) { return false; }
        inline constexpr static size_t size(serde_struct_size &adaptor) { return adaptor.size; }
        inline constexpr static bool is_struct(serde_struct_size &adaptor) { return true; }
    };


    template<typename T> struct serde_adaptor<serde_struct_size, T> {
        inline constexpr static void from(serde_struct_size& s, std::string_view key, T& data) {}
        inline constexpr static void into(serde_struct_size &s, std::string_view key, const T& data) { s.visit(); } 
    };

    template<typename T>
    inline constexpr size_t type_size() {
        return serde::serialize<serde_struct_size>(T{}).size;
    }

    namespace info {
        struct serde_struct_info_hook {
            template<typename T> void func(T& type, std::string_view key) {}
        };

        template<typename Hook=serde_struct_info_hook>
        class type_info_adaptor {
        public:
            constexpr type_info_adaptor(Hook& hook): hook_(hook) {}
            template<typename T>
            inline constexpr void hook(T& type, std::string_view key) {
                hook_.func(type, key);
            }
        private:
            Hook& hook_;
        };


        struct serde_struct_size {
            DERIVE_SERDE(serde_struct_size, _SF_(size))
            constexpr serde_struct_size() = default;
            template<typename T>
            constexpr void func(T &data, std::string_view key) { size++; }
            size_t size=0;
        };

        template<typename M>
        class MemberFromKey {
        public:
            friend type_info_adaptor<MemberFromKey<M>>;
            DERIVE_SERDE(MemberFromKey, _SF_(key_)_SF_(member_))
            constexpr MemberFromKey(std::string_view key) : key_(key) {}
            M& value() { return *member_; }
        private:
            template<typename T>
            inline constexpr void func(T &data, std::string_view key) {
                if constexpr(std::is_same_v<serde::remove_cvref_t<T>, M>) {
                    if(key == key_) { member_ = &data; }
                }
            }
            M* member_;
            std::string_view key_;
        };

        template<typename M>
        class MemberFromIndex {
        public:
            friend type_info_adaptor<MemberFromIndex<M>>;
            DERIVE_SERDE(MemberFromIndex, _SF_(index_)_SF_(name_)_SF_(member_))
            constexpr MemberFromIndex(size_t index) : index_(index), visit_(0) {}
            constexpr M& value() { return *member_; }
            constexpr std::string_view key() const { return name_; }
        private:
            template<typename T>
            inline constexpr void func(T &data, std::string_view key) {
                if constexpr(std::is_same_v<serde::remove_cvref_t<T>, M>) {
                    if(index_ == visit_) {
                        member_ = &data;
                        name_ = key;
                    }
                }
                visit_++;
            }
            size_t index_;
            size_t visit_;
            std::string_view name_;
            M* member_;
        };

        class MemberNames {
        public:
            friend type_info_adaptor<MemberNames>;
            DERIVE_SERDE(MemberNames, _SF_(members_))
            MemberNames(size_t size)  { members_.reserve(size); }
            inline constexpr const std::vector<std::string_view>& names() const { return members_; }
          private:
            template <typename T>
            inline constexpr void func(T &data, std::string_view key) {
                members_.push_back(key);
            }
            std::vector<std::string_view> members_;
        };

    };
    
    template<typename T>
    class serde_struct_info {
    public:
        using Type=T;
        DERIVE_SERDE(serde_struct_info, _SF_(size)_SF_(name))
        constexpr serde_struct_info(size_t size_, std::string_view name_) : size(size_), name(name_) {}

        template<typename M>
        constexpr info::MemberFromKey<M> member(T& type, std::string_view key) const {
            auto info = info::MemberFromKey<M>(key);
            serde::deserialize_to(info::type_info_adaptor(info), type);
            return info;
        }

        template<typename M>
        constexpr info::MemberFromIndex<M> member(Type& type, size_t index) const {
            auto info = info::MemberFromIndex<M>(index);
            if(index > size) {
                throw serde::type_error("struct size overflow["+ to_string(index) + "]:" + to_string(*this) );
            }
            serde::deserialize_to(info::type_info_adaptor(info), type);
            return info;
        }

         info::MemberNames member_names() const {
            auto info = info::MemberNames(size);
            serde::deserialize<Type>(info::type_info_adaptor(info));
            return info;
        }

        size_t size;
        std::string_view name;
    };

    template<typename T> struct serde_adaptor_helper<info::type_info_adaptor<T>>
        : derive_serde_adaptor_helper<info::type_info_adaptor<T>> {
        inline constexpr static bool is_null(info::type_info_adaptor<T> &adaptor, std::string_view key) {
            return false;
        }
        inline constexpr static size_t size(info::type_info_adaptor<T> &adaptor) { return 1; }
        inline constexpr static bool is_struct(info::type_info_adaptor<T> &adaptor) { return true; }
    };


    template<typename T, typename Hook> struct serde_adaptor<info::type_info_adaptor<Hook>, T> {
        inline constexpr static void from(info::type_info_adaptor<Hook>& s, std::string_view key, T& data) {
            s.hook(data, key);
        }
        inline constexpr static void into(info::type_info_adaptor<Hook> &s, std::string_view key, const T& data) {
            s.hook(data, key);
        } 
    };

    template<typename T>
    inline constexpr serde_struct_info<T> type_info() {
        return serde_struct_info<T>(type_size<T>(), nameof::nameof_type<T>());
    }
}

#endif
