#pragma once

#ifndef __SERDEPP_ATTIRBUTE_CLI11_HPP__
#define __SERDEPP_ATTIRBUTE_CLI11_HPP__

#include <serdepp/adaptor/cli11.hpp>
#include <serdepp/attribute/utility.hpp>

namespace serde::attribute::cli11 {
    struct flag {
        using Hook = std::function<void(CLI::Option*)>;
        flag(std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : key_(key), desc_(desc), hook_(std::move(hook)) {};
        std::string_view key_, desc_;
        Hook&& hook_;
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                hook_(ctx.adaptor.add_flag(std::string{key_}, data, std::string{desc_}));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct group_flag {
        using Hook = std::function<void(CLI::Option*)>;
        group_flag(std::string_view group, std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : group_(group), key_(key), desc_(desc), hook_(std::move(hook)) {};
        std::string_view group_, key_, desc_;
        Hook&& hook_;
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                hook_(ctx.adaptor.get_option_group(std::string{group_})->add_flag(std::string{key_}, data, std::string{desc_}));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct option {
        using Hook = std::function<void(CLI::Option*)>;
        option(std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : key_(key), desc_(desc), hook_(std::move(hook)) {};
        std::string_view key_, desc_;
        Hook&& hook_;
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                hook_(ctx.adaptor.add_option(std::string{key_}, data, std::string{desc_}));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct group_option {
        using Hook = std::function<void(CLI::Option*)>;
        group_option(std::string_view group, std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : group_(group), key_(key), desc_(desc), hook_(std::move(hook)) {};
        std::string_view group_, key_, desc_;
        Hook&& hook_;
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                hook_(ctx.adaptor.get_option_group(std::string{group_})->add_option(std::string{key_}, data, std::string{desc_}));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct desc {
        desc(std::string_view desc) : desc_(desc) {};
        std::string_view desc_;
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                ctx.adaptor.description(std::string{desc_});
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct command {
        using Hook = std::function<void(CLI::App&)>;
        command(std::string_view desc, Hook&& hook=[](CLI::App& a){}) : desc_(desc), hook_(std::move(hook)) {};
        std::string_view desc_;
        Hook&& hook_;
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                ctx.adaptor.description(std::string{desc_});
                hook_(ctx.adaptor);
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    template<class Hook>
    struct callback {
        callback(Hook&& hook) : hook_(hook) {};
        Hook&& hook_;
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                ctx.adaptor.final_callback([&data, hook = std::move(hook_)](){ hook(data); });
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };

    struct option_group {
        using Hook = std::function<void(CLI::App*)>;
        option_group(std::string_view name, std::string_view desc, Hook&& hook=[](CLI::App* o){}) : name_(name), desc_(desc), hook_(std::move(hook)) {};
        std::string_view name_, desc_;
        Hook&& hook_;
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(std::is_same_v<typename serde_ctx::Adaptor, CLI::App>) {
                hook_(ctx.adaptor.add_option_group(std::string{name_}, std::string{desc_}));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    };
}

#endif
