#pragma once
#ifndef WORKER_H
#define WORKER_H

#include <thread>
#include <deque>
#include <mutex>

#include "task.h"
#include "types.h"

namespace tdl {

    /**
     * @brief The Worker class is responsible for managing
     *        a worker-thread. Tasks are pushed to the worker
     *        by calling push_task() and submit(). When started,
     *        workers begin executing their do_work() method in
     *        a separate thread.
     */
    class Worker final {
    public:
        /** Constructs a Worker. */
        Worker(bool is_main_worker);

        /**
         * @brief Starts the Worker's thread which
         *        executes do_work().
         */
        void start();

        /**
         * @brief Signals the do_work() method to
         *        stop execution when all Tasks are
         *        finished.
         */
        void stop();

        /**
         * @brief Joins the Worker's thread and block
         *        until do_work() finishes execution.
         */
        void join();

        /**
         * @brief Locks the mutex guarding the deque.
         */
        void lock();

        /**
         * @brief Unlocks the mutex guarding the deque.
         */
        void unlock();

        /**
         * @brief Locks this Worker and the other in the
         *        order of their thread IDs. Used during
         *        work-stealing to avoid deadlocking when
         *        two workers are trying to steal from each
         *        other at the same time.
         */
        void lock_in_order(worker_ptr other);

        /**
         * @brief Unlocks this worker and the other in the
         *        order of their thread IDs. Used during
         *        work-stealing to avoid deadlocking when
         *        two workers are trying to steal from each
         *        other at the same time.
         */
        void unlock_in_order(worker_ptr other);

        /**
         * @brief Pushes a Task to the back of the deque.
         *        Used by the scheduler to push new Tasks
         *        to the worker.
         * @param Task to submit for the worker.
         */
        void submit(task_ptr task);

        /**
         * @brief Pushes a Task to the front of the deque.
         *        Used by the worker to push child tasks
         *        and continuations of the currently
         *        executing or finishing Task.
         * @param Task to push to the worker.
         */
        void push_task(task_ptr task);

        /**
         * @brief   Attempts to steal a Task from the worker,
         *          by trying to pop from the front of the
         *          deque. Returns the stolen tdl::task_ptr
         *          if successful, or nullptr otherwise.
         * @details The try_steal() method itself does not
         *          attempts to acquire a lock on the deque,
         *          and the caller must ensure the proper
         *          synchronisation.
         */
        task_ptr try_steal();

        /**
         * @brief Returns a tdl::task_ptr to the
         *        currently executing Task.
         */
        task_ptr current_task() const;

        /**
         * @brief Returns the number of Tasks in the
         *        Worker's deque.
         */
        std::size_t task_count() const;

        /**
         * @brief Returns the ID of the worker.
         *       (Same as the worker's thread ID)
         */
        std::thread::id get_id() const;

        /**
         * @brief The main method of the Worker.
         *        Repeatedly tries to pop a Task
         *        from it's internal queue, and
         *        executes it.
         */
        void do_work();

    private:
        bool                    m_can_steal;
        volatile bool           m_stop_flag;
        std::mutex              m_deque_guard;
        std::deque<task_ptr>    m_deque;
        std::thread             m_thread;
        std::thread::id         m_thread_id;
        task_ptr                m_current_task;
    };

} // namespace tdl

#endif // WORKER_H
