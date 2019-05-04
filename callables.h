#pragma once
#ifndef CALLABLES_H
#define CALLABLES_H

#include <functional>

#include "task.h"

namespace tdl {

    /**
     * @brief The CallableWithoutReturn class is a
     *        helper class that is used by make_task()
     *        to bind Callable entities (such as functions,
     *        lambdas, functors, etc.) to Tasks.
     *        This particular class is used to bind
     *        Callables without a return type.
     *        See make_task() for details.
     */
    class CallableWithoutReturn : public Task {
    public:
        using function_t = std::function<void()>;

        /**
         * @brief Constructs a CallableWithoutReturn object.
         * @param Callable object convertible to function_t.
         */
        CallableWithoutReturn(function_t function);
    private:
        function_t m_function;

        /**
         * @brief Executes the bound Callable.
         */
        virtual void execute() override;
    };

} // namespace tdl

#endif // CALLABLES_H
