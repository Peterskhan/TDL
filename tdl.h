#pragma once
#ifndef TDL_H
#define TDL_H

#include "types.h"
#include "task.h"
#include "worker.h"
#include "dispatcher.h"
#include "make.h"
#include "callables.h"
#include "schedulers.h"
#include "exceptions.h"

/**
 * Namespace tdl groups all functionality and types
 * provided by the TDL library. Nested namespaces
 * are tdl::this_task and tdl::detail (see below).
 */
namespace tdl {

    /**
     * @brief Sets the global scheduler algorithm for TDL.
     *        This call is only effective prior to initialization.
     * @param A Callable object or function convertible
     *        to tdl::scheduler_t. See types.h for details.
     *        (default: tdl::load_balancing_scheduler)
     */
    void set_scheduler(scheduler_t scheduler);

    /**
     * @brief Sets the count of worker threads to create
     *        upon initialization. This call is only effective
     *        prior to initialization.
     * @param The number of workers (default: std::hardware_concurrency())
     */
    void set_worker_count(std::size_t count);

    /**
     * @brief Returns the number of worker threads.
     */
    std::size_t get_worker_count();

    /**
     * @brief   Initializes TDL.
     * @details TDL must be initialized before use by calling
     *          this function. Calling any TDL methods without
     *          proper initialization may result in an exception
     *          of type tdl::initialization_exception being thrown.
     *          This method must be called from the main thread,
     *          invoking it from any other thread is undefined
     *          behavior. Subsequent initialization attempts
     *          are ineffective.
     */
    void initialize();

    /**
     * @brief Returns true if TDL has been initialized,
     *        otherwise false.
     */
    bool initialized();

    /**
     * @brief   Shuts down the TDL implementation.
     * @details Invoking this method sends a stop signal
     *          to all TDL workers, and blocks until they
     *          have finished execution. After shutdown,
     *          calls to TDL methods are ineffective.
     */
    void shutdown();

    /**
     * @brief   Submits a task to the TDL implementation for
     *          scheduling.
     * @details Invokes the scheduler to decide which worker
     *          should handle the supplied task and assigns
     *          it to the worker for later execution.
     * @param   Task to be scheduled (of type tdl::task_ptr).
     */
    void submit(task_ptr task);

    /**
     * @brief   When inside an executing task's body, spawns
     *          the supplied task as a children of the caller.
     * @details The spawned task's parent is automatically set
     *          to the caller, and the task is pushed to the
     *          executing worker's queue. This method requires
     *          to be in the context of an executing task,
     *          invoking it from outside of task execution
     *          results in a tdl::task_context_exception
     *          being thrown.
     * @param   Task to be spawned as a child of the caller.
     */
    void spawn(task_ptr task);

    /**
     * @brief   Processes Tasks with main-thread affinity.
     * @details This method is the only way to process
     *          Tasks that are marked for main-thread
     *          execution. This method must be only called
     *          from the main thread. Calling it from outside
     *          the main thread results in tdl::wrong_thread_exception
     *          being thrown. This method blocks until all Tasks
     *          marked for main-thread execution are done.
     */
    void process_main();

    /**
     * The tdl::this_task namespace groups methods useful
     * for getting information of the currently executing
     * task. As these methods require being in the context
     * of task execution, invoking any of them outside of
     * such context results in a tdl::task_context_exception
     * being thrown (if not otherwise noted).
     */
    namespace this_task {

        /**
         * @brief Returns a tdl::task_ptr to the currently
         *        executing task.
         */
        task_ptr get();

        /**
         * @brief Returns a tdl::task_ptr to the parent
         *        of the currently executing task.
         */
        task_ptr parent();

        /**
         * @brief Returns a tdl::task_ptr to the continuation
         *        of the currently executing task.
         */
        task_ptr continuation();

        /**
         * @brief Returns the reference count of the
         *        currently executing task.
         */
        std::size_t refcount();

    } // namespace this_task

    /**
     * The tdl::detail namespace groups functionality
     * internally used by the implementation, not
     * intended to be used by the library user.
     */
    namespace detail {

        /**
         * Returns the static central dispatcher coordinating
         * worker threads and dispatching calls made to TDL.
         */
        Dispatcher& get_dispatcher();

        /**
         * @brief Pushes a task to the queue of the calling
         *        worker. Used for continuation pushing when
         *        a task's refcount reaches zero.
         * @param Task to push to the caller's queue.
         */
        void push_task(task_ptr task);

        /**
         * @brief   Returns a tdl::task_ptr to the worker
         *          currently executing the caller task.
         * @details If invoked outside of task execution
         *          context, throws tdl::task_context_exception.
         */
        worker_ptr current_worker();

        /**
         * @brief Returns a tdl::worker_ptr to a randomly
         *        choosen worker. Used during work-stealing
         *        by workers to find a victim to steal from.
         */
        worker_ptr choose_victim();

        /**
         * @brief Checks if TDL has been initialized prior to
         *        the invocation of this method, and throws
         *        tdl::initialization_exception if not.
         */
        void initialization_check();

    } // namespace detail

} // namespace tdl

#endif // TDL_H
