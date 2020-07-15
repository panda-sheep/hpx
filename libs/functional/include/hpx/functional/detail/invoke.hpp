//  Copyright (c) 2017-2020 Agustin Berge
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#include <functional>
#include <type_traits>
#include <utility>

namespace hpx { namespace util { namespace detail {

    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct is_reference_wrapper : std::false_type
    {
    };

    template <typename T>
    struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type
    {
    };

    ///////////////////////////////////////////////////////////////////////////
    // when `pm` is a pointer to member of a class `C` and
    // `is_base_of_v<C, remove_reference_t<T>>` is `true`;
    template <typename C, typename T,
        typename = typename std::enable_if<std::is_base_of<C,
            typename std::remove_reference<T>::type>::value>::type>
    static constexpr T&& mem_ptr_target(T&& v) noexcept
    {
        return std::forward<T>(v);
    }

    // when `pm` is a pointer to member of a class `C` and
    // `remove_cvref_t<T>` is a specialization of `reference_wrapper`;
    template <typename C, typename T,
        typename = typename std::enable_if<
            detail::is_reference_wrapper<typename std::remove_cv<
                typename std::remove_reference<T>::type>::type>::value>::type>
    static constexpr auto mem_ptr_target(T&& v) noexcept -> decltype(v.get())
    {
        return v.get();
    }

    // when `pm` is a pointer to member of a class `C` and `T` does not
    // satisfy the previous two items;
    template <typename C, typename T>
    static constexpr auto mem_ptr_target(T&& v) noexcept(
        noexcept(*std::forward<T>(v))) -> decltype(*std::forward<T>(v))
    {
        return *std::forward<T>(v);
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename T, bool IsFunction>
    struct invoke_mem_ptr;

    template <typename T, typename C>
    struct invoke_mem_ptr<T C::*, /*IsFunction=*/false>
    {
        T C::*pm;

    public:
        constexpr invoke_mem_ptr(T C::*pm) noexcept
          : pm(pm)
        {
        }

        template <typename T1>
        constexpr auto operator()(T1&& t1) const noexcept(
            noexcept(detail::mem_ptr_target<C>(std::forward<T1>(t1)).*pm))
            -> decltype(detail::mem_ptr_target<C>(std::forward<T1>(t1)).*pm)
        {
            return detail::mem_ptr_target<C>(std::forward<T1>(t1)).*pm;
        }
    };

    template <typename T, typename C>
    struct invoke_mem_ptr<T C::*, /*IsFunction=*/true>
    {
        T C::*pm;

    public:
        constexpr invoke_mem_ptr(T C::*pm) noexcept
          : pm(pm)
        {
        }

        template <typename T1, typename... Tn>
        constexpr auto operator()(T1&& t1, Tn&&... tn) const
            noexcept(noexcept((detail::mem_ptr_target<C>(std::forward<T1>(t1)).*
                pm)(std::forward<Tn>(tn)...)))
                -> decltype((detail::mem_ptr_target<C>(std::forward<T1>(t1)).*
                    pm)(std::forward<Tn>(tn)...))
        {
            return (detail::mem_ptr_target<C>(std::forward<T1>(t1)).*pm)(
                std::forward<Tn>(tn)...);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename F,
        typename FD = typename std::remove_cv<
            typename std::remove_reference<F>::type>::type>
    struct dispatch_invoke
    {
        using type = F&&;
    };

    template <typename F, typename T, typename C>
    struct dispatch_invoke<F, T C::*>
    {
        using type = invoke_mem_ptr<T C::*, std::is_function<T>::value>;
    };

    template <typename F>
    using invoke = typename dispatch_invoke<F>::type;

#define HPX_INVOKE(F, ...)                                                     \
    (::hpx::util::detail::invoke<decltype((F))>(F)(__VA_ARGS__))

}}}    // namespace hpx::util::detail
