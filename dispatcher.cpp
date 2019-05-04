#include "dispatcher.h"
#include <iostream>

namespace tdl {

    Dispatcher::Dispatcher()
        : m_initialized {false},
          m_scheduler {load_balancing_scheduler()},
          m_worker_count {std::thread::hardware_concurrency()},
          m_main_processing {false}
    {}

    Dispatcher::~Dispatcher() {
        // Stopping and joining worker threads
        shutdown();
    }

    void Dispatcher::set_scheduler(scheduler_t scheduler) {
        if (!m_initialized) m_scheduler = scheduler;
    }

    scheduler_t Dispatcher::get_scheduler() const {
        return m_scheduler;
    }

    void Dispatcher::set_worker_count(std::size_t count) {
        if (!m_initialized) m_worker_count = count;
    }

    std::size_t Dispatcher::get_worker_count() const {
        return m_worker_count;
    }

    void Dispatcher::initialize() {
        // Checking multiple initialization attempts
        if (m_initialized) return;

        // Saving main thread id
        m_main_thread_id = std::this_thread::get_id();

        // Creating main thread worker:
        // The main thread worker is special as it does
        // not manage a separate thread of execution.
        // It is not started, stopped or joined, and only
        // does processing when do_work() is explicitly called
        // by process_main(). It's stop flag is always set
        // to true, thus it immediately returns after it has
        // finished it's Tasks. It does not participate in
        // load balancing, and can not be accessed by the
        // scheduler.
        worker_ptr main_worker = std::make_shared<Worker>(true);
        m_workers.push_back(main_worker);

        // Creating workers
        for (std::size_t i = 0; i < m_worker_count; i++) {
            // Creating new worker
            worker_ptr new_worker = std::make_shared<Worker>(false);

            // Pushing worker into container
            m_workers.push_back(new_worker);
        }

        // Starting workers
        for (auto it = ++m_workers.begin(); it != m_workers.end(); it++) {
            (*it)->start();
        }

        // Setting initialization flag
        m_initialized = true;
    }

    bool Dispatcher::initialized() const {
        return m_initialized;
    }

    void Dispatcher::shutdown() {
        // Signalling workers to stop
        for (auto it = ++m_workers.begin(); it != m_workers.end(); it++) {
            (*it)->stop();
        }

        // Joining with worker threads
        for (auto it = ++m_workers.begin(); it != m_workers.end(); it++) {
            (*it)->join();
        }
    }

    void Dispatcher::submit(task_ptr task) {
        // Checking main thread affinity
        if (task->get_thread_affinity() == thread_affinity::main) {
            // Submitting task to main thread worker
            (*m_workers.begin())->submit(task);
            return;
        }

        // Calling scheduler to select a worker for the task
        auto selected = m_scheduler(++m_workers.begin(),
                                    m_workers.end());

        // Check iterator returned by the scheduler
        if (selected == m_workers.end())
            throw scheduler_exception();

        // Submitting task to the worker
        (*selected)->submit(task);
    }

    void Dispatcher::spawn(task_ptr task) {
        // Finding worker associated with calling thread
        worker_ptr spawner = current_worker();

        // Setting parent of the task to the caller
        task_ptr parent = spawner->current_task();
        task->set_parent(parent);
        parent->increment_refcount();

        // Pushing task to the worker
        spawner->push_task(task);
    }

    void Dispatcher::process_main() {
        // Checking if calling thread is the main thread
        if (std::this_thread::get_id() != m_main_thread_id)
            throw wrong_thread_exception();

        // Setting processing flag
        m_main_processing = true;

        // Processing main thread Tasks
        (*m_workers.begin())->do_work();

        // Unsetting processing flag
        m_main_processing = false;
    }

    worker_ptr Dispatcher::current_worker() {
        // Finding worker thread with the same id as the calling thread
        auto it = std::find_if(m_workers.begin(), m_workers.end(), [&](const worker_ptr &element) {
            return element->get_id() == std::this_thread::get_id();
        });

        // Check main thread context
        if (it == m_workers.begin() && !m_main_processing)
            throw task_context_exception();

        // Check task-execution context
        if (it != m_workers.end()) return *it;
        else throw task_context_exception();
    }

    worker_ptr Dispatcher::choose_victim() {
        // Generating index from range [1; worker_count)
        std::size_t index = 1 + std::rand() % (m_worker_count);
        return m_workers[index];
    }

    void Dispatcher::push_task(task_ptr task) {
        // Finding worker associated with calling thread
        worker_ptr spawner = current_worker();

        // Pushing task to the worker
        spawner->push_task(task);
    }

} // namespace tdl

