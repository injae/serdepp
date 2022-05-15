#pragma once

#ifndef __SERDEPP_ADAPTOR_RFLECTION_HPP__
#define __SERDEPP_ADAPTOR_RFLECTION_HPP__

#include <serdepp/serde.hpp>

namespace serde {
    namespace info {
        struct serde_struct_info_hook {
            template<typename T> void func(T& type, std::string_view key) {}
        };

        template<class H=serde_struct_info_hook>
        class type_info_adaptor {
        public:
            using Hook = H;
            constexpr type_info_adaptor(Hook& hook): hook_(hook) {}
            template<typename T>
            inline constexpr void hook(T& type, std::string_view key) {
                hook_.func(type, key);
            }
        private:
            Hook& hook_;
        };


        template<typename M>
        class MemberFromKey {
        public:
            friend type_info_adaptor<MemberFromKey<M>>;
            DERIVE_SERDE(MemberFromKey, _SF_(member_)_SF_(key_))
            constexpr MemberFromKey(std::string_view key) : key_(key) {}
            M& value() { return *member_; }
        private:
            template<typename U>
            inline constexpr void func(U& data, std::string_view key) {
                if constexpr(std::is_same_v<serde::remove_cvref_t<U>, M>) {
                    if(key == key_) { member_ = &data; }
                }
            }
            M* member_;
            std::string_view key_;
        };

        template<typename T, size_t Index>
        class MemberFromIndex {
        public:
            DERIVE_SERDE(MemberFromIndex,(&Self::member_, "member"))
            friend type_info_adaptor<MemberFromIndex<T, Index>>;
            using M = std::tuple_element_t<Index, serde::to_tuple_t<T>>;
            constexpr static size_t index = Index;
            constexpr MemberFromIndex() : visit_(0) {}
            constexpr inline M& value() { return *member_; }
        private:
            template<typename U>
            inline constexpr void func(U& data, std::string_view key) {
                if constexpr(std::is_same_v<serde::remove_cvref_t<U>, M>) {
                    if(index == visit_) { member_ = &data; }
                }
                visit_++;
            }
            size_t visit_;
            M* member_;
        };

        template<typename T, size_t Index>
        class MemberFromIndexWithKey {
        public:
            DERIVE_SERDE(MemberFromIndexWithKey, (&Self::name_, "name")(&Self::member_, "member"))
            friend type_info_adaptor<MemberFromIndexWithKey<T, Index>>;
            using M = std::tuple_element_t<Index, serde::to_tuple_t<T>>;
            constexpr static size_t index = Index;
            constexpr MemberFromIndexWithKey() : visit_(0) {}
            constexpr inline M& value() { return *member_; }
            constexpr inline std::string_view name() const { return name_; }
        private:
            template<typename U>
            inline constexpr void func(U& data, std::string_view key) {
                if constexpr(std::is_same_v<serde::remove_cvref_t<U>, M>) {
                    if(index == visit_) {
                        member_ = &data;
                        name_ = key;
                    }
                }
                visit_++;
            }
            size_t visit_;
            std::string_view name_;
            M* member_;
        };

        template<typename T>
        class MemberArray {
        public:
            using value_type = meta::to_variant_t<to_tuple_t<T>>;
            DERIVE_SERDE(MemberArray,(&Self::array_, "array"))
            friend type_info_adaptor<MemberArray<T>>;
            constexpr MemberArray() : visit_(0) {};
            constexpr inline auto& value() { return array_; }
        private:
            template<typename U>
            inline constexpr void func(U& data, std::string_view key) { array_[visit_++] = data; }
            std::array<value_type, tuple_size_v<T>> array_;
            size_t visit_;
        };

        template<class T>
        class MemberNames {
        public:
            DERIVE_SERDE(MemberNames, _SF_(members_))
            friend type_info_adaptor<MemberNames>;
            constexpr MemberNames() = default;
            inline constexpr const auto& members() const { return members_; }
          private:
            template<typename U>
            inline constexpr void func(U& data, std::string_view key) { members_[index_++]=key; }
            std::array<std::string_view, tuple_size_v<T>> members_;
            size_t index_;
        };
    };
    
    template<class T>
    struct serde_struct_info {
        DERIVE_SERDE(serde_struct_info)
        using Type= meta::remove_cvref_t<T>;
        constexpr static size_t size = tuple_size_v<Type>;
        constexpr static std::string_view name = nameof::nameof_type<Type>();

        constexpr serde_struct_info() = default;
        //constexpr std::string_view name() { return nameof::nameof_type<Type>();}

        template<size_t index>
        constexpr typename info::MemberFromIndex<Type, index>::M& member(Type& type) const {
            auto info = info::MemberFromIndex<Type, index>();
            static_assert(index < size);
            serde::deserialize_to(info::type_info_adaptor(info), type);
            return info.value();
        }

        template<size_t index>
        constexpr info::MemberFromIndexWithKey<Type, index> member_info(Type& type) const {
            auto info = info::MemberFromIndexWithKey<Type, index>();
            static_assert(index < size);
            serde::deserialize_to(info::type_info_adaptor(info), type);
            return info;
        }

        template<typename M>
        constexpr M& member(Type& type, std::string_view key) const {
            auto info = info::MemberFromKey<M>(key);
            serde::deserialize_to(info::type_info_adaptor(info), type);
            return info.value();
        }

         constexpr info::MemberNames<Type> member_names() const {
            auto info = info::MemberNames<Type>();
            serde::deserialize<Type>(info::type_info_adaptor(info));
            return info;
        }
    };

    template<typename T> struct serde_adaptor_helper<info::type_info_adaptor<T>>
        : derive_serde_adaptor_helper<info::type_info_adaptor<T>> {
        using Adaptor = info::type_info_adaptor<T>;
        inline constexpr static bool is_null(Adaptor& adaptor, std::string_view key) { return false; }
        inline constexpr static size_t size(Adaptor& adaptor) { return 1; }
        inline constexpr static bool is_struct(Adaptor& adaptor) { return true; }
    };

    template<typename T, typename F, typename Hook> struct serde_adaptor<info::type_info_adaptor<Hook>, T, F> {
        using Adaptor = info::type_info_adaptor<Hook>;
        inline constexpr static void from(Adaptor& s, std::string_view key, T& data) { s.hook(data, key); }
        inline constexpr static void into(Adaptor& s, std::string_view key, const T& data) { s.hook(data, key); } 
    };

    template<typename T>
    [[maybe_unused]] constexpr static auto type_info = serde_struct_info<T>();

    namespace detail {
        template<class T, std::size_t... idx>
        auto make_tuple_impl(T& value, std::index_sequence<idx...>) {
            return std::make_tuple(type_info<T>.template member<idx>(value)...);
        }
    }

    template<class T>
    to_tuple_t<T> make_tuple(T& value) {
        using rtype = meta::remove_cvref_t<T>;
        return detail::make_tuple_impl(value, std::make_index_sequence<tuple_size_v<rtype>>());  
    }

    template<class H> struct special_adaptor<info::type_info_adaptor<H>> : std::true_type {};
}

#endif
