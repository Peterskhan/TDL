#pragma once
#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <random>
#include <functional>
#include <algorithm>

#include "make.h"
#include "schedulers.h"
#include "worker.h"
#include "task.h"
#include "types.h"
#include "exceptions.h"

namespace tdl {

    /**
     * @brief The Dispatcher class acts as central dispatch
     *        for the TDL library. A single static instance
     *        is created upon application startup, which is
     *        destroyed after main() returns. The Dispatcher
     *        coordinates worker threads and provides the
     *        functionality for the global functions of TDL.
     */
    class Dispatcher final {
    public:
        /** Constructs the Dispatcher object.*/
        Dispatcher();

        /**
         * @brief   Destroys the Dispatcher object.
         * @details All of the workers are signalled
         *          to stop, and joined by calling
         *          shutdown().
         */
        ~Dispatcher();

        /**
         * See tdl::set_scheduler() for details.
         */
        void set_scheduler(scheduler_t scheduler);

        /**
         * See tdl::set_worker_count() for details.
         */
        void set_worker_count(std::size_t count);

        /**
         * See tdl::get_scheduler() for details.
         */
        scheduler_t get_scheduler() const;

        /**
         * See tdl::get_worker_count() for details.
         */
        std::size_t get_worker_count() const;

        /**
         * @brief Creates and starts the worker threads,
         *        and configures main thread specific
         *        functionality.
         */
        void initialize();

        /**
         * See tdl::initialized() for details.
         */
        bool initialized() const;

        /**
         * See tdl::shutdown() for details.
         */
        void shutdown();

        /**
         * See tdl::submit() for details.
         */
        void submit(task_ptr task);

        /**
         * See tdl::spawn() for details.
         */
        void spawn(task_ptr task);

        /**
         * See tdl::process_main() for details.
         */
        void process_main();

        /**
         * See tdl::detail::push_task() for details.
         */
        void push_task(task_ptr task);

        /**
         * See tdl::detail::current_worker() for details.
         */
        worker_ptr current_worker();

        /**
         * See tdl::detail::choose_victim() for details.
         */
        worker_ptr choose_victim();

    private:
        bool                     m_initialized;
        workerlist_t             m_workers;
        scheduler_t              m_scheduler;
        std::size_t              m_worker_count;
        std::thread::id          m_main_thread_id;
        bool                     m_main_processing;
    };

} // namespace tdl

#endif // DISPATCHER_H
