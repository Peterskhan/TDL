#include "callables.h"

namespace tdl {

    CallableWithoutReturn::CallableWithoutReturn(function_t function)
        : m_function(function)
    {}

    void CallableWithoutReturn::execute() {
        m_function();
    }

} // namespace tdl
