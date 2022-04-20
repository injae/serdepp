#pragma once

#ifndef __SERDEPP_ATTIRBUTE_CLI11_HPP__
#define __SERDEPP_ATTIRBUTE_CLI11_HPP__

#include <serdepp/adaptor/cli11.hpp>

namespace serde::attribute::cli11 {
    struct flag {
        using Hook = std::function<void(CLI::Option*)>;
        flag(const std::string& key, const std::string& desc, Hook&& hook = [](auto* o){})
            : key_(key), desc_(desc), hook_(std::move(hook)) {};
        const std::string& key_;
        const std::string& desc_;
        Hook&& hook_;
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                hook_(ctx.adaptor.add_flag(key_, data, desc_));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct option {
        using Hook = std::function<void(CLI::Option*)>;
        option(const std::string& key, const std::string& desc, Hook&& hook = [](auto* o){})
            : key_(key), desc_(desc), hook_(std::move(hook)) {};
        const std::string& key_;
        const std::string& desc_;
        Hook&& hook_;
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                hook_(ctx.adaptor.add_option(key_, data, desc_));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct desc {
        using Hook = std::function<void(CLI::Option*)>;
        desc(const std::string& desc) : desc_(desc) {};
        const std::string& desc_;
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        constexpr inline void from(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            next_attr.from(ctx, data, key, remains...);
        }

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                ctx.adaptor.description(desc_);
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };
}

#endif
