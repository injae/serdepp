#pragma once

#ifndef __SERDEPP_ATTIRBUTE_CLI11_HPP__
#define __SERDEPP_ATTIRBUTE_CLI11_HPP__

#include <serdepp/adaptor/cli11.hpp>
#include <serdepp/attribute/utility.hpp>

namespace serde::attribute::cli11 {

    class flag {
    public:
        using Hook = std::function<void(CLI::Option*)>;
        flag(std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : key_(key), desc_(desc), hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                hook_(ctx.adaptor.add_flag(std::string{key_}, data, std::string{desc_}));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view key_, desc_;
        Hook&& hook_;
    };

    // require flag
    class flag_r {
    public:
        using Hook = std::function<void(CLI::Option*)>;
        flag_r(std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : key_(key), desc_(desc), hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                hook_(ctx.adaptor.add_flag(std::string{key_}, data, std::string{desc_})->required());
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view key_, desc_;
        Hook&& hook_;
    };

    class group_flag {
    public:
        using Hook = std::function<void(CLI::Option*)>;
        group_flag(std::string_view group, std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : group_(group), key_(key), desc_(desc), hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                hook_(ctx.adaptor.get_option_group(std::string{group_})
                      ->add_flag(std::string{key_}, data, std::string{desc_}));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view group_, key_, desc_;
        Hook&& hook_;
    };

    // group require flag
    class group_flag_r {
    public:
        using Hook = std::function<void(CLI::Option*)>;
        group_flag_r(std::string_view group, std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : group_(group), key_(key), desc_(desc), hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                hook_(ctx.adaptor.get_option_group(std::string{group_})
                      ->add_flag(std::string{key_}, data, std::string{desc_})
                      ->required());
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view group_, key_, desc_;
        Hook&& hook_;
    };

    class option {
    public:
        using Hook = std::function<void(CLI::Option*)>;
        option(std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : key_(key), desc_(desc), hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                hook_(ctx.adaptor.add_option(std::string{key_}, data, std::string{desc_}));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view key_, desc_;
        Hook&& hook_;
    };

    class group_option {
    public:
        using Hook = std::function<void(CLI::Option*)>;
        group_option(std::string_view group, std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : group_(group), key_(key), desc_(desc), hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                hook_(ctx.adaptor.get_option_group(std::string{group_})
                      ->add_option(std::string{key_}, data, std::string{desc_}));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view group_, key_, desc_;
        Hook&& hook_;
    };

    // require option
    class option_r {
    public:
        using Hook = std::function<void(CLI::Option*)>;
        option_r(std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : key_(key), desc_(desc), hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                hook_(ctx.adaptor.add_option(std::string{key_}, data, std::string{desc_})->required());
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view key_, desc_;
        Hook&& hook_;
    };

    // group require option
    class group_option_r {
    public:
        using Hook = std::function<void(CLI::Option*)>;
        group_option_r(std::string_view group, std::string_view key, std::string_view desc, Hook&& hook = [](CLI::Option* o){})
            : group_(group), key_(key), desc_(desc), hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                hook_(ctx.adaptor.get_option_group(std::string{group_})
                      ->add_option(std::string{key_}, data, std::string{desc_})
                      ->required());
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view group_, key_, desc_;
        Hook&& hook_;
    };

    class command {
    public:
        using Hook = std::function<void(CLI::App&)>;
        command(std::string_view desc, Hook&& hook=[](CLI::App& a){})
            : desc_(desc), hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                ctx.adaptor.description(std::string{desc_});
                hook_(ctx.adaptor);
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view desc_;
        Hook&& hook_;
    };

    // for struct attributes(...)
    class desc {
    public:
        desc(std::string_view desc) : desc_(desc) {};

        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                ctx.adaptor.description(std::string{desc_});
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view desc_;
    };


    template<class Hook>
    class callback {
    public:
        callback(Hook&& hook) : hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                ctx.adaptor.final_callback([&data, hook = std::move(hook_)](){ hook(data); });
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        Hook&& hook_;
    };

    template<class Hook>
    class sub_command {
    public:
        sub_command(std::string_view name, std::string_view desc, Hook&& hook)
            : name_(name), desc_(desc), hook_(std::move(hook)) {};

        SERDEPP_DEFAULT_ATTRIBUTE_FROM()
        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                ctx.adaptor
                    .add_subcommand(std::string{name_}, std::string{desc_})
                    ->final_callback([&data, hook = std::move(hook_)](){
                        hook(data);
                        throw CLI::Success{};
                    });
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view name_, desc_;
        Hook&& hook_;
    };

    class option_group {
    public:
        using Hook = std::function<void(CLI::App*)>;
        option_group(std::string_view name, std::string_view desc, Hook&& hook=[](CLI::App* o){})
            : name_(name), desc_(desc), hook_(std::move(hook)) {};
        SERDEPP_DEFAULT_ATTRIBUTE_FROM()

        template<typename T, typename serde_ctx, typename Next, typename ...Attributes>
        inline void into(serde_ctx& ctx, T& data, std::string_view key,
                                    Next&& next_attr, Attributes&&... remains) {
            if constexpr(is_adaptor_v<serde_ctx, CLI::App>) {
                hook_(ctx.adaptor.add_option_group(std::string{name_}, std::string{desc_}));
            }
            next_attr.into(ctx, data, key, remains...);
        }
    private:
        std::string_view name_, desc_;
        Hook&& hook_;
    };
}

#endif
