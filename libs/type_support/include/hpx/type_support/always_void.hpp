//  Copyright (c) 2013-2020 Agustin Berge
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace hpx { namespace util {

    template <typename... T>
    struct always_void
    {
        typedef void type;
    };

    template <typename... T>
    using void_t = void;

}}    // namespace hpx::util
