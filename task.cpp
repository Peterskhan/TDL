#include "task.h"
#include "tdl.h"

namespace tdl {

    std::atomic_uint Task::s_task_id_counter {0};

    Task::Task()
        : m_task_id(++s_task_id_counter),
          m_refcount(1),
          m_parent(nullptr),
          m_continuation(nullptr),
          m_affinity(thread_affinity::none)
    {}

    void Task::process() {
        // Executing task
        execute();

        // Decrementing parent refcount
        if (m_parent != nullptr)
            m_parent->decrement_refcount();

        // Decrementing own refcount
        decrement_refcount();
    }

    void Task::wait() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_wait_cv.wait(lock, [this](){
            return m_refcount == 0;
        });
    }

    std::size_t Task::get_id() const {
        return m_task_id;
    }

    std::size_t Task::get_refcount() const {
        return m_refcount;
    }

    task_ptr Task::get_parent() const {
        return m_parent;
    }

    task_ptr Task::get_continuation() const {
        return m_continuation;
    }

    thread_affinity Task::get_thread_affinity() const {
        return m_affinity;
    }

    void Task::set_parent(task_ptr parent) {
        m_parent = parent;
    }

    task_ptr Task::set_continuation(task_ptr continuation) {
        m_continuation = continuation;
        return continuation;
    }

    void Task::set_thread_affinity(thread_affinity affinity) {
        m_affinity = affinity;
    }

    void Task::increment_refcount() {
        m_refcount++;
    }

    void Task::decrement_refcount() {
        if (--m_refcount == 0) {
            // Pushing continuation
            if (m_continuation != nullptr) {
                tdl::detail::push_task(m_continuation);
            }

            // Waking up threads waiting for completion
            m_wait_cv.notify_all();
        }
    }

} // namespace tdl

