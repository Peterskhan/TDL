#pragma once
#ifndef SCHEDULERS_H
#define SCHEDULERS_H

#include "dispatcher.h"
#include "types.h"

namespace tdl {

    /**
     * @brief The load_balancing_scheduler struct is a
     *        functor which can be passed to tdl::set_scheduler(),
     *        and implements a load balancing scheduling algorithm.
     *        The algorithm performs naive distribution and picks
     *        the worker with the least amount of active Tasks.
     */
    struct load_balancing_scheduler final {
        /**
         * @brief Returns an iterator to the least busy worker.
         * @param Iterator to the first element.
         * @param Iterator to the last element.
         */
        workerlist_t::iterator operator()(workerlist_t::iterator begin,
                                          workerlist_t::iterator end);
    };

    /**
     * @brief The round_robin_scheduler struct is a
     *        functor which can be passed to tdl::set_scheduler(),
     *        and implements a round-robin scheduling algorithm.
     *        The algorithm picks the next worker in sequence.
     */
    struct round_robin_scheduler final {
        /**
         * @brief Returns an iterator to the next worker.
         * @param Iterator to the first element.
         * @param Iterator to the last element.
         */
        workerlist_t::iterator operator()(workerlist_t::iterator begin,
                                          workerlist_t::iterator end);
    };

    /**
     * @brief The random_scheduler struct is a
     *        functor which can be passed to tdl::set_scheduler(),
     *        and implements a random scheduling algorithm.
     *        The algorithm picks the next worker at random.
     */
    struct random_scheduler final {
        /**
         * @brief Constructs a random_scheduler object.
         *        Used to seed the random-generator.
         */
        random_scheduler();

        /**
         * @brief Returns an iterator to a random worker.
         * @param Iterator to the first element.
         * @param Iterator to the last element.
         */
        workerlist_t::iterator operator()(workerlist_t::iterator begin,
                                          workerlist_t::iterator end);
    };

} // namespace tdl

#endif // SCHEDULERS_H
