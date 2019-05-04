#include <random>
#include "schedulers.h"

namespace tdl {

    workerlist_t::iterator load_balancing_scheduler::operator()(workerlist_t::iterator begin,
                                                                workerlist_t::iterator end)
    {
        return std::min_element(begin, end, [](worker_ptr lhs, worker_ptr rhs){
            return lhs->task_count() < rhs->task_count();
        });
    }

    workerlist_t::iterator round_robin_scheduler::operator()(workerlist_t::iterator begin,
                                                             workerlist_t::iterator end)
    {
        static std::size_t counter = 1;
        std::size_t index = counter++ % (end-begin);
        return (begin + index);
    }

    random_scheduler::random_scheduler(){
        std::srand(std::time(nullptr));
    }

    workerlist_t::iterator random_scheduler::operator()(workerlist_t::iterator begin,
                                                        workerlist_t::iterator end)
    {
        std::size_t index = std::rand() % (end-begin);
        return (begin + index);
    }

} // namespace tdl

