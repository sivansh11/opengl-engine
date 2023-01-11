#include "execution_queue.hpp"

namespace core {

ExecutionQueue::ExecutionQueue() {}
ExecutionQueue::~ExecutionQueue() {}

void ExecutionQueue::push(std::function<void(Window&, std::shared_ptr<Scene>)> func) {
    std::scoped_lock<std::mutex> lock(mutex);
    m_funcs.emplace_back(func);
}

void ExecutionQueue::exec(Window& window, std::shared_ptr<Scene> activeScene) {
    std::scoped_lock<std::mutex> lock(mutex);
    for (auto& func : m_funcs) {
        func(window, activeScene);
    }
    m_funcs.clear();
}

} // namespace core
