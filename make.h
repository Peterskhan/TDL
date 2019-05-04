#pragma once
#ifndef MAKE_H
#define MAKE_H

#include <functional>
#include <type_traits>

#include "task.h"
#include "types.h"
#include "callables.h"

namespace tdl {

    /**
     * @brief   Allocates and constructs a Task of the specified
     *          type, forwarding the provided arguments to it's
     *          constructor.
     * @param   Arguments to the Task constructor.
     * @return  A tdl::task_ptr to the created Task.
     */
    template <class TaskType, class ...Args>
    inline task_ptr make(Args... args) {
        // Static assertion for TaskType
        static_assert(std::is_base_of<Task,TaskType>::value,
                      "tdl::make_task() requires the template-argument "
                      "to be derived from tdl::Task.");
        return std::make_shared<TaskType>(std::forward<Args>(args)...);
    }

    /**
     * @brief   Allocates and construct a Task from the provided
     *          Callable by binding the provided arguments to it.
     *          The result returned from the callable is discarded.
     * @param   Callable to be executed when processing the Task.
     * @param   Arguments to be bound for Task execution.
     * @return  A tdl::task_ptr to the created Task.
     */
    template <class Function, class... Args>
    task_ptr discards(Function &&function, Args&&... args) {
        return std::make_shared<CallableWithoutReturn>(std::bind(function, std::forward<Args>(args)...));
    }

    /**
     * @brief   Allocates and construct a Task from the provided
     *          Callable by binding the provided arguments to it.
     *          The result returned from the callable is stored
     *          into the provided result reference.
     * @param   Callable to be executed when processing the Task.
     * @param   Reference to where the returned result should be
     *          stored.
     * @param   Arguments to be bound for Task execution.
     * @return  A tdl::task_ptr to the created Task.
     */
    template <class Function, class Result, class... Args>
    task_ptr returns(Function&& function, Result &result, Args&&... args) {
        return discards([=, &result](){
           result = function(args...);
        });
    }

} // namespace tdl

#endif // MAKE_H
