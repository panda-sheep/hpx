//
// Created by shahrzad on 4/22/20.
//

#ifndef HPX_SPLITTABLE_EXECUTOR_HPP
#define HPX_SPLITTABLE_EXECUTOR_HPP

#include <hpx/config.hpp>
#include <hpx/execution/traits/is_executor_parameters.hpp>
#include <hpx/serialization/serialize.hpp>
#include <hpx/timing.hpp>
#include <hpx/iterator_support/iterator_range.hpp>

#include <hpx/execution/executors/execution_parameters.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace hpx { namespace parallel { namespace execution {
            ///////////////////////////////////////////////////////////////////////////
            /// Loop iterations are divided into pieces and then assigned to threads.
            /// The number of loop iterations combined is determined based on
            /// measurements of how long the execution of 1% of the overall number of
            /// iterations takes.
            /// This executor parameters type makes sure that as many loop iterations
            /// are combined as necessary to run for the amount of time specified.
            ///
            struct splittable_executor:parallel_executor
            {
            public:
                /// Construct an \a splittable_executor executor parameters object
                ///
                /// \note Default constructed \a splittable_executor executor parameter
                ///       types will use 80 microseconds as the minimal time for which
                ///       any of the scheduled chunks should run.
                ///
                constexpr splittable_executor()
                {
                    std::cout<<"splittable"<<std::endl;
                }

                /// Construct an \a splittable_executor executor parameters object
                ///
                /// \param rel_time     [in] The time duration to use as the minimum
                ///                     to decide how many loop iterations should be
                ///                     combined.

                /// \cond NOINTERNAL
                // Estimate a chunk size based on number of cores used.
                template <typename Executor, typename F>
                std::size_t get_chunk_size(
                    Executor&& exec, F&& f, std::size_t cores, std::size_t count)
                {
                    return count;
                }
                /// \endcond

                template <typename F, typename S, typename... Ts>
                static std::vector<hpx::future<
                    typename detail::bulk_function_result<F, S, Ts...>::type>>
                bulk_async_execute(F&& f, S const& shape, Ts&&... ts)  {
                    std::vector<hpx::future<typename detail::bulk_function_result<F,
                        S, Ts...>::type>>
                        results;
                    results.reserve(hpx::util::size(shape));

                    for (auto const& elem : shape)
                    {
                        results.push_back(
                            hpx::async(make_splittable_task(std::forward<F>(f), hpx::util::get<0>(elem), hpx::util::get<1>(elem))));
                    }

                    return results;
                }


            private:
                friend class hpx::serialization::access;

                /// \cond NOINTERNAL

            };

            template <>
            struct is_bulk_two_way_executor<splittable_executor> : std::true_type
            {
            };

            template <typename Param, typename Exec, typename F>
            std::size_t get_chunk_size(
                Param& param, Exec& exec, F&& f, std::size_t core, std::size_t count)
            {
                return count;
            }
//            template <>
//            struct is_executor_parameters<splittable_executor>
//                : std::true_type
//            {
//            };

//            template <>
//            struct extract_has_variable_chunk_size<splittable_executor>
//                : std::true_type
//            {
//            };
            /// \endcond
            template <typename AnyParameters, typename Executor>
            HPX_FORCEINLINE static std::size_t processing_units_count(AnyParameters&& params,
                                                                      Executor&& exec)
            {
                return hpx::get_os_thread_count();
            }
        }}}    // namespace hpx::parallel::execution

//namespace hpx { namespace parallel { inline namespace v2 {
//            // for_loop
//            namespace detail {
//
//            }
//        }}}
#endif    //HPX_SPLITTABLE_EXECUTOR_HPP
