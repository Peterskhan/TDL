#pragma once
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

namespace tdl {

    /**
     * @brief The task_context_exception class is used
     *        to indicate when the user attempts to invoke
     *        a method only valid inside task-execution
     *        context outside of such circumstances.
     */
    class task_context_exception final : public std::exception {
    public:
        virtual const char *what() const noexcept override {
            return "tdl::task_context_exception: Task context method invoked from "
                   "non-task context.";
        }
    };

    /**
     * @brief The initialization_exception class is used
     *        to indicate when the user attempts to invoke
     *        TDL methods without the library being initialized.
     */
    class initialization_exception final : public std::exception {
    public:
        virtual const char *what() const noexcept override {
            return "tdl::initialization_exception: Invoking TDL methods "
                   "before initialization.";
        }
    };

    /**
     * @brief The wrong_thread_exception class is used
     *        to indicate when the user attempts to invoke
     *        tdl::process_main() from outside the main thread.
     */
    class wrong_thread_exception final : public std::exception {
    public:
        virtual const char *what() const noexcept override {
            return "tdl::wrong_thread_exception: Invoking TDL method with "
                   "main thread affinity from outside the main thread.";
        }
    };

    /**
     * @brief The scheduler_exception class is used
     *        to indicate when the supplied scheduler
     *        could not schedule a submitted Task.
     */
    class scheduler_exception final : public std::exception {
    public:
        virtual const char *what() const noexcept override {
            return "tdl::scheduler_exception: Applied scheduler could not "
                   "schedule the submitted task.";
        }
    };



} // namespace tdl

#endif // EXCEPTIONS_H
