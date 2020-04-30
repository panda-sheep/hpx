//
// Created by shahrzad on 4/23/20.
//

#ifndef HPX_SPLITTABLE_TASK_HPP
#define HPX_SPLITTABLE_TASK_HPP

template <typename F>
struct splittable_task
{
    std::size_t start_;
    std::size_t stop_;
    F f_;
    static std::size_t num_tasks;


    template <typename F_>
    splittable_task(F_&& f, std::size_t start, std::size_t stop)
        : start_(start), stop_(stop), f_(std::forward<F_>(f))
    {
    }

    void operator()()
    {

        std::vector<hpx::future<void>> futures;
        std::size_t num_free = 1;
        if ( num_tasks < 8 )  //split the current task among the idle cores
        {
            futures.push_back(hpx::async(
                splittable_task(f_, start_, start_ + (stop_ - start_) / 2)));
            start_ = start_ + (stop_ - start_) / 2 ;
            num_tasks += 1;
        }

//        std::cout<<"task "<<num_tasks<<" from: "<<start_<<" to: "<<stop_<<std::endl;

        f_(start_, stop_ - start_, 0);


        if (!futures.empty())
        {
            wait_all(futures);
        }

    }
};

template <typename F>
std::size_t splittable_task<F>::num_tasks = 0;

template <typename F>
splittable_task<F> make_splittable_task(F&& f, std::size_t start, std::size_t stop)
{
    return splittable_task<F>(std::forward<F>(f), start, stop);
}

#endif    //HPX_SPLITTABLE_TASK_HPP
