#include "worker.h"
#include "tdl.h"

namespace tdl {

    Worker::Worker(bool is_main_worker)
        : m_can_steal(!is_main_worker),
          m_stop_flag(false),
          m_current_task(nullptr)
    {
        if (is_main_worker) {
            m_thread_id = std::this_thread::get_id();
            m_stop_flag = true;
        }
    }

    void Worker::start() {
        // Starting thread executing do_work()
        m_thread = std::thread(&Worker::do_work, this);

        // Assigning m_thread_id
        m_thread_id = m_thread.get_id();
    }

    void Worker::stop() {
        m_stop_flag = true;
    }

    void Worker::join() {
        if (m_thread.joinable())
            m_thread.join();
    }

    void Worker::lock() {
        m_deque_guard.lock();
    }

    void Worker::unlock() {
        m_deque_guard.unlock();
    }

    void Worker::lock_in_order(worker_ptr other) {
        if (get_id() < other->get_id()) {
            lock(); other->lock();
        }
        else {
            other->lock(); lock();
        }
    }

    void Worker::unlock_in_order(worker_ptr other) {
        if (get_id() < other->get_id()) {
            other->unlock(); unlock();
        }
        else {
            unlock(); other->unlock();
        }
    }

    void Worker::submit(task_ptr task) {
        std::lock_guard<std::mutex> guard(m_deque_guard);
        m_deque.push_back(task);
    }

    void Worker::push_task(task_ptr task) {
        std::lock_guard<std::mutex> guard(m_deque_guard);
        m_deque.push_front(task);
    }

    task_ptr Worker::try_steal() {
        // Return nullptr if no Tasks available
        if (m_deque.empty()) return nullptr;

        // Otherwise remove the Task from the deque
        task_ptr stolen = m_deque.front();
        m_deque.pop_front();

        return stolen;
    }

    task_ptr Worker::current_task() const {
        return m_current_task;
    }

    std::size_t Worker::task_count() const {
        return m_deque.size();
    }

    std::thread::id Worker::get_id() const {
        return m_thread_id;
    }

    void Worker::do_work() {
        while (!m_deque.empty() || !m_stop_flag) {

            // Check if there is a task available
            std::unique_lock<Worker> guard(*this);
            if (!m_deque.empty()) {
                m_current_task = m_deque.front();
                m_deque.pop_front();
            } else {
                m_current_task = nullptr;
            }
            guard.unlock();

            // Executing Task
            if (m_current_task != nullptr) {
                m_current_task->process();
            }
            else if (m_can_steal){
                // Yielding CPU time to others
                std::this_thread::yield();
                std::this_thread::sleep_for(std::chrono::microseconds{1});

                // Choosing a victim
                worker_ptr victim = detail::choose_victim();
                if (victim.get() == this) continue;

                // Trying to steal from victim
                lock_in_order(victim);
                m_current_task = victim->try_steal();
                unlock_in_order(victim);

                // Executing stolen task
                if (m_current_task != nullptr) {
                    m_current_task->process();
                }
            }
        }
    }

} // namespace tdl
