#include "tdl.h"

namespace tdl {

    void set_scheduler(scheduler_t scheduler) {
        detail::get_dispatcher().set_scheduler(scheduler);
    }

    scheduler_t get_scheduler() {
        return detail::get_dispatcher().get_scheduler();
    }

    void set_worker_count(std::size_t count) {
        detail::get_dispatcher().set_worker_count(count);
    }

    std::size_t get_worker_count() {
        return detail::get_dispatcher().get_worker_count();
    }

    void initialize() {
        detail::get_dispatcher().initialize();
    }

    bool initialized() {
        return detail::get_dispatcher().initialized();
    }

    void shutdown() {
        detail::initialization_check();
        detail::get_dispatcher().shutdown();
    }

    void submit(task_ptr task) {
        if(task != nullptr) {
            detail::initialization_check();
            detail::get_dispatcher().submit(task);
        }
    }

    void spawn(task_ptr task) {
        if(task != nullptr) {
            detail::initialization_check();
            detail::get_dispatcher().spawn(task);
        }
    }

    void process_main() {
        detail::initialization_check();
        detail::get_dispatcher().process_main();
    }

    namespace this_task {

        task_ptr get() {
            detail::initialization_check();
            return detail::current_worker()->current_task();
        }

        task_ptr parent() {
            detail::initialization_check();
            return get()->get_parent();
        }

        task_ptr continuation() {
            detail::initialization_check();
            return get()->get_continuation();
        }

        std::size_t refcount() {
            detail::initialization_check();
            return get()->get_refcount();
        }

    } // namespace this_task

    namespace detail {

        Dispatcher& get_dispatcher() {
            static Dispatcher s_dispatcher;
            return s_dispatcher;
        }

        void push_task(task_ptr task) {
            if(task != nullptr)
                detail::get_dispatcher().push_task(task);
        }

        worker_ptr current_worker() {
            return detail::get_dispatcher().current_worker();
        }

        worker_ptr choose_victim() {
            return detail::get_dispatcher().choose_victim();
        }

        void initialization_check() {
            if (!get_dispatcher().initialized())
                throw initialization_exception();
        }

    } // namespace detail

} // namespace tdl
