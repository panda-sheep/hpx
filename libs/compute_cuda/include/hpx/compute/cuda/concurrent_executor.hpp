//  Copyright (c) 2016 Thomas Heller
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if defined(HPX_HAVE_CUDA)
#include <hpx/execution/traits/executor_traits.hpp>

#include <hpx/async_cuda/target.hpp>
#include <hpx/compute/cuda/concurrent_executor_parameters.hpp>
#include <hpx/compute/cuda/default_executor.hpp>
#include <hpx/compute/host/block_executor.hpp>
#include <hpx/compute/host/target.hpp>

#include <array>
#include <atomic>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

namespace hpx { namespace cuda { namespace experimental {
    template <typename Executor =
                  hpx::parallel::execution::restricted_thread_pool_executor>
    struct concurrent_executor
    {
    private:
        typedef hpx::compute::host::block_executor<Executor> host_executor_type;
        typedef cuda::experimental::default_executor cuda_executor_type;

    public:
        // By default, this executor relies on a special executor parameters
        // implementation which knows about the specifics of creating the
        // bulk-shape ranges for the accelerator.
        typedef concurrent_executor_parameters executor_parameters_type;

        concurrent_executor(hpx::cuda::experimental::target const& cuda_target,
            std::vector<hpx::compute::host::target> const& host_targets)
          : host_executor_(host_targets)
          , current_(0)
        {
            std::size_t num_targets = host_targets.size();
            cuda_executors_.reserve(num_targets);
            for (std::size_t i = 0; i != num_targets; ++i)
            {
                hpx::cuda::experimental::target t(
                    cuda_target.native_handle().get_device());
                t.native_handle().get_stream();
                cuda_executors_.emplace_back(std::move(t));
            }
        }

        concurrent_executor(concurrent_executor const& other)
          : host_executor_(other.host_executor_)
          , cuda_executors_(other.cuda_executors_)
          , current_(0)
        {
        }

        concurrent_executor(concurrent_executor&& other)
          : host_executor_(std::move(other.host_executor_))
          , cuda_executors_(std::move(other.cuda_executors_))
          , current_(other.current_.load())
        {
        }

        concurrent_executor& operator=(concurrent_executor const& other)
        {
            if (&other != this)
            {
                host_executor_ = other.host_executor_;
                cuda_executors_ = other.cuda_executors_;
                current_ = 0;
            }

            return *this;
        }

        concurrent_executor& operator=(concurrent_executor&& other)
        {
            if (&other != this)
            {
                host_executor_ = std::move(other.host_executor_);
                cuda_executors_ = std::move(other.cuda_executors_);
                current_ = other.current_.load();
            }

            return *this;
        }

        /// \cond NOINTERNAL
        bool operator==(concurrent_executor const& rhs) const noexcept
        {
            return host_executor_ == rhs.host_executor_ &&
                std::equal(cuda_executors_.begin(), cuda_executors_.end(),
                    rhs.cuda_executors_.begin());
        }

        bool operator!=(concurrent_executor const& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        hpx::compute::host::target const& context() const noexcept
        {
            return host_executor_.context();
        }
        /// \endcond

        std::size_t processing_units_count() const
        {
            return cuda_executors_.size();
        }

        template <typename F, typename... Ts>
        void post(F&& f, Ts&&... ts)
        {
            std::size_t current = ++current_ % cuda_executors_.size();
            parallel::execution::post(
                host_executor_,
                [this, current](F&& f, Ts&&... ts) mutable {
                    parallel::execution::post(cuda_executors_[current],
                        std::forward<F>(f), std::forward<Ts>(ts)...);
                },
                std::forward<F>(f), std::forward<Ts>(ts)...);
        }

        template <typename F, typename... Ts>
        hpx::future<void> async_execute(F&& f, Ts&&... ts)
        {
            std::size_t current = ++current_ % cuda_executors_.size();
            return parallel::execution::async_execute(
                host_executor_,
                [this, current](F&& f, Ts&&... ts) mutable {
                    return parallel::execution::async_execute(
                        cuda_executors_[current], std::forward<F>(f),
                        std::forward<Ts>(ts)...);
                },
                std::forward<F>(f), std::forward<Ts>(ts)...);
        }

        template <typename F, typename... Ts>
        void sync_execute(F&& f, Ts&&... ts)
        {
            std::size_t current = ++current_ % cuda_executors_.size();
            parallel::execution::sync_execute(
                host_executor_,
                [this, current](F&& f, Ts&&... ts) mutable {
                    parallel::execution::sync_execute(cuda_executors_[current],
                        std::forward<F>(f), std::forward<Ts>(ts)...);
                },
                std::forward<F>(f), std::forward<Ts>(ts)...);
        }

        template <typename F, typename Shape, typename... Ts>
        std::vector<hpx::future<void>> bulk_async_execute(
            F&& f, Shape const& shape, Ts&&... ts)
        {
            std::size_t cnt = util::size(shape);
            std::vector<hpx::future<void>> result;
            result.reserve(cnt);

            for (auto const& s : shape)
            {
                std::size_t current = ++current_ % cuda_executors_.size();
                result.push_back(parallel::execution::async_execute(
                    host_executor_,
                    [this, current, s](F&& f, Ts&&... ts) mutable {
                        typedef typename hpx::util::decay<decltype(s)>::type
                            shape_type;

                        std::array<shape_type, 1> cuda_shape{{s}};
                        parallel::execution::bulk_sync_execute(
                            cuda_executors_[current], f, cuda_shape, ts...);
                    },
                    f, ts...));
            }
            return result;
        }

    private:
        host_executor_type host_executor_;
        std::vector<cuda_executor_type> cuda_executors_;
        std::atomic<std::size_t> current_;
    };
}}}    // namespace hpx::cuda::experimental

namespace hpx { namespace parallel { namespace execution {
    template <typename Executor>
    struct executor_execution_category<
        cuda::experimental::concurrent_executor<Executor>>
    {
        typedef parallel::execution::parallel_execution_tag type;
    };

    template <typename Executor>
    struct is_one_way_executor<
        cuda::experimental::concurrent_executor<Executor>> : std::true_type
    {
    };

    template <typename Executor>
    struct is_two_way_executor<
        cuda::experimental::concurrent_executor<Executor>> : std::true_type
    {
    };

    template <typename Executor>
    struct is_bulk_one_way_executor<
        cuda::experimental::concurrent_executor<Executor>> : std::true_type
    {
    };

    template <typename Executor>
    struct is_bulk_two_way_executor<
        cuda::experimental::concurrent_executor<Executor>> : std::true_type
    {
    };
}}}    // namespace hpx::parallel::execution

#endif
