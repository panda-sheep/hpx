//
// Created by shahrzad on 4/23/20.
//

#ifndef HPX_SPLITTABLE_TASK_HPP
#define HPX_SPLITTABLE_TASK_HPP

#include <type_traits>
#include <utility>

template <typename F>
struct splittable_task
{
    std::size_t start_;
    std::size_t stop_;
    std::size_t index_;
    F f_;
    static std::size_t num_tasks;


    template <typename F_, typename Shape>
    splittable_task(F_&& f, Shape const& elem)
        : start_(hpx::util::get<0>(elem)), stop_(hpx::util::get<1>(elem)), index_(hpx::util::get<2>(elem)), f_(std::forward<F_>(f))
    {
    }

    void operator()()
    {

        std::vector<hpx::future<void>> futures;
        std::size_t num_free = 1;
        if ( num_tasks < 8 )  //split the current task among the idle cores
        {
            futures.push_back(hpx::async(
                splittable_task(f_, hpx::util::make_tuple(start_, start_ + (stop_ - start_) / 2, index_ + 1))));
            start_ = start_ + (stop_ - start_) / 2 ;
            num_tasks += 1;
        }

//        std::cout<<"task "<<num_tasks<<" from: "<<start_<<" to: "<<stop_<<std::endl;

        f_(hpx::util::make_tuple(start_, stop_, 0));


        if (!futures.empty())
        {
            wait_all(futures);
        }

    }
};

template <typename F>
std::size_t splittable_task<F>::num_tasks = 0;

template <typename F, typename Shape>
splittable_task<typename std::decay<F>::type> make_splittable_task(F&& f, Shape const& s)
{
    return splittable_task<typename std::decay<F>::type>(std::forward<F>(f), s);
}

#endif    //HPX_SPLITTABLE_TASK_HPP
