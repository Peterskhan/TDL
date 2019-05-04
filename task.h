#pragma once
#ifndef TASK_H
#define TASK_H

#include <atomic>
#include <memory>
#include <condition_variable>

#include "types.h"

namespace tdl {

    /**
     * @brief Task can have thread affinities: main and none.
     *        Task with thread_affinity::main assigned are
     *        guaranteed to be processed only in the main
     *        thread.
     */
    enum class thread_affinity { main, none };

    /**
     * @brief The Task class represents a piece of work to
     *        be done. It is the central concept in the TDL
     *        library.
     */
    class Task {
    public:
        /** Constucts a Task object. */
        Task();

        /** Virtual destructor for derived classes. */
        virtual ~Task() = default;

        /** Copying a task is forbidden. */
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        /**
         * @brief Executes the Task, then decrements
         *        the reference count of the parent
         *        (if any). Finally decrements the
         *        Task's own reference count.
         */
        void process();

        /**
         * @brief Blocks the current thread until the
         *        Task is finished. Calling wait() during
         *        Task execution is not adviced, as it
         *        prevents the worker from processing
         *        other Tasks in the meantime.
         */
        void wait();

        /** Getters for Task properties. */
        std::size_t         get_id() const;
        std::size_t         get_refcount() const;
        task_ptr            get_parent() const;
        task_ptr            get_continuation() const;
        thread_affinity     get_thread_affinity() const;

        /** Setters for Task properties. */
        task_ptr    set_continuation(task_ptr continuation);
        void        set_parent(task_ptr parent);
        void        set_thread_affinity(thread_affinity affinity);

        /**
         * @brief Increments the reference count of the
         *        Task. Used when spawning child Tasks,
         *        to increase the reference count of the
         *        parent.
         */
        void increment_refcount();

        /**
         * @brief Decrements the reference count of the
         *        Task. Decrements occur when the Task
         *        itself or one of it's children finish
         *        their execution. When the reference
         *        count reaches zero, the continuation
         *        (if any) is pushed to the queue of the
         *        worker thread who initiated the decrement.
         */
        void decrement_refcount();

    private:
        std::size_t             m_task_id;
        std::atomic_uint        m_refcount;
        std::condition_variable m_wait_cv;
        std::mutex              m_mutex;
        task_ptr                m_parent;
        task_ptr                m_continuation;
        thread_affinity         m_affinity;

        /** Task ID generator. */
        static std::atomic_uint s_task_id_counter;

        /**
         * @brief The main body of the Task.
         *        Override this method to implement
         *        the Task's functionality.
         */
        virtual void execute() = 0;
    };

} // namespace tdl

#endif // TASK_H
