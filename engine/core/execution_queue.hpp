#ifndef EXECUTION_QUEUE_HPP
#define EXECUTION_QUEUE_HPP

#include "window.hpp"
#include "entity.hpp"

#include <queue>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>

namespace core {

class ExecutionQueue {
public:
    ExecutionQueue();
    ~ExecutionQueue();

    void push(std::function<void(Window&, std::shared_ptr<Scene>)> func);

    void exec(Window& window, std::shared_ptr<Scene> activeScene);

private:
    std::mutex mutex;
    std::vector<std::function<void(Window&, std::shared_ptr<Scene>)>> m_funcs;
};

} // namespace core

#endif