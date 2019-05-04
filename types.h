#pragma once
#ifndef TYPES_H
#define TYPES_H

#include <memory>
#include <vector>
#include <functional>

namespace tdl {

    class Task;
    class Worker;

    /** RAII smart pointer for referencing Tasks. */
    using task_ptr = std::shared_ptr<Task>;

    /** RAII smart pointer for referencing Workers. */
    using worker_ptr = std::shared_ptr<Worker>;

    /** Container type for storing Workers. */
    using workerlist_t = std::vector<worker_ptr>;

    /** Callable type for storing a scheduling algorithm. */
    using scheduler_t = std::function<workerlist_t::iterator(workerlist_t::iterator begin,
                                                             workerlist_t::iterator end)>;

} // namespace tdl

#endif // TYPES_H
