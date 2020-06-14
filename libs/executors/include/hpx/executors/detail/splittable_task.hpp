//  Copyright (c) 2007-2020 Hartmut Kaiser
//  Copyright (c) 2020 Shahrzad Shirzad
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_SPLITTABLE_TASK_HPP
#define HPX_SPLITTABLE_TASK_HPP

#include <hpx/modules/datastructures.hpp>
#include <hpx/modules/execution.hpp>
#include <hpx/modules/runtime_local.hpp>
#include <hpx/modules/synchronization.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace hpx { namespace parallel { namespace execution {

    ///////////////////////////////////////////////////////////////////////////
    enum class splittable_mode
    {
        all = 0,
        idle = 1
    };

    char const* const get_splittable_mode_name(splittable_mode mode)
    {
        static constexpr char const* const splittable_mode_names[] = {
            "all", "idle"};
        return splittable_mode_names[static_cast<std::size_t>(mode)];
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename Executor, typename F>
    struct splittable_task
    {
        template <typename F_, typename Shape>
        splittable_task(Executor& exec, F_&& f, Shape const& elem,
            std::size_t cores, splittable_mode split_type)
          : start_(hpx::util::get<0>(elem))
          , stop_(hpx::util::get<1>(elem))
          , index_(hpx::util::get<2>(elem))
          , num_free_(cores)
          , f_(std::forward<F_>(f))
          , exec_(exec)
          , split_type_(split_type)
        {
            //if (split_type_ == splittable_mode::all)
            //    num_free_ = cores;
            //else
            //    num_free_ = hpx::threads::get_idle_core_count() + 1;
        }

        void operator()(hpx::latch* outer_latch = nullptr)
        {
            if (split_type_ == splittable_mode::idle)
            {
                num_free_ = hpx::threads::get_idle_core_count() + 1;
            }

            {
                hpx::latch l(2);

                std::size_t remainder =
                    ((stop_ - start_) * (num_free_ - 1)) / num_free_;

                if ((num_free_ > 1) && (remainder != 0))
                {
                    // split the current task
                    --num_free_;
                    exec_.post(splittable_task(exec_, f_,
                                   hpx::util::make_tuple(
                                       stop_ - remainder, stop_, index_ + 1),
                                   num_free_, split_type_),
                        &l);
                    stop_ = stop_ - remainder;
                }
                else
                {
                    l.count_down(1);
                }

                f_(hpx::util::make_tuple(start_, stop_ - start_, index_));

                // wait for task scheduled above
                l.arrive_and_wait(1);
            }

            // notify outer waiting task
            if (outer_latch != nullptr)
            {
                outer_latch->count_down(1);
            }
        }

    private:
        std::size_t start_;
        std::size_t stop_;
        std::size_t index_;
        std::size_t num_free_;
        F f_;
        Executor& exec_;
        splittable_mode split_type_;
    };

    template <typename Executor, typename F, typename Shape>
    splittable_task<Executor, typename std::decay<F>::type>
    make_splittable_task(
        Executor& exec, F&& f, Shape const& s, splittable_mode split_type)
    {
        std::size_t cores = hpx::get_os_thread_count();
        return splittable_task<Executor, typename std::decay<F>::type>(
            exec, std::forward<F>(f), s, cores, split_type);
    }

}}}    // namespace hpx::parallel::execution

#endif    // HPX_SPLITTABLE_TASK_HPP
